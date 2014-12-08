boolean firstTimeLoadCalled = true;
int previousSelectedPosition = 0;
int currentSelectedPosition = 0;
String names[7] = {"DTRM001.txt", "DTRM002.txt", "DTRM003.txt", "DTRM004.txt", "DTRM005.txt", "DTRM006.txt", "Cancel"};
boolean firstLineRead = false;
char lineBuf[50];
int bufferPosition = 0;

// This generates the loadMode and allows the user to select the files using the load button.
// Once the user is content, they can use the originalButton to load the file and get out of load mode
void loadFromSD() {
  if(firstTimeLoadCalled || !loadModeEnabled) {
     tft.fillScreen(ILI9341_WHITE);
     displayFileNames();
     firstTimeLoadCalled = false; 
     loadModeEnabled = true;
     previousSelectedPosition = 0;
     currentSelectedPosition = 0;
  } else {
    // this is called when the load button is pressed after the first time
    if(previousSelectedPosition > 5) {
      tft.fillRect(200, 6 * 30 + 12, 10, 10, ILI9341_BLACK);
      previousSelectedPosition = -1;
    }
    tft.fillRect(200, previousSelectedPosition * 30 + 12, 10, 10, ILI9341_BLACK);
    previousSelectedPosition++;
    currentSelectedPosition = previousSelectedPosition;
    tft.fillRect(200, previousSelectedPosition * 30 + 12, 10, 10, ILI9341_BLUE);
  }
}

// Called when the user presses the originalButton while in loadMode
void captureOriginalButton() {
 if ((millis() - lastDebounceTime) > debounceDelay && changeit == true && loadModeEnabled) {
     // Clear screen is cancel is selected
     if(currentSelectedPosition == 6) {
        clearEKG(); 
        updateMessage("Press button to run");     
        loadModeEnabled = false; 
     // Called when any of the files are selected to load
     } else {
        bufferPosition = 0;
        loadFile(names[currentSelectedPosition]);
        clearEKG();
        updateMessage("Press button to re-run"); 
        drawECG(ILI9341_BLACK, 0, 320);
        translateGraph();
        loadModeEnabled = false; 
     }
     buttonState = LOW;   
     changeit = false;
  } 
}

// Loads and Displays the Files in the loadMode so that the user may select one of those files
void displayFileNames() {
  for(int i = 0; i < 7; i++) {
    tft.fillRect(0, i * 30 + 10, 320, 20, ILI9341_BLACK);
    String name = names[i];
    tft.setCursor(50, i * 30 + 12); tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2); tft.println(name);
  }
  tft.fillRect(200, 12, 10, 10, ILI9341_BLUE);
}

// loadFile from SD
void loadFile(String name) {
  int i = 0;
  // re-open the file for reading:
  char charBuf[12];
  name.toCharArray(charBuf, 12);
  myFile = SD.open(charBuf);
  if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      char c = (char)myFile.read();
      // Detects new line and analyzes and resets buffer
      if(c == '\n') {
        firstLineRead = true;
        i = 0;
        if(firstLineRead) {
          tockenizeAndAnalyzeBuffer(lineBuf, ",");
        }
      }
      // Stores data in buffer
      if(firstLineRead) {
        lineBuf[i] = c;
        i++;
      }
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("File does not exist");
  }
}

// To tokenize a string with a given delimiter
void tockenizeAndAnalyzeBuffer(char str[], char delimeter[]) {
    char * pch;
    int i=0;
    pch = strtok (str,delimeter);
    while (pch != NULL) {
      int pos = atoi(&pch[1]); 
      if(bufferPosition < 7500) {
        beatsBuffer[bufferPosition] = pos;
        bufferPosition++;
      }
      i++;
      pch = strtok (NULL, delimeter);
    }
}

