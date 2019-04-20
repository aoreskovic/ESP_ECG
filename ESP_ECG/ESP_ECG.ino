
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

SSD1306Wire  display(0x3c, 5, 4);



#define NUM_SAMPLES 20
#define BUZZ_PIN 12

#define RUNNING_AVG 100

int sensorPin = 39;    // select the input pin for the potentiometer      // select the pin for the LED
int sensorValue[NUM_SAMPLES] = {};  // variable to store the value coming from the sensor
int outputValue[1000] = {};
int i = 0;
int n = 0;
int avg = 0;
int sum = 0;
int runningAverage = 0;

int sumsum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  pinMode(BUZZ_PIN, OUTPUT);
}


void drawTextAlignmentDemo() {
    // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 33, "Right aligned (128,33)");
}




unsigned long currentMillis = 0, previousMillis = 0;

void buzz(int val, int avg){
  int interval = 100;
  int treshold;
  treshold = avg * 1.2;


  
  currentMillis = millis();
  if(val > treshold){
    digitalWrite(BUZZ_PIN, HIGH);
    previousMillis = currentMillis;
  }

  currentMillis = millis();
   
  if (currentMillis - previousMillis >= interval) {
    
    digitalWrite(BUZZ_PIN, LOW);
  }

}


void loop() {
  // read the value from the sensor:
  sensorValue[i] = analogRead(sensorPin);
  sum += sensorValue[i];
  i++;

  
  avg = sum/NUM_SAMPLES;
  
  delay(1);

  if(i == NUM_SAMPLES ){
    
    i = 0;
    sum = 0;

    outputValue[n] = avg;

    for(int k = 0; k < RUNNING_AVG; k++){
      sumsum += outputValue[k];
      
    }
    runningAverage = sumsum/RUNNING_AVG;
    sumsum = 0;
    
    n++;
    if(n == RUNNING_AVG)
      n = 0;

    buzz(avg,runningAverage);
    Serial.print(avg);
    Serial.print(" ");
    Serial.println(runningAverage);
  }
}
