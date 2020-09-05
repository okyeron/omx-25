int ticks = 0;            // A tick of the clock
bool clockSource = 0;     // Internal clock (0), external clock (1)
bool playing = 0;         // Are we playing a tune?
bool paused = 0;          // Are we paused?
bool stopped = 1;         // Are we stopped? (Must init to 1)
byte songPosition = 0;    // A place to store the current MIDI song position
byte lastNote;            // A place to remember the last MIDI note we played
byte seqPos = 0;          // What position in the sequence are we in?
bool seqLedRefresh = 1;   // Should we refresh the LED array?
int playingPattern = 0;  // The currently playing pattern, 0-7
byte patternAmount = 2;   // How many patterns will play


// Determine how to play a step
// -1: restart
// 0: mute
// 1: play
// 2: accent
int stepPlay[8][16] = {
  {1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
  { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Slide note: 1
// Normal note: 0
bool stepSlide[8][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// The MIDI values of the notes in each step.
// C3 = 40
int stepNote[8][16] = {
  {40, 40, 0, 0, 42, 42, 43, 40, 40, 0, 0, 38, 38, 0, 40, 11},
  {40, 40, 0, 0, 42, 42, 43, 40, 40, 0, 0, 38, 38, 0, 40, 11},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void seqStart() {
  playing = 1;
  paused = 0;
  stopped = 0;
}

void seqContinue() {
  playing = 1;
  paused = 0;
  stopped = 0;
}

void seqPause() {
  playing = 0;
  paused = 1;
  stopped = 0;
}

void seqStop() {
  ticks = 0;
  seqPos = 0;
  playing = 0;
  paused = 0;
  stopped = 1;
  seqLedRefresh = 1;
}

// Play a note
void playNote() {
  //Serial.println(stepNote[playingPattern][seqPos]); // Debug

  switch (stepPlay[playingPattern][seqPos]) {
    case -1:
      // Skip the remaining notes
      seqPos = 16;
      break;
    case 0:
      // Don't play a note
      break;
    case 1:
      // Turn off the previous note and play a new note.

      //usbMIDI.sendNoteOff(lastNote, 0, midiChannel);
      //usbMIDI.sendNoteOn(stepNote[playingPattern][seqPos], 127, midiChannel);
      lastNote = stepNote[playingPattern][seqPos];
      break;
    case 2:
      // Turn off the previous note, and play a new accented note

      //usbMIDI.sendNoteOff(lastNote, 0, midiChannel);
      //usbMIDI.sendNoteOn(stepNote[playingPattern][seqPos], 127, midiChannel);
      lastNote = stepNote[playingPattern][seqPos];
      break;
  }
}