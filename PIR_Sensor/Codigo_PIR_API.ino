#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define clockPin 14  
#define dataPin  4   
#define latchPin 5   
#define PIRPin   12  
#define PIRPin2  13   

int numOfRegisters = 2;
byte* registerState;

long effectId = 0;
long prevEffect = 0;
long effectRepeat = 0;
long effectSpeed = 30;


const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";
HTTPClient httpClient;
WiFiClient wClient;
String URL = "http://10.22.128.153:5074/Sensores/SensorMovimiento"; 

int estadoAnterior = LOW;                    

void setup() {
  Serial.begin(9600);

  
  registerState = new byte[numOfRegisters];
  for (size_t i = 0; i < numOfRegisters; i++) {
    registerState[i] = 0;
  }

 
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(PIRPin, INPUT); 
  pinMode(PIRPin2,INPUT);


  Serial.println("***Inicializando WiFi***");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a red WiFi \"");
  Serial.print(ssid);
  Serial.print("\"");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConectado! IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int movimiento1 = digitalRead(PIRPin); 
  int movimiento2 = digitalRead(PIRPin2); 
  int movimiento = (movimiento1 == HIGH && movimiento2 == HIGH) ? HIGH : LOW;

  
  if (movimiento==HIGH && estadoAnterior==LOW){
    Serial.println("Nuevo acercamiento detectado!");
    logIntento(1);
  }        
  

  if (movimiento == HIGH) {
    Serial.println("Movimiento detectado");

   
    do {
      effectId = random(6);
    } while (effectId == prevEffect);
    prevEffect = effectId;

    
    switch (effectId) {
      case 0: effectRepeat = random(1, 2); break;
      case 1: effectRepeat = random(1, 2); break;
      case 3: effectRepeat = random(1, 5); break;
      case 4: effectRepeat = random(1, 2); break;
      case 5: effectRepeat = random(1, 2); break;
    }

   
    for (int i = 0; i < effectRepeat; i++) {
      effectSpeed = random(10, 70);
      switch (effectId) {
        case 0: effectA(effectSpeed); break;
        case 1: effectB(effectSpeed); break;
        case 3: effectC(effectSpeed); break;
        case 4: effectD(effectSpeed); break;
        case 6: effectE(effectSpeed); break;
      }
    }

  } else {
    for (int i = 0; i < 16; i++) {
      regWrite(i, LOW);
    }
  }
}

void logIntento(int x){
  if(WiFi.status() == WL_CONNECTED){
    String postData = "{\"State_movement\":" + String(x) + "}";
    Serial.print("Post Data String: ");
    Serial.println(postData);

    httpClient.begin(wClient, URL);
    httpClient.addHeader("Content-Type", "application/json");
    int httpCode = httpClient.POST(postData);

    Serial.print("Response Code: ");
    Serial.println(httpCode);
    httpClient.end();
  }
}

void effectA(int speed) {
  for (int i = 0; i < 16; i++) {
    for (int k = i; k < 16; k++) {
      regWrite(k, HIGH);
      delay(speed);
      regWrite(k, LOW);
    }
    regWrite(i, HIGH);
  }
}

void effectB(int speed) {
  for (int i = 15; i >= 0; i--) {
    for (int k = 0; k < i; k++) {
      regWrite(k, HIGH);
      delay(speed);
      regWrite(k, LOW);
    }
    regWrite(i, HIGH);
  }
}

void effectC(int speed) {
  int prevI = 0;
  for (int i = 0; i < 16; i++) {
    regWrite(prevI, LOW);
    regWrite(i, HIGH);
    prevI = i;
    delay(speed);
  }

  for (int i = 15; i >= 0; i--) {
    regWrite(prevI, LOW);
    regWrite(i, HIGH);
    prevI = i;
    delay(speed);
  }
}

void effectD(int speed) {
  for (int i = 0; i < 8; i++) {
    for (int k = i; k < 8; k++) {
      regWrite(k, HIGH);
      regWrite(15 - k, HIGH);
      delay(speed);
      regWrite(k, LOW);
      regWrite(15 - k, LOW);
    }
    regWrite(i, HIGH);
    regWrite(15 - i, HIGH);
  }
}

void effectE(int speed) {
  for (int i = 7; i >= 0; i--) {
    for (int k = 0; k <= i; k++) {
      regWrite(k, HIGH);
      regWrite(15 - k, HIGH);
      delay(speed);
      regWrite(k, LOW);
      regWrite(15 - k, LOW);
    }
    regWrite(i, HIGH);
    regWrite(15 - i, HIGH);
  }
}

void regWrite(int pin, bool state) {
  int reg = pin / 8; 
  int actualPin = pin - (8 * reg);  
  digitalWrite(latchPin, LOW); 

  for (int i = 0; i < numOfRegisters; i++) {
    byte* states = &registerState[i];
    if (i == reg) {
      bitWrite(*states, actualPin, state);
    }
    shiftOut(dataPin, clockPin, MSBFIRST, *states);
  }

  digitalWrite(latchPin, HIGH); 
}

