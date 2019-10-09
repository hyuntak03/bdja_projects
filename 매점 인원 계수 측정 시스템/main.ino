#include <SimpleTimer.h>
#include <SPI.h>
#include <SD.h>

File math_file;
SimpleTimer timer;

unsigned long timeVal = 0;
unsigned long previousVal = 0;
int sensor_pin = A0;
int sensor_value_current;
int sensor_value_last;
int human_count = 0;
int last_value = 0;
int current_value = 0;
int sec = 0;
int min = 0;
int hour = 0;
int day = 0;
void setup() {
  timer.setInterval(600000, reset);
  pinMode(sensor_pin,INPUT);
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("Initializaion failed!");
    while(1);
  }
  Serial.println("Initialization done.");
}

void sd_write() {
  math_file = SD.open("MATH.txt", FILE_WRITE);
  if(math_file) {
    Serial.print("Writing...");
    math_file.print(day);
    math_file.print(": ");
    math_file.print(hour);
    math_file.print(": ");
    math_file.print(min);
    math_file.print(": ");
    math_file.print(sec);
    math_file.print("  ");
    math_file.println(human_count);
    math_file.close();
  }else {
    Serial.println("error opening file");
  }
}

void count_human() {
  sensor_value_current = analogRead(sensor_pin);
  current_value = sensor_value_current;
  if(current_value <= 100){
  if(last_value == current_value){
    Serial.println("No count");
  }else if(last_value != current_value){
    human_count ++;
    Serial.println(human_count);
  }
  }
  last_value = analogRead(sensor_pin);
}

void reset() {
  sd_write();
  human_count = 0;
}

void time() {
  timeVal = millis();

  if(timeVal - previousVal >=1000){
    previousVal = timeVal;
    sec ++;
  }
  if(sec >=60){
    sec = 0;
    min ++;
  }
  if(min >= 60){
    min = 0;
    hour ++;
  }
  if(hour >= 24){
    hour = 0;
    day ++;
  }
}
void loop() {
  time();
  timer.run();
  count_human();
  Serial.print(hour);
  Serial.print(": ");
  Serial.print(min);
  Serial.print(": ");
  Serial.println(sec);
}