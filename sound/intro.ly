\version "2.12.2"

\score {
  \new Staff \with {midiInstrument = #"electric grand"} {
    \relative c, {
      c8 d e d c d e4 c c
    }
  }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 160 4)
    }
  }
}
