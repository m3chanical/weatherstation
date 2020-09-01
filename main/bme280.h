#ifndef _BME280_H
#define _BME280_H 

// INCLUDE FILES


// EXPORTED FUNCTIONS

int bme280_init(void);
int bme280_get_sensor_data(int data);

#endif /* _BME280_H */
