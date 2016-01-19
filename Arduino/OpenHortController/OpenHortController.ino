/*OpenHort Controller for arduino
 Copyright (C) 2012; Ákos Kerékgyártó;

License (English)

 This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact: Ákos Kerékgyártó <gijnka@mailbox.hu>


License (Magyar)

 Ez a program szabad szoftver; terjeszthető illetve módosítható a 
Free Software Foundation által kiadott GNU General Public License
dokumentumában leírtak; akár a licenc 2-es, akár (tetszőleges) későbbi 
változata szerint.

Ez a program abban a reményben kerül közreadásra, hogy hasznos lesz, 
de minden egyéb GARANCIA NÉLKÜL, az ELADHATÓSÁGRA vagy VALAMELY CÉLRA 
VALÓ ALKALMAZHATÓSÁGRA való származtatott garanciát is beleértve. 
További részleteket a GNU General Public License tartalmaz.

A felhasználónak a programmal együtt meg kell kapnia a GNU General 
Public License egy példányát; ha mégsem kapta meg, akkor
ezt a Free Software Foundationnak küldött levélben jelezze 
(cím: Free Software Foundation Inc., 59 Temple Place, Suite 330Boston, MA 02111-1307, USA.)


Rendszerlerás:

Az arduino a rooter felol kapja unixtime formatumban, egy "T" fejleccel ellatva 
a pontos idot. A TimeAlarms library segitsegevel vannak az egyes feladatok utemezve.
A loop csupan arra figyel, hogy ha elmaszna az ido, kezdemenyezze a szinkronizaciot.

Jelenleg 4 funkciot lat el a program.
Bekapcsolja a lampat, a pumpat, meri a homersekletet, es ezt naplozza.

Amikor valamelyik Alarm vagy Timer lejar, meghivodik a beallitott fugveny,
mely futasa vegen beallitja a kikapcsolas idejet, es elkuldi a log-ot.

Minden fugveny, melyet logolni kell, sajat log fugvennyel rendelkezik.
Eredetileg minden be volt irva a logMsg[] tombbe, es a log egy sor csv volt.
Jelenleg inkabb kulon kerulnek kuldesre az adatok.

A Serial.println() hatasara egy inverz W lesz minden sor vegen az OpenWRT-n.
Helyette Serial.print("\n")
*/

#include <Time.h>
#include <TimeAlarms.h>
#include <OneWire.h>


//---------------------------------------   
//----------VALTOZOK KONSTANSOK----------
//---------------------------------------  


//Time syncron
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
//#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
#define timeRef 0

//LIGHT
const int dayLenght=12;  //[h]

// const int sunRise=15;  //[h]
const int sunRise=15;  //[h] # kx + nona
unsigned long sunSet=12*3600;  //[s] Az a Alarm.timerRepeat() sec.-ben kéri az idot, nem oraban 
#define sunPin 4

//TEMPERATURE
#define sensorReadTime 1800    //szenzor beolvasas [s]
//DS1820 szenzor beolvasásához szükséges változók:
byte i;
byte present = 0;
byte data[12];
byte addr[8];    //Szenzor címet tároló változó

OneWire  ds(10);    //10-es pint előkészíti a szenzorok kezelésére

byte s0;    //Szenzorlánc aktuális tagja 
float T[2];  //Szenzorértékek tárolása
const byte TLen = sizeof(T);

//CHORES
#define choresPin 6 
#define choresTime 3600 //chores kapcsolgatas [s] FIXME
#define choresLength 300 //chores ennyi ideig megy [s] FIXME 

//------------------------------
//----------LOGMESSAGE----------
//------------------------------

/*A logmassage-ekben küldjük a szenzorértékeket, és a beavatkozó szervek állapotváltozásait,
a hozzájuk tartozó időponttal. Az adatok további feldolgozása érdekében, pontosvesszővel elválasztott
adatokat küldünk, egy időponthoz egy sor tartozik.
A webes adatbazisba wtf:value alakban kerülnek kuldesre az adatok.

A csv nagyon kenyelmes ssh-n terminalban, az egyszerusitett uzenetek kezelese kenyelmes bash-ban es php-ban.
Ezert egyenlore mind a ketto mukodik.*/

