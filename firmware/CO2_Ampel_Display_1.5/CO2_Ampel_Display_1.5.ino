/*
  CO2 Ampel
  By: Schülerforschungszentrum Region Freiburg
  Date: October, 2020
  Version: V1.5

  Functiondescription:
  - Reads CO2, humidity and temperature from the Sensirion SCD30 Sensor
  - Display readings on an 128x32 OLED Display
  - Generates an alarm (tone and LED flash) when threashold is excceded






*/

#define SWversion 1.5



#include <SPI.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library
#include "Adafruit_SSD1306.h" // https://github.com/adafruit/Adafruit_SSD1306
#include "Adafruit_GFX.h" //needed by Adafruit_SSD1306 library https://github.com/adafruit/Adafruit-GFX-Library also needs https://github.com/adafruit/Adafruit_BusIO


//Function announcment
float average (float * array, unsigned int len);
void setRgbLed(float co2Avg);
void alarm(float co2Avg);
bool calibration(unsigned int co2concentration);

//DISPLAY SETTINGS
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define DISPLAY_ADR 0x3c
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 100000, 100000);
//Print buffer for alignment of CO2 value on the Display
char printBuffer[] = {"            "};



//PIN DEFINITIONS
#define LED_R 11  // RGB-LED RED, Pin D11
#define LED_G 9  // RGB-LED GREEN, Pin D9
#define LED_B 10  // RGB-LED BLUE, Pin D10
#define LED_CA A3 // RGB-LED Common Anode, Pin A3
#define BUZZER 6  // Buzzer pin D6
#define FLASH1 2 // Red Flash LED, pin D2, near Display
#define FLASH2 3 // Red Flash LED, pin D3, near Arduino
#define IN1 5 // Jumper input IN1, not used at the moment
#define IN2 4 // Jumper input IN2, not used at the moment

//BUZZER SETTINGS
#define BUZZER_INTERVAL 300000 //Butzzer interval in ms, default 300000, every 5 Minutes
#define BUZZER_LEVEL 1000     // CO2 Level for buzzer
#define BUZZER_TONE_LENGTH 1000 // in ms

//FLASH SETTINGS
#define FLASH_INTERVAL 5000 // in ms
#define FLASH_LENGTH 150 // in ms

// RGB LED FADING SETTINGS
#define GREEN_LOWER_LIMIT 0
#define GREEN_UPPER_LIMIT 1000
#define RED_LOWER_LIMIT   800
#define RED_UPPER_LIMIT   1000
#define FADING false // FADING false = hard color changes, FADING true = color fades from 0 to 255 within the LIMITs


//VARIABLES FOR SENSOR DATA
float co2 = 0;
float temperature = 0;
float humidity = 0;
float co2Avg = 0;
int temperaturOffset = 3;
uint16_t calibConcentration = 450; // This value is set as concentration during calibration process, default is 400 (normal average outside air)

unsigned long nextNewData = 0;


//ROLLING AVERAGE BUFFER FOR CO2 VALUE
#define AVERAGING_NUMBER 15 // Number of elements for averaging, default 15 (equals 30s averaging)
float co2LastMinute [AVERAGING_NUMBER];
unsigned char co2LastMinutePointer = 0;

//TIMESTAMPS FOR BUZZER & FLASH
unsigned long lastExceed = 0;
unsigned long nextTone = 0;
unsigned long nextFlash = 0;


//Display Strings
char calibrationScrolling1[] = "          Now place jumper IN2 for calibration or remove IN1 to exit.   Sensor must be exposed to outside air for more than 5 minutes for calibration!";

//Declaration of SCD30 instence
SCD30 airSensor;


