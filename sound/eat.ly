\version "2.12.2"

\score {
  \new Staff \with {midiInstrument = #"electric grand"} {
    \relative c,, {
      c32 r c r c r e
    }
  }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 120 4)
    }
  }
}
