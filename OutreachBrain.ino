// by Matt Gaidica, PhD | gaidica@wustl.edu
// Neurotech Hub | WUSTL
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define CEREBELLUM_LED 8
#define OCCIPITAL_LED 9
#define PARIETAL_LED 10
#define TEMPORAL_LED 11
#define FRONTAL_LED 12

#define CEREBELLUM_SENSE A0
#define OCCIPITAL_SENSE A1
#define PARIETAL_SENSE A2
#define TEMPORAL_SENSE A3
#define FRONTAL_SENSE A6
#define SECRET_SENSE A7

#define CAL_SAMPLES 10
int cerebellum_cal[CAL_SAMPLES];
int occipital_cal[CAL_SAMPLES];
int parietal_cal[CAL_SAMPLES];
int temporal_cal[CAL_SAMPLES];
int frontal_cal[CAL_SAMPLES];
int secret_cal[CAL_SAMPLES];

int cerebellum_offset = 0;
int occipital_offset = 0;
int parietal_offset = 0;
int temporal_offset = 0;
int frontal_offset = 0;
int secret_offset = 0;
float sense_std = 0;

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(115200);
  pinMode(CEREBELLUM_LED, OUTPUT);
  pinMode(OCCIPITAL_LED, OUTPUT);
  pinMode(PARIETAL_LED, OUTPUT);
  pinMode(TEMPORAL_LED, OUTPUT);
  pinMode(FRONTAL_LED, OUTPUT);

  pinMode(CEREBELLUM_SENSE, INPUT);
  pinMode(OCCIPITAL_SENSE, INPUT);
  pinMode(PARIETAL_SENSE, INPUT);
  pinMode(TEMPORAL_SENSE, INPUT);
  pinMode(FRONTAL_SENSE, INPUT);
  pinMode(SECRET_SENSE, INPUT);

  resetLEDs();

  matrix.begin(0x70);  // pass in the address
  randomSeed(analogRead(0));

  for (int i = 0; i < CAL_SAMPLES; i++) {
    cerebellum_cal[i] = analogRead(CEREBELLUM_SENSE);
    occipital_cal[i] = analogRead(OCCIPITAL_SENSE);
    parietal_cal[i] = analogRead(PARIETAL_SENSE);
    temporal_cal[i] = analogRead(TEMPORAL_SENSE);
    frontal_cal[i] = analogRead(FRONTAL_SENSE);
    secret_cal[i] = analogRead(SECRET_SENSE);
    delay(10);
  }
  cerebellum_offset = calculateMean(cerebellum_cal, CAL_SAMPLES);
  occipital_offset = calculateMean(occipital_cal, CAL_SAMPLES);
  parietal_offset = calculateMean(parietal_cal, CAL_SAMPLES);
  temporal_offset = calculateMean(temporal_cal, CAL_SAMPLES);
  frontal_offset = calculateMean(frontal_cal, CAL_SAMPLES);
  secret_offset = calculateMean(secret_cal, CAL_SAMPLES);
  sense_std = 8;  //calculateStandardDeviation(secret_cal, CAL_SAMPLES);

  // while (true) {
  //   int cerebellum = analogRead(CEREBELLUM_SENSE);
  //   int occipital = analogRead(OCCIPITAL_SENSE);
  //   int parietal = analogRead(PARIETAL_SENSE);
  //   int temporal = analogRead(TEMPORAL_SENSE);
  //   int frontal = analogRead(FRONTAL_SENSE);
  //   int secret = analogRead(SECRET_SENSE);
  //   Serial.print(cerebellum);
  //   Serial.print(",");
  //   Serial.print(occipital);
  //   Serial.print(",");
  //   Serial.print(parietal);
  //   Serial.print(",");
  //   Serial.print(temporal);
  //   Serial.print(",");
  //   Serial.print(frontal);
  //   Serial.print(",");
  //   Serial.println(secret);
  //   delay(100);
  //   lightLEDs();
  //   delay(200);
  // }
}

void lightLEDs() {
}

static const uint8_t PROGMEM
  waveform00_bmp[] = {
    B00000010,
    B00000110,
    B00011100,
    B11110000,
    B00011111,
    B00000001,
    B00000011,
    B00000010
  },
  waveform01_bmp[] = { B00011000, B00110000, B11100000, B00110000, B00011000, B00001111, B00001100, B00011000 }, waveform02_bmp[] = { B00000001, B00111111, B00000011, B00001111, B00000011, B11111111, B00000011, B00000001 }, waveform03_bmp[] = { B11000000, B10000000, B11111110, B11100000, B11000000, B10000000, B11111100, B11100000 }, waveform04_bmp[] = { B00011000, B01110000, B00011111, B11110000, B00011110, B00011000, B01110000, B00011000 };

void resetLEDs() {
  digitalWrite(CEREBELLUM_LED, LOW);
  digitalWrite(OCCIPITAL_LED, LOW);
  digitalWrite(PARIETAL_LED, LOW);
  digitalWrite(TEMPORAL_LED, LOW);
  digitalWrite(FRONTAL_LED, LOW);
}

void lightRandomLED() {
  resetLEDs();
  if (random(2)) {
    digitalWrite(CEREBELLUM_LED, HIGH);
  }
  if (random(2)) {
    digitalWrite(OCCIPITAL_LED, HIGH);
  }
  if (random(2)) {
    digitalWrite(PARIETAL_LED, HIGH);
  }
  if (random(2)) {
    digitalWrite(TEMPORAL_LED, HIGH);
  }
  if (random(2)) {
    digitalWrite(FRONTAL_LED, HIGH);
  }
}

int x_min = -8, x_max = 8, x = -8;
int text_min = -80, text_max = 8, text = -80;
long magnetTimer = 0;
void loop() {
  bool magnetPresent = false;
  if (x >= x_max) {
    x = x_min;
  }
  x++;

  if (text <= text_min) {
    text = text_max;
  }
  text--;

  matrix.clear();
  // matrix.setCursor(x, 0);
  // matrix.print("Hello");
  // matrix.drawBitmap(0, x, waveform01_bmp, 8, 8, LED_GREEN);

  int cerebellum = analogRead(CEREBELLUM_SENSE);
  int occipital = analogRead(OCCIPITAL_SENSE);
  int parietal = analogRead(PARIETAL_SENSE);
  int temporal = analogRead(TEMPORAL_SENSE);
  int frontal = analogRead(FRONTAL_SENSE);
  int secret = analogRead(SECRET_SENSE);

  int cer_val = abs(cerebellum - cerebellum_offset);
  int occ_val = abs(occipital - occipital_offset);
  int par_val = abs(parietal - parietal_offset);
  int tem_val = abs(temporal - temporal_offset);
  int fro_val = abs(frontal - frontal_offset);

  if (cer_val > sense_std) {
    if (!magnetPresent) {
      digitalWrite(CEREBELLUM_LED, HIGH);
      matrix.drawBitmap(0, x, waveform00_bmp, 8, 8, LED_GREEN);
      magnetPresent = true;
    }
  } else {
    digitalWrite(CEREBELLUM_LED, LOW);
  }
  if (occ_val > sense_std) {
    if (!magnetPresent) {
      digitalWrite(OCCIPITAL_LED, HIGH);
      matrix.drawBitmap(0, x, waveform01_bmp, 8, 8, LED_GREEN);
      magnetPresent = true;
    }
  } else {
    digitalWrite(OCCIPITAL_LED, LOW);
  }
  if (par_val > sense_std) {
    if (!magnetPresent) {
      digitalWrite(PARIETAL_LED, HIGH);
      matrix.drawBitmap(0, x, waveform02_bmp, 8, 8, LED_RED);
      magnetPresent = true;
    }
  } else {
    digitalWrite(PARIETAL_LED, LOW);
  }
  if (tem_val > sense_std) {
    if (!magnetPresent) {
      digitalWrite(TEMPORAL_LED, HIGH);
      matrix.drawBitmap(0, x, waveform03_bmp, 8, 8, LED_YELLOW);
      magnetPresent = true;
    }
  } else {
    digitalWrite(TEMPORAL_LED, LOW);
  }
  if (fro_val > sense_std) {
    if (!magnetPresent) {
      digitalWrite(FRONTAL_LED, HIGH);
      matrix.drawBitmap(0, x, waveform04_bmp, 8, 8, LED_RED);
      magnetPresent = true;
    }
  } else {
    digitalWrite(FRONTAL_LED, LOW);
  }

  if (magnetPresent) {
    magnetTimer = millis();
  } else {
    if (millis() - magnetTimer > 3000) {
      matrix.setRotation(2);
      matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
      matrix.setTextColor(LED_GREEN);
      matrix.setCursor(text, 0);
      Serial.println(text);
      matrix.print("BRAIN GAME");
      matrix.setRotation(3);  // reset
      if (text % 10 == 0) {
        lightRandomLED();
      }
    }
  }
  matrix.writeDisplay();
  delay(50);
  // matrix.clear();
  // matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_GREEN);
  // matrix.writeDisplay();
  // lightRandomLED();
  // delay(500);

  // matrix.clear();
  // matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_YELLOW);
  // matrix.writeDisplay();
  // lightRandomLED();
  // delay(500);

  // matrix.clear();
  // matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_RED);
  // matrix.writeDisplay();
  // lightRandomLED();
  // delay(500);

  // matrix.clear();
  // matrix.drawRect(0, 0, 8, 8, LED_RED);
  // matrix.fillRect(2, 2, 4, 4, LED_GREEN);
  // matrix.writeDisplay();  // write the changes we just made to the display
  // lightRandomLED();
  // delay(500);

  // matrix.clear();
  // matrix.drawCircle(3, 3, 3, LED_YELLOW);
  // matrix.writeDisplay();  // write the changes we just made to the display
  // lightRandomLED();
  // delay(500);

  // matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  // matrix.setTextColor(LED_GREEN);
  // positive number shifts left
}

float calculateStandardDeviation(int arr[], int size) {
  float mean = 0;
  float sum = 0;
  float stdDev = 0;

  // Calculate the mean
  for (int i = 0; i < size; i++) {
    mean += arr[i];
  }
  mean /= size;

  // Calculate the sum of squared differences from the mean
  for (int i = 0; i < size; i++) {
    sum += pow(arr[i] - mean, 2);
  }

  // Calculate the standard deviation
  stdDev = sqrt(sum / size);

  return stdDev;
}

float calculateMean(int arr[], int size) {
  float sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum / size;
}
