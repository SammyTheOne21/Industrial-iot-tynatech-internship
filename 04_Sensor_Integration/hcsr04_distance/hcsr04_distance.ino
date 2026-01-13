/*
 * HC-SR04 Ultrasonic Distance Sensor
 * 
 * Purpose: Measure distance using ultrasonic waves
 * Common for level monitoring, proximity detection
 * 
 * Hardware:
 * - ESP32
 * - HC-SR04 sensor (arrives Jan 13)
 * 
 * Connections:
 * HC-SR04 → ESP32
 * VCC → 5V
 * TRIG → GPIO 5
 * ECHO → GPIO 18
 * GND → GND
 * 
 * Note: ECHO outputs 5V, but ESP32 GPIO is 5V tolerant
 * For safety, can use voltage divider on ECHO
 * 
 * Author: [Your Name]
 * Date: January 2026
 */

// Pin definitions
#define TRIG_PIN 5
#define ECHO_PIN 18

// Configuration
const long TIMEOUT_US = 30000;  // 30ms timeout = ~5m max range
const int NUM_SAMPLES = 5;

// Timing
unsigned long previousMillis = 0;
const long interval = 100;  // Read every 100ms

// Statistics
float minDistance = 999, maxDistance = 0;
int readingCount = 0;
int errorCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║    HC-SR04 Ultrasonic Distance         ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Range: 2cm - 400cm                     ║");
  Serial.println("║ TRIG: GPIO 5                           ║");
  Serial.println("║ ECHO: GPIO 18                          ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("HC-SR04 Specifications:");
  Serial.println("  Range: 2-400 cm");
  Serial.println("  Accuracy: ±3mm");
  Serial.println("  Beam angle: ~15°");
  Serial.println();
  
  Serial.println("Time\t\tDistance(cm)\tStatus");
  Serial.println("─────────────────────────────────────────────");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    float distance = measureDistance();
    displayReading(distance);
  }
}

float measureDistance() {
  // Take multiple samples and average
  float sum = 0;
  int validSamples = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    float dist = singleMeasurement();
    
    if (dist > 0) {
      sum += dist;
      validSamples++;
    }
    
    delay(20);  // Wait between samples
  }
  
  if (validSamples == 0) {
    return -1;  // Error
  }
  
  return sum / validSamples;
}

float singleMeasurement() {
  // Send 10µs trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo pulse duration
  long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
  
  if (duration == 0) {
    return -1;  // Timeout
  }
  
  // Calculate distance
  // Speed of sound: 343 m/s = 0.0343 cm/µs
  // Distance = (duration / 2) * 0.0343
  float distance = (duration / 2.0) * 0.0343;
  
  return distance;
}

void displayReading(float distance) {
  readingCount++;
  
  Serial.print(millis() / 1000);
  Serial.print("\t\t");
  
  if (distance < 0) {
    Serial.println("Error\t\t❌ No echo");
    errorCount++;
    return;
  }
  
  // Validation
  if (distance < 2 || distance > 400) {
    Serial.print(distance, 1);
    Serial.println("\t\t⚠️ Out of range");
    errorCount++;
    return;
  }
  
  // Update statistics
  if (distance < minDistance) minDistance = distance;
  if (distance > maxDistance) maxDistance = distance;
  
  // Display
  Serial.print(distance, 1);
  Serial.print("\t\t");
  
  // Status indicators
  if (distance < 10) {
    Serial.println("🔴 Very close!");
  } else if (distance < 30) {
    Serial.println("🟡 Close");
  } else if (distance < 100) {
    Serial.println("🟢 Normal");
  } else {
    Serial.println("🔵 Far");
  }
  
  // Periodic summary
  if (readingCount % 50 == 0) {
    printSummary();
  }
}

void printSummary() {
  Serial.println("\n┌─────────────────────────────────────┐");
  Serial.println("│     DISTANCE STATISTICS             │");
  Serial.println("├─────────────────────────────────────┤");
  Serial.print("│ Readings: ");
  Serial.print(readingCount);
  Serial.print(" (");
  Serial.print(errorCount);
  Serial.println(" errors)        │");
  Serial.print("│ Range: ");
  Serial.print(minDistance, 1);
  Serial.print(" - ");
  Serial.print(maxDistance, 1);
  Serial.println(" cm         │");
  Serial.print("│ Success Rate: ");
  float successRate = ((readingCount - errorCount) / (float)readingCount) * 100;
  Serial.print(successRate, 1);
  Serial.println("%          │");
  Serial.println("└─────────────────────────────────────┘\n");
}

