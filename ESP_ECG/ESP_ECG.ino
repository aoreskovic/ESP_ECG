
#include <Wire.h>
#include <Ticker.h>
#include "SSD1306Wire.h"

SSD1306Wire  display(0x3c, 5, 4);



#define NUM_SAMPLES 40
#define BUZZ_PIN 12

#define RUNNING_AVG 128

#define PLOT_Y_H 64
#define PLOT_Y_L 20


int sensorPin = 39;
int sensorValue[NUM_SAMPLES] = {};
int outputValue[1000] = {};
int i = 0;
int n = 0;
int avg = 0;
int sum = 0;
int runningAverage = 0;
float bias = 1800;

int sumsum = 0;

Ticker sampler;

Ticker tickerBPM;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  display.init();

  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.setColor(WHITE);

  pinMode(BUZZ_PIN, OUTPUT);

  sampler.attach_ms(1, sampleAndCalculate);
  tickerBPM.attach_ms(6000, calculateBPM);
}




int pulseCounter = 0;
int pulseCounterOld = 0;
int BPM = 60;
int BPMOld = 60;
void calculateBPM(void) {
  BPM = 5 * (pulseCounter + pulseCounterOld);
  BPM = (BPM + BPMOld) / 2;
  BPMOld = BPM;
  pulseCounterOld = pulseCounter;
  pulseCounter = 0;
}

unsigned long currentMillis = 0, previousMillis = 0;

bool beat = false;
bool beatOld = false;

void buzz(int val, int avg) {
  int interval = 100;
  int treshold;
  treshold = avg * 1.4;


  currentMillis = millis();
  if (val > treshold) {
    digitalWrite(BUZZ_PIN, HIGH);
    previousMillis = currentMillis;
    beatOld = beat;
    beat = true;
  }


  if (currentMillis - previousMillis >= interval) {

    digitalWrite(BUZZ_PIN, LOW);
    beatOld = beat;
    beat = false;
  }

  if ((beatOld == true) && (beat == false)) {
    pulseCounter++;
  }


}


void sampleAndCalculate() {
  sensorValue[i] = analogRead(sensorPin);
  sum += sensorValue[i];
  i++;

  avg = sum / NUM_SAMPLES;


  if (i == NUM_SAMPLES ) {

    i = 0;
    sum = 0;

    bias = (20*bias + avg)/21.0;
    outputValue[n] = avg;

    for (int k = 0; k < RUNNING_AVG; k++) {
      sumsum += outputValue[k];

    }
    runningAverage = sumsum / RUNNING_AVG;
    sumsum = 0;

    n++;
    if (n == RUNNING_AVG)
      n = 0;

    buzz(avg, runningAverage);
    Serial.print(bias);
    Serial.print(" ");
    
    Serial.println(avg);

  }
}




void plotGraph(void) {
  int maxVal = 0;
  int minVal = 10000;

  for (int i = 0; i < 128; i++) {
    maxVal = max(maxVal, outputValue[i]);
    minVal = min(minVal, outputValue[i]);
  }

  int lastVal = map(outputValue[0], minVal-1, maxVal+1, PLOT_Y_H, PLOT_Y_L);
  int val = 1500;


  for (int i = 0; i < 128; i++) {
    val = map(outputValue[i], minVal-1, maxVal+1, PLOT_Y_H, PLOT_Y_L);
    display.drawVerticalLine(i, val, 1 + max(val, lastVal) - min(val, lastVal));
    lastVal = val;
  }

}


void loop() {
  display.clear();
  plotGraph();
  String tekst = "BPM: ";
  tekst = tekst + String(BPM);
  display.drawString(0, 0, tekst);
  display.display();
  delay(20);
}
