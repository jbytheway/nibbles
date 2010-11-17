LY_SOURCES := $(wildcard *.ly)
BASES := $(basename $(LY_SOURCES))
PDFS := $(addsuffix .pdf,$(BASES))
MIDIS := $(addsuffix .midi,$(BASES))
WAVS := $(addsuffix .wav,$(BASES))
FLACS := $(addsuffix .flac,$(BASES))
MKAS := $(addsuffix .mka,$(BASES))

all: $(MKAS)

clean:
	rm -f *.flac *.wav *.midi *.mka *.pdf *.ps *.ana

$(MIDIS): %.midi: %.ly
	lilypond $<

$(WAVS): %.wav: %.midi
	timidity -s 48000 -Ow $<

#$(FLACS): %.flac: %.midi
#	timidity -s 48000 -OF $<

$(FLACS): %.flac: %.wav
	rm -f $@
	flac $<

$(MKAS): %.mka: %.flac
	mkvmerge -o $@ $<

.DELETE_ON_ERROR:

