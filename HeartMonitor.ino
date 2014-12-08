/* 
  CSE 466 Lab 8: Rushabh Mehta (1239509), David Tran (1168345)
  Direct Programming of ARM Cortex Registers to read from the circuit and plot data and save on the SD
 */

// pdb and adc
#define PDB_CH0C1_TOS 0x0100
#define PDB_CH0C1_EN 0x01

uint16_t samples[16];
volatile uint32_t* valueADSC;

int sensorPin = A0; 
int sensorValue = 0; 
int beatsBuffer[7500];
int counterIndex = 0;
#include "SPI.h"
#include "ILI9341_t3.h"
#define TFT_DC  9
#define TFT_CS 10
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
int thickness = 2;
int translation = 100;

boolean stable = false;
int bufferSize = 7500;
int upperThreshold = 3800;
int lowerThreshold = 300;

// button1
const int buttonPin = 2;            
int buttonState;             
int lastButtonState = HIGH;   
long lastDebounceTime = 0;  
long debounceDelay = 50;    
boolean changeit = false;
// button2
const int buttonPin1 = 3;            
int buttonState1;             
int lastButtonState1 = LOW;   
long lastDebounceTime1 = 0;  
long debounceDelay1 = 50;    
boolean changeit1 = false;
// potentiometer
int sensorValuePotentiometer = 0; 

// sd card
#include <SD.h>
#include <SPI.h>
File myFile;
const int chipSelect = 4;
int counterSD = 0;
String fileName = "DTRM";

// Message
String currentMessage = "";
String currentTimerMessage = "";
String currentQSRMessage = "";
int startTime = 0;
int currentTime = 0;

// BUFFER
int num;
boolean interupt;

// Moving AVE
int movingAve = 0;

// Load from SD
boolean loadModeEnabled = false;

// pdb and adc
static const uint8_t channel2sc1a[] = {
  5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
  0, 19, 3, 21, 26, 22
};

boolean buttonPressedOnce = false;

// pdb and adc
#define ADC_CONFIG1 (ADC_CFG1_ADIV(1) | ADC_CFG1_MODE(1) | ADC_CFG1_ADLSMP)
#define ADC_CONFIG2 (ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(3))

void setup() {
       // sd card
      Serial.begin(9600);
      delay(1000);
      // pdb and adc
      adcInit();
      pdbInit();
      
      Serial.print("Initializing SD card..."); 
       pinMode(10, OUTPUT);
       if (!SD.begin(chipSelect)) {
         Serial.println("initialization failed!");
         return;
       }
       Serial.println("initialization done.");

        // tft
        tft.begin();
        tft.setRotation(3);
        
        // initial ekg
        clearEKG(); 
       updateMessage("Press button to run");        
        
         // button
        pinMode(buttonPin, INPUT);
        pinMode(buttonPin1, INPUT);

        // buffer initialize
        for(int i = 0; i < 7500; i++) {
          beatsBuffer[i] = 2048;
        }
}

