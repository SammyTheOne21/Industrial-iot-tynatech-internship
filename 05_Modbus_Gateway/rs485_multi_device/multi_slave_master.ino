/*
 * Modbus Master - Multiple Slave Devices
 * 
 * Purpose: Poll multiple Modbus slaves on same bus
 * Industrial IoT gateway pattern
 * 
 * Slaves:
 * - Device 1 (Addr 1): Temperature sensor
 * - Device 2 (Addr 2): Humidity sensor  
 * - Device 3 (Addr 3): Pressure sensor
 * 
 * Author: Samrat Sharma
 * Date: January 2026
 */

#include <ModbusMaster.h>

#define RX2 16
#define TX2 17
#define DE_RE 4

// Create Modbus instances for each slave
ModbusMaster slave1;
ModbusMaster slave2;
ModbusMaster slave3;

// Data storage
struct SensorData {
  float temperature;
  float humidity;
  float pressure;
  bool valid;
} sensorData[3];

unsigned long previousMillis = 0;
const long interval = 5000;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);
  
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);
  
  // Initialize all slaves
  slave1.begin(1, Serial2);
  slave2.begin(2, Serial2);
  slave3.begin(3, Serial2);
  
  // Set callbacks
  slave1.preTransmission(preTransmission);
  slave1.postTransmission(postTransmission);
  slave2.preTransmission(preTransmission);
  slave2.postTransmission(postTransmission);
  slave3.preTransmission(preTransmission);
  slave3.postTransmission(postTransmission);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   Multi-Slave Modbus Master            ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Polling 3 slaves every 5 seconds      ║");
  Serial.println("╚════════════════════════════════════════╝\n");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    pollAllSlaves();
    displayData();
  }
}

void pollAllSlaves() {
  Serial.println("═══ Polling Slaves ═══");
  
  // Poll slave 1
  uint8_t result = slave1.readHoldingRegisters(100, 1);
  if (result == slave1.ku8MBSuccess) {
    sensorData[0].temperature = slave1.getResponseBuffer(0) / 10.0;
    sensorData[0].valid = true;
    Serial.println("✓ Slave 1: Success");
  } else {
    sensorData[0].valid = false;
    Serial.println("✗ Slave 1: Failed");
  }
  delay(100);
  
  // Poll slave 2
  result = slave2.readHoldingRegisters(100, 1);
  if (result == slave2.ku8MBSuccess) {
    sensorData[1].humidity = slave2.getResponseBuffer(0) / 10.0;
    sensorData[1].valid = true;
    Serial.println("✓ Slave 2: Success");
  } else {
    sensorData[1].valid = false;
    Serial.println("✗ Slave 2: Failed");
  }
  delay(100);
  
  // Poll slave 3
  result = slave3.readHoldingRegisters(100, 1);
  if (result == slave3.ku8MBSuccess) {
    sensorData[2].pressure = slave3.getResponseBuffer(0) / 10.0;
    sensorData[2].valid = true;
    Serial.println("✓ Slave 3: Success");
  } else {
    sensorData[2].valid = false;
    Serial.println("✗ Slave 3: Failed");
  }
  delay(100);
}

void displayData() {
  Serial.println("\n┌─────────────────────────────────────┐");
  Serial.println("│      AGGREGATED SENSOR DATA         │");
  Serial.println("├─────────────────────────────────────┤");
  
  if (sensorData[0].valid) {
    Serial.print("│ Temperature: ");
    Serial.print(sensorData[0].temperature, 1);
    Serial.println(" °C              │");
  } else {
    Serial.println("│ Temperature: N/A                    │");
  }
  
  if (sensorData[1].valid) {
    Serial.print("│ Humidity: ");
    Serial.print(sensorData[1].humidity, 1);
    Serial.println(" %                 │");
  } else {
    Serial.println("│ Humidity: N/A                       │");
  }
  
  if (sensorData[2].valid) {
    Serial.print("│ Pressure: ");
    Serial.print(sensorData[2].pressure, 1);
    Serial.println(" hPa            │");
  } else {
    Serial.println("│ Pressure: N/A                       │");
  }
  
  Serial.println("└─────────────────────────────────────┘\n");
}

void preTransmission() {
  digitalWrite(DE_RE, HIGH);
  delayMicroseconds(50);
}

void postTransmission() {
  delayMicroseconds(50);
  digitalWrite(DE_RE, LOW);
}

/*
 * MULTI-SLAVE POLLING STRATEGY:
 * 
 * SEQUENTIAL POLLING:
 * 1. Query Slave 1
 * 2. Wait for response
 * 3. Query Slave 2
 * 4. Wait for response
 * 5. Query Slave 3
 * 6. Wait for response
 * 
 * Delay between queries prevents bus collisions
 * 
 * TIMING CONSIDERATIONS:
 * 
 * At 9600 baud:
 * - Query frame: ~11 bytes = ~12ms
 * - Response frame: ~7 bytes = ~8ms
 * - Total per slave: ~20ms + delays
 * 
 * For 3 slaves: ~100ms total
 * Can poll 10 times per second
 * 
 * SCALING:
 * 
 * More slaves = longer polling time
 * 10 slaves at 9600 baud ≈ 300ms
 * Still acceptable for most applications
 * 
 * For faster polling:
 * - Increase baud rate (19200, 38400)
 * - Poll only changed values
 * - Use Modbus TCP instead
 * 
 * ERROR HANDLING:
 * 
 * If slave doesn't respond:
 * - Mark data as invalid
 * - Continue with other slaves
 * - Don't block entire system
 * - Log error for troubleshooting
 * 
 * GATEWAY PATTERN:
 * 
 * This ESP32 acts as gateway:
 * Modbus Slaves ──RS485──> ESP32 ──LoRaWAN──> Cloud
 * 
 * Benefits:
 * - Legacy Modbus devices get IoT connectivity
 * - No device modification needed
 * - Single LoRaWAN connection
 * - Centralized data aggregation
 * 
 * TYNATECH APPLICATION:
 * 
 * Real scenario:
 * - Slave 1: Energy meter (Modbus)
 * - Slave 2: Temperature controller (Modbus)
 * - Slave 3: PLC (Modbus)
 * 
 * ESP32 gateway:
 * - Polls all devices
 * - Aggregates data
 * - Sends via LoRaWAN
 * - Battery powered
 * - Remote facility monitoring
 */
