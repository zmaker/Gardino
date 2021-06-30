/*
 * trasmette - un bit
* https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
* https://github.com/nRF24/RF24
* 
* SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 6); // CE, CSN
const byte address[6] = "00001";

#include <SD.h>
File myFile;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(100);
  }

  delay(200);

  pinMode(2, OUTPUT); //SS SD
  pinMode(6, OUTPUT); //CNS NRF
  pinMode(7, OUTPUT); //CE NRF
  
  Serial.print("SD init: ");
  
  if (!SD.begin(2)) {
    Serial.println("error");
    //while (1);
  } else {
    sdread();   
    //digitalWrite(2, LOW); //disabilito SD 
    Serial.println("ok");    
  }

  delay(200);
  
  Serial.println("Radio ");
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.print("OK");
}

int count = 0;

void loop() {  
  static int stato = 0;
  //trasmette 0 e 1
  Serial.print(stato);
  radio.write(&stato, sizeof(stato));
  Serial.println(".");
  delay(1000);
  stato = !stato;
  count++;

  if ((count % 10) == 0) {
    sdread();
  }
}

void sdread() {   
  myFile = SD.open("test3.txt");
  if (myFile) {
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening file");
  }   
}
