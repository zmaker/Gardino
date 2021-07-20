/*
 * Ricevitore con led sul pin2 e ARDUINO NANO
 * 
* https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
* https://github.com/nRF24/RF24
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  pinMode(4, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  for (int i = 0; i < 10; i++) {
    digitalWrite(4, HIGH);
    delay(50);  
    digitalWrite(4, LOW);
    delay(50);  
  }
  Serial.println("pronto...");
}

void loop() {
  if (radio.available()) {
    int stato = 0;
    radio.read(&stato, sizeof(stato));   
    Serial.println(stato); 
    if (stato == 1) {
      
      digitalWrite(4, HIGH);
    } else {
      digitalWrite(4, LOW);
    }
  }
}
