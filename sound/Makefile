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

#$(WAVS): %.wav: %.midi
#	timidity -Ow $<

$(FLACS): %.flac: %.midi
	timidity -OF $<

#$(FLACS): %.flac: %.wav
#	flac -f $<

$(MKAS): %.mka: %.flac
	mkvmerge -o $@ $<

.DELETE_ON_ERROR:

