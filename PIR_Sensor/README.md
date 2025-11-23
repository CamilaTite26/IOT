
**PIR Sensor Overview**
- **Name:** PIR sensor + LED strip logger
- **Code file:** `Codigo_PIR_API.ino`
- **Purpose:** Detect nearby movement with two PIR sensors, play LED effects on a strip driven by shift registers, and send a small JSON POST to a server every time a new approach is detected so a remote dashboard can count visits.

**Hardware**
- **Microcontroller:** ESP8266 (code uses ESP8266 libraries)
- **Sensors:** Two PIR motion sensors (used together — both must read HIGH to consider motion present)
- **Actuators:** LED strip composed of individual LEDs driven by shift registers (the sketch is configured for 2 registers = 16 outputs). 
- **Shift register model:** the code expects generic 8-bit shift registers 74HC595 or similar. 
**Wiring / Pin Mapping**
- **Clock pin:** `clockPin = 14`
- **Data pin:** `dataPin = 4`
- **Latch pin:** `latchPin = 5`
- **PIR sensor 1:** `PIRPin = 12`
- **PIR sensor 2:** `PIRPin2 = 13`

Note: The code sets `numOfRegisters = 2`, which provides 16 outputs. If your LED strip has more LEDs (e.g., 26), add more shift registers and update `numOfRegisters`.

**How the Code Works**
- **Motion detection:** The sketch reads both PIR sensors. It treats motion as present when both sensors are HIGH. A transition from LOW to HIGH is considered a "new approach" and triggers an HTTP POST to the configured server.
- **LED effects:** When motion is present the code selects a random effect (several animations are implemented) and runs it a random number of times. When no motion is present the code clears the outputs.
- **State handling:** The code keeps a small in-memory state for the shift registers (`registerState`) and writes outputs using `shiftOut` and `latchPin`.

**API / Server**
- **Server URL:** configured in the sketch via the `URL` variable. Default in the sketch: `http://10.22.128.153:5074/Sensores/SensorMovimiento`.
- **HTTP method:** POST
- **Content-Type:** `application/json`
- **Payload:** `{"State_movement": 1}` is sent when a new approach is detected. The server response code is printed to Serial for debugging.

**Configuration**
- **WiFi:** set your WiFi network in the sketch: `ssid` and `password` variables.
- **Server endpoint:** set the `URL` variable to point to your API endpoint.
- **Number of outputs:** change `numOfRegisters` to match your chain of shift registers: each register adds 8 outputs.
- **Serial debug:** `Serial.begin(9600)` — open Serial Monitor at 9600 baud to view connection logs and HTTP responses.

**Uploading / Getting Started**
- Open `Codigo_PIR_API.ino` in the Arduino IDE (or PlatformIO configured for ESP8266).
- Update `ssid`, `password`, and `URL` to match your network and server.
- Build and upload to your ESP8266 module.
- Open Serial Monitor (9600 baud) to follow WiFi connection and HTTP POST logs.

**Troubleshooting**
- If the ESP never connects to WiFi: verify SSID/password, WiFi mode (`WIFI_STA`) and that the network is available on 2.4 GHz (ESP8266 does not support 5 GHz).
- If HTTP POSTs return errors: check the `URL` is reachable from the ESP network and that the server accepts POST with `application/json`.
- If LEDs don't behave as expected: verify shift-register wiring (clock/data/latch) and that `numOfRegisters` equals the number of chained 8-bit registers. Also check the power supply for the LEDs.
- If motion detection is unreliable: adjust PIR sensor placement and sensitivity (some PIR modules include a potentiometer), and ensure both PIRs have a clear field of view for the monitored area.
