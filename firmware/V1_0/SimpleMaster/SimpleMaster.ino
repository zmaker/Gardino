/**
 * Gardino - Master Board
 * Use: MKR1010 WiFi
 * 
 * ver: 1.0
 * Read google calendar and check for "ZONA 1" event. 
 * If event present:
 *   - activate master relè 
 *   - activate satellite relè (for zona 1)
 */
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID; 
char pass[] = SECRET_PASS; 
char server[40] = GOOGLE_SERVER;   
String path = GOOGLE_SERVICE;

unsigned long t1, dt;
int keyIndex = 0;          
int status = WL_IDLE_STATUS;

WiFiSSLClient client;
StaticJsonDocument<200> doc;

//NRF24 setup
RF24 radio(7, 6); // CE, CSN
const byte address[6] = NRF_MASTER_ADDR;

void setup() {  
  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  //Setup NRF24 module
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  info("NRF - Ready\n");

  //setup Wifi
  if (WiFi.status() == WL_NO_MODULE) {
    info("Communication with WiFi module failed!\nSTOPPED\n");    
    while (true);
  }

  while (status != WL_CONNECTED) {
    debug("Attempting to connect to SSID: ");
    debug(ssid);    
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  info("Connected to wifi\n");
}

int stato = 0;

void loop() {
  switch(stato){
    case 0:
      //waiting... RTC to call server
      waitcmd();     
      break;
    case 1:
      //call the service
      call();
      break;
    case 2:
      //decode server answer/data
      getHeader();
      break;
    case 3:
      //read data and take actions
      readdata();
      break;
    case 4:
      //delay      
      attesa();
      break;
    case 10:      
      error();
      break;
  }
}

boolean FIRST = true;

void go(int st){
  stato = st;
  FIRST = true;
  delay(300);
}

void waitcmd() {
  if (FIRST) {
    info("Press ENTER to start call\n");
    FIRST = false;
    t1 = millis();
  }

  if ((millis() - t1) == (SCHEDULING_TIME_S * 1000)) {
    debug(F("Calling server\n"));
    //go(1)
    t1 = millis(); //rescheduling
  }

  
  while (Serial.available()){
    char c = Serial.read();
    Serial.print(c);
    if (c == '\n') {      
      go(1);  
    }
  }
}

void call() {
  if (FIRST) {
    //debug(String("Calling..."+server+"\n"));
    //debug(path+"\n");    
    FIRST = false;
  }
  if (client.connect(server, 443)) {
    //debug("connected to server\n");
    // Make a HTTP request:
    client.println("GET "+path+" HTTP/1.1");
    client.println("Host: "+String(server));
    client.println("Connection: close");
    client.println();
    go(2);
  }
}

int requestCode;

void getHeader() {
  if (FIRST) {    
    FIRST = false;
  }

  if (client.available()) {
    String line = client.readStringUntil('\n');    
    
    if (line.indexOf("HTTP/1.1") >= 0) {   
      requestCode = getRequestCode(line);
      
    } else if ((requestCode == 302) && (line.indexOf("Location:") >= 0)) {
      int pos = line.indexOf(" ");
      String newaddr = line.substring(pos+1);
      debug("redirected to: ");
      debug(newaddr+"\n");
      int s = line.indexOf("//")+2;
      int f = line.indexOf("/", s) + 1;
      String str = line.substring(s, f);
      str.toCharArray(server, str.length());
      //debug("server="+server+"\n");      
      path = line.substring(f-1);
      //debug("path="+path+"\n");      
      
    } else if (line.indexOf("Connection: close") >= 0){      
      if (requestCode == 302) {
        //redirect  
        debug("follow redirect\n");
        client.flush();
        go(1);
      } else if (requestCode == 200){
        //leggo i dati  
        debug("read data\n");
        go(3);
      } else {
        //non gestito
        debug("unknow\n");
        client.flush();
      }
    }
  }
}

int getRequestCode(String line) {
  int n = 0;
  char ll[100] = ""; 
    line.toCharArray(ll, line.length());
    int i = 0;
    char* token = strtok(ll, " ");
    while (token != NULL) {
        i++;        
        token = strtok(NULL, " ");
        if (i == 1) {  //qui ho il codice della risposta
          n = atoi(token);
        }        
    }  
    return n;
}

int linea;
void readdata(){
  if (FIRST) {    
    Serial.println("Reading data from service");    
    FIRST = false;
    linea = 0;
  }
  if (client.available()) {
    String line = client.readStringUntil('\n');
    if (linea == 3) {
      Serial.println(line);  

      //analizzo i dati json
      char json[100];
      line.toCharArray(json, line.length());
      DeserializationError error = deserializeJson(doc, json);
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        go(10);
      } else {
        //fetch value
        /**
         * We will receive JSON in this format:
         * {"status":"success","data":{"zona":"OFF","state":0}}
         * 
         */
        const char* status = doc["status"];
        const char* zone = doc["data"]["zone"]; 
        const int pinValue = doc["data"]["state"];
        // Print values.
        Serial.print("status: ");
        Serial.println(status);
        Serial.print("zone: ");
        Serial.println(zone);
        Serial.print("state: ");
        Serial.println(pinValue);

        if (strcmp(status, "success") == 0) {
          //call satellite and set its action (ON/OFF)
          Serial.print("calling zone");          
          setZoneStatus(pinValue);
          Serial.print("calling zone: DONE");
        }        
        go(4);
      }          
    }    
    linea++; 
  }
}

void attesa(){
  if (FIRST) {    
    Serial.println("Wait...");    
    FIRST = false;    
    t1 = millis();     
  }
  dt = millis() - t1;
  if (dt >= 500) {
    Serial.println("Ready for a nwe call");    
    //eseguo una nuova chiamata
    go(0);  
  }
}

void error() {
  if (FIRST) {    
    Serial.println("Error - stopped");    
    FIRST = false;    
  }
}

void setZoneStatus(int state) {
  radio.write(&state, sizeof(state));  
}
