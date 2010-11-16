\version "2.12.2"

\score {
  \new Staff \with {midiInstrument = #"electric grand"} {
    \relative c,, {
      %e32 f g e f d c
      %e64 r f r g r e r f r d r c
      e64. r128 f64. r128 g64. r128 e64. r128 f64. r128 d64. r128 c64.
    }
  }
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 120 4)
    }
  }
}
