
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_DISTANCE,
    CONF_ID,
    CONF_ILLUMINANCE,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_ILLUMINANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_LUX,
    UNIT_MILLIMETER,
)

DEPENDENCIES = ["i2c"]

CONF_APDS9900_ID = "apds9900_id"
CONF_PDRIVE = "led_drive"
CONF_ATIME = "als_timing"
CONF_WTIME = "wait_time"
CONF_AGAIN = "ambient_light_gain"

PDRIVE = {"100ma": 0, "50ma": 1, "25ma": 2, "12.5ma": 3}
ATIME = {"2.72ms": 1, "27.2ms": 10, "100.64ms": 37, "174.08ms": 64, "696.32ms": 256}
WTIME = {"2.72ms": 1, "201.29ms": 74, "696.32ms": 256}
AGAIN = {"1x": 0, "8x": 1, "16x": 2, "120x": 3}

apds9900_ns = cg.esphome_ns.namespace("apds9900")
APDS9900Component = apds9900_ns.class_(
    "APDS9900Component", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(APDS9900Component),
            cv.Optional(CONF_ILLUMINANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_LUX,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_ILLUMINANCE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETER,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_DISTANCE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PDRIVE, "100mA"): cv.enum(PDRIVE, lower=True),
            cv.Optional(CONF_ATIME, "100.64ms"): cv.enum(ATIME, lower=True),
            cv.Optional(CONF_WTIME, "2.72ms"): cv.enum(WTIME, lower=True),
            cv.Optional(CONF_AGAIN, "1x"): cv.enum(AGAIN, lower=True),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x39))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_pdrive(config[CONF_PDRIVE]))
    cg.add(var.set_atime(config[CONF_ATIME]))
    cg.add(var.set_wtime(config[CONF_WTIME]))
    cg.add(var.set_again(config[CONF_AGAIN]))

    if light_config := config.get(CONF_ILLUMINANCE):
        sens = await sensor.new_sensor(light_config)
        cg.add(var.set_light_sensor(sens))

    if proximity_config := config.get(CONF_DISTANCE):
        sens = await sensor.new_sensor(proximity_config)
        cg.add(var.set_proximity_sensor(sens))