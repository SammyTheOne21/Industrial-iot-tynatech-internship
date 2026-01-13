# Hardware Setup Guide

Complete wiring and setup instructions for all components.

## Component List

### Main Controller
- **Heltec ESP32 LoRa V3**
  - Built-in LoRa radio (868/915 MHz)
  - Built-in OLED display (128x64)
  - WiFi & Bluetooth
  - 240MHz dual-core
  - 8MB Flash

### Sensors
1. **DHT22** - Temperature & Humidity
2. **ACS712-5A** - Current Sensor
3. **BMP280** - Barometric Pressure (x2)
4. **HC-SR04** - Ultrasonic Distance
5. **SW-420** - Vibration Detection

### Communication
- **RS485 to TTL Module** - Modbus RTU

### Accessories
- Breadboards
- Jumper wires
- USB-C cable
- Power supply (5V 2A recommended)

---

## ESP32 Pin Assignments

| GPIO | Function | Sensor/Module |
|------|----------|---------------|
| 2 | Digital Out | Built-in LED |
| 4 | Digital Out | RS485 DE/RE |
| 5 | Digital Out | HC-SR04 TRIG |
| 13 | Digital In | SW-420 Digital |
| 15 | Digital In | DHT22 Data |
| 16 | UART RX2 | RS485 RO |
| 17 | UART TX2 | RS485 DI |
| 18 | Digital In | HC-SR04 ECHO |
| 21 | I2C SDA | BMP280 SDA |
| 22 | I2C SCL | BMP280 SCL |
| 34 | Analog In | ACS712 / SW-420 Analog |

**Note:** GPIO 34-39 are input-only on ESP32.

---

## Wiring Diagrams

### 1. DHT22 Temperature & Humidity
```
DHT22          ESP32
─────────────────────
VCC    ───────  3.3V
DATA   ───────  GPIO 15
GND    ───────  GND

Note: 10kΩ pull-up resistor between DATA and VCC
(Usually included on DHT22 modules)
```

**Tips:**
- Use 3.3V, not 5V
- Keep wires under 20 meters
- Add 100nF capacitor near sensor for stability

---

### 2. ACS712-5A Current Sensor
```
ACS712         Voltage Divider    ESP32
───────────────────────────────────────
VCC    ─────────────────────────  5V
GND    ─────────────────────────  GND
OUT    ───  10kΩ  ───┬───────────  GPIO 34
                     │
                   10kΩ
                     │
                    GND
```

**CRITICAL:** Voltage divider required!
- ACS712 outputs 0-5V
- ESP32 max input: 3.3V
- 2:1 divider: 10kΩ + 10kΩ resistors

**Current Connections:**
- Connect load between IP+ and IP- terminals
- Polarity matters for DC (affects sign)
- For AC, polarity doesn't matter

**Safety:**
- Disconnect power before wiring
- Use appropriate wire gauge for current
- Ensure good connections
- Test with small loads first

---

### 3. BMP280 Pressure Sensor
```
BMP280         ESP32
─────────────────────
VCC    ───────  3.3V
GND    ───────  GND
SDA    ───────  GPIO 21
SCL    ───────  GPIO 22
```

**I2C Address:**
- Default: 0x76 (SDO to GND)
- Alternate: 0x77 (SDO to VCC)
- Run I2C scanner to confirm

**Tips:**
- Can connect two BMP280 with different addresses
- 4.7kΩ pull-up resistors on SDA/SCL (usually on module)
- Keep I2C wires under 1 meter

---

### 4. HC-SR04 Ultrasonic Sensor
```
HC-SR04        ESP32
─────────────────────
VCC    ───────  5V
TRIG   ───────  GPIO 5
ECHO   ───────  GPIO 18
GND    ───────  GND
```

**Note:** ECHO outputs 5V, but ESP32 GPIO 18 is 5V tolerant.

For extra safety, add voltage divider on ECHO:
```
ECHO ─── 1kΩ ───┬─── GPIO 18
                │
              2kΩ
                │
               GND
```

**Mounting:**
- Point sensor perpendicular to target
- Avoid soft/angled surfaces
- Keep clear of beam angle (~15°)

---

### 5. SW-420 Vibration Sensor
```
SW-420         ESP32
─────────────────────
VCC    ───────  3.3V (or 5V)
DO     ───────  GPIO 13
AO     ───────  GPIO 34 (optional)
GND    ───────  GND
```

**Sensitivity Adjustment:**
- Rotate potentiometer on module
- Clockwise: Less sensitive
- Counter-clockwise: More sensitive
- LED indicates detection

