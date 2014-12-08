#define NZEROS 4
#define NPOLES 4
#define GAIN   1.240141088e+03

float xv[NZEROS + 1], yv[NZEROS + 1];
void initializeBuffer() {
  xv[0] = 2.03;
  xv[1] = 2.03;
  xv[2] = 2.02;
  xv[3] = 2.01;
  xv[4] = 2.00;
  
  yv[0] = 2550.85;
  yv[1] = 2558.74;
  yv[2] = 2559.73;
  yv[3] = 2555.41;
  yv[4] = 2550.41;
}

void IIRFilter(){ 
    xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
    xv[4] =  beatsBuffer[counterIndex] / GAIN;
    yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
    yv[4] =   (xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2]
                 + ( -0.3708142159 * yv[0]) + (  1.8475509441 * yv[1])
                 + ( -3.5071937247 * yv[2]) + (  3.0175552387 * yv[3]);
     beatsBuffer[counterIndex] = yv[4];
     
//     Serial.print(xv[0]);
//     Serial.print(" ");
//     Serial.print(xv[1]);
//     Serial.print(" ");
//     Serial.print(xv[2]);
//     Serial.print(" ");
//     Serial.print(xv[3]);
//     Serial.print("  ");
//     Serial.print(yv[0]);
//     Serial.print(" ");
//     Serial.print(yv[1]);
//     Serial.print(" ");
//     Serial.print(yv[2]);
//     Serial.print(" ");
//     Serial.println(yv[3]);
     
}

