#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "e959b95a4c8948c0bf78c91febc1fd96";
char ssid[] = "SECTOR VII";
char pass[] = "03183557!!";

#define DHTPIN 0          // D3
#define DHTTYPE DHT11     // DHT 11
#define ledp D7
#define pirPin D1         // Input for HC-S501
#define gasPin D0
#define buzzer D2
#define button2 D4
#define motor D5
#define ledt D6
#define sensorPin A0       //sensor pin connected to analog pin A0
#define ldr D8

WidgetLED ledtemp(V5);
WidgetLED ledpir(V7);
WidgetLED button1p(V6);

float liquid_level;
float liquid_percentage;
float difference;
int state;
float ratio;
int top_level = 600;//Maximum water level
int bottom_level = 0;//Minimum water level
int pirValue; // Place to store read PIR Value
float h;
float t;
int g;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.


void setup()
{
  // Debug console
  Serial.begin(9600);

  pinMode(ledp, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(button2, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(gasPin, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(ledt, OUTPUT);
  pinMode(ldr, INPUT);
  Blynk.begin(auth, ssid, pass);
  dht.begin();

  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void sendSensor()
{
  int ldrval = digitalRead(ldr);
  Serial.print("ldrval   ");
  Serial.println(ldrval);
  h = dht.readHumidity();
  Serial.print("Humidty : ");
  Serial.println(h);

  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  Serial.print("Temperature : ");
  Serial.println(t);

  if (t > 40.00)
  {
    ledtemp.on();
    digitalWrite(ledt, HIGH);
  }
  else
  {
    digitalWrite(ledt, LOW);
    ledtemp.off();
  }

  g = digitalRead(gasPin);
  Serial.print("Gas Sensor : ");
  Serial.println(g);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  }

  state = digitalRead(pirPin);
  Serial.print("PIR state ");
  Serial.println(state);

  if (state == HIGH)
  {
    ledpir.on();
    digitalWrite(ledp, HIGH);
  }
  else
  {
    ledpir.off();
    digitalWrite(ledp, LOW);
  }


  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, g);

  liquid_level = analogRead(sensorPin);
  Serial.print("Liquid Level : ");
  Serial.println(liquid_level);

  difference = liquid_level - bottom_level;
  ratio = difference / top_level;
  liquid_percentage = (ratio * 100); //Percentage of water in the container

  Serial.print("Liquid Percentage : ");
  Serial.println(liquid_percentage);

  Blynk.virtualWrite(V3, liquid_level);
  Blynk.virtualWrite(V4, liquid_percentage);

  if (liquid_percentage > 40)
  {
    digitalWrite(motor, LOW);
    digitalWrite(buzzer, HIGH);
    Serial.println("motor off");
  }
  else
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(motor, HIGH);
    Serial.println("motor on");
  }
  BLYNK_WRITE(V9);
  BLYNK_WRITE(V8);
}


BLYNK_WRITE(V8) {
  int temperature_button = param.asInt();

  if (temperature_button == 255) {
    ledtemp.on();
    digitalWrite(ledt, HIGH);
  }
  if (temperature_button == 0) {
    digitalWrite(ledt, LOW);
    ledtemp.off();
  }
}

BLYNK_WRITE(V9) {
  int pir_button = param.asInt();

  if (pir_button == 255) {
    ledpir.on();
    digitalWrite(ledp, HIGH);
  }
  if (pir_button == 0) {
    ledpir.off();
    digitalWrite(ledp, LOW);
  }
}