void setup()
{
  //PIN mode definition
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_CA, OUTPUT);
  pinMode(FLASH1, OUTPUT);
  pinMode(FLASH2, OUTPUT);
  pinMode(IN1, INPUT_PULLUP);   //not used at the moment
  pinMode(IN2, INPUT_PULLUP);   //not used at the moment
  pinMode(BUZZER, OUTPUT);



  //Set initial output values
  digitalWrite(FLASH1, LOW);  //Flash LED OFF
  digitalWrite(FLASH2, LOW);  //Flash LED OFF
  digitalWrite(BUZZER, LOW);  //Buzzer LED OFF
  digitalWrite(LED_R, HIGH);    // RGB-LED red OFF
  digitalWrite(LED_G, HIGH);    // RGB-LED green OFF
  digitalWrite(LED_B, HIGH);    // RGB-LED blue OFF
  digitalWrite(LED_CA, HIGH); //LED Common Anode ON



  //Bootdelay for propper sensor boot
  delay(2000);

  //Start serial connection for project / debug information
  Serial.begin(115200);
  Serial.println("CO2 Ampel");
  Serial.println("Schülerforschungszentrum Region Freiburg");

  //Start I2C
  Wire.begin();
  Wire.setClock(100000);

  //CO2 Sensor Init
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring.");
    while (1);
  }

  delay(100);

  //Display Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed. Please check wiring.");
    while (1);
  }
  //Clear display
  display.clearDisplay();
  display.display();


  if (digitalRead(IN1) == LOW && digitalRead(IN2) == HIGH) {
    calibration(calibConcentration);
  }





}

void loop()
{




  //Check if new sensor data is available, the SCD30 has data ready every two seconds
  if (nextNewData < millis() && airSensor.dataAvailable())
  {




    nextNewData = millis() + 2000; // only request new data when the SCD30 potentially has new Data available

    //Read sensor data
    co2 = airSensor.getCO2();
    temperature = airSensor.getTemperature() - temperaturOffset;
    humidity = airSensor.getHumidity();

    //Update CO2 rolling average buffer
    co2LastMinute[co2LastMinutePointer] = co2;
    co2LastMinutePointer = (co2LastMinutePointer + 1) % AVERAGING_NUMBER;

    //Calculate average
    co2Avg = average(co2LastMinute, AVERAGING_NUMBER);




    //UPDATE DISPLAY
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);



    sprintf(printBuffer, "%5d", (unsigned int)co2Avg);
    display.print(printBuffer);
    //    display.print(co2Avg, 0);
    delay(10);

    display.setTextSize(2);
    delay(10);
    display.setCursor(92, 0);
    delay(10);
    display.print("ppm");
    delay(10);
    display.setTextSize(1);
    delay(10);
    display.setCursor(0, 24);
    delay(10);
    display.print(temperature, 1);
    delay(10);
    display.print("C ");
    delay(10);
    display.print(humidity, 0);
    delay(10);
    display.print("% by SFZ");
    delay(10);


    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    if (co2LastMinutePointer % 2 == 0) {
      display.drawPixel(127, 31, WHITE);
      delay(10);
      display.invertDisplay(false);
      delay(10);

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);


    }
    else if (co2Avg > BUZZER_LEVEL) {

      display.invertDisplay(true);
      delay(10);



    }

    // Hier bleibt es hängen!
    display.display();

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);



    //Set RGB led according level definitions
    setRgbLed(co2Avg);


    //Evaluate if alarm threshtold is exceeded and genatate tone and/or red flash according to settings
    alarm(co2Avg);

  }
  else {


    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    //Delay to not stress SCD30, if no new data available wait 100ms before the next request
    delay(100);

  }

}


//Set RGB led according level definitions
void setRgbLed(float co2Avg) {
  if (FADING == true) {
    //SET RGB LED ACCORDING TO DEFINED LEVEL SETTINGS WITH SMOOTH FADING
    analogWrite(LED_G, map(constrain(co2Avg, GREEN_LOWER_LIMIT, GREEN_UPPER_LIMIT), GREEN_LOWER_LIMIT, GREEN_UPPER_LIMIT, 0, 255));
    analogWrite(LED_R, map(constrain(co2Avg, RED_LOWER_LIMIT, RED_UPPER_LIMIT), RED_LOWER_LIMIT, RED_UPPER_LIMIT, 255, 0));
  }
  else {
    //SET RGB LED ACCORDING TO DEFINED LEVEL SETTINGS WITH HARD COLOR CHANGES
    if (co2Avg < GREEN_UPPER_LIMIT) {
      digitalWrite(LED_G, LOW);
    }
    else {
      digitalWrite(LED_G, HIGH);
    }
    if (co2Avg > RED_LOWER_LIMIT) {
      digitalWrite(LED_R, LOW);
    }
    else {
      digitalWrite(LED_R, HIGH);
    }
  }
}


