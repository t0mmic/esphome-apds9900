# esphome-apds9900
Light and proximity sensor

APDS9900 Sensor Usage Guide
The APDS9900 sensor is a combined proximity and ambient light sensor that allows accurate detection of ambient light (lux) and proximity data (distance). This guide provides an overview of its features, usage, and adjustable parameters based on the datasheet and ESPhome implementation.
________________________________________
Features
1.	Ambient Light Sensing:
o	Measures light intensity in lux.
o	Uses dual photodiodes for accurate lux calculations under varying light conditions.
2.	Proximity Sensing:
o	Measures proximity values up to approximately 100 mm.
o	Uses IR LED pulses and reflected light for proximity measurements.
3.	Configurable Parameters:
o	LED drive current for proximity sensing.
o	Integration time for ambient light sensing.
o	Wait time between proximity or light measurements.
o	Gain for ambient light sensing.
o	Enable/disable proximity sensing.
________________________________________
Adjustable Parameters
Here are the key parameters you can configure for the APDS9900 sensor:
1. LED Drive (PDRIVE)
•	Sets the current for the proximity IR LED.
•	Options:
o	100mA (default): Maximum drive current.
o	50mA
o	25mA
o	12.5mA
2. ALS Timing (ATIME)
•	Determines the integration time for ambient light sensing.
•	Options:
o	2.72ms
o	27.2ms
o	100.64ms (default)
o	174.08ms
o	696.32ms
3. Wait Time (WTIME)
•	Sets the delay between measurements.
•	Options:
o	2.72ms (default)
o	201.29ms
o	696.32ms
4. Ambient Light Gain (AGAIN)
•	Adjusts the gain for ambient light measurements.
•	Options:
o	1x (default): No amplification.
o	8x
o	16x
o	120x
________________________________________
I. Example Configuration in ESPhome
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
  pdrive: 100ma
  atime: 100.64ms
  wtime: 2.72ms
  again: 1x


________________________________________
II. Example Configuration in ESPhome 
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
      type: local  #add to local or git, see manual
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
    #address: 0x39 
  
________________________________________
Integration Time and Proximity Calculations
Ambient Light Integration Time
The integration time can be adjusted via ATIME:
•	ALSIT (ms) = 2.72 × (256 – ATIME)
•	Lower integration time increases speed but reduces accuracy.
Proximity Sensing
Proximity sensing uses LED pulses to detect objects. Proximity values can range from 0 to approximately 1023, corresponding to distances from 0 mm to ~100 mm.
________________________________________
Usage Notes
1.	Ensure proper placement of the APDS9900 sensor for accurate readings (e.g., avoid direct sunlight or IR interference).
2.	For proximity sensing, a reflective object (like a grey card) improves detection accuracy.
3.	Adjust parameters (PDRIVE, AGAIN, etc.) based on your environment and desired accuracy.
________________________________________
With this guide, you can effectively integrate and configure the APDS9900 sensor in your ESPhome projects for precise ambient light and proximity detection.


