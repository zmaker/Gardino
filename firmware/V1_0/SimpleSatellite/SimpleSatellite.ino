#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup() {
  pinMode(4, OUTPUT); //rele
  
  Serial.begin(9600);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

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
