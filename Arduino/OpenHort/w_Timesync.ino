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