---

### 6. RS485 to TTL Module
```
RS485 Module   ESP32
─────────────────────
VCC    ───────  3.3V
GND    ───────  GND
DI     ───────  GPIO 17 (TX2)
RO     ───────  GPIO 16 (RX2)
DE     ───────  GPIO 4
RE     ───────  GPIO 4 (tied together)
```

**RS485 Bus Wiring:**
```
Device 1       Device 2       Device 3
  A ─────────────A─────────────A
  B ─────────────B─────────────B
 GND───────────GND───────────GND

120Ω          Optional        120Ω
 A-B          Devices          A-B
```

**Termination:**
- 120Ω resistor between A and B at both ends
- Critical for reliable communication
- Use twisted pair cable

---

## Complete System Wiring

### Power Distribution
```
USB Power (5V)
      │
      ├─── ESP32 (via USB-C)
      ├─── ACS712 VCC
      └─── HC-SR04 VCC

ESP32 3.3V Pin
      ├─── DHT22 VCC
      ├─── BMP280 VCC
      ├─── SW-420 VCC
      └─── RS485 Module VCC

Common GND
      ├─── All sensor GNDs
      ├─── ESP32 GND
      └─── Power supply GND
```

**Power Requirements:**
- ESP32: ~500mA peak (WiFi/LoRa active)
- All sensors: ~100mA total
- Recommended: 5V 2A power supply
- For battery: 2000mAh+ Li-ion

---

## Assembly Steps

### Step 1: Prepare Breadboard
1. Place ESP32 on breadboard (center)
2. Ensure all pins accessible
3. Connect power rails (red = 3.3V/5V, black = GND)

### Step 2: Connect Power
1. USB-C to ESP32
2. 5V rail to breadboard from ESP32 VIN or external
3. 3.3V rail from ESP32 3.3V pin
4. GND rail common

### Step 3: Connect Digital Sensors
1. DHT22 to GPIO 15
2. SW-420 to GPIO 13
3. HC-SR04: TRIG=5, ECHO=18

### Step 4: Connect I2C Sensors
1. BMP280 to SDA=21, SCL=22
2. Verify with I2C scanner

### Step 5: Connect Analog Sensors
1. ACS712 with voltage divider to GPIO 34

### Step 6: Connect RS485 (Optional)
1. RS485 module to UART2 (GPIO 16/17)
2. DE/RE to GPIO 4

### Step 7: Test Each Sensor
1. Upload individual test codes
2. Verify readings
3. Fix any wiring issues

### Step 8: Upload Complete System
1. All sensors working individually
2. Upload multi-sensor dashboard
3. Monitor all readings

---

## Troubleshooting

### Sensor Not Found
- Check power connections (VCC, GND)
- Verify correct GPIO pin
- Run I2C scanner for I2C devices
- Check sensor module LED (if present)

### Incorrect Readings
- Verify voltage levels (3.3V vs 5V)
- Check for loose connections
- Add decoupling capacitors (100nF)
- Keep wires short and neat

### Intermittent Issues
- Check breadboard connections
- Solder headers if needed
- Use quality jumper wires
- Avoid long wire runs

### No Serial Output
- Check baud rate (115200)
- Verify USB cable (data, not charge-only)
- Install CP210x drivers if needed
- Try different USB port

---

## Safety Guidelines

### Electrical Safety
- ⚠️ Disconnect power before wiring
- ⚠️ Check polarity before connecting
- ⚠️ Use voltage dividers where needed
- ⚠️ Don't exceed GPIO voltage limits (3.3V max)

### ACS712 Safety (High Current)
- ⚠️ Use appropriate wire gauge
- ⚠️ Ensure good screw terminal connections
- ⚠️ Start testing with low currents
- ⚠️ Don't exceed 5A rating
- ⚠️ Watch for heating

### ESD Protection
- Touch grounded object before handling ESP32
- Use anti-static mat if available
- Store in anti-static bag when not in use

---

## Next Steps

After hardware setup:
1. ✅ Verify each sensor individually
2. ✅ Run multi-sensor dashboard
3. ✅ Test LoRa transmission (Day 7)
4. ✅ Implement Modbus gateway (Day 6)
5. ✅ Deploy complete system (Day 7-8)

---

## Support Resources

- **ESP32 Pinout:** [esp32.com/pinout](https://esp32.com/pinout)
- **Sensor Datasheets:** See `08_Resources/datasheets/`
- **Troubleshooting:** See `07_Documentation/troubleshooting.md`
- **Forum:** [forum.arduino.cc](https://forum.arduino.cc)