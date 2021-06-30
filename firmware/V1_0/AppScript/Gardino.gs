/**
 *
 - Create a shared/public calendar called "arduino" and add events named "ZONA1". Events duration control activation time
 
 - Nel calendario chiamato "arduino" cerca eventi con titolo "ZONA1" 
    Usa la durata per attivare l'irrigazione
 */

var _calendarName = 'arduino';
var _time_offset = 60*60; //finestra di ricerca eventi in secondi (1h)

function doGet() {
  return ContentService.createTextOutput(getOutput());
}

function getOutput() {
  return JSON.stringify(checkEvents());
}

function checkEvents(){
  var cal = CalendarApp.getCalendarsByName(_calendarName)[0];
  console.log("calname: " + _calendarName);
  if (!cal) {
    return {status: 'error', message: 'Error, calendar "' + calendarName + '" does not exist.'};
  }
  
  var t1 = new Date();
  var t2 = new Date();  
  t2.setSeconds(t1.getSeconds() + _time_offset);
  
  console.log("t1: " + t1);
  console.log("t2: " + t2);

  var items = cal.getEvents(t1, t2);
  var length = items.length;
  var response = {};
    
  console.log("eventi trovati: " + length);
  
  if (length){
    //il primo evento:
    ev1 = items[0];
    var title = ev1.getTitle();
    var startTime = ev1.getStartTime();
    var endTime = ev1.getEndTime();

    /*var pin = 0;
    if (title.length > 0) {
      token = title.split(" ");
      pin = token[1];
    }
    */

    console.log("evt: " + items[0].getTitle());   
    console.log(startTime +" - " + endTime);   

    var statopin = 0;
    if ((t1 >= startTime) && (t1 < endTime)) {
      statopin = 1;
    }
    //restituisce info se la zona è attiva e quale zona dobbiuamo attivare
    //per ora è la scheda Master che gestisce le zone e sa quali satelliti chiamare
    response = {zona: title, state: statopin};

  } else {
    response = {zona: 'OFF', state: 0};
  }
  return {status: 'success', data: response};
  
}