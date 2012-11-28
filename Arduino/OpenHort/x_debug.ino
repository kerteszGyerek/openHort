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
