#include <MIDIUSB.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const int fsrPins[5] = {A0, A1, A2, A3, A4};
int fsrValues[5];
bool notePlaying[5] = {false, false, false, false, false};
int noteVelocities[5] = {0, 0, 0, 0, 0};

const int rootNotes[] = {60, 61, 62, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71};
int rootIndex = 0;
bool isMajor = true;
int octaveShift = 0;

const int majorPentatonic[] = {0, 2, 4, 7, 9};
const int minorPentatonic[] = {0, 3, 5, 7, 10};

const int threshold = 200;
const int deadzone = 20;

bool lastOctaveUpState = HIGH;
bool lastOctaveDownState = HIGH;

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  updateDisplay();
}

void loop() {

  if(digitalRead(WIO_5S_UP) == LOW){
    rootIndex = (rootIndex + 1) % 12;
    updateDisplay();
    delay(200);
  }else if(digitalRead(WIO_5S_DOWN) == LOW){
    rootIndex = (rootIndex - 1 + 12) % 12;
    updateDisplay();
    delay(200);
  }else if(digitalRead(WIO_5S_PRESS) == LOW){
    isMajor = !isMajor;
    updateDisplay();
    delay(200);
  }

  if(digitalRead(WIO_KEY_A) == LOW && lastOctaveUpState == HIGH){
      octaveShift = constrain(octaveShift + 12, -24, 24);
      Serial.println("Octave Up");
  }
  lastOctaveUpState = digitalRead(WIO_KEY_A);

  if (digitalRead(WIO_KEY_B) == LOW && lastOctaveDownState == HIGH) {
    octaveShift = constrain(octaveShift - 12, -24, 24);  // Shift down by an octave (12 semitones)
    Serial.println("Octave down");
  }
  lastOctaveDownState = digitalRead(WIO_KEY_B);


  for(int i = 0; i < 5; i++){
    fsrValues[i] = analogRead(fsrPins[i]);
    Serial.print("FSR ");
    Serial.print(i);
    Serial.print(" Value: ");
    Serial.println(fsrValues[i]);
  }


  const int* scale = isMajor ? majorPentatonic : minorPentatonic;
  for(int i = 0; i < 5; i++){

    int note = rootNotes[rootIndex] + scale[i] + octaveShift;


    if(fsrValues[i] > threshold + deadzone){
      if(!notePlaying[i]){
        int velocity = map(fsrValues[i], threshold, 500, 0, 127);
        velocity = constrain(velocity, 0, 127);
        noteVelocities[i] = velocity;
        Serial.print("Calculated Velocity: "); // Add this line to debug
        Serial.println(velocity);
        
        midiEventPacket_t noteOn = {0x09, 0x90, note, velocity};
        MidiUSB.sendMIDI(noteOn);
        MidiUSB.flush();
        notePlaying[i] = true;
        Serial.print("Note on: ");
        Serial.println(note);
      }
    } else if(fsrValues[i] <= threshold && notePlaying[i]){
        midiEventPacket_t noteOff = {0x09, 0x80, note, 0};
        MidiUSB.sendMIDI(noteOff);
        MidiUSB.flush();
        notePlaying[i] = false;
        Serial.print("Note off: ");
        Serial.println(note);
    }
  }
  
  MidiUSB.read();
  drawVelocity();
}

void updateDisplay(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.print("Root: ");
  tft.println(noteToString(rootNotes[rootIndex]));

  tft.print("Mode: ");
  tft.println(isMajor ? "Major Pentatonic" : "Minor Pentatonic");
}

void drawVelocity() {
  tft.setCursor(25, 100); // Position for velocity visualization
  tft.fillRect(0, 100, 240, 80, TFT_BLACK); // Clear previous visualizer

  for (int i = 0; i < 5; i++) {

    if (notePlaying[i]) {
      int velocity = map(fsrValues[i], threshold, 1023, 0, 127);
      velocity = constrain(velocity, 0, 127);
      int barHeight = map(velocity, 0, 127, 0, 70); 

      tft.fillRect(i * 48, 100 + (70 - barHeight), 40, barHeight, TFT_GREEN); 
    }
    tft.setCursor(i * 48, 100 - 20);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.print(noteToString(rootNotes[rootIndex] + (isMajor ? majorPentatonic[i] : minorPentatonic[i])));
  }
}

String noteToString(int note){
  const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  return noteNames[note % 12]; 
}