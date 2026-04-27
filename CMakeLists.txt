cmake_minimum_required(VERSION 3.13)

add_library(hx711 
    src/hx711.c
)

# Expose the include directory to projects that link this library
target_include_directories(hx711 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link against the necessary Pico SDK libraries
target_link_libraries(hx711 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_timer
)