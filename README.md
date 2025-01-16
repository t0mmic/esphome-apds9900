
# ESPHome-APDS9900

**Light and Proximity Sensor**

## APDS9900 Sensor Usage Guide

The APDS9900 sensor is a combined proximity and ambient light sensor, capable of accurately detecting ambient light levels (lux) and proximity data (distance). This guide provides an overview of its features, usage, and adjustable parameters based on the datasheet and ESPHome implementation.

---

## Features

1. **Ambient Light Sensing**:
   - Measures light intensity in lux.
   - Uses dual photodiodes for accurate lux calculations under varying light conditions.
2. **Proximity Sensing**:
   - Measures proximity values up to approximately 100 mm.
   - Utilizes IR LED pulses and reflected light for proximity measurements.
3. **Configurable Parameters**:
   - LED drive current for proximity sensing.
   - Integration time for ambient light sensing.
   - Wait time between proximity or light measurements.
   - Gain for ambient light sensing.
   - Enable/disable proximity sensing.

---

## Adjustable Parameters

| **Parameter**           | **Description**                                | **Options**                   | **Default**    |
|--------------------------|-----------------------------------------------|--------------------------------|----------------|
| **LED Drive (PDRIVE)**   | Sets the current for the proximity IR LED.    | `100mA`, `50mA`, `25mA`, `12.5mA` | `100mA`       |
| **ALS Timing (ATIME)**   | Integration time for ambient light sensing.   | `2.72ms`, `27.2ms`, `100.64ms`, `174.08ms`, `696.32ms` | `100.64ms` |
| **Wait Time (WTIME)**    | Delay between measurements.                   | `2.72ms`, `201.29ms`, `696.32ms` | `2.72ms`      |
| **Ambient Light Gain (AGAIN)** | Gain for ambient light measurements.       | `1x`, `8x`, `16x`, `120x`     | `1x`          |

---

## Example Configuration in ESPHome

### Example 1: Custom Component

```yaml
esphome:
  name: esp32_apds9900
  platform: ESP32
  board: esp32dev

i2c:
  sda: 21
  scl: 22
  scan: True

sensor:
  - platform: custom
    lambda: |-
      auto *apds9900 = new APDS9900Component();
      App.register_component(apds9900);
      return {apds9900->get_light_sensor(), apds9900->get_proximity_sensor()};
    sensors:
      - name: "Ambient Light"
        unit_of_measurement: "lux"
        accuracy_decimals: 1
        device_class: "illuminance"
        state_class: "measurement"
      - name: "Proximity"
        unit_of_measurement: "mm"
        accuracy_decimals: 1
        device_class: "distance"
        state_class: "measurement"

apds9900:
  led_drive: 100ma
  als_timing: 27.2ms
  wait_time: 2.72ms
  ambient_light_gain: 8x
```

---

### Example 2: External Component

```yaml
esphome:
  name: esp32_apds9900
  platform: ESP32
  board: esp32dev

i2c:
  sda: 21
  scl: 22
  scan: True

external_components:
  - source:
      type: local
      path: components
    components: [ apds9900 ]

sensor:
  - platform: apds9900
    id: apds9900_sensor
    distance:
      id: apds9900_proximity_sensor
      name: "Proximity"
      unit_of_measurement: "mm"
      accuracy_decimals: 0
    illuminance:
      id: apds9900_light_sensor
      name: "Light"
      unit_of_measurement: "lx"
      accuracy_decimals: 0
    led_drive: 100ma
    als_timing: 27.2ms
    wait_time: 2.72ms
    ambient_light_gain: 8x
    # address: 0x39
```

---

## Integration Time and Proximity Calculations

### Ambient Light Integration Time
- The integration time can be adjusted via the `ATIME` parameter:
  - **Formula**: `ALSIT (ms) = 2.72 × (256 – ATIME)`
  - Lower integration time increases speed but reduces accuracy.

### Proximity Sensing
- Proximity sensing uses LED pulses to detect objects.
- Proximity values range from `0` to approximately `1023`, corresponding to distances from `0 mm` to ~`100 mm`.

---

## Usage Notes

1. Ensure proper placement of the APDS9900 sensor for accurate readings (e.g., avoid direct sunlight or IR interference).
2. For proximity sensing, a reflective object (like a grey card) improves detection accuracy.
3. Adjust parameters (`PDRIVE`, `AGAIN`, etc.) based on your environment and desired accuracy.

---

With this guide, you can effectively integrate and configure the APDS9900 sensor in your ESPHome projects for precise ambient light and proximity detection.

---
