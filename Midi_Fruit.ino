#include <Wire.h>
#include <MIDI.h>
#include "Adafruit_MPR121.h"

MIDI_CREATE_DEFAULT_INSTANCE();

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

int seqOne[] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  Wire.begin();
  MIDI.begin();
  MIDI.turnThruOff();
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
//      Serial.print(i); Serial.println(" touched");
      MIDI.sendNoteOff(seqOne[i], 0, 1);
      MIDI.sendNoteOn(seqOne[i], 100, 1);  // Send a Note (pitch 42, velo 127 on channel 1)
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
//      Serial.print(i); Serial.println(" released");
      MIDI.sendNoteOff(seqOne[i], 0, 1);   // Stop the note
    }
  }

  // reset our state
  lasttouched = currtouched;
  return;
}
