# Weight Sensor Overview

-   **Name:** Weight sensor + LCD + WiFi API
-   **Code file:** `Codigo_Weight_API.ino`
-   **Purpose:** Read the weight of a shelf using a load cell
    with HX711 module, display the value on an I2C LCD screen, and send
    readings to a server whenever a significant weight change is detected.

# Hardware

## Microcontroller

-   **ESP8266** (uses WiFi libraries for ESP8266)

## Sensors / Modules

-   **HX711** --- ADC module for load cell
-   **Load cell** 5--20 kg capacity
-   **LCD I2C 16x2** (address `0x27`)

## Network

-   WiFi connection to the **Tec-IoT** network
-   Sends POST to the server configured in `URL`

# Wiring / Pin Mapping

## HX711

  Function   ESP8266 Pin
  --------- -------------
  DT        `D2`
  SCK       `D1`

## LCD I2C

  Function   ESP8266 Pin
  --------- -------------
  SDA       `D5`
  SCL       `D6`

# How the Code Works

## 1. WiFi Setup

The ESP8266 connects in station mode (WIFI_STA) to the configured network with:

``` cpp
const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";
```

## 2. LCD Initialization

The 16x2 screen is configured via I2C to display status messages and readings.

## 3. HX711 Calibration

The `calibrarCelda()` function: - Reads the **offset** without weight
- Requests placing a **reference weight** (210 g by default)
- Calculates the **calibration_factor** using:

    calibration = (lecturaConPeso - offset) / pesoReferencia

## 4. Weight Reading

In each loop iteration: - If the reading is negative or small, it is
forced to 0. - It is displayed on the LCD:\
`Weight: XX g`

## 5. Change Detection

If the difference compared to the previous reading is greater than or equal to
**10 g**, it is considered a real change and triggers a POST to the server:

``` cpp
if(abs(prevWeight - weight) >= 10)
```

## 6. Sending to the API

The `guardarBD()` function sends a POST with JSON:

``` json
{
  "Peso_detectado": xx,
  "Cambio": yy,
  "Id_estante": "EST001"
}
```

# API / Server

-   **Server URL:** configured in
    `String URL = "http://10.22.194.2:5074/Sensores";`
-   **HTTP method:** `POST`
-   **Content-Type:** `application/json`
-   **Payload:** current weight, change, and shelf ID.
# Configuration

-   Change `ssid` and `password` for your network.
-   Adjust `URL` for your API.
-   You can change the **reference weight** for calibration:

``` cpp
const float referenciaPeso = 210.0;
```

# Uploading / Getting Started

1.  Open the `.ino` file in Arduino IDE or PlatformIO
2.  Install libraries if not already present:
    -   `HX711`
    -   `LiquidCrystal_I2C`
    -   ESP8266 core
3.  Configure WiFi and URL
4.  Upload the code to the ESP8266
5.  Place the reference weight when prompted
6.  Observe readings on the LCD and in your database

# Troubleshooting

## If floating weights or large changes occur:
-   Check that the load cell is fixed and free of vibrations.
-   Adjust the **threshold** in:

``` cpp
if(abs(prevWeight - weight) >= 10)
```

## If WiFi does not connect:

-   Verify SSID/password
-   Ensure it is **2.4 GHz**
-   Ensure the ESP has a good signal

## If the LCD does not turn on:

-   Check SDA/SCL in `Wire.begin(D5, D6)`
-   Confirm the I2C address (`0x27` or `0x3F`)

## If the server does not register data:

-   Verify that the API accepts JSON
-   Test the URL with Swagger or similar tools
-   Verify that the ESP is on the same network as the server