#include "hx711.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

void hx711_init(hx711_config_t *hx, uint clock_pin, uint data_pin) {
    hx->clock_pin = clock_pin;
    hx->data_pin = data_pin;
    hx->offset = 0;
    hx->scale = 1.0f;
    hx->gain = HX711_GAIN_128; // Default

    gpio_init(hx->clock_pin);
    gpio_set_dir(hx->clock_pin, GPIO_OUT);
    gpio_put(hx->clock_pin, 0);

    gpio_init(hx->data_pin);
    gpio_set_dir(hx->data_pin, GPIO_IN);
    
    hx711_set_gain(hx, hx->gain);
}

bool hx711_is_ready(hx711_config_t *hx) {
    return gpio_get(hx->data_pin) == 0;
}

void hx711_set_gain(hx711_config_t *hx, hx711_gain_t gain) {
    hx->gain = gain;
    gpio_put(hx->clock_pin, 0);
    hx711_read(hx);
}

int32_t hx711_read(hx711_config_t *hx) {
    // Wait for the chip to become ready
    while (!hx711_is_ready(hx)) {
        tight_loop_contents();
    }

    int32_t value = 0;
    
    // Read 24 bits
    for (int i = 0; i < 24; i++) {
        gpio_put(hx->clock_pin, 1);
        busy_wait_us(1); // Ensure clock high time is met (>0.2us)
        
        value = value << 1;
        gpio_put(hx->clock_pin, 0);
        busy_wait_us(1); // Ensure clock low time is met (>0.2us)
        
        if (gpio_get(hx->data_pin)) {
            value++;
        }
    }

    // Pulse for gain setting
    for (int i = 0; i < hx->gain; i++) {
        gpio_put(hx->clock_pin, 1);
        busy_wait_us(1);
        gpio_put(hx->clock_pin, 0);
        busy_wait_us(1);
    }

    // The data is 24-bit two's complement. Sign extend to 32 bits.
    if (value & 0x00800000) {
        value |= 0xFF000000;
    }

    return value;
}

int32_t hx711_read_average(hx711_config_t *hx, uint8_t times) {
    int64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read(hx);
    }
    return (int32_t)(sum / times);
}

double hx711_get_value(hx711_config_t *hx, uint8_t times) {
    return hx711_read_average(hx, times) - hx->offset;
}

double hx711_get_units(hx711_config_t *hx, uint8_t times) {
    return hx711_get_value(hx, times) / hx->scale;
}

void hx711_tare(hx711_config_t *hx, uint8_t times) {
    double sum = hx711_read_average(hx, times);
    hx->offset = sum;
}

void hx711_set_scale(hx711_config_t *hx, float scale) {
    hx->scale = scale;
}

void hx711_power_down(hx711_config_t *hx) {
    gpio_put(hx->clock_pin, 0);
    busy_wait_us(1);
    gpio_put(hx->clock_pin, 1);
    // Setting clock pin high for >60us powers down the HX711
    busy_wait_us(65); 
}

void hx711_power_up(hx711_config_t *hx) {
    gpio_put(hx->clock_pin, 0);
}