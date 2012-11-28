//--------------------------
//----------ACTION----------
//--------------------------


//LIGHT
void lightOn(){
  //világitást kapcsolja be, elküldi az időpontot és a státuszt
  digitalWrite(sunPin, HIGH);
  lightOn_Log();
//  Serial.print("Naplemente ennyi sec múlva: ");
//  Serial.println(sunSet, DEC);
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

//WATER
void pumpOn(){
  //pumpat kapcsolja be, elküldi az időpontot és a státuszt
  digitalWrite(pumpPin, HIGH);
//  pumpOn_Log();    //Nem szemeteljuk tele percenkent 2 bejegyzessel a logot
  Alarm.timerOnce(pumpLenght, pumpOff);  //kikapcsolás hívása
}

void pumpOff(){
  //pumpat kapcsolja ki, elküldi az időpontot és a státuszt
  digitalWrite(pumpPin, LOW);
//  pumpOff_Log();
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
//        Serial.println("CRC is not valid!");
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
//    Serial.println();
  
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
//    Serial.println();
    s0=s0+1;
  }
  
//  Serial.println("No more addresses.");
//  Serial.println();
  ds.reset_search();
  
  Alarm.delay(250);
  sensorReadDS_Log();
  return;  
}

