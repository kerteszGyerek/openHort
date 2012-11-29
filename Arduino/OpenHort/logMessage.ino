//------------------------------
//----------LOGMESSAGE----------
//------------------------------

/*A logmassage-ekben küldjük a szenzorértékeket, és a beavatkozó szervek állapotváltozásait,
a hozzájuk tartozó időponttal. Az adatok további feldolgozása érdekében, pontosvesszővel elválasztott
adatokat küldünk, egy időponthoz egy sor tartozik.
A webes adatbazisba wtf:value alakban kerülnek kuldesre az adatok.

A csv nagyon kenyelmes ssh-n terminalban, az egyszerusitett uzenetek kezelese kenyelmes bash-ban es php-ban.
Ezert egyenlore mind a ketto mukodik.*/

char* logMsg[]={"SensorNum1","Temperature1","SensorNum2","Temperature2","SensorNum3","Temperature3","pin1","WTF1","Status1","pin2","WTF2","Status2"};
const byte logMsgLen = sizeof(logMsg)/sizeof(char*);  //Ez igy mukodik. Nem tudom miért. Ha nincs ott a /sizeof(char*) ketszeres ertekekkel ter vissza.


//-------------------------------
//-----SZERKESZTO FUGVENYEK------
//-------------------------------

/*Naplozando esemenyekkor, az esemeny megtortentekor meghivott fugvenyek.
Atirjak a rajuk vonatkozo reszt a logMsg tombben, majd elinditjak a kuldo fugvenyt logMessageSennder.*/

void dBaseActor(byte x, byte y){
  //wtf:value alakban kikuldi a parameterkent kapott tagjait a logMsg[] nek. Ez megy az actor adatbazisba.
  Serial.print(logMsg[x]);
  Serial.print(":");
  Serial.print(logMsg[y]);
  Serial.print("\n");
}

void dBaseSensor(byte x, byte y){
  //value:value alakban kikuldi a parameterkent kapott tagjait a logMsg[] nek. Ez megy a szenzor adatbazisba.
  Serial.print(logMsg[x]);
  Serial.print(":");
  Serial.print(logMsg[y]);
  Serial.print("\n");
}

void lightOn_Log(){
  itoa(sunPin,logMsg[6],10); //pin1 number
  logMsg[7]="Vilagitas";     //wtf1
  logMsg[8]="1";            //ststus1
  dBaseActor(7,8);
  logMessageSender();
}

void lightOff_Log(){
  itoa(sunPin,logMsg[6],10); //pin1 number
  logMsg[7]="Vilagitas";     //wtf1
  logMsg[8]="0";
  dBaseActor(7,8);
  logMessageSender();
}

void lightStart_Log(){
  Serial.print("Villany fel!");
  Serial.print("\n");
}

void pumpOn_Log(){
  itoa(pumpPin,logMsg[9],10);
  logMsg[10]="Pump";
  logMsg[11]="1";
  dBaseActor(10,11);  
  logMessageSender();
}

void pumpOff_Log(){
  logMsg[10]="Pump";
  logMsg[11]="0";
  dBaseActor(10,11);
  logMessageSender();
}
void sensorReadDS_Log(){
  
  //1.szenzor
  logMsg[0]="1.Temp";
  dtostrf(T[0],5, 2, logMsg[1]);    //dtostrf(float val, meret, tizedes jegyek szama,string) http://dereenigne.org/electronics/arduino/arduino-float-to-string
  
  //2. szenzor
  logMsg[2]="2.Temp";
  dtostrf(T[1],5, 2, logMsg[3]);
  dBaseSensor(1,3);
  
  //3. szenzor
//  logMsg[4]="3.Temp";
//  dtostrf(0,5, 2, logMsg[5]);

  
  logMessageSender();
}

void logMessageSender(){
  /*Kuldo fugveny
  eloszor kiirjuk a pontos idot
  majd a forciklusban kiprinteli az elemeket, ; vel elvalasztva*/
  ido();  //debug tabon
  Serial.print(";");
  for (byte i = 0;i<logMsgLen; i++){
    Serial.print(logMsg[i]);
    Serial.print(";");
  }
  Serial.print("\n");
}
