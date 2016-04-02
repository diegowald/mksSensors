#include "DHT.h"

#define SensorTemperaturaPIN 2
#define BotonTestPIN 3
#define LEDLowPIN 4
#define LEDHighPIN 5
#define BuzzerPIN 6
#define RelePIN 7

#define MQ2PIN A0
#define MQ3PIN A1
#define MQ9PIN A2
#define PotePIN A3

#define DHTTYPE DHT11

DHT dht(SensorTemperaturaPIN, DHTTYPE);

float temperatura;
float humedad;
float mq2Value;
float mq3Value;
float mq9Value;
float poteValue;

bool dispararLedLow;
bool dispararLedHigh;
bool dispararBuzzer;
bool dispararRele;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  dht.begin();
  pinMode(LEDLowPIN, OUTPUT);
  pinMode(LEDHighPIN, OUTPUT);
  pinMode(BuzzerPIN, OUTPUT);
  pinMode(RelePIN, OUTPUT);
  pinMode(BotonTestPIN, INPUT);

  temperatura = 0.;
  humedad = 0.;
  mq2Value = 0.;
  mq3Value = 0.;
  mq9Value = 0.;

  dispararLedLow = false;
  dispararLedHigh = false;
  dispararBuzzer = false;
  dispararRele = false;
}

void loop() {
  readSensores();
  Serial.print("temp = ");
  Serial.print(temperatura);
  Serial.print(", humedad = ");
  Serial.print(humedad);
  Serial.print(", mq2 = ");
  Serial.print(mq2Value);
  Serial.print(", mq3 = ");
  Serial.print(mq3Value);
  Serial.print(", mq9 = ");
  Serial.print(mq9Value);
  Serial.print(", pote = ");
  Serial.print(poteValue);
  analizarValores();
  alarmas();
}

void analizarValores() {
  float maxValue = mq2Value;
  if (maxValue < mq3Value)
  {
    maxValue = mq3Value;
  }
  if (maxValue < mq9Value)
  {
    maxValue = mq9Value;
  }
  if (maxValue < poteValue)
  {
    maxValue = poteValue;
  }

  dispararLedLow = false;
  dispararLedHigh = false;
  dispararBuzzer = false;
  dispararRele = false;

  int val = digitalRead(BotonTestPIN);  // read input value
  if (val == LOW) {            // check if the input is HIGH
    maxValue = 5.0;
  }
  Serial.println(maxValue);

  /*  if ((0 <= maxValue) && (maxValue <= 1.))
    {
      dispararLedLow = false;
    }*/
  if ((1. <= maxValue) && (maxValue <= 2.))
  {
    dispararLedLow = true;
  }
  if ((2. <= maxValue) && (maxValue <= 3.))
  {
    dispararLedLow = true;
    dispararLedHigh = true;
  }
  if ((3. <= maxValue) && (maxValue <= 4.))
  {
    dispararLedLow = true;
    dispararLedHigh = true;
    dispararBuzzer = true;
  }
  if ((4. <= maxValue) && (maxValue <= 5.))
  {
    dispararLedLow = true;
    dispararLedHigh = true;
    dispararBuzzer = true;
    dispararRele = true;
  }
}

void alarmas() {
  alarmaBaja(dispararLedLow);
  alarmaAlta(dispararLedHigh);
  buzzer(dispararBuzzer);
  rele(dispararRele);
}

void alarmaBaja(bool activa) {
  if (activa)
  {
    digitalWrite(LEDLowPIN, HIGH);
  }
  else
  {
    digitalWrite(LEDLowPIN, LOW);
  }
}

void alarmaAlta(bool activa) {
  if (activa)
  {
    digitalWrite(LEDHighPIN, HIGH);   //Turn off led
  }
  else
  {
    digitalWrite(LEDHighPIN, LOW);    //Turn on led
  }
}

void buzzer(bool activa) {
  if (activa) {
    unsigned char i, j; //define variable
    //while (1)
    {
      for (i = 0; i < 80; i++) // output a frequency sound
      {
        digitalWrite(BuzzerPIN, HIGH); // sound
        delay(1);//delay1ms
        digitalWrite(BuzzerPIN, LOW); //not sound
        delay(1);//ms delay
      }
      for (i = 0; i < 100; i++) // output a frequency sound
      {
        digitalWrite(BuzzerPIN, HIGH); // sound
        digitalWrite(BuzzerPIN, LOW); //not sound
        delay(2);//2ms delay
      }
    }
  }
}

void rele(bool activa) {
  if (activa) {
    digitalWrite(RelePIN, HIGH);
  }
  else {
    digitalWrite(RelePIN, LOW);
  }
}

void readSensores() {
  readSensorTemperatura();
  readSensorMQ2();
  readSensorMQ3();
  readSensorMQ9();
  readPote();
}
void readSensorTemperatura() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  humedad = h;
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  temperatura = t;
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  /*Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");*/
}

void readSensorMQ2() {
  int sensorValue = analogRead(MQ2PIN);
  float sensorVoltage = ((float) sensorValue) / 1024.0 * 5.0;
  mq2Value = sensorVoltage;
}

void readSensorMQ3() {
  int sensorValue = analogRead(MQ3PIN);
  float sensorVoltage = ((float) sensorValue) / 1024.0 * 5.0;
  mq3Value = sensorVoltage;
}

void readSensorMQ9() {
  int sensorValue = analogRead(MQ9PIN);
  /*Serial.print("Sensor value = ");
    Serial.print(sensorValue);
    Serial.println(" ");*/

  float sensorVoltage = ((float) sensorValue) / 1024.0 * 5.0;
  mq9Value = sensorVoltage;
  float RS_Gas = (5.0 - sensorVoltage) / sensorVoltage;

  float ratio = RS_Gas / 1.0 /*R0_MQ9*/;

  /*Serial.print("Sensor voltage = ");
    Serial.print(sensorVoltage);
    Serial.println(" V");

    Serial.print("RS Gas = ");
    Serial.print(RS_Gas);
    Serial.println(" Ohm");

    Serial.print("RS/R0 = ");
    Serial.println(ratio);*/
}

void readPote() {
  int sensorValue = analogRead(PotePIN);
  /*Serial.print("Sensor value = ");
    Serial.print(sensorValue);
    Serial.println(" ");*/

  float sensorVoltage = ((float) sensorValue) / 1024.0 * 5.0;
  poteValue = sensorVoltage;
}

