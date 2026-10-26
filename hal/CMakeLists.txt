set(HAL_RP2040_SOURCES
    rp2040/hal_gpio.c
    rp2040/hal_spi.c
    rp2040/hal_timer.c
    rp2040/hal_uart.c
)

add_library(hal STATIC)

target_sources(hal
  PRIVATE
    ${HAL_RP2040_SOURCES}
    include/hal_gpio.h
    include/hal_spi.h
    include/hal_uart.h
    include/hal_timer.h
)

target_include_directories(hal
  PUBLIC
    include/
)

target_link_libraries(hal 
  PRIVATE
    pico_stdlib
    hardware_spi
)

set_source_files_properties(${HAL_RP2040_SOURCES}
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion"
)