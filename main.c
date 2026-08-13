// === NOTE DEFINITIONS ===
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_A5  880
#define REST     0

// === PIN DEFINITIONS ===
const int pinON = 6;
const int buzzer = 3;

const int pinLeftForward = 11;
const int pinLeftBackward = 12;
const int pinLeftPWM = 10;

const int pinRightForward = 7;
const int pinRightBackward = 8;
const int pinRightPWM = 9;

const int ledPin = 13;
const int led1 = 14;
const int led2 = 15;

// === SONG SETUP ===
const int tempo = 180;
int melody[] = {
  REST,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2,
  NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2,
  NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;

// === STATE VARIABLES ===
bool playing = false;
int currentNote = 0;
unsigned long noteStartTime = 0;
unsigned long ledTimer = 0;
bool ledState = false;

int motionStep = 0;
unsigned long motionStartTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinON, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  pinMode(pinLeftForward, OUTPUT);
  pinMode(pinLeftBackward, OUTPUT);
  pinMode(pinLeftPWM, OUTPUT);

  pinMode(pinRightForward, OUTPUT);
  pinMode(pinRightBackward, OUTPUT);
  pinMode(pinRightPWM, OUTPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  stopMotors();
  analogWrite(pinLeftPWM, 200);
  analogWrite(pinRightPWM, 200);
}

void loop() {
  if (!playing && digitalRead(pinON) == LOW) {
    delay(20);
    if (digitalRead(pinON) == LOW) {
      playing = true;
      currentNote = 0;
      noteStartTime = millis();
      ledTimer = millis();
      motionStep = 0;
      motionStartTime = millis();
    }
  }

  if (playing) {
    playMelodyStep();
    updateLEDs();
    runMotionStep();
  }
}

void playMelodyStep() {
  if (currentNote >= notes * 2) {
    noTone(buzzer);
    return;
  }

  int divider = melody[currentNote + 1];
  int duration = (divider > 0) 
    ? wholenote / divider 
    : wholenote / abs(divider) * 1.5;

  if (millis() - noteStartTime >= duration) {
    currentNote += 2;
    if (currentNote < notes * 2) {
      int nextNote = melody[currentNote];
      int nextDuration = (melody[currentNote + 1] > 0)
        ? wholenote / melody[currentNote + 1]
        : wholenote / abs(melody[currentNote + 1]) * 1.5;
      tone(buzzer, nextNote, nextDuration * 0.9);
      noteStartTime = millis();
    } else {
      noTone(buzzer);
    }
  }
}

void updateLEDs() {
  if (millis() - ledTimer >= 800) {
    ledState = !ledState;
    digitalWrite(led1, ledState);
    digitalWrite(led2, !ledState);
    ledTimer = millis();
  }
}

void runMotionStep() {
  unsigned long now = millis();
  switch (motionStep) {
    case 0: // forward
      digitalWrite(pinLeftForward, HIGH);
      digitalWrite(pinLeftBackward, LOW);
      digitalWrite(pinRightForward, HIGH);
      digitalWrite(pinRightBackward, LOW);
      motionStartTime = now;
      motionStep++;
      break;
    case 1:
      if (now - motionStartTime >= 2100) {
        stopMotors();
        motionStartTime = now;
        motionStep++;
      }
      break;
    case 2: // spin right
      digitalWrite(pinLeftForward, HIGH);
      digitalWrite(pinLeftBackward, LOW);
      digitalWrite(pinRightForward, LOW);
      digitalWrite(pinRightBackward, HIGH);
      motionStartTime = now;
      motionStep++;
      break;
    case 3:
      if (now - motionStartTime >= 1420) {
        stopMotors();
        motionStartTime = now;
        motionStep++;
      }
      break;
    case 4: // forward again
      digitalWrite(pinLeftForward, HIGH);
      digitalWrite(pinLeftBackward, LOW);
      digitalWrite(pinRightForward, HIGH);
      digitalWrite(pinRightBackward, LOW);
      motionStartTime = now;
      motionStep++;
      break;
    case 5:
      if (now - motionStartTime >= 2100) {
        stopMotors();
        motionStartTime = now;
        motionStep++;
      }
      break;
    case 6: // spin left
      digitalWrite(pinLeftForward, LOW);
      digitalWrite(pinLeftBackward, HIGH);
      digitalWrite(pinRightForward, HIGH);
      digitalWrite(pinRightBackward, LOW);
      motionStartTime = now;
      motionStep++;
      break;
    case 7:
      if (now - motionStartTime >= 1420) {
        stopMotors();
        motionStep++;
      }
      break;
    case 8:
      // all done
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      playing = false;
      break;
  }
}

void stopMotors() {
  digitalWrite(pinLeftForward, LOW);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinRightForward, LOW);
  digitalWrite(pinRightBackward, LOW);
  analogWrite(pinLeftPWM, 200);
  analogWrite(pinRightPWM, 200);
}
