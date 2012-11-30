void setup(){
  Serial.begin(9600);
//  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
  
  //PIN SETUP
  pinMode(sunPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  
  
//----------------------------   
//----------IDOZITES----------
//----------------------------  

  while(timeStatus()!= timeSet){
    //Az alarmok nem fognak rendesn mukodni, ha nincs szinkronizalva az ido, ezzel kell kezdeni.
    Serial.println("Waiting for sync message");
    if(Serial.available() ){
      processSyncMessage();
    }
    Alarm.delay(1000);
  }
  //Debug
  Alarm.timerRepeat(2317, debugLog);  //kikuld egy logMsg-t
  Alarm.timerOnce(20, sensorReadDS);  //leolvassa a homeroket indulas utan

  //LIGHT
  //Csak a bekapcsolást időzítjük, a kikapcsolást a lightOn fügvényből hívjuk meg.
  if (starter(24*3600, sunSet, sunRise*3600)){  //starter(long period, long duration, long offset){ if ( (now()-offset) % period < duration){
    lightStart(86400, 43200, 54000);  /*Csak a time_t tipusu ertekek lennenek eleg nagyok,
    hogy ne keletkezzen tulcsordulas, ezert vannak a szamok kozvetlenul beirva a forrasba.*/
  }
  Alarm.alarmRepeat(sunRise,0,0,lightOn);  //Altalanos idozites beallitas

  
  //WATER PUMP
  //Csak a bekapcsolást időzítjük, a kikapcsolást a pumpOn fügvényből hívjuk meg.
  Alarm.timerRepeat(pumpTime, pumpOn);
  
  //TEMPERATURE
  Alarm.timerRepeat(sensorReadTime, sensorReadDS);
  
  
}

void loop(){
  /*Felugyeli, hogy ne masszon el az ido*/
  if(Serial.available() ) //Resen kell lenni, kulonben az ember konnyen 2042-ben talalhatja magat
  {
    processSyncMessage();  //lsd Timesync tabon.
  }
  
  if(timeStatus()!= timeSet){  //Ha az ido nincs kalibralva, kuncsorog a rootertol egy kis pontossagot.
    Serial.println("Waiting for sync message");
  }
  Alarm.delay(1000);  //Ebben a formaban kell a delay-t hasznalni a TimeAlarm miatt
}
//--------------------
//------STARTER-------
//--------------------
/*
Eldonti be kell-e legyen eppen kapcsolva valami.
Minden ertek sec-ben ertendo.
period= a periodus hossza, mennyi 
    idonkent kapcsol be ujra. Lampanal 24 óra, pumpanal 60s.
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

