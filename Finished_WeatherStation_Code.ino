#include <ADSWeather.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h> 
#include <dht.h>
#include <Servo.h>

#define ANEMOMETER_PIN 2
#define VANE_PIN A0
#define RAIN_PIN 3
#define DHT11_PIN 4
#define CALC_INTERVAL 2000
#define CE_PIN   9
#define CSN_PIN 10

Servo tracker;  // create servo object to control a servo 
int eastLDRPin = 1;  //Assign analogue pins
int westLDRPin = 2;
int eastLDR = 0;   //Create variables for the east and west sensor values
int westLDR = 0;
int error = 0;
int calibration = 204;  //Calibration offset to set error to zero when both sensors receive an equal amount of light
int trackerPos = 90;    //Create a variable to store the servo position

const byte slaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

dht DHT;

char speeddata[30];
char speeddatafrac[30];
char speedWords[30] = "Wind Speed ";
int Speedint = 0;
int Speedfrac = 0;

char raindata[30];
char rainWords[30] = "Rain Ammount ";
int rain = 0;

char TempData[30];
char TempWords[30] = "Temperature = ";
int temp = 0;

char HumData[30];
char HumWords[30] = "Humidity = ";
int hum = 0;

char DirectData[30];
char DirectWords[30] = "Wind Direction = ";
int dir = 0;



unsigned long nextCalc;
unsigned long timer;
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

int windDir;
int windSpeed;
int rainAmmount;



ADSWeather ws1(RAIN_PIN, VANE_PIN, ANEMOMETER_PIN); //This should configure all pins correctly


void setup() {
  Serial.begin(9600); 

  tracker.attach(6);  // attaches the servo on pin 6 to the servo object
  
  Serial.println("SimpleTx Starting");

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
    
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), ws1.countRain, FALLING); 
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), ws1.countAnemometer, FALLING); 
  nextCalc = millis() + CALC_INTERVAL;

}

void loop() {

  eastLDR = calibration + analogRead(eastLDRPin);    //Read the value of each of the east and west sensors
  westLDR = analogRead(westLDRPin);
  if(eastLDR<350 && westLDR<350)  //Check if both sensors detect very little light, night time
  {
    while(trackerPos<=160)  //Move the tracker all the way back to face east for sunrise
    {
      trackerPos++;
      tracker.write(trackerPos);
      delay(100);
    }
  }
  error = eastLDR - westLDR;          //Determine the difference between the two sensors.
  if(error>15)        //If the error is positive and greater than 15 then move the tracker in the east direction
  {
    if(trackerPos<=160)  //Check that the tracker is not at the end of its limit in the east direction
    {
      trackerPos++;
      tracker.write(trackerPos);  //Move the tracker to the east
    }
  }
  else if(error<-15)  //If the error is negative and less than -15 then move the tracker in the west direction
  {
    if(trackerPos>20)  //Check that the tracker is not at the end of its limit in the west direction
    {
      trackerPos--;
      tracker.write(trackerPos);  //Move the tracker to the west
    }
  }

  timer = millis();

  int rainAmmount;
  int windGust;
  long windDirection;


  ws1.update(); 

  if (timer > nextCalc)
  {

    nextCalc = timer + CALC_INTERVAL;

    int chk = DHT.read11(DHT11_PIN);

    String wind_Direc;

    rainAmmount = ws1.getRain();
    windDirection = (ws1.getWindDirection());
    windGust = ws1.getWindGust();
    int Speedint = (ws1.getWindSpeed() / 10);
    int Speedfrac = (ws1.getWindSpeed() % 10);

    int temp = (DHT.temperature);
    int hum = (DHT.humidity);


    if (windDirection >= 350 || windDirection < 45) {
      wind_Direc = "North";
    }
    else if (windDirection >=45 && windDirection < 85) {
      wind_Direc = "North East";
    }
    else if (windDirection >=85 && windDirection < 125) {
      wind_Direc = "East";
    }
    else if (windDirection >=125 && windDirection < 170) {
      wind_Direc = "South East";
    }
    else if (windDirection >=170 && windDirection < 225) {
      wind_Direc = "South";
    }
    else if (windDirection >=225 && windDirection < 260) {
      wind_Direc = "South West";
    }
    else if (windDirection >=260 && windDirection < 290) {
      wind_Direc = "West";
    }
    else if (windDirection >=290 && windDirection < 350) {
      wind_Direc = "North West";
    }

    char speedWords[30] = "Wind Speed = ";
    sprintf(speeddata, "%d", Speedint);
    strcat(speedWords, speeddata);
    char dec[30] = ".";
    strcat(speedWords, dec);
    sprintf(speeddatafrac, "%d", Speedfrac);
    strcat(speedWords, speeddatafrac);
    char sp[30] = " mph";
    strcat(speedWords, sp);

    //     windSpeed / 10 will give the interger component of the wind speed
    //     windSpeed % 10 will give the fractional component of the wind speed

    String DirectWords = "Wind Direction = ";
    DirectWords = DirectWords + wind_Direc;
    char WindDirection[30];
    DirectWords.toCharArray(WindDirection, 30);

    //Serial.print(DirectWords);
  
    char rainWords[30] = "Rain Ammount = ";
    sprintf(raindata, "%d", rain);
    strcat(rainWords, raindata);
    char mil[30] = " mil";
    strcat(rainWords, mil);
  
    char TempWords[30] = "Temperature = ";
    sprintf(TempData, "%d", temp);
    strcat(TempWords, TempData);
    char cel[30] = " C";
    strcat(TempWords, cel);

    char HumWords[30] = "Humidity = ";
    sprintf(HumData, "%d", hum);
    strcat(HumWords, HumData);
    char per[30] = "%";
    strcat(HumWords, per);
    
  
    bool Speedy;
      Speedy = radio.write( &speedWords, sizeof(speedWords) );
  
      Serial.print("Data Sent ");
      Serial.print(speedWords);
      if (Speedy) {
          Serial.println("  Acknowledge received");
          
      }
      else {
          Serial.println("  Tx failed");
          
      }


    bool Direction;
      Direction = radio.write( &WindDirection, sizeof(WindDirection) );
  
      Serial.print("Data Sent ");
      Serial.print(WindDirection);
      if (Direction) {
          Serial.println("  Acknowledge received");
          
      }
      else {
          Serial.println("  Tx failed");
          
      }  
  
      
    bool Rain;
      Rain = radio.write( &rainWords, sizeof(rainWords) );
  
      Serial.print("Data Sent ");
      Serial.print(rainWords);
      if (Rain) {
          Serial.println("  Acknowledge received");
          
      }
      else {
          Serial.println("  Tx failed");
          
      }

      
    bool Temp;
      Temp = radio.write( &TempWords, sizeof(TempWords) );
  
      Serial.print("Data Sent ");
      Serial.print(TempWords);
      if (Temp) {
          Serial.println("  Acknowledge received");
          
      }
      else {
          Serial.println("  Tx failed");
          
      }


    bool Hum;
      Hum = radio.write( &HumWords, sizeof(HumWords) );
  
      Serial.print("Data Sent ");
      Serial.print(HumWords);
      if (Hum) {
          Serial.println("  Acknowledge received");
          
      }
      else {
          Serial.println("  Tx failed");
          
      }
     //delay(2000);
  }
}