char* logMsg[]={"SensorNum1","Temperature1","SensorNum2","Temperature2","SensorNum3","Temperature3","pin1","WTF1","Status1","pin2","WTF2","Status2","pin3", "WTF3", "Status3"};
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

void choresStart_Log(){
  Serial.print("Chores fel!"); //FIXME
  Serial.print("\n");
}
void choresOn_Log(){
  itoa(choresPin,logMsg[13],14); // FIXME logMsg, 10, 9 ?
  logMsg[13]="Chores";
  logMsg[14]="1"; // FIXME: 10, 11, ???
  dBaseActor(13,14);
  logMessageSender();
}
void choresOff_Log(){
  logMsg[13]="Chores";
  logMsg[14]="0"; // FIXME: 10, 11, ??
  dBaseActor(13,14);
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

void setup(){
  Serial.begin(9600);

  pinMode(sunPin, OUTPUT);
  pinMode(choresPin, OUTPUT);
  
  
//----------------------------   
//----------IDOZITES----------
//----------------------------  

  while(timeStatus()!= timeSet){
    //Az alarmok nem fognak rendesn mukodni, ha nincs szinkronizalva az ido, ezzel kell kezdeni.
    Serial.print("Waiting for sync message");
    Serial.print("\n");		//A println kuld egy "\r"-t is amit az openWrt nem kezel le, es ezert kell kulon lezarni a sorokat
    if(Serial.available() ){
      processSyncMessage();
    }
    Alarm.delay(1000);
  }
  //Debug
  Alarm.timerRepeat(2317, debugLog);  //kikuld egy logMsg-t
  Alarm.timerOnce(1, choresOn);
  Alarm.timerOnce(20, sensorReadDS);  //leolvassa a homeroket indulas utan

  //LIGHT
  //Csak a bekapcsolást időzítjük, a kikapcsolást a lightOn fügvényből hívjuk meg.
  if (starter(24*3600, sunSet, sunRise*3600)){  //starter(long period, long duration, long offset){ if ( (now()-offset) % period < duration){
    lightStart(86400, 43200, 54000);  /*Csak a time_t tipusu ertekek lennenek eleg nagyok,
    hogy ne keletkezzen tulcsordulas, ezert vannak a szamok kozvetlenul beirva a forrasba.*/
  }
  Alarm.alarmRepeat(sunRise,0,0,lightOn);  //Altalanos idozites beallitas

  //CHORES
  Alarm.timerRepeat(choresTime, choresOn); 
  
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
    Serial.print("Waiting for sync message");
    Serial.print("\n");
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

//--------------------------
//----------ACTION----------
//--------------------------


//LIGHT
void lightOn(){
  //világitást kapcsolja be, elküldi az időpontot és a státuszt
  digitalWrite(sunPin, HIGH);
  lightOn_Log();
  Alarm.timerOnce(43200, lightOff);  //kikapcsolás hívása
}
void lightOff(){
  //világytást kapcsolja ki, elküldi az időpontot és a státuszt
  digitalWrite(sunPin, LOW);
  lightOff_Log();
}



void lightStart(long period, long duration, long offset){
  //világitást kapcsolja be, elküldi az időpontot és a státuszt
//  Serial.print(period, DEC);
//  Serial.print("\n");
//  Serial.print(duration, DEC);
//  Serial.print("\n");
//  Serial.print(offset, DEC);
//  Serial.print("\n");
  digitalWrite(sunPin, HIGH);
  lightStart_Log();
  lightOn_Log();
//  Serial.print(now(), DEC);
//  Serial.print("( now() - timeRef - offset )= ");
//  Serial.print(( now() - timeRef - offset ), DEC);
//  Serial.print("\n");
//  Serial.print("   %period= ");
//  Serial.print(( now() - timeRef - offset ) % period, DEC);
  long t = duration-(( now() - timeRef - offset ) % period);    //Ennyi id mulva kell kikapcsolni a lampat
//  Serial.print("NO: ");
//  Serial.print(t, DEC);
  Alarm.timerOnce(t, lightOff);  //kikapcsolás hívása
}

//TEMPERATURE
void sensorReadDS(){
  s0=0;
  while ( ds.search(addr)) {    //amig talal a program ds eszkozt, amit beir az addr valtozoba
    
    
//    Serial.print("ROM =");    //ezkoz cimet kiirja
//    for( i = 0; i < 8; i++) {
//      Serial.write(' ');
//      Serial.print(addr[i], HEX);
//    }
//  
//    if (OneWire::crc8(addr, 7) != addr[7]) {
//        Serial.print("CRC is not valid!");
//        Serial.print("\n");
//        return;
//    }
   
    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end
    
    Alarm.delay(1500);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
    
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
  
//    Serial.print("  Data = ");
//    Serial.print(present,HEX);
//    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
//      Serial.print(data[i], HEX);
//      Serial.print(" ");
    }
//    Serial.print(" CRC=");
//    Serial.print(OneWire::crc8(data, 8), HEX);
//    Serial.print();
//    Serial.print("\n");
  
    // convert the data to actual temperature
  
    unsigned int raw = (data[1] << 8) | data[0];
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    } 
    T[s0] = (float)raw / 16.0;
//    Serial.print("T[");
//    Serial.print(s0, DEC);
//    Serial.print("]  = ");
//    Serial.print(T[s0]);
//    Serial.print(" Celsius, ");
//    Serial.print("\n");
    s0=s0+1;
  }
  
//  Serial.print("No more addresses.");
//  Serial.print("\n");
//  Serial.print("\n");
  ds.reset_search();
  
  Alarm.delay(250);
  sensorReadDS_Log();
  return;  
}

