/*
 * SW-420 Vibration Sensor
 * 
 * Purpose: Detect vibration for predictive maintenance
 * Digital sensor with adjustable sensitivity
 * 
 * Hardware:
 * - ESP32
 * - SW-420 sensor (arrives Jan 14)
 * 
 * Connections:
 * SW-420 → ESP32
 * VCC → 3.3V or 5V
 * GND → GND
 * DO (Digital Out) → GPIO 13
 * AO (Analog Out) → GPIO 34 (optional)
 * 
 * Note: Adjust potentiometer on module for sensitivity
 * 
 * Author: Samrat Sharma
 * Date: January 2026
 */

// Pin definitions
#define DIGITAL_PIN 13
#define ANALOG_PIN 34

// Configuration
const int VIBRATION_THRESHOLD = 2000;  // For analog reading
const int DEBOUNCE_MS = 50;

// Timing
unsigned long lastVibrationTime = 0;
unsigned long sessionStart = 0;

// Statistics
int vibrationCount = 0;
int totalEvents = 0;
float maxAnalogValue = 0;

// State
bool currentlyVibrating = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(DIGITAL_PIN, INPUT);
  pinMode(ANALOG_PIN, INPUT);
  
  sessionStart = millis();
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║      SW-420 Vibration Sensor           ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.println("║ Digital Pin: GPIO 13                   ║");
  Serial.println("║ Analog Pin: GPIO 34                    ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("SW-420 Setup:");
  Serial.println("1. Adjust potentiometer for sensitivity");
  Serial.println("2. LED on module lights when vibration detected");
  Serial.println("3. Digital output: LOW = vibration, HIGH = no vibration");
  Serial.println();
  
  Serial.println("Monitoring for vibration events...\n");
  Serial.println("Time(s)\tDigital\tAnalog\tStatus\t\tEvent#");
  Serial.println("─────────────────────────────────────────────────────");
}

void loop() {
  // Read both digital and analog outputs
  int digitalValue = digitalRead(DIGITAL_PIN);
  int analogValue = analogRead(ANALOG_PIN);
  
  // Track maximum analog value
  if (analogValue > maxAnalogValue) {
    maxAnalogValue = analogValue;
  }
  
  // Digital detection (inverted: LOW = vibration)
  if (digitalValue == LOW) {
    handleVibrationDetected(analogValue);
  } else {
    if (currentlyVibrating) {
      handleVibrationStopped();
    }
  }
  
  // Analog threshold detection
  if (analogValue > VIBRATION_THRESHOLD) {
    Serial.print(millis() / 1000);
    Serial.print("\t");
    Serial.print(digitalValue ? "HIGH" : "LOW");
    Serial.print("\t");
    Serial.print(analogValue);
    Serial.println("\t🔴 High vibration!");
  }
  
  delay(10);  // Small delay for stability
}

void handleVibrationDetected(int analogValue) {
  // Debouncing
  if (millis() - lastVibrationTime < DEBOUNCE_MS) {
    return;
  }
  
  if (!currentlyVibrating) {
    // New vibration event started
    currentlyVibrating = true;
    totalEvents++;
    lastVibrationTime = millis();
    
    Serial.print(lastVibrationTime / 1000);
    Serial.print("\t");
    Serial.print("LOW");
    Serial.print("\t");
    Serial.print(analogValue);
    Serial.print("\⚠️ Vibration START\t#");
    Serial.println(totalEvents);
  }
  
  vibrationCount++;
}

void handleVibrationStopped() {
  currentlyVibrating = false;
  unsigned long duration = millis() - lastVibrationTime;
  
  Serial.print(millis() / 1000);
  Serial.print("\t");
  Serial.print("HIGH");
  Serial.print("\t");
  Serial.print(analogRead(ANALOG_PIN));
  Serial.print("\t✓ Vibration STOP\t");
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.println("ms");
  
  // Check if this was a significant event
  if (duration > 1000) {
    Serial.println("\t\t\t\t🚨 Long duration event!");
  }
  
  // Periodic summary
  if (totalEvents % 10 == 0 && totalEvents > 0) {
    printSummary();
  }
}

void printSummary() {
  unsigned long uptime = (millis() - sessionStart) / 1000;
  
  Serial.println("\n┌─────────────────────────────────────────┐");
  Serial.println("│      VIBRATION MONITORING SUMMARY       │");
  Serial.println("├─────────────────────────────────────────┤");
  Serial.print("│ Uptime: ");
  Serial.print(uptime);
  Serial.println(" seconds                  │");
  Serial.print("│ Total Events: ");
  Serial.print(totalEvents);
  Serial.println("                        │");
  Serial.print("│ Vibration Count: ");
  Serial.print(vibrationCount);
  Serial.println("                   │");
  Serial.print("│ Max Analog: ");
  Serial.print(maxAnalogValue);
  Serial.println("                       │");
  Serial.print("│ Events/Hour: ");
  float eventsPerHour = (totalEvents / (float)uptime) * 3600;
  Serial.print(eventsPerHour, 1);
  Serial.println("                    │");
  Serial.println("└─────────────────────────────────────────┘\n");
}

