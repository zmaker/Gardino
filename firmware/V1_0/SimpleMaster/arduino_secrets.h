//debug level: 0 off, 1 info, 2 debug (verbose)
#define DEBUG_GARDINO 2

#define SCHEDULING_TIME_S 60

#define SECRET_SSID "www.reelco.it"
#define SECRET_PASS "cardu450IX"
#define GOOGLE_SERVER "script.google.com"
#define GOOGLE_SERVICE "/macros/s/AKfycbzdZoaGB5S2Php5EFiXFx2G_4bIIquCkGuJo9SZXOrnAvMmmE-7jQflaPleVkByKEV4/exec"

#define NRF_MASTER_ADDR "00001"

void info(String msg){
  #if defined(DEBUG_GARDINO) && (DEBUG_GARDINO >= 1)
  Serial.print(msg);  
  #endif
}

void debug(String msg){
  #if defined(DEBUG_GARDINO) && (DEBUG_GARDINO == 2)
  Serial.print(msg);  
  #endif
}
