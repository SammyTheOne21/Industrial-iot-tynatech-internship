/*
 * LoRaWAN OTAA Connection
 * 
 * Purpose: Connect to The Things Network using OTAA
 * OTAA = Over The Air Activation (secure, recommended)
 * 
 * Hardware: Heltec WiFi LoRa 32 V3
 * 
 * Prerequisites:
 * 1. Create account on The Things Network (thethingsnetwork.org)
 * 2. Register application
 * 3. Register device
 * 4. Get keys: AppEUI, DevEUI, AppKey
 * 
 * Author: Samrat Sharma
 * Date: January 2026
 */

// NOTE: This is a template - you'll add your keys on Day 7

/*
 * LORAWAN ARCHITECTURE:
 * 
 * Your Device → LoRa Gateway → Network Server → Application Server
 * (This ESP32)  (TTN Gateway)  (The Things Network)  (Your App)
 * 
 * ACTIVATION METHODS:
 * 
 * OTAA (Over The Air Activation):
 * - Device joins network dynamically
 * - Secure session keys generated
 * - Recommended method
 * - This code!
 * 
 * ABP (Activation By Personalization):
 * - Keys pre-configured
 * - No join procedure
 * - Less secure
 * - Faster startup
 * 
 * DEVICE CLASSES:
 * 
 * Class A (your device):
 * - Lowest power
 * - Uplink when needed
 * - Downlink after uplink only
 * - Perfect for sensors
 * 
 * Class B:
 * - Scheduled downlinks
 * - Beacon synchronized
 * - Medium power
 * 
 * Class C:
 * - Always listening
 * - Highest power
 * - For actuators
 * 
 * TTN SETUP STEPS (Day 7):
 * 
 * 1. Go to console.thethingsnetwork.org
 * 2. Create account
 * 3. Create application
 * 4. Add end device
 * 5. Choose: Manually register device
 * 6. Select: Heltec WiFi LoRa 32 V3
 * 7. Choose: OTAA
 * 8. Copy three keys:
 *    - AppEUI (8 bytes)
 *    - DevEUI (8 bytes)  
 *    - AppKey (16 bytes)
 * 9. Paste keys in code below
 * 10. Upload and join!
 * 
 * CODE TEMPLATE:
 */

void setup() {
  Serial.begin(115200);
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      LoRaWAN OTAA Join Template       ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("⚠ This is a template file");
  Serial.println("\nSteps to complete on Day 7:");
  Serial.println("1. Register on The Things Network");
  Serial.println("2. Create application");
  Serial.println("3. Add device (OTAA)");
  Serial.println("4. Get AppEUI, DevEUI, AppKey");
  Serial.println("5. Add keys to this code");
  Serial.println("6. Install 'MCCI LoRaWAN LMIC library'");
  Serial.println("7. Use library examples as base");
  Serial.println("\nRecommended library:");
  Serial.println("'MCCI LoRaWAN LMIC library' by IBM");
}

void loop() {
  delay(10000);
}

/*
 * KEYS EXPLAINED:
 * 
 * DevEUI (Device EUI):
 * - Unique device identifier
 * - Like a MAC address
 * - 8 bytes / 16 hex characters
 * - Example: 70B3D57ED005B4E3
 * 
 * AppEUI (Application EUI):
 * - Identifies your application
 * - 8 bytes / 16 hex characters
 * - Often all zeros for TTN
 * - Example: 0000000000000000
 * 
 * AppKey (Application Key):
 * - Secret key for encryption
 * - 16 bytes / 32 hex characters
 * - NEVER share publicly!
 * - Example: 2B7E151628AED2A6ABF7158809CF4F3C
 * 
 * SECURITY:
 * 
 * - AppKey never transmitted
 * - Session keys derived during join
 * - All messages encrypted (AES-128)
 * - Message integrity checks
 * - Replay protection
 * 
 * DATA RATES:
 * 
 * DR0: SF12/125kHz - 250 bps - Max range
 * DR1: SF11/125kHz - 440 bps
 * DR2: SF10/125kHz - 980 bps
 * DR3: SF9/125kHz  - 1760 bps
 * DR4: SF8/125kHz  - 3125 bps
 * DR5: SF7/125kHz  - 5470 bps - Min range
 * 
 * ADR (Adaptive Data Rate):
 * Network automatically adjusts:
 * - Data rate
 * - TX power
 * - Based on link quality
 * - Optimizes airtime
 * 
 * DUTY CYCLE (Europe):
 * 
 * 868 MHz band: 1% duty cycle
 * Means: Transmit max 36 seconds per hour
 * 
 * Example:
 * - 1 message takes 0.5 seconds
 * - Can send 72 messages per hour
 * - Or 1 message every 50 seconds
 * 
 * FAIR ACCESS POLICY:
 * TTN limits:
 * - 30 seconds uplink airtime per day
 * - 10 downlink messages per day
 * 
 * Design accordingly:
 * - Send data every 5-10 minutes
 * - Not every few seconds!
 * 
 * PAYLOAD SIZE:
 * 
 * Depends on data rate:
 * DR0: 51 bytes max
 * DR5: 242 bytes max
 * 
 * Keep payloads small:
 * - 10-20 bytes typical
 * - Use efficient encoding
 * - Binary better than JSON
 * 
 * YOUR APPLICATION (Day 7):
 * 
 * Sensor node:
 * - Read DHT22, ACS712, BMP280
 * - Format data efficiently
 * - Send every 10 minutes
 * - Display on TTN console
 * - Low power sleep between sends
 * 
 * Gateway:
 * - Receive Modbus data from sensors
 * - Aggregate data
 * - Send via LoRaWAN
 * - Acts as IoT bridge
 */
