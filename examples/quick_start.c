#include <stdio.h>
#include "pico/stdlib.h"
#include "hx711.h"

int main() {
    stdio_init_all();
    sleep_ms(2000); // Wait for serial connection

    printf("Starting HX711 Test...\n");

    hx711_config_t hx;
    hx711_init(&hx, 14, 15); // GPIO 14 (Clock), GPIO 15 (Data)

    // Calibration: Set this to a known value based on your specific load cell
    hx711_set_scale(&hx, 2280.f); 
    
    printf("Taring scale... please wait.\n");
    hx711_tare(&hx, 10);
    printf("Tare complete. Offset: %ld\n", hx.offset);

    while (true) {
        printf("Weight: %.2f\n", hx711_get_units(&hx, 5));
        sleep_ms(500);
    }

    return 0;
}