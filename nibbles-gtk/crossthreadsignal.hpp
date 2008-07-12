#ifndef NIBBLES_GTK__CROSSTHREADSIGNAL_HPP
#define NIBBLES_GTK__CROSSTHREADSIGNAL_HPP

//#include <nibbles/utility/argpack.hpp>

namespace nibbles { namespace gtk {

// Signal which when called on one thread will be raised asynchronously on the
// GUI thread (and thus can safely mess with widgets)
template<typename... Args>
class CrossThreadSignal {
  public:
    CrossThreadSignal() {
      callAlert_.connect(sigc::mem_fun(this, &CrossThreadSignal::flush));
    }

    template<typename Function>
    void connect(const Function& f) {
      signal_.connect(f);
    }

    void operator()(const Args&... args) {
      {
        boost::lock_guard<boost::mutex> lock(pendingCallsMutex_);
        pendingCalls_.push_back(
            boost::bind(&CrossThreadSignal::raise, this, args...)
          );
      }
      callAlert_();
    }
  private:
    void raise(const Args&... args) {
      signal_(args...);
    }
    
    typedef decltype(
        boost::bind(
          &CrossThreadSignal::raise,
          ((CrossThreadSignal*)0), *((Args*) 0)...
        )
      ) PendingCall;
    // Each list entry is a set of arguments
    std::list<PendingCall> pendingCalls_;
    boost::mutex pendingCallsMutex_; // Mutex for above list
    Glib::Dispatcher callAlert_; // The trigger that can signal between threads
    // Signal to connect to; raised on GUI thread
    boost::signal<void (const Args&...)> signal_;

    void flush() {
      boost::lock_guard<boost::mutex> lock(pendingCallsMutex_);
      while (!pendingCalls_.empty()) {
        pendingCalls_.front()();
        pendingCalls_.pop_front();
      }
    }
};

}}

#endif // NIBBLES_GTK__CROSSTHREADSIGNAL_HPP

