/*
 * LoRa Sender - Basic Transmission
 * 
 * Purpose: Send data using LoRa radio
 * This is your Heltec ESP32's built-in LoRa!
 * 
 * Hardware: Heltec WiFi LoRa 32 V3
 * 
 * Important: This is LoRa (not LoRaWAN yet)
 * Day 4: Test LoRa radio functionality
 * Day 7: Upgrade to full LoRaWAN
 * 
 * Author: [Your Name]
 * Date: January 2026
 */

#include <LoRa.h>
#include <Wire.h>

// LoRa pins for Heltec ESP32 V3
#define LORA_SCK 9
#define LORA_MISO 11
#define LORA_MOSI 10
#define LORA_SS 8
#define LORA_RST 12
#define LORA_DIO0 14

// LoRa frequency (choose based on region)
#define LORA_BAND 868E6  // Europe: 868 MHz
// #define LORA_BAND 915E6  // North America: 915 MHz
// #define LORA_BAND 433E6  // Asia: 433 MHz

int messageCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║         LoRa Sender Test               ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Hardware: Heltec ESP32 LoRa V3        ║");
  Serial.println("║ Frequency: 868 MHz                     ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // Initialize LoRa
  Serial.print("Initializing LoRa... ");
  
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("❌ Failed!");
    Serial.println("Check connections and frequency setting");
    while (1);
  }
  
  Serial.println("✓ Success!");
  
  // Configure LoRa parameters
  LoRa.setSpreadingFactor(12);  // SF7-SF12, higher = longer range
  LoRa.setSignalBandwidth(125E3);  // 125 kHz
  LoRa.setCodingRate4(5);  // 4/5 error correction
  LoRa.setTxPower(20);  // 20 dBm maximum
  
  Serial.println("\nLoRa Configuration:");
  Serial.println("  Spreading Factor: 12");
  Serial.println("  Bandwidth: 125 kHz");
  Serial.println("  TX Power: 20 dBm");
  Serial.println("\nSending messages every 5 seconds...\n");
}

void loop() {
  sendLoRaMessage();
  delay(5000);
}

void sendLoRaMessage() {
  messageCount++;
  
  // Create message with sensor-like data
  String message = "MSG#" + String(messageCount);
  message += " TEMP:25.5 HUM:60 RSSI:";
  
  Serial.print("📤 Sending: ");
  Serial.println(message);
  
  // Send LoRa packet
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  
  Serial.println("   ✓ Sent!");
}

/*
 * LORA BASICS:
 * 
 * LoRa = Long Range radio modulation
 * - Range: 2-15 km (urban/rural)
 * - Low power consumption
 * - Low data rate (0.3-50 kbps)
 * - Chirp Spread Spectrum modulation
 * 
 * FREQUENCY BANDS:
 * - Europe: 868 MHz
 * - North America: 915 MHz  
 * - Asia: 433 MHz
 * Check your country's regulations!
 * 
 * KEY PARAMETERS:
 * 
 * SPREADING FACTOR (SF7-SF12):
 * - Higher SF = longer range, slower speed
 * - SF12: Max range, ~250 bps
 * - SF7: Min range, ~5500 bps
 * 
 * BANDWIDTH:
 * - 125 kHz, 250 kHz, 500 kHz
 * - Lower BW = better sensitivity, slower
 * 
 * CODING RATE (4/5 to 4/8):
 * - Error correction overhead
 * - 4/5 = best speed
 * - 4/8 = best error correction
 * 
 * TX POWER:
 * - 2 to 20 dBm
 * - Higher = longer range, more power
 * 
 * LORA vs LORAWAN:
 * 
 * LoRa:
 * - Raw radio communication
 * - Point-to-point
 * - No network infrastructure
 * - This code!
 * 
 * LoRaWAN:
 * - Protocol on top of LoRa
 * - Network with gateways
 * - Encryption, authentication
 * - The Things Network
 * - Day 7 code!
 * 
 * RANGE FACTORS:
 * 
 * Good range:
 * + High spreading factor
 * + High TX power
 * + Clear line of sight
 * + Elevated antenna
 * + Good antenna match
 * 
 * Poor range:
 * - Buildings/obstacles
 * - Low TX power
 * - Poor antenna
 * - Interference
 * 
 * TESTING:
 * 
 * Without receiver:
 * - Code compiles and runs
 * - LoRa.begin() succeeds
 * - Messages sent without errors
 * 
 * With receiver (Day 4):
 * - Upload lora_receiver.ino to second ESP32
 * - Place devices apart
 * - Check received messages
 * - Test different distances
 * 
 * NEXT STEPS:
 * 
 * Day 4: Add receiver, test range
 * Day 7: Upgrade to LoRaWAN
 * Day 7: Connect to The Things Network
 * Day 7: Send real sensor data
 */