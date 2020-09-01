#include "bme280.h" 
#include "driver/i2c.h"

#define BME280_ADDR 0x77

static gpio_num_t i2c_gpio_sda = 18;
static gpio_num_t i2c_gpio_scl = 19;
static uint32_t i2c_frequency = 100000;
static i2c_port_t i2c_port = I2C_NUM_0;
// internal functions
void i2c_init();

int bme280_init(void)
{
    // set up I2C for BME280 sensor
    // From the datasheet: 
    //      Connecting SDO to GND results in slave address 1110110 (0x76); connection it to 
    //          VDDIO results in slave address 1110111 (0x77)
    // On my board I have SDO connected to VDDIO: 0x77

    i2c_init();

    return 0;
}

int bme280_get_sensor_data(int data)
{
    return 0;
}

static esp_err_t i2c_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c_gpio_scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_frequency
    };

    return i2c_param_config(i2c_port, &conf);

}
