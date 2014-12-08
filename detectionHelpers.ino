// Global Variables for detection helpers
int maxDetected = 0;
int previousIndex = 0;
int maxGlob = 0;
int numQSR = 0;
int BPM = 0;

// Check if the graph displayed is stable
boolean isStable() {
  int counter = 0; 
  for(int i = (counterIndex - 30)% 7500; i < (counterIndex)% 7500; i++) {
      if(beatsBuffer[i] > upperThreshold || beatsBuffer[i] < lowerThreshold) {
        counter++;
      }
   }
  return counter < 4;
}

// Called when button is pressed to reset the state of the robust system
void resetVarsButton() {
    maxDetected = 0;
    previousIndex = 0;
    maxGlob = 0;
    numQSR = 0;
    buttonState = LOW;
    stable = false;
    counterIndex = 0;
    tft.setCursor(290, 5); tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2); tft.println(String((currentTime - startTime) / 1000));
}

// QSR Detection Brilliant Algorithm. Takes 5 point deravitive with dynamic threshold calculation to ensure that the QSR is precisely calculated.
// It worked for 14/15 of the test subjects with 95% accuracy.
void QSRDetection() {
  clearQSRCircles();
  // Access the chunk of the array (250 length) to calculate the QSR
  int index = 0;
  int frame = 125;
  // Out of bounds fix
  if(counterIndex > bufferSize) {
      index = bufferSize;
  } else {
      index = counterIndex;
  }
  
  // Calcultating the maximum from the derivative array
  int maxNum = 0;
  int derivativeArray[25];
  for(int j = counterIndex - frame + 2; j < index; j+=5) {
      derivativeArray[j/5] = (-1 * beatsBuffer[j + 2] + 8 * beatsBuffer[j + 1] - 8 * beatsBuffer[j - 1] + beatsBuffer[j - 2]) / 12;
      if (derivativeArray[j/5] > maxNum){
          maxNum = derivativeArray[j/5];
      }  
  }
  // Preventing false positives
  if (maxGlob < 60) {
      maxGlob = 0.8 * maxNum;
  }
  // Using the maximum from the previous steps to detect and plot the QSR Peaks
  for(int j = counterIndex - frame + 2; j < index; j+=5) {
      derivativeArray[j/5] = (-1 * beatsBuffer[j + 2] + 8 * beatsBuffer[j + 1] - 8 * beatsBuffer[j - 1] + beatsBuffer[j - 2]) / 12;
      if(derivativeArray[j/5] > maxNum - 10 && maxNum > maxGlob) {
          if(j - previousIndex > 30) {
            printRate();
            previousIndex = j;
            plotQSRonDisplay(j);
            numQSR++;
          }
      }
  }
  
}