//Evaluate if alarm threshtold is exceeded and genatate tone and/or red flash
void alarm(float co2Avg) {
  // ALARM LEVEL
  if (co2Avg > BUZZER_LEVEL) {
    lastExceed = millis();
    if (lastExceed > nextFlash && nextNewData > (lastExceed + FLASH_LENGTH))
    {
      nextFlash = lastExceed + FLASH_INTERVAL;
      digitalWrite(FLASH1, HIGH);
      digitalWrite(FLASH2, HIGH);
      delay(FLASH_LENGTH);
      digitalWrite(FLASH1, LOW);
      digitalWrite(FLASH2, LOW);
    }
    if (lastExceed > nextTone  && nextNewData > (lastExceed + BUZZER_TONE_LENGTH)) {
      nextTone = lastExceed + BUZZER_INTERVAL;
      digitalWrite(BUZZER, HIGH);
      delay(BUZZER_TONE_LENGTH);
      digitalWrite(BUZZER, LOW);
    }
  }
  else {
    nextTone = lastExceed;
    nextFlash = lastExceed;
  }
}

//Calibrate the SCD30 to a given concentration, default is 450 ppm (approximately outside air)
bool calibration(unsigned int co2concentration) {

  while ( digitalRead(IN1) == LOW &&  digitalRead(IN2) == HIGH) {

    digitalWrite(LED_R, HIGH);    // RGB-LED red OFF
    digitalWrite(LED_G, HIGH);    // RGB-LED green OFF
    digitalWrite(LED_B, LOW);    // RGB-LED blue ON
    digitalWrite(LED_CA, HIGH); //LED Common Anode ON


    display.clearDisplay();
    display.setTextWrap(false);
    display.setTextSize(2);
    //    display.setTextColor(WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
    delay(10);
    display.print("CALIB");
    delay(10);
    display.setTextSize(1);
    delay(10);
    display.setCursor(62, 0);
    delay(10);
    display.print(calibConcentration);
    display.print("ppm V");
    display.print(SWversion);
    delay(10);
    display.setTextSize(2);
    delay(10);
    display.display();

    for (unsigned int i = 0; i < strlen(calibrationScrolling1) - 10; i++) {
      display.setCursor(0, 16);
      delay(10);
      display.print(&calibrationScrolling1[i]);
      delay(10);
      display.display();
      delay(200);

      if ( digitalRead(IN1) == LOW &&  digitalRead(IN2) == LOW) break;
      if ( digitalRead(IN1) == HIGH) {
        digitalWrite(LED_B, HIGH);    // RGB-LED blue OFF
        return false;
      }
    }






    if ( digitalRead(IN1) == LOW &&  digitalRead(IN2) == LOW) {
      display.fillRect(0, 16, 128, 16, BLACK);
      display.setCursor(0, 16);
      delay(10);
      display.setTextSize(1);
      delay(10);
      display.print("CALIB starts in ");
      delay(10);
      display.setCursor(0, 24);
      display.print("Remove jumper to EXIT");
      display.display();



      for (int i = 0; i < 300; i++) {
        display.setCursor(100, 16);
        delay(10);
        display.print(300 - i);
        delay(10);
        display.print("s");
        delay(10);
        display.display();
        delay(900);

        if ( digitalRead(IN1) == HIGH ||  digitalRead(IN2) == HIGH) {
          digitalWrite(LED_B, HIGH);    // RGB-LED blue OFF
          return false;
        }
      }

      airSensor.setForcedRecalibrationFactor(calibConcentration);
      digitalWrite(LED_B, HIGH);    // RGB-LED blue OFF
      return true;
    }

  }
  digitalWrite(LED_B, HIGH);    // RGB-LED blue OFF
}



//AVERAGE CALCULATION OF AN FLOAT ARRAY
float average (float * array, unsigned int len)  // assuming array is float.
{
  float sum = 0L ;  // sum will be larger than an item, float for safety.
  for (int i = 0 ; i < len ; i++)
    sum += array [i] ;
  return  ((float) sum) / len ;  // average will be fractional, so float may be appropriate.
}
