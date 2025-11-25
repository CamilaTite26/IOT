#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";
HTTPClient httpClient;
WiFiClient wClient;
String URL = "http://10.22.194.2:5074/Sensores";

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DT  D2
#define SCK D1

HX711 scale;

const float referenciaPeso = 210.0;

long offset = 0;
float calibration_factor = 1.0;
float prevWeight = 0;

void setup() {

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Wire.begin(D5, D6);  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(1000);

  scale.begin(DT, SCK);

  calibrarCelda(referenciaPeso);

}

void loop() {
  if (scale.is_ready()) {
    long reading = scale.read();
    float weight = (reading - offset) / calibration_factor;

    if (weight < 0.5) weight = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Peso: ");
    lcd.print(weight);
    lcd.print(" g");

    if(abs(prevWeight - weight) >= 10){
      float diference = weight - prevWeight;
      guardarBD(weight, diference);
    }

    prevWeight = weight;

  } else {
    lcd.setCursor(0, 0);
    lcd.print("HX711 no listo  ");
  }
  delay(2500);
}

void calibrarCelda(float pesoReferencia) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tomando offset");
  delay(1500);

  offset = scale.read();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Offset:");
  lcd.setCursor(0, 1);
  lcd.print(offset);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pon el peso...");
  delay(5000);

  long lecturaConPeso = scale.read();

  calibration_factor = (float)(lecturaConPeso - offset) / pesoReferencia;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calib listo");
  lcd.setCursor(0, 1);
  lcd.print(calibration_factor, 6);
  delay(2000);

  lcd.clear();
}

void guardarBD(float weight, float change){
  if(WiFi.status() == WL_CONNECTED){
    String pesoStr = String(weight);
    String changeStr = String(change);

    String postData;
    postData = "{\"Peso_detectado\":" + pesoStr +
    ", \"Cambio\":" + changeStr +
    ", \"Id_estante\": \"EST001\"}";

    httpClient.begin(wClient, URL);
    httpClient.addHeader("Content-Type", "application/json");

    int httpCode = httpClient.POST(postData);
 
    httpClient.end(); 
  }
  return;
}
