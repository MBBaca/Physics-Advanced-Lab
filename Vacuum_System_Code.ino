#define CALC_INTERVAL 1000                        // read the analog pin and print to serial once every 1/4 second
#define WAIT 60000

int hornetPin = A1; 
int CG1Pin = A0;
int CG2Pin = A2;
int rpmPin = A3;
int turboPin = 3;
int valvePin = 4;
int scrollPin = 5;
unsigned long nextCalc;
unsigned long nextAct;
unsigned long timer;

void setup() {
  Serial.begin(9600);                             //  setup serial

  pinMode(turboPin, OUTPUT);
  pinMode(valvePin, OUTPUT);
  pinMode(scrollPin, OUTPUT);
  nextCalc = millis() + CALC_INTERVAL;
  nextAct = millis() + WAIT;
  digitalWrite(valvePin, LOW);
}

void loop() {
  Serial.print("Enter 1 to start vacuum system");
  while (Serial.available() == 0) {
    
  }
  
  timer = millis();
  int i = 0;

  while (i != 1)
  {
    digitalWrite(scrollPin, HIGH);
    timer = millis();
    if (timer >= nextCalc) {
      nextCalc = timer + CALC_INTERVAL;
    
      float hornetValue = analogRead(hornetPin) * 2 ;                        // read the input pin
      float hornetVoltage = 5 * (hornetValue / 1023.0);                     // convert analog input to voltage value
      float hornetPressure = pow(10, (hornetVoltage - 10))*100000;         // pressure in torr calculated from the measured voltage
      
      float CG2Value = analogRead(CG2Pin) * 2;
      float CG2Voltage = 5 * (CG2Value / 1023.0);
      float CG2Pressure = pow(10, (CG2Voltage - 3));
  
      float CG1Value = analogRead(CG1Pin) * 2;
      float CG1Voltage = 5 * (CG1Value / 1023.0);
      float CG1Pressure = pow(10, (CG1Voltage - 3));

      float rpmValue = analogRead(rpmPin);
      float rpmVoltage = 5*(rpmValue/1024.0);
      
      Serial.print("Hornet = ");
      Serial.print(hornetPressure);
      Serial.println(" microtorr");
  
      Serial.print("CG1 = ");
      Serial.print(CG1Pressure);
      Serial.println(" millitorr");

      Serial.print("CG2 = ");
      Serial.print(CG2Pressure);
      Serial.println(" millitorr");
  

      Serial.print("Time = ");
      Serial.print(timer/1000/60);
      Serial.println(" minutes");
  
      
      if (CG1Pressure < 500 && CG2Pressure < 500) {
        digitalWrite(turboPin, HIGH);
        Serial.println("Turbo Pump Is On");
      }
      if (timer > 3*pow(10, 5) && hornetPressure > 700) {
        digitalWrite(turboPin, LOW);
        delay(1000);
        i = 1;
      }
    }
  }

  while (i=1) {
    timer = millis();
    int a = 0;
    if (a=0) {
      timer = 0;
      nextAct = millis() + WAIT;
      a = 1;
    }
    float CG1Value = analogRead(CG1Pin) * 2;
    float CG1Voltage = 5 * (CG1Value / 1024.0);
    float CG1Pressure = pow(10, (CG1Voltage - 3));
    Serial.print("CG1 = ");
    Serial.print(CG1Pressure);
    Serial.println(" millitorr");
    delay(1000);
    if (timer >= nextAct) {
      digitalWrite(scrollPin, LOW);
    }
    if (timer >= nextAct*2) {
      digitalWrite(valvePin, HIGH);
    }
  }
 }
