# Pico HX711

A lightweight, dependency-free C library for the HX711 24-bit ADC (Load Cell Amplifier), designed specifically for the Raspberry Pi Pico C/C++ SDK. 

This library is structured to be easily imported into any Pico project using CMake's `FetchContent` module, keeping your main project directory clean and modular.

## Features

* **Native Pico SDK Support:** Uses standard `hardware_gpio` and `hardware_timer` libraries.
* **FetchContent Ready:** Designed as an `INTERFACE`-friendly library for modern CMake workflows.
* **Complete Feature Set:** Supports raw reading, averaging, taring, unit scaling, and power down/up.
* **Configurable Gain/Channel:** Support for Channel A (Gain 128 or 64) and Channel B (Gain 32).
* **Multi-Device Support:** Uses a configuration struct, allowing multiple load cells to be read on different GPIO pins simultaneously.

## Installation via FetchContent

You do not need to download the source files manually. Add the following to your main project's `CMakeLists.txt` to pull the library directly from GitHub during the build process:

```cmake
include(FetchContent)

# Declare the repository
FetchContent_Declare(
    pico_hx711
    GIT_REPOSITORY [https://github.com/IoT-gamer/pico-hx711.git](https://github.com/IoT-gamer/pico-hx711.git)
    GIT_TAG        main # Consider pinning to a specific release tag, e.g., v1.0.0
)
FetchContent_MakeAvailable(pico_hx711)

# Link it to your executable
target_link_libraries(your_executable_name 
    pico_stdlib 
    hx711
)
```

## Quick Start Example
Include the header in your C file and initialize the configuration struct with your chosen GPIO pins.

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hx711.h"

int main() {
    stdio_init_all();
    sleep_ms(2000); // Allow USB serial to connect

    // Initialize the HX711 configuration
    hx711_config_t hx;
    const uint CLOCK_PIN = 14; // GPIO 14
    const uint DATA_PIN = 15;  // GPIO 15
    
    hx711_init(&hx, CLOCK_PIN, DATA_PIN);

    // Set your calibration scale
    // (Known weight raw value / Known weight in units)
    hx711_set_scale(&hx, 2280.f); 

    // Tare the scale (zero it out)
    printf("Taring scale...\n");
    hx711_tare(&hx, 10); // Average 10 readings for a stable tare
    printf("Scale tared. Offset: %ld\n", hx.offset);

    // Read data in a loop
    while (true) {
        printf("Weight: %.2f units\n", hx711_get_units(&hx, 5));
        sleep_ms(500);
    }

    return 0;
}
```
## API Reference
### nitialization & Configuration
* `void hx711_init(hx711_config_t *hx, uint clock_pin, uint data_pin)`: Sets up GPIOs and default gain.
* `void hx711_set_gain(hx711_config_t *hx, hx711_gain_t gain)`: Sets channel and gain (`HX711_GAIN_128`, `HX711_GAIN_64`, or `HX711_GAIN_32`).
* `void hx711_set_scale(hx711_config_t *hx, float scale)`: Sets the divisor used to convert raw data to human-readable units.
### Reading Data
* `int32_t hx711_read(hx711_config_t *hx)`: Blocks until ready, then reads a single raw 24-bit value.
* `int32_t hx711_read_average(hx711_config_t *hx, uint8_t times)`: Returns the average of `times` raw readings.
* `double hx711_get_value(hx711_config_t *hx, uint8_t times)`: Returns the averaged raw reading minus the tare offset.
* `double hx711_get_units(hx711_config_t *hx, uint8_t times)`: Returns `hx711_get_value()` divided by the scale factor.
### Calibration & Power
* `void hx711_tare(hx711_config_t *hx, uint8_t times)`: Sets the current reading as the zero offset.
* `void hx711_power_down(hx711_config_t *hx)`: Puts the HX711 into a low-power sleep state (60uA).
* `void hx711_power_up(hx711_config_t *hx)`: Wakes the HX711 up.

## Calibration Process
1. To calibrate your load cell for the first time:

2. Call `hx711_set_scale(&hx, 1.f)`; to bypass scaling.

3. Call `hx711_tare(&hx, 10)`; with no weight on the scale.

4. Place a known weight (e.g., 1000g) on the scale.

5. Call `hx711_get_value(&hx, 10)`; and record the result.

6. Divide the result by your known weight (e.g., result / 1000.0).

7. Pass that calculated float into `hx711_set_scale()` for all future uses.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## References
* [HX711 Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf)