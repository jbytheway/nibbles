\version "2.12.2"

\score {
  \new Staff \with {midiInstrument = #"electric grand"} {
    \relative c, {
      c4 d e d c d e2 c c
    }
  }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 800 4)
    }
  }
}
