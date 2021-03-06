//PROJECT LIBRA SCALE MEASUREMENT SYSTEM
//CODE BY MICHAEL KLOPFER + Calit2 Team (2017)

#include <Hx711.h> //Load Cell A/D (HX711 library)
#include <Adafruit_TLC59711.h> //12 Ch LED Controller, uses SPI only MOSI and CLK
#include <SPI.h>

//Constants
#define portTICK_PERIOD_MS 10


//Interface Assignments
#define HX711CLK 19
#define HX711DA 18
#define PIEZO 4
#define TAREBTN 21
#define TLC59711CLK 23
#define TLC59711DA 5

//Peripheral Setup
#define NUM_TLC59711 1  // How many boards do you have chained? (Only 1 in this example)

 //Define Objects
Hx711 scale(HX711DA, HX711CLK); // Object for scale - Hx711.DOUT - pin #A2 & Hx711.SCK - pin #A3 ---this sensor uses a propriatary synchronus serial communication, not I2C
Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, TLC59711CLK, TLC59711DA);

//----------------------------------
//Global Variables
float calibrated_scale_weight_g=0; //Holding variable for weight
float calibrated_scale_weight_g_1 = 0; //For Averaging
float calibrated_scale_weight_g_2 = 0; //For Averaging
float calibrated_scale_weight_g_3 = 0; //For Averaging
float tare_subtraction_factor=0;  //When TARE is processed, this value is subtracted for the displayed weight

//Initial Calibration Factors and offsets
float scale_gain=-.0022103;   //Initial Sensor Calibration Offset Value (fill in the `ratio` value here) [ratio = (w - offset) / 1000]  where W is a known weight, say 1000 grams
float scale_offset=36820; //Initial Sensor Calibration Offset Value (fill in the `offset` value here)
float inherent_offset=33781;  //Weight of unloaded sensor

//Piezo Parameters
int freq1 = 2000;
int freq2 = 125;
int dutyCycle = 50;
int channel = 0;
int resolution = 8;
int piezoch = 3;

//------------------------------------


//---------------------------------------------------------
void setup()
{
  Serial.begin(115200);  //begin Serial for LCD
    
  //User Interface Buttons Initialize
  pinMode(TAREBTN, INPUT_PULLUP);      //Tare Button   

  pinMode(PIEZO, OUTPUT);     //Select Button      
  digitalWrite(PIEZO, LOW);

  //Initalize Piezo
  ledcSetup(channel, freq1, resolution);
  ledcAttachPin(4, channel);

  //Initialize Piezo Driver
  tlc.begin();
  tlc.write();
  
  //Set the offset and gain values
  scale.setScale(scale_gain); //Gain
  scale.setOffset(scale_offset); //Offset
  
  Serial.println("****************END OF BIOS MESSAGES****************");
  Serial.println("");
}

//---------------------------------------------------------
void loop() 
{
//Read scale Value - rememberm this can have multiple calls that are averaged, check the library for the HX711 if this is not running properly!
  calibrated_scale_weight_g_1=scale.getGram() - inherent_offset;  //subtract inherent offset from pan weight (Read 1)
  calibrated_scale_weight_g_2=scale.getGram() - inherent_offset;  //subtract inherent offset from pan weight (Read 2)
  calibrated_scale_weight_g_3=scale.getGram() - inherent_offset;  //subtract inherent offset from pan weight (Read 3)
  calibrated_scale_weight_g = (calibrated_scale_weight_g_1 + calibrated_scale_weight_g_2 + calibrated_scale_weight_g_3)/3;  //return mean average from 3 readings

  if (digitalRead(TAREBTN) != 0) //Look for Tare button Press
    {
     tare_subtraction_factor = calibrated_scale_weight_g;
    }
    
  calibrated_scale_weight_g = calibrated_scale_weight_g - tare_subtraction_factor; //Calculate weight
  //Serial.println(scale.averageValue()); //output of value prior to gram calculation
  Serial.print("Weight in grams: ");
  Serial.println(calibrated_scale_weight_g);
  Serial.print("Weight in Oz: ");
  Serial.println(calibrated_scale_weight_g*0.035274);
  Serial.println();

  //Demo lights - state 1 (all ON)
  tlc.setLED(0, 65534, 65534, 65534);
        tlc.write();
  tlc.setLED(1, 65534, 65534, 65534);
        tlc.write();
  tlc.setLED(2, 65534, 65534, 65534);
        tlc.write();
  tlc.setLED(3, 65534, 65534, 65534);
        tlc.write();
        
  //Make a Sound!
  //ledcWriteTone(channel, freq2);
        
vTaskDelay(1000 / portTICK_PERIOD_MS);

//Demo lights - state 1 (all Off)
  tlc.setLED(0, 0, 0, 0);
  tlc.write();
  tlc.setLED(1, 0, 0, 0);
  tlc.write();
  tlc.setLED(2, 0, 0, 0);
  tlc.write();
  tlc.setLED(3, 0, 0, 0);
  tlc.write();

   //Make a Sound!
  //ledcWriteTone(channel, freq1);
  //ledcWrite(channel, dutyCycle);
}
