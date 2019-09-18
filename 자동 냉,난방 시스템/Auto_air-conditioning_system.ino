#include "DHT.h"      
#define DHTPIN 2      
#define DHTTYPE DHT11 
#include <IRremote.h> 
IRsend irsend;
DHT dht(DHTPIN, DHTTYPE);
int count = 0;

void setup() {
Serial.begin(9600);    
}
void loop() {
int temperature = dht.readTemperature(); 
if(count !=1){
    if(temperature > 기준온도){
        irsend.sendNEC(0xFFA50F,32);//change data type,data,bit && turn on code
        count ++;
    }
}else if(count == 1){
    if(temperature <= 기준온도){
        irsend.sendNEC(0xFFA50F,32);//change data type,data,bit && turn off code
        count = 0;
    }
}
}