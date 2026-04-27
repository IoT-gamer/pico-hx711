#ifndef HX711_H
#define HX711_H

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

// Gain settings corresponding to the number of pulses
typedef enum {
    HX711_GAIN_128 = 1, // 25 pulses (Channel A)
    HX711_GAIN_32  = 2, // 26 pulses (Channel B)
    HX711_GAIN_64  = 3  // 27 pulses (Channel A)
} hx711_gain_t;

typedef struct {
    uint clock_pin;
    uint data_pin;
    hx711_gain_t gain;
    int32_t offset;
    float scale;
} hx711_config_t;

// Initialize the HX711 pins and set default gain
void hx711_init(hx711_config_t *hx, uint clock_pin, uint data_pin);

// Set the gain and channel
void hx711_set_gain(hx711_config_t *hx, hx711_gain_t gain);

// Check if the HX711 is ready for a read
bool hx711_is_ready(hx711_config_t *hx);

// Read raw data from the HX711
int32_t hx711_read(hx711_config_t *hx);

// Read multiple times and return the average
int32_t hx711_read_average(hx711_config_t *hx, uint8_t times);

// Get the raw value minus the offset
double hx711_get_value(hx711_config_t *hx, uint8_t times);

// Get the final calculated weight (value / scale)
double hx711_get_units(hx711_config_t *hx, uint8_t times);

// Tare the scale (set current weight as zero offset)
void hx711_tare(hx711_config_t *hx, uint8_t times);

// Set the scale factor (calibration)
void hx711_set_scale(hx711_config_t *hx, float scale);

// Put the HX711 into low power mode
void hx711_power_down(hx711_config_t *hx);

// Wake the HX711 up
void hx711_power_up(hx711_config_t *hx);

#endif // HX711_H