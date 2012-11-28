void setup(){
  Serial.begin(9600);
//  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.print("Waiting for sync message");
  Serial.print("\n");
  
  //PIN SETUP
  pinMode(sunPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  
  
//----------------------------   
//----------IDOZITES----------
//----------------------------  

  while(timeStatus()!= timeSet){
    Serial.print("Waiting for sync message");
    Serial.print("\n");
    if(Serial.available() ){
      processSyncMessage();
    }
    Alarm.delay(1000);
  }
  //Debug
//  Alarm.timerOnce(10, joreggelt);
  Alarm.timerRepeat(30, debugLog);
  Alarm.timerOnce(20, sensorReadDS);

  //LIGHT
  //Csak a bekapcsolást időzítjük, a kikapcsolást a lightOn fügvényből hívjuk meg.
  if (starter(24*3600, sunSet, sunRise*3600)){  //starter(long period, long duration, long offset){ if ( (now()-offset) % period < duration){
    lightStart(86400, 43200, 54000);
  }
  Alarm.alarmRepeat(sunRise,0,0,lightOn);
  Serial.print("sunSet: ");
  Serial.println(sunSet, DEC); 

  
  //WATER PUMP
  //Csak a bekapcsolást időzítjük, a kikapcsolást a pumpOn fügvényből hívjuk meg.
  Alarm.timerRepeat(pumpTime, pumpOn);
  
  //TEMPERATURE
  Alarm.timerRepeat(sensorReadTime, sensorReadDS);
  
  
}

void loop(){
  if(Serial.available() ) 
  {
    processSyncMessage();
  }
  
  if(timeStatus()!= timeSet){
    Serial.print("Waiting for sync message");
    Serial.print("\n");
  }
  Alarm.delay(1000);
}
//--------------------
//------STARTER-------
//--------------------
/*
Eldonti be kell-e legyen eppen kapcsolva valami.
period= a periodus hossza, mennyi 
    idonkent kapcsol be ujra
duration= mennyi ideig van bekapcsolt allapotban
offset= mennyi az eltolodas 0 ora 0 perchez kepest

He eppen be kel legyen kapcsolva visszater 1, ha nem, 0
*/
boolean starter(long period, long duration, long offset){
  if ( (now()-offset) % period < duration){
    return 1;
  }
  else{
    return 0;
  }
}