//TAKARITAS
void choresOn(){
  //FIXME kapcsolja be, elküldi az időpontot és a státuszt
  digitalWrite(choresPin, HIGH);
  choresOn_Log();
  Alarm.timerOnce(choresLength, choresOff);  //kikapcsolás hívása
}
void choresOff(){
  //FIXME kapcsolja ki, elküldi az időpontot és a státuszt
  digitalWrite(choresPin, LOW);
  choresOff_Log();
}



void choresStart(long period, long duration, long offset){
  //FIXME kapcsolja be, elküldi az időpontot és a státuszt
//  Serial.print(period, DEC);
//  Serial.print("\n");
//  Serial.print(duration, DEC);
//  Serial.print("\n");
//  Serial.print(offset, DEC);
//  Serial.print("\n");
  digitalWrite(choresPin, HIGH);
  choresStart_Log();
  choresOn_Log();
//  Serial.print(now(), DEC);
//  Serial.print("( now() - timeRef - offset )= ");
//  Serial.print(( now() - timeRef - offset ), DEC);
//  Serial.print("\n");
//  Serial.print("   %period= ");
//  Serial.print(( now() - timeRef - offset ) % period, DEC);
  long t = duration-(( now() - timeRef - offset ) % period);    //Ennyi id mulva kell kikapcsolni a FIXME
//  Serial.print("NO: ");
//  Serial.print(t, DEC);
  Alarm.timerOnce(t, choresOff);  //kikapcsolás hívása
}


//---------------------------------------
//----------TIME SYNCRONISATION----------
//---------------------------------------


void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
//    Serial.print(c);
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }  
  }
}

//time_t requestSync()
  //Ettol teljesen megkergul, jobb nem kiengedni a pecset alol
//{
//  Serial.print("Waiting for sync message");
//  Serial.print("\n");
//  Alarm.delay(1000);
//  return 0; // the time will be sent later in response to serial mesg
//}

void joreggelt(){
  Serial.print("Most kezdodik a buli:)");
  Serial.print("logMsgLen: ");
  Serial.print(logMsgLen,DEC);
  Serial.print(", sunPin: ");
  Serial.print(sunPin,DEC);
  Serial.print("\n");
}
void ido(){
  Serial.print(day());
  Serial.print("-");
  Serial.print(hour());
  Serial.print("-");
  Serial.print(minute());
  Serial.print("-");
  Serial.print(second());
//  Serial.print("\n");
  return;
}

void debugLog(){
  logMessageSender();
}

//-----------------
//------KONYEC-----
//-----------------
