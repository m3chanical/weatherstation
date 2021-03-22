#include "bme280.h" 
#include "bme280_driver.h" 
#include "driver/i2c.h"
#include "esp32/rom/ets_sys.h"

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html

#define BME280_ADDR 0x77

static gpio_num_t i2c_gpio_sda = 21;
static gpio_num_t i2c_gpio_scl = 22;
static uint32_t i2c_frequency = 100000;
static i2c_port_t i2c_port = I2C_NUM_0;
// internal functions
void bme280_get_data(void);
int8_t bme280_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);
int8_t bme280_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
void bme280_delay(uint32_t period, void *intf_ptr);
static esp_err_t i2c_init();

int bme2800_init(void)
{
    // set up I2C for BME280 sensor
    // From the datasheet: 
    //      Connecting SDO to GND results in slave address 1110110 (0x76); connection it to 
    //          VDDIO results in slave address 1110111 (0x77)
    // On my board I have SDO connected to VDDIO: 0x77
    esp_err_t err;
    struct bme280_dev dev;
    int8_t result = BME280_OK;

    err = i2c_init(); // init i2c on the board, then bme280. no need to be generic since only bme280 will be on the bus

    // get register info and ensure chip is good
    // reset the chip
    // get calibration data

    dev.intf = BME280_I2C_INTF; //BME280_I2C_ADDR_SEC
    dev.write = bme280_write;
    dev.read = bme280_read;
    dev.delay_us = bme280_delay;

    result = bme280_init(&dev);
    if(result != BME280_OK)
        return -1;

    return 0;
}

void bme280_get_data()
{

}

// internal functions
int8_t bme280_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write(cmd, (uint8_t *) data, len, true);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, 10);

    i2c_cmd_link_delete(cmd);

    if(err)
        return -1;

    return 0;
}

int8_t bme280_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, 10);
    i2c_cmd_link_delete(cmd);

    if(err)
        return -1;

    return 0;
}

void bme280_delay(uint32_t period, void *intf_ptr)
{
    ets_delay_us(period);
}

static esp_err_t i2c_init()
{
    esp_err_t err = 1;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c_gpio_scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_frequency
    };
    err = i2c_param_config(i2c_port, &conf);
    if(!err) err = i2c_driver_install(i2c_port, conf.mode, 0, 0, 0);

    return err;
}
