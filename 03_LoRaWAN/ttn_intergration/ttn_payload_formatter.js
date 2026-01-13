/*
 * The Things Network Payload Formatter
 * 
 * Purpose: Decode binary sensor data on TTN
 * Location: TTN Console → Application → Payload Formatters
 * 
 * This decodes your sensor data into human-readable format
 * 
 * Author: [Your Name]
 * Date: January 2026
 */

// UPLINK DECODER (Device → TTN)
function decodeUplink(input) {
  var data = {};
  var bytes = input.bytes;
  
  // Check minimum payload size
  if (bytes.length < 8) {
    return {
      data: { error: "Payload too short" },
      warnings: ["Expected at least 8 bytes"],
      errors: []
    };
  }
  
  // Decode sensor data
  // Format: [TEMP_H][TEMP_L][HUM_H][HUM_L][CURR_H][CURR_L][PRES_H][PRES_L]
  
  // Temperature (signed, 0.1°C resolution)
  var temp_raw = (bytes[0] << 8) | bytes[1];
  if (temp_raw & 0x8000) {  // Check sign bit
    temp_raw = temp_raw - 0x10000;
  }
  data.temperature = temp_raw / 10.0;
  
  // Humidity (unsigned, 0.1% resolution)
  var hum_raw = (bytes[2] << 8) | bytes[3];
  data.humidity = hum_raw / 10.0;
  
  // Current (signed, 0.01A resolution)
  var curr_raw = (bytes[4] << 8) | bytes[5];
  if (curr_raw & 0x8000) {
    curr_raw = curr_raw - 0x10000;
  }
  data.current = curr_raw / 100.0;
  
  // Pressure (unsigned, 0.1 hPa resolution)
  var pres_raw = (bytes[6] << 8) | bytes[7];
  data.pressure = pres_raw / 10.0;
  
  // Calculate power (assuming 230V)
  data.power = Math.abs(data.current) * 230;
  
  return {
    data: data,
    warnings: [],
    errors: []
  };
}

// DOWNLINK ENCODER (TTN → Device)
function encodeDownlink(input) {
  var data = input.data;
  var bytes = [];
  
  // Example: Send configuration
  // Format: [CMD][VALUE_H][VALUE_L]
  
  if (data.command === "SET_INTERVAL") {
    bytes[0] = 0x01;  // Command: Set interval
    var interval = data.interval || 60;  // Default 60 seconds
    bytes[1] = (interval >> 8) & 0xFF;
    bytes[2] = interval & 0xFF;
  }
  else if (data.command === "RESET") {
    bytes[0] = 0xFF;  // Command: Reset device
  }
  
  return {
    bytes: bytes,
    fPort: 1,
    warnings: [],
    errors: []
  };
}

/*
 * PAYLOAD ENCODING STRATEGY:
 * 
 * WHY BINARY?
 * - Smaller payload = less airtime
 * - Less airtime = more battery life
 * - Stay within duty cycle limits
 * 
 * EXAMPLE: Temperature = 25.5°C
 * 
 * JSON: {"temp":25.5} = 14 bytes
 * Binary: [0x00, 0xFF] = 2 bytes
 * Savings: 7x smaller!
 * 
 * ENCODING TIPS:
 * 
 * 1. Use appropriate resolution
 *    - 0.1°C for temperature (1 byte = -12.8 to 12.7°C)
 *    - 0.01A for current
 *    - 0.1hPa for pressure
 * 
 * 2. Use unsigned if values always positive
 *    - Humidity: 0-100% (1 byte)
 *    - Pressure: 900-1100 hPa (2 bytes)
 * 
 * 3. Pack multiple values
 *    - Status bits in single byte
 *    - Flags and states
 * 
 * EXAMPLE ESP32 ENCODING:
 * 
 * ```cpp
 * // On ESP32 (encoding)
 * uint8_t payload[8];
 * 
 * // Temperature: 25.5°C → 255
 * int16_t temp = temperature * 10;
 * payload[0] = (temp >> 8) & 0xFF;
 * payload[1] = temp & 0xFF;
 * 
 * // Humidity: 60.2% → 602
 * uint16_t hum = humidity * 10;
 * payload[2] = (hum >> 8) & 0xFF;
 * payload[3] = hum & 0xFF;
 * 
 * // ... encode rest
 * 
 * // Send
 * LMIC_setTxData2(1, payload, sizeof(payload), 0);
 * ```
 * 
 * TTN CONSOLE USAGE:
 * 
 * 1. Go to your application
 * 2. Click "Payload formatters"
 * 3. Select "Custom Javascript formatter"
 * 4. Paste decoder function
 * 5. Test with sample payload
 * 6. Save
 * 
 * Now all uplinks are automatically decoded!
 * Data appears in nice format in console and integrations
 * 
 * TESTING YOUR FORMATTER:
 * 
 * Use TTN console test feature:
 * Input: 00FF025801F404B0
 * Should decode to:
 * - temperature: 25.5
 * - humidity: 60.0
 * - current: 5.00
 * - pressure: 1200.0
 * - power: 1150.0
 */