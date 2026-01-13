/*
 * Modbus RTU Master - ESP32 Implementation
 * 
 * Purpose: ESP32 as Modbus Master reading slave devices
 * Uses RS485 for communication
 * 
 * Hardware:
 * - ESP32 (Heltec WiFi LoRa 32 V3)
 * - RS485 to TTL module
 * - Modbus slave devices (sensors, PLCs, etc.)
 * 
 * Connections:
 * ESP32 GPIO 17 (TX2) → RS485 DI
 * ESP32 GPIO 16 (RX2) → RS485 RO
 * ESP32 GPIO 4 → RS485 DE/RE
 * RS485 A/B → Modbus bus
 * 
 * Author: Samrat Sharma
 * Date: January 2026
 */

#include <ModbusMaster.h>

// RS485 control
#define RX2 16
#define TX2 17
#define DE_RE 4

// Modbus object
ModbusMaster node;

// Timing
unsigned long previousMillis = 0;
const long interval = 2000;

// Statistics
int successfulReads = 0;
int failedReads = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize RS485 control pin
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);  // Receive mode
  
  // Initialize Modbus communication
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);
  node.begin(1, Serial2);  // Slave ID 1
  
  // Callbacks for RS485 control
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║     Modbus RTU Master (ESP32)         ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Baud: 9600, 8N1                       ║");
  Serial.println("║ Slave Address: 1                      ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("Reading registers from slave device...\n");
  Serial.println("Time\tReg100\tReg101\tReg102\tStatus");
  Serial.println("─────────────────────────────────────────────────");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    readModbusRegisters();
  }
}

void readModbusRegisters() {
  uint8_t result;
  uint16_t data[3];
  
  // Read 3 holding registers starting at address 100
  result = node.readHoldingRegisters(100, 3);
  
  Serial.print(millis() / 1000);
  Serial.print("\t");
  
  if (result == node.ku8MBSuccess) {
    // Success - read data
    data[0] = node.getResponseBuffer(0);
    data[1] = node.getResponseBuffer(1);
    data[2] = node.getResponseBuffer(2);
    
    Serial.print(data[0]);
    Serial.print("\t");
    Serial.print(data[1]);
    Serial.print("\t");
    Serial.print(data[2]);
    Serial.println("\t✓ Success");
    
    successfulReads++;
    
    // Interpret data (example)
    float temperature = data[0] / 10.0;  // Temp in 0.1°C
    float humidity = data[1] / 10.0;     // Humidity in 0.1%
    float pressure = data[2] / 10.0;     // Pressure in 0.1 hPa
    
    Serial.print("\t\tDecoded: Temp=");
    Serial.print(temperature);
    Serial.print("°C, Hum=");
    Serial.print(humidity);
    Serial.print("%, Press=");
    Serial.print(pressure);
    Serial.println("hPa");
    
  } else {
    // Error
    Serial.print("---\t---\t---\t❌ Error: ");
    printModbusError(result);
    failedReads++;
  }
  
  // Summary every 30 reads
  if ((successfulReads + failedReads) % 30 == 0) {
    printSummary();
  }
}

void preTransmission() {
  digitalWrite(DE_RE, HIGH);  // Transmit mode
  delayMicroseconds(50);
}

void postTransmission() {
  delayMicroseconds(50);
  digitalWrite(DE_RE, LOW);   // Receive mode
}

void printModbusError(uint8_t result) {
  switch(result) {
    case node.ku8MBIllegalFunction:
      Serial.println("Illegal Function");
      break;
    case node.ku8MBIllegalDataAddress:
      Serial.println("Illegal Data Address");
      break;
    case node.ku8MBIllegalDataValue:
      Serial.println("Illegal Data Value");
      break;
    case node.ku8MBSlaveDeviceFailure:
      Serial.println("Slave Device Failure");
      break;
    case node.ku8MBInvalidSlaveID:
      Serial.println("Invalid Slave ID");
      break;
    case node.ku8MBInvalidFunction:
      Serial.println("Invalid Function");
      break;
    case node.ku8MBResponseTimedOut:
      Serial.println("Response Timeout");
      break;
    case node.ku8MBInvalidCRC:
      Serial.println("Invalid CRC");
      break;
    default:
      Serial.print("Unknown (");
      Serial.print(result);
      Serial.println(")");
  }
}

void printSummary() {
  int totalReads = successfulReads + failedReads;
  float successRate = (successfulReads / (float)totalReads) * 100;
  
  Serial.println("\n┌─────────────────────────────────────┐");
  Serial.println("│     MODBUS COMMUNICATION STATS      │");
  Serial.println("├─────────────────────────────────────┤");
  Serial.print("│ Successful: ");
  Serial.print(successfulReads);
  Serial.println("                      │");
  Serial.print("│ Failed: ");
  Serial.print(failedReads);
  Serial.println("                          │");
  Serial.print("│ Success Rate: ");
  Serial.print(successRate, 1);
  Serial.println("%               │");
  Serial.println("└─────────────────────────────────────┘\n");
}

/*
 * MODBUS MASTER FUNCTIONS:
 * 
 * READ FUNCTIONS:
 * - readCo
/*
 * MODBUS MASTER FUNCTIONS:
 * 
 * READ FUNCTIONS:
 * - readCoils(address, quantity)
 * - readDiscreteInputs(address, quantity)
 * - readHoldingRegisters(address, quantity)
 * - readInputRegisters(address, quantity)
 * 
 * WRITE FUNCTIONS:
 * - writeSingleCoil(address, value)
 * - writeSingleRegister(address, value)
 * - writeMultipleRegisters(address, quantity, buffer)
 * 
 * EXAMPLE USAGE:
 * 
 * // Read temperature from register 100
 * result = node.readHoldingRegisters(100, 1);
 * if (result == node.ku8MBSuccess) {
 *   uint16_t temp = node.getResponseBuffer(0);
 * }
 * 
 * // Write setpoint to register 200
 * result = node.writeSingleRegister(200, 250);
 * 
 * MULTI-SLAVE SETUP:
 * 
 * To communicate with multiple slaves:
 * 
 * ModbusMaster slave1;
 * ModbusMaster slave2;
 * 
 * slave1.begin(1, Serial2);  // Address 1
 * slave2.begin(2, Serial2);  // Address 2
 * 
 * COMPANY'S APPLICATION:
 * 
 * ESP32 Modbus Master reads:
 * - Slave 1: Temperature sensor
 * - Slave 2: Humidity sensor
 * - Slave 3: Pressure sensor
 * 
 * Then aggregates and sends via LoRaWAN
 * → Complete IoT gateway solution!
 * 
 * TROUBLESHOOTING:
 * 
 * Timeout errors:
 * → Check RS485 wiring (A/B not swapped)
 * → Verify baud rate matches slave
 * → Check slave device address
 * → Add termination resistors (120Ω)
 * 
 * CRC errors:
 * → Electrical noise on bus
 * → Use shielded cable
 * → Check ground connections
 * → Reduce baud rate
 */
