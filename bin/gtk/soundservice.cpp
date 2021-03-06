#include "soundservice.hpp"

#include <cstring>
#include <vector>
#include <list>

#ifdef NIBBLES_ENABLE_SOUND
#include <sndfile.h>
#include <portaudio.h>
#endif

namespace nibbles { namespace gtk {

#ifdef NIBBLES_ENABLE_SOUND

// I've written to two differernt portaudio interfaces; undef this to use the
// other one
#define NEW_PORTAUDIO

class SoundService::Impl {
  public:
    ~Impl();
};

class PaPlaying;

class PaSound : public Sound {
  public:
    PaSound(
      boost::filesystem::path const&,
      std::shared_ptr<SoundService::Impl> impl_
    );
    ~PaSound();

    void asyncPlay(bool supressExceptions);
    SF_INFO const& info() const { return info_; }
    std::vector<int16_t> const& data() const { return data_; }
  private:
    SF_INFO info_;
    std::vector<int16_t> data_;
    std::list<std::unique_ptr<PaPlaying>> playings_;
    // All Sounds keep a pointer here so that Pa_Terminate isn't called until
    // they are all gone
    std::shared_ptr<SoundService::Impl> handle_;
};

class PaPlaying {
  public:
    PaPlaying(PaSound const&);
    ~PaPlaying();
  private:
    static int paCallback(
#ifdef NEW_PORTAUDIO
      const void* input,
#else
      void* input,
#endif
      void* output,
      unsigned long frameCount,
#ifdef NEW_PORTAUDIO
      const PaStreamCallbackTimeInfo* timeInfo,
      PaStreamCallbackFlags statusFlags,
#else
      PaTimestamp outTime,
#endif
      void* userData
    );

    PaSound const& sound_;
    PaStream* stream_;
    size_t pos_;
};

class PaException : public std::runtime_error {
  public:
    PaException(std::string const& context, PaError const error) :
      std::runtime_error(
        "Portaudio error: "+context+": "+Pa_GetErrorText(error)
      )
    {}
};

SoundService::SoundService() :
  impl_(new Impl())
{
  PaError const error = Pa_Initialize();
  if (error != paNoError) {
    throw PaException("Initialize", error);
  }
}

SoundService::Impl::~Impl()
{
  PaError const error = Pa_Terminate();
  if (error != paNoError) {
    fprintf(stderr, "Error closing down sound system: %s\n",
      Pa_GetErrorText(error));
  }
}

std::unique_ptr<Sound>
SoundService::makeSound(boost::filesystem::path const& path) const
{
  return std::unique_ptr<Sound>{new PaSound(path, impl_)};
}

PaSound::PaSound(
  boost::filesystem::path const& path,
  std::shared_ptr<SoundService::Impl> handle
) :
  handle_(std::move(handle))
{
  info_.format = 0;
  SNDFILE* sndfile = sf_open(path.c_str(), SFM_READ, &info_);
  if (!sndfile) {
    throw std::runtime_error("open of '"+path.string()+"' failed");
  }
  size_t const numSamplesToRead = info_.channels*256;
  size_t numRead;
  do {
    int16_t buffer[numSamplesToRead];
    numRead = sf_read_short(sndfile, buffer, numSamplesToRead);
    std::copy(buffer, buffer+numRead, std::back_inserter(data_));
  } while (numRead >= numSamplesToRead);
  sf_close(sndfile);
}

PaSound::~PaSound()
{
  // The PaPlayings must be destructed before the shared_ptr to the
  // SoundService
  playings_.clear();
}

void PaSound::asyncPlay(bool supressExceptions)
{
  try {
    playings_.push_back(std::unique_ptr<PaPlaying>{new PaPlaying(*this)});
  } catch (PaException const&) {
    if (!supressExceptions) throw;
  }
}

PaPlaying::PaPlaying(PaSound const& sound) :
  sound_(sound),
  pos_(0)
{
  PaError error = Pa_OpenDefaultStream(
    &stream_,
    0, // no input channels
    sound.info().channels,
    paInt16,
    sound.info().samplerate,
#ifdef NEW_PORTAUDIO
    paFramesPerBufferUnspecified,
#else
    256,
    1,
#endif
    &PaPlaying::paCallback,
    this
  );
  if (error != paNoError) {
    throw PaException("OpenDefaultStream", error);
  }
  error = Pa_StartStream(stream_);
  if (error != paNoError) {
    throw PaException("StartStream", error);
  }
}

PaPlaying::~PaPlaying()
{
#ifdef NEW_PORTAUDIO
  if (!Pa_IsStreamStopped(stream_)) {
#else
  if (!Pa_StreamActive(stream_)) {
#endif
    PaError error = Pa_AbortStream(stream_);
    if (error != paNoError) {
      fprintf(stderr, "Error aborting stream: %s\n",
        Pa_GetErrorText(error));
    }
  }
  PaError error = Pa_CloseStream(stream_);
  if (error != paNoError) {
    fprintf(stderr, "Error closing stream: %s\n",
      Pa_GetErrorText(error));
  }
}

int PaPlaying::paCallback(
#ifdef NEW_PORTAUDIO
  const void*,
#else
  void*,
#endif
  void* output,
  unsigned long frameCount,
#ifdef NEW_PORTAUDIO
  const PaStreamCallbackTimeInfo* /*timeInfo*/,
  PaStreamCallbackFlags /*statusFlags*/,
#else
  PaTimestamp /*outTime*/,
#endif
  void* userData
)
{
  PaPlaying& playing = *reinterpret_cast<PaPlaying*>(userData);
  PaSound const& sound = playing.sound_;
  int16_t* out = reinterpret_cast<int16_t*>(output);
  size_t const dataLeft = sound.data().size() - playing.pos_;
  size_t const numSamples = sound.info().channels*frameCount;
  if (numSamples > dataLeft) {
    // Wants more data than there is; stop instead
    std::memcpy(out, &sound.data()[playing.pos_], sizeof(int16_t)*dataLeft);
#ifdef NEW_PORTAUDIO
    return paComplete;
#else
    return 1;
#endif
  } else {
    std::memcpy(out, &sound.data()[playing.pos_], sizeof(int16_t)*numSamples);
    playing.pos_ += numSamples;
    return 0;
  }
}

#else // NIBBLES_ENABLE_SOUND

class SoundService::Impl {
  public:
};

class DudSound : public Sound {
  public:
    virtual void asyncPlay(bool) {
      // Do nothing
    }
};

SoundService::SoundService() :
  impl_()
{
}

std::unique_ptr<Sound>
SoundService::makeSound(boost::filesystem::path const&) const
{
  return std::unique_ptr<Sound>{new DudSound()};
}

#endif // NIBBLES_ENABLE_SOUND

}}

