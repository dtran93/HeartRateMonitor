// sd card
void writeToSd() {
  // Creating char array manually to save memory on Arduino
     counterSD++;
      char h[2];
      char t[2];
      char o[2];
      String str;
      str = String(counterSD / 100);
      str.toCharArray(h,2);
      str = String((counterSD / 10) % 10);
      str.toCharArray(t,2);
      str = String(counterSD % 10);
      str.toCharArray(o,2);
      
     char fileName[12];
     fileName[0] = 'D';
     fileName[1] = 'T';
     fileName[2] = 'R';
     fileName[3] = 'M';
     fileName[4] = h[0];
     fileName[5] = t[0];
     fileName[6] = o[0];
     fileName[7] = '.';
     fileName[8] = 't';
     fileName[9] = 'x';
     fileName[10] = 't';
     fileName[11] = '\0';
     Serial.println(fileName);
     myFile = SD.open(fileName, FILE_WRITE);
     
     char fileHeader[17];
     for(int i = 0; i < 11; i++) {
       fileHeader[i] = fileName[i];
     }
     
     fileHeader[11] = ',';
     fileHeader[12] = ' ';
     fileHeader[13] = '2';
     fileHeader[14] = '5';
     fileHeader[15] = '0';
     fileHeader[16] = '\0';

     myFile.println(fileHeader);
     int counterVal = 0;
     while(counterVal < bufferSize) {
         // end println
         if (counterVal % 8 == 7) {
           myFile.print(", ");
           myFile.println(beatsBuffer[counterVal]);
         // fence post
         }else if (counterVal % 8 == 0){
           myFile.print(beatsBuffer[counterVal]);
         // general
         }else {
           myFile.print(", ");
           myFile.print(beatsBuffer[counterVal]);
         }
         counterVal++;
     }
     if (counterVal % 8 != 0) {
       myFile.println();
     }
     Serial.println("HERE");
     myFile.println("EOF");
     myFile.close();
     Serial.println("HERE");
}