/*
 * SW-420 VIBRATION SENSOR:
 * 
 * TECHNOLOGY:
 * - Spring-loaded conductive ball
 * - Vibration causes ball to move
 * - Makes/breaks electrical contact
 * - Very simple but effective!
 * 
 * OUTPUTS:
 * 
 * Digital Output (DO):
 * - HIGH: No vibration
 * - LOW: Vibration detected
 * - Goes to comparator with threshold
 * - Threshold set by potentiometer
 * 
 * Analog Output (AO):
 * - Voltage proportional to vibration
 * - More sensitive than digital
 * - Can measure vibration intensity
 * 
 * SENSITIVITY ADJUSTMENT:
 * 
 * 1. Rotate potentiometer on module
 * 2. Clockwise: Less sensitive
 * 3. Counter-clockwise: More sensitive
 * 4. LED on module indicates detection
 * 5. Adjust until desired sensitivity
 * 
 * TYPICAL SETTINGS:
 * - High sensitivity: Detect footsteps
 * - Medium: Detect knocking
 * - Low: Only strong vibration
 * 
 * LIMITATIONS:
 * 
 * 1. NOT A PRECISION SENSOR
 *    - Can't measure acceleration
 *    - Can't measure frequency
 *    - Just detects presence/absence
 * 
 * 2. BINARY OUTPUT
 *    - Either vibrating or not
 *    - Can't distinguish types
 * 
 * 3. MECHANICAL
 *    - Can wear out over time
 *    - Not for high-frequency vibration
 * 
 * FOR PRECISE VIBRATION:
 * Use accelerometer like:
 * - ADXL345 (3-axis, digital)
 * - MPU6050 (IMU with gyro)
 * - LIS3DH (low power)
 * 
 * INDUSTRIAL APPLICATIONS:
 * 
 * PREDICTIVE MAINTENANCE:
 * - Detect abnormal machine vibration
 * - Early warning of bearing wear
 * - Misalignment detection
 * - Imbalance detection
 * 
 * Baseline approach:
 * 1. Measure normal operation vibration
 * 2. Set threshold above normal
 * 3. Alert when exceeded
 * 4. Schedule maintenance
 * 
 * SECURITY:
 * - Door/window intrusion detection
 * - Safe tampering detection
 * - Perimeter monitoring
 * - Equipment theft alarm
 * 
 * EARTHQUAKE DETECTION:
 * - Low-cost seismic sensor
 * - Early warning system
 * - Building evacuation trigger
 * 
 * TRANSPORT MONITORING:
 * - Package handling detection
 * - Shock detection in shipping
 * - Vehicle accident detection
 * 
 * EXAMPLE: MOTOR MONITORING
 * 
 * Normal motor:
 * - Low vibration count
 * - Short duration events
 * - Predictable pattern
 * 
 * Failing bearing:
 * - Increased vibration count
 * - Longer duration events
 * - Random pattern
 * 
 * Alarm logic:
 * if (vibrationCount > baseline * 2) {
 *   Serial.println("⚠️ Abnormal vibration!");
 *   Serial.println("Schedule maintenance check");
 * }
 * 
 * DEBOUNCING:
 * 
 * Why needed:
 * - Mechanical bounce
 * - Multiple triggers for one event
 * - False positives
 * 
 * Solution (in code):
 * - Ignore events within 50ms
 * - Filters mechanical bounce
 * - Groups related detections
 * 
 * EVENT COUNTING:
 * 
 * Track:
 * - Total events
 * - Events per hour
 * - Event duration
 * - Time between events
 * 
 * Pattern analysis:
 * - Regular pattern: Periodic source (motor RPM)
 * - Random pattern: External disturbance
 * - Increasing frequency: Degradation
 * 
 * Company's APPLICATION:
 * 
 * Machine health monitoring:
 * - SW-420: Vibration detection
 * - ACS712: Current monitoring
 * - DHT22: Temperature
 * 
 * Combined analysis:
 * - High vibration + high current = overload
 * - High vibration + high temp = bearing failure
 * - Normal current + high vibration = mechanical issue
 * 
 * Alert via LoRaWAN:
 * - Immediate alert on abnormal vibration
 * - Periodic health reports
 * - Remote monitoring without wiring
 * 
 * DATA LOGGING:
 * 
 * Log to SD card or cloud:
 * Timestamp, Event#, Duration, Analog_Max
 * 2026-01-14 10:23:45, 1, 234ms, 2340
 * 2026-01-14 10:25:12, 2, 156ms, 1980
 * 
 * Analysis:
 * - Plot vibration frequency over time
 * - Identify trends
 * - Predict maintenance needs
 * 
 * CALIBRATION:
 * 
 * 1. Install on equipment
 * 2. Run for 24 hours
 * 3. Note normal vibration count
 * 4. Set alert threshold:
 *    Alert = Normal × 2 or 3
 * 
 * ADVANCED FEATURES:
 * 
 * If using accelerometer instead:
 * - FFT analysis (frequency domain)
 * - Identify specific fault frequencies
 * - Bearing defect detection
 * - Gear mesh analysis
 * 
 * Common fault frequencies:
 * - Bearing outer race: 0.4 × RPM
 * - Bearing inner race: 0.6 × RPM
 * - Imbalance: 1 × RPM
 * - Misalignment: 2 × RPM
 * 
 * NEXT STEPS:
 * 
 * Day 5: Mount on vibrating equipment
 * Day 6: Integrate with Modbus
 * Day 7: Send alerts via LoRaWAN
 */
