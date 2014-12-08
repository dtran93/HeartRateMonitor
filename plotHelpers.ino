// ============================================= DISPLAYING =================================================

// Draws the red grid
void drawGrid() {
  //  Draw grid
  for (int i = 0; i < 35; i++){  
     if(i > 4 && i < 22) { 
       tft.drawLine(0, i * 10, 350, i * 10, ILI9341_RED);
     }
     tft.drawLine(i * 10, 45, i * 10, 220, ILI9341_RED);
  }
}

// Draws the initial graph
void drawECG(int color, int lowerBound, int higherBound) {
   for(int i = lowerBound; i < higherBound - 1; i++) {
       updateLine(color, beatsBuffer[i], beatsBuffer[i + 1], i);
    }
}

// Updates the graph
void updateLine(int color, int startLine, int endLine, int index) {
  // Dividing by 41 because 4096/100 is roughly 41
  startLine = startLine / 41;
  endLine = endLine / 41;
  if((index % 320) == 0) {
  } else {
    tft.drawLine((index - 1) % 320, startLine + translation, (index % 320), endLine + translation, color);
  }
}

// Plotting QSR Circles on the bottom of the display to show the accurate detection
void plotQSRonDisplay(int j) {
  tft.fillCircle(j % 320, 232, 5, ILI9341_WHITE); 
}

// To control movement of the graph after the button is pressed/30 seconds are over
void translateGraph() {
  if(currentMessage.equals("Press button to re-run") == 1) {
    int sensorValuePotentiometerNew = analogRead(A2); 
    if(sensorValuePotentiometerNew < 200) {
      sensorValuePotentiometerNew = 200;
    }
    int upperBound = (int) sensorValuePotentiometerNew * 1.83;
    int lowerBound = upperBound - 320;
    if(abs(sensorValuePotentiometerNew - sensorValuePotentiometer) > 200) {
      clearGraph();
      drawECG(ILI9341_BLACK, lowerBound, upperBound);
      sensorValuePotentiometer = sensorValuePotentiometerNew;
    }
  }  
}

// ============================================= CLEARING =================================================

// To clear sections of the old graph
void eraser(int color, int index) {
  tft.drawLine((index - 1) % 320, 20, (index % 320), 220, color);
}

// Clears everything on the screen
void clearEKG() {
  tft.fillScreen(ILI9341_WHITE);
  drawGrid();
  tft.fillRect(0, 0, 320, 20, ILI9341_BLACK);
  tft.fillRect(0, 20, 320, 25, ILI9341_BLACK);
  tft.fillRect(0, 220, 320, 240, ILI9341_BLACK);
}

// Clear graph area
void clearGraph() {
  tft.fillRect(0, 45, 320, 180, ILI9341_WHITE);
  drawGrid();
}

// Clear detection circles
void clearQSRCircles() {
  tft.fillRect(0, 220, 320, 240, ILI9341_BLACK);
} 

// ============================================= MESSAGES =================================================

// new message
void updateMessage(String newMessage) {
    tft.setCursor(5, 5); tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2); tft.println(currentMessage);
    tft.setCursor(5, 5); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println(newMessage);
    currentMessage = newMessage;
}

// new timer message
void timerMessage(String newTimerMessage) {
    tft.setCursor(290, 5); tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2); tft.println(currentTimerMessage);
    tft.setCursor(290, 5); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println(newTimerMessage);
    currentTimerMessage = newTimerMessage;
    currentTime = millis();
}

// Displays the QSR rate on the display
void printRate() {
  BPM = (int) (bufferSize / counterIndex * numQSR * 2);
  char h[2];
  char t[2];
  char o[2];
  String str;
  str = String(BPM / 100);
  str.toCharArray(h,2);
  str = String((BPM / 10) % 10);
  str.toCharArray(t,2);
  str = String(BPM % 10);
  str.toCharArray(o,2);
      
  // Creating char array manually to save memory on Arduino
  char newQSRMessageArray[8];
  newQSRMessageArray[0] = 'B';
  newQSRMessageArray[1] = 'P';
  newQSRMessageArray[2] = 'M';
  newQSRMessageArray[3] = ':';
  newQSRMessageArray[4] = ' ';
  newQSRMessageArray[5] = h[0];
  newQSRMessageArray[6] = t[0];
  newQSRMessageArray[7] = o[0];
  String newQSRMessage = String(newQSRMessageArray);
  if(!currentQSRMessage.equals(newQSRMessage)) {
    tft.setCursor(220, 25); tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2); tft.println(currentQSRMessage);
    currentQSRMessage = newQSRMessage;
    tft.setCursor(220, 25); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println(currentQSRMessage);
  }
}

// Prints Disease Type
// Tachycardia -> Exceeds 110
// Bradycardia -> Below 50
void printDiseaseType() {
  if(BPM > 110) {
    tft.setCursor(0, 25); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println("Tachycardia");
  } else if(BPM < 50 ) {
    tft.setCursor(0, 25); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println("Bradycardia");
  } else {
    tft.setCursor(0, 25); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println("Normal. Congrats!");
  }
}


