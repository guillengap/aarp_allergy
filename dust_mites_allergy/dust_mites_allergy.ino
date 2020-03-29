//AUTHOR: GUILLERMO PEREZ GUILLEN

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;      
#include <TouchScreen.h>

int16_t BOXSIZE;
uint16_t ID, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT
//String UVIndex = "0";
String Index = " ";  

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

  int IRED = 24; // Pin digital para el IRED
 
  // Constant times for the IRED control pulse
  int retardo_1 = 280;
  int retardo_2 = 40;
  int retardo_3 = 9680;
 
  // Auxiliary program variables
  int valor = 0;
  float ppm = 0;
  float voltaje = 0;
  float densidad_polvo = 0;
  float ppm_real = 0;
  int i=0;

void setup()
{
  pinMode(IRED,OUTPUT); 
    while (!Serial);
  Serial.begin(57600);
   
    uint16_t tmp;
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);
}
 
void loop()
{
    i=i+1; // Control pulse counter
    // IRED will be activated with LOW and deactivated with HIGH according to the internal sensor connections
    digitalWrite(IRED,LOW); // LED ON
    delayMicroseconds(retardo_1); // Delay: 0,28ms

    float sensorValue = analogRead(A8);
    ppm = ppm + sensorValue; // Vo weighted average
    delayMicroseconds(retardo_2); // Delay: 0,04ms
    digitalWrite(IRED,HIGH); // LED OFF
    delayMicroseconds(retardo_3); // Delay: 9,68ms

    // Mathematical formulas for calculating GP2Y10 sensor values
    voltaje = ppm/i*0.0049; // Voltage in volts (average of the ppm values obtained)
    densidad_polvo = 0.17*voltaje-0.1; // Density of dust particles in mg / m³
    ppm_real = (voltaje-0.0356)*120000; // Dust particle concentration in ppm
    if (ppm_real < 0)
      ppm_real = 0;
    if (densidad_polvo < 0 )
      densidad_polvo = 0;
    if (densidad_polvo > 0.5)
      densidad_polvo = 0.5;

    Serial.print("Voltage: ");
    Serial.print(voltaje,2);
    Serial.println(" V");    
    Serial.print("Densidad: ");
    Serial.print(densidad_polvo,3);
    Serial.println(" mg x m3");       
    Serial.print("Concentracion: ");
    Serial.print(ppm_real,2);    
    Serial.println(" ppm");    
    Serial.println(" ");

////////////////////////// Concentration

  if(ppm_real<75000.0)
  {
    Index = "EXCELLENT";
  }
  else if (ppm_real>=75000.0 && ppm_real<150000.0)
  {
    Index = "VERY GOOD";
  }
  else if (ppm_real>=150000.0 && ppm_real<300000.0)
  {
    Index = "GOOD";    
  }
  else if (ppm_real>=300000.0 && ppm_real<1050000.0)
  {
    Index = "ACCEPTABLE";    
  }else if (ppm_real>=1050000.0 && ppm_real<3000000.0)
  {
    Index = "POOR";    
  }
  else if (ppm_real>=3000000.0)
  {
    Index = "VERY POOR";    
  } 
  
/////////////////////////////////////

   tft.setCursor(0, 5);
   tft.setTextSize(3);   
   tft.setTextColor(MAGENTA, BLACK);   
   tft.println(" DUST SENSOR");
   tft.println(" ");   
   tft.setTextColor(YELLOW, BLACK);
   tft.println("Density ");  
   tft.println(String(densidad_polvo,3) + " mg/m3");
   tft.println(" ");
   tft.println("Concentration"); 
   tft.println(String(ppm_real,0) + " ppm ");
   tft.setTextColor(WHITE, BLACK); 
   tft.println(" ");   
   tft.println(String(Index) + " ");     
   delay(1000);
}