void loop(){
  // button1 -> stores value in buttonState
  int reading = digitalRead(buttonPin);
  if (reading == HIGH) {
     lastDebounceTime = millis();
     changeit = true;
  }
  // button2 -> stores value in buttonState
  int reading1 = digitalRead(buttonPin1);
  if (reading1 == HIGH) {
     lastDebounceTime1 = millis();
     changeit1 = true;
  }
  
  // Debouncing the button2 to cancel out the false positives
  if ((millis() - lastDebounceTime1) > debounceDelay1 && changeit1 == true) {
    // LOAD from SD Card Here 
    if(buttonState == LOW) {
       if(changeit1) {
          loadFromSD();
       } 
      }
     if (buttonState1 == HIGH) {
        buttonState1 = LOW;
     } else {
         buttonState1 = HIGH;
     }
     changeit1 = false;
  }
  
  if(loadModeEnabled) {
    captureOriginalButton();
  }
  
  // To control movement of the graph after the button is pressed/30 seconds are over
  translateGraph();
  
   // Debouncing the button to cancel out the false positives
  if ((millis() - lastDebounceTime) > debounceDelay && changeit == true && !loadModeEnabled) {
      if (buttonState == HIGH) {
         resetVarsButton();
         updateMessage("Press button to re-run");
         printDiseaseType();
         writeToSd();  
         buttonPressedOnce = true;
     } else {
         buttonState = HIGH;
         updateMessage("Stabilizing"); 
     }
     changeit = false;
  }
    // Occurs on interupt PDB
    if (interupt) {
        Serial.println(num);
        interupt = false;
        // if buttonState is high meaning "doing reading" thus stabalize and start reading
        if (buttonState) {
            if(stable == false) {
                beatsBuffer[counterIndex % bufferSize] = num;
                counterIndex++;
                // buffers 500 interrupts
                if(counterIndex > 1000 && isStable()) {
                  clearEKG();
                  updateMessage("Recording");
                  timerMessage("30");
                  startTime = millis();
                  stable = true;
                  counterIndex = 0;
                  // buffer initialize
                  for(int i = 0; i < 7500; i++) {
                    beatsBuffer[i] = 2048;
                  }
                } 
            // stablized as in waited 500 ms and reading reasonable
            } else {
                beatsBuffer[counterIndex % bufferSize] = num;
                if (counterIndex == 4) {
                   initializeBuffer();
                }else if (counterIndex > 4) {
                     IIRFilter();
                   if(counterIndex % 125 == 0) {
                     QSRDetection();
                   }
                }
                updateLine(ILI9341_WHITE, beatsBuffer[(counterIndex - 320 - 1) % bufferSize],  beatsBuffer[(counterIndex - 320) % bufferSize], counterIndex - 320);
                updateLine(ILI9341_BLACK, beatsBuffer[(counterIndex - 1) % bufferSize],  beatsBuffer[counterIndex % bufferSize], counterIndex);     
                timerMessage(String((currentTime - startTime) / 1000));
                counterIndex++;
                if (counterIndex >= 7500) {
                    updateMessage("Press button to re-run");
                    resetVarsButton();
                    writeToSd();
                }
            } 
        }  
    }
}

// pdb and adc
void adcInit() {
  ADC0_CFG1 = ADC_CONFIG1;
  ADC0_CFG2 = ADC_CONFIG2;
  // Voltage ref vcc, hardware trigger, DMA
  ADC0_SC2 = ADC_SC2_REFSEL(0) | ADC_SC2_ADTRG | ADC_SC2_DMAEN;

  // Enable averaging, 4 samples
  ADC0_SC3 = ADC_SC3_AVGE | ADC_SC3_AVGS(0);

  adcCalibrate();
  Serial.println("calibrated");

  // Enable ADC interrupt, configure pin
  ADC0_SC1A = ADC_SC1_AIEN | channel2sc1a[0];
  NVIC_ENABLE_IRQ(IRQ_ADC0);
}

// pdb and adc
void adcCalibrate() {
  uint16_t sum;

  // Begin calibration
  ADC0_SC3 = ADC_SC3_CAL;
  // Wait for calibration
  while (ADC0_SC3 & ADC_SC3_CAL);

  // Plus side gain
  sum = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
  sum = (sum / 2) | 0x8000;
  ADC0_PG = sum;

  // Minus side gain (not used in single-ended mode)
  sum = ADC0_CLMS + ADC0_CLM4 + ADC0_CLM3 + ADC0_CLM2 + ADC0_CLM1 + ADC0_CLM0;
  sum = (sum / 2) | 0x8000;
  ADC0_MG = sum;
}

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE \
  | PDB_SC_CONT | PDB_SC_PRESCALER(7) | PDB_SC_MULT(1))

// 48 MHz / 128 / 10 / 1 Hz = 37500
#define PDB_PERIOD (F_BUS / 128 / 10 / 250)

// pdb and adc
void pdbInit() {
  // Enable PDB clock
  SIM_SCGC6 |= SIM_SCGC6_PDB;
  // Timer period
  PDB0_MOD = PDB_PERIOD;
  // Interrupt delay
  PDB0_IDLY = 0;
  // Enable pre-trigger
  PDB0_CH0C1 = PDB_CH0C1_TOS | PDB_CH0C1_EN;
  // PDB0_CH0DLY0 = 0;
  PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
  // Software trigger (reset and restart counter)
  PDB0_SC |= PDB_SC_SWTRIG;
  // Enable interrupt request
  NVIC_ENABLE_IRQ(IRQ_PDB);
}

// pdb and adc
void adc0_isr() {
  interupt = true;
  num = 4096 - ADC0_RA;
}

// pdb and adc
void pdb_isr() {
  Serial.println("here");
  PDB0_SC &= ~PDB_SC_PDBIF;
}