/*
 * HC-SR04 WORKING PRINCIPLE:
 * 
 * 1. Send 10µs HIGH pulse on TRIG pin
 * 2. Sensor sends 8 ultrasonic pulses (40kHz)
 * 3. Wait for echo return
 * 4. ECHO pin goes HIGH for duration of echo
 * 5. Calculate distance from pulse duration
 * 
 * TIMING DIAGRAM:
 * 
 * TRIG:  ____┌──┐_____________________
 *            10µs
 * 
 * ECHO:  ________┌──────────────────┐__
 *              Duration = distance
 * 
 * DISTANCE CALCULATION:
 * 
 * Sound speed in air: 343 m/s at 20°C
 * = 343 m/s = 34300 cm/s = 0.0343 cm/µs
 * 
 * Distance = (Time × Speed) / 2
 * (Divide by 2 because sound travels to object and back)
 * 
 * Distance (cm) = (duration_µs × 0.0343) / 2
 * Distance (cm) = duration_µs / 58.0
 * 
 * SPECIFICATIONS:
 * 
 * Range: 2cm - 400cm
 * Resolution: 0.3cm
 * Accuracy: ±0.3cm
 * Angle: ~15° cone
 * Frequency: 40kHz
 * Power: 5V, 15mA
 * 
 * FACTORS AFFECTING ACCURACY:
 * 
 * TEMPERATURE:
 * Speed of sound changes with temperature
 * v = 331.3 + (0.606 × T)  m/s
 * Where T is temperature in °C
 * 
 * Compensation (if needed):
 * float tempC = 20.0;  // Get from DHT22
 * float speedSound = (331.3 + 0.606 * tempC) / 10000.0;  // cm/µs
 * distance = (duration / 2.0) * speedSound;
 * 
 * SURFACE MATERIAL:
 * Hard surfaces: Good reflection
 * Soft surfaces: Poor reflection (foam, cloth)
 * Angled surfaces: Reflection away from sensor
 * 
 * OBJECT SIZE:
 * Small objects: May miss detection
 * Minimum size: ~5cm diameter
 * 
 * LIMITATIONS:
 * 
 * 1. MINIMUM DISTANCE: 2cm
 *    Sensor needs time to switch TX→RX
 * 
 * 2. BEAM ANGLE: ~15°
 *    Can detect objects to the side
 *    Use baffles for narrow beam
 * 
 * 3. SOFT MATERIALS:
 *    Foam, cloth absorb ultrasound
 *    May get no echo
 * 
 * 4. TEMPERATURE:
 *    Speed of sound varies
 *    ±15°C = ±3% error
 * 
 * 5. MULTIPLE SENSORS:
 *    Crosstalk if too close
 *    Use time multiplexing
 * 
 * TROUBLESHOOTING:
 * 
 * No reading (timeout):
 * → Check wiring
 * → Verify 5V power
 * → Object too far (>4m)
 * → Object too soft
 * → Object too small
 * 
 * Erratic readings:
 * → Electrical noise (add capacitor)
 * → Object moving
 * → Multiple reflections
 * → Use averaging (done in code)
 * 
 * Always reads same value:
 * → ECHO pin stuck
 * → Check connections
 * → Try different GPIO
 * 
 * INDUSTRIAL APPLICATIONS:
 * 
 * TANK LEVEL MONITORING:
 * - Mount sensor at top
 * - Measure to liquid surface
 * - Calculate fill level
 * - Alert when low/high
 * 
 * Level (%) = (MaxDist - CurrentDist) / MaxDist × 100
 * 
 * PROXIMITY DETECTION:
 * - Robot obstacle avoidance
 * - Vehicle parking sensors
 * - Conveyor monitoring
 * - Safety zones
 * 
 * POSITION CONTROL:
 * - Actuator position feedback
 * - Gate/door position
 * - Platform leveling
 * 
 * OBJECT COUNTING:
 * - Detect when object passes
 * - Count items on conveyor
 * - Inventory tracking
 * 
 * EXAMPLE: TANK MONITORING
 * 
 * Tank height: 200cm
 * Sensor mounted at top
 * Empty tank: 200cm reading
 * Full tank: 10cm reading (some clearance)
 * 
 * Fill level calculation:
 * float tankHeight = 200.0;
 * float emptyDistance = 200.0;
 * float fullDistance = 10.0;
 * 
 * float fillLevel = ((emptyDistance - distance) / 
 *                    (emptyDistance - fullDistance)) * 100;
 * 
 * if (fillLevel < 20) {
 *   Serial.println("⚠️ Low level - refill needed!");
 * }
 * 
 * ALTERNATIVE SENSORS:
 * 
 * For limitations of HC-SR04, consider:
 * 
 * VL53L0X (Time-of-Flight laser):
 * + Narrow beam
 * + Works on any surface
 * + Higher accuracy
 * - Shorter range (<2m)
 * 
 * Radar sensor (RCWL-1601):
 * + Works through materials
 * + Longer range
 * - More expensive
 * 
 * Submersible ultrasonic:
 * + Waterproof
 * + For underwater measurement
 * 
 * TYNATECH APPLICATION:
 * 
 * Combined monitoring system:
 * - HC-SR04: Tank level
 * - ACS712: Pump current
 * - BMP280: Atmospheric pressure
 * 
 * Send via LoRaWAN:
 * - Remote tank monitoring
 * - No wiring needed
 * - Battery powered
 * 
 * Alert conditions:
 * - Level < 20%: Refill needed
 * - Level > 95%: Overflow risk
 * - No level change + pump running: Blockage?
 * 
 * NEXT STEPS:
 * 
 * Day 5: Test with objects at different distances
 * Day 6: Add to Modbus registers
 * Day 7: Send level data via LoRaWAN
 */