/*OpenHort vezerlorendszer.
Arduino 1.0.1
A program 5 nagyobb egysegre oszlik. 
Ezen a tabon vannak a konstansok es a fontosabb valtozok temakoronkent csoportositva.
A logMessage tabon az uzenetkuldes, Action tabon a vegrehajtas,
Timesync-en az idoszinronizacio.

Az arduino a rooter felol kapja unixtime formatumban, egy "T" fejleccel ellatva 
a pontos idot. A TimeAlarms library segitsegevel vannak az egyes feladatok utemezve.
A loop csupan arra figyel, hogy ha elmaszna az ido, kezdemenyezze a szincronizaciot.

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

const int sunRise=15;  //[h]
unsigned long sunSet=12*3600;  //[s] Az a Alarm.timerRepeat() sec.-ben kéri az idot, nem oraban 
#define sunPin 4

//WATER
#define pumpTime 60  //Ennyi idonkent kapcsol be a pumpa [s]
#define pumpLenght 20  //Ennyi ideig megy a pumpa [s]
#define pumpPin 3

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


//setup és loop másik tabon












