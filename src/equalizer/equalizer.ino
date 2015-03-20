//////////////////////
// Library Includes //
//////////////////////
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#include "color.h"

/////////////////////
// Hardware Hookup //
/////////////////////
// R0, G0, B0, R1, G1, B1 should be connected to pins 
// 2, 3, 4, 5, 6, and 7 respectively. Their pins aren't defined,
// because they're controlled directly in the library. These pins
// can be moved (somewhat):
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3 // Comment this line out if you're using a 32x16
// CLK can be moved but must remain on PORTB(8, 9, 10, 11, 12, 13)
#define CLK 11  // MUST be on PORTB!

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

int8_t cursorX = 0;  // Cursor x position, initialize left
int8_t cursorY = 0;  // Cursor y position, initialize top

const int res = 8;
const int strobe = 12;

#define EQU_HIGH 16
//const int bandValues[]  = {895, 767, 639, 511, 383, 255, 127, 80, 60, 40, 20};
const int bandValues[]  = {895, 820, 767, 720, 639, 590, 511, 470, 383, 330, 255, 190, 127, 100, 80, 50};
//const int equHigh[]     = {16 , 13 , 11 , 9  , 7  , 5  , 3  , 1 , 0};
//const TColor bcs[]      = {TColor(0xff0000), TColor(0xffa000), TColor(0x00ff00), TColor(0xff00ff), TColor(0x0000ff), TColor(0xffff00), TColor(0xffffff)};
const TColor bcs[]      = {TColor(0x00ff00), TColor(0x00ff00), TColor(0x00ff00), TColor(0x00ff00), TColor(0x00ff00), TColor(0x00ff00), TColor(0x00ff00)};
  
int left[7]; // store band values in these arrays
int right[7];
int band;


void setup() {
  Serial.begin(115200);
  
  pinMode(A6,INPUT);
  digitalWrite(A6,LOW);
  
  pinMode(res, OUTPUT); // reset
  pinMode(strobe, OUTPUT); // strobe
  digitalWrite(res,LOW); // reset low
  digitalWrite(strobe,HIGH);

 // put your setup code here, to run once:
  matrix.begin();       // Initialize the matrix.
  blankEasel(); // Blank screen
}

int i;
void loop() { 
#ifdef SIMULATE  
  simulateMSGEQ7();
#else
  readMSGEQ7();
#endif

  int leftPos = 0;
  int rightPos = 0;
  
  for ( band = 0; band<7; band++ ) {
    leftPos = band;
    rightPos = 9 + band;
    drawBar(leftPos, equValue(left[band]), bcs[band]);
    drawBar(rightPos, equValue(right[band]), bcs[6-band]);
  }

#ifdef SERIAL_TEST  
  testPrintBands();
#endif

  delay(50);
}

#ifdef SERIAL_TEST
void testPrintBands() {
 // display values of left channel on serial monitor
 for (band = 0; band < 7; band++)
 {
 Serial.print(left[band]);
 Serial.print("/"); Serial.print(equValue(left[band]));
 Serial.print(" ");
 }
 Serial.print("\t");
// display values of right channel on serial monitor
 for (band = 0; band < 7; band++)
 {
 Serial.print(right[band]);
 Serial.print("/"); Serial.print(equValue(right[band]));
 Serial.print(" ");
 }
 Serial.println();
}
#endif

#ifdef SIMULATE
int last_left[] = {4,4,4,4,4,4,4,4,4};
int last_right[] = {4,4,4,4,4,4,4,4,4};
void simulateMSGEQ7() {
  for ( int i=0; i<7; i++) {
    int left_idx = (int)floor((rand()*100)+0.5) % 8;
    int right_idx = (int)floor((rand()*100)+0.5) % 8;
    int l = abs( (right_idx < 6 ? last_right[i] + 1 : last_right[i] - 1) % 8 );
    int r = abs( (left_idx < 6 ? last_left[i] + 1 : last_left[i] - 1) % 8 );    
    left[i] = bandValues[l]+1;
    right[i] = bandValues[r]+1;
    last_left[i] = l;
    last_right[i] = r;
  }
  delay(200);
}
#endif

void readMSGEQ7()
// Function to read 7 band equalizers
{
 digitalWrite(res, HIGH);
 digitalWrite(res, LOW);
 delayMicroseconds(75);
 
 for(band=0; band <7; band++)
 {
   digitalWrite(strobe,LOW); // strobe pin on the shield - kicks the IC up to the next band 
   delayMicroseconds(45); // 
 
   left[band] = analogRead(A6); // store left band reading
   right[6-band] = left[band]; //analogRead(A4); // ... and the right
  
   digitalWrite(strobe,HIGH); 
   delayMicroseconds(45);
 }
}

// Convert band level to a discrete display value
int equValue(int bandLevel) {
  int i=0;
  for ( ; i < EQU_HIGH && bandLevel < bandValues[i]; i++ ) ;
  return EQU_HIGH - i;
}

void blankEasel()
{
  cursorX = 0;
  cursorY = 0;
  matrix.fillScreen(0); // Blank screen
}

void drawBar(int barIdx, int height, const TColor& color) {
  drawBar(barIdx,height,color.red,color.green,color.blue); 
}
void drawBar(int barIdx,int height, uint8_t r, uint8_t g, uint8_t b) {
  if ( barIdx > 15 || barIdx < 0 || height > 16) return;
  
  int r_y = 8;
  int x = barIdx*2;
  int y = matrix.height();
  int dh = height;
  
  int gh = min(height,r_y);
  int rh = max(0, dh-r_y);
   
  int w = 1;
  
  matrix.fillRect(x,0,w,y,matrix.Color444(0,0,0));
  
  if ( gh == 0 ) return;
//  matrix.fillRect(x,y-height,w,y,matrix.Color444(r,g,b));
  matrix.fillRect(x,0,w,gh,matrix.Color444(r,g,b));
  if ( rh > 0 ) matrix.fillRect(x,gh,w,rh,matrix.Color444(255,0,0));
}
