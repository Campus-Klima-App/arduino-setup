#include <TheThingsNetwork.h>

// First install "DHT sensor library" via the Library Manager
#include <DHT.h>

// Set your AppEUI and AppKey
const char *appEui = "XXXXXXXXXXXXXXX";
const char *appKey = "XXXXXXXXXXXXXXX";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868
#define DHTPIN 2

//Choose your DHT sensor moddel
#define DHTTYPE DHT11
//#define DHTTYPE DHT21
//#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

float coSensorMeasure(){
    float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    int sensorValue = analogRead(A0);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL

    /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
    ratio = RS_gas/0.08;  // ratio = RS/R0
     /*-----------------------------------------------------------------------*/
    Serial.print("sensor_volt = ");
    Serial.println(sensor_volt);
    Serial.print("RS_ratio = ");
    Serial.println(RS_gas);
    Serial.print("Rs/R0 = ");
    Serial.println(ratio);
    Serial.print("\n\n");
    delay(1000);

    return RS_gas;
}

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  dht.begin();
}

void loop()
{
  delay(60000);
  debugSerial.println("-- LOOP");

  // Read sensor values and multiply by 100 to effictively have 2 decimals
  uint16_t humidity = dht.readHumidity(false) * 100;

  // false: Celsius (default)
  // true: Farenheit
  uint16_t temperature = dht.readTemperature(false) * 100;

  // Split both words (16 bits) into 2 bytes of 8
  uint16_t co = coSensorMeasure()*100;
  //uint16_t co = 5;
  
  debugSerial.print("CO: ");
  debugSerial.println(co);
  
  debugSerial.print("Temperature: ");
  debugSerial.println(temperature);
  debugSerial.print("Humidity: ");
  debugSerial.println(humidity);
  
  byte payload[6];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);
  payload[2] = highByte(humidity);
  payload[3] = lowByte(humidity);
  payload[4] = highByte(co);
  payload[5] = lowByte(co);

  ttn.sendBytes(payload, sizeof(payload));
}
