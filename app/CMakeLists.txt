set(UART_RELAY_APP_SOURCES
    uart_relay.c
)

set(RADIO_DRIVER_APP_SOURCES
    radio_driver.c
)

add_library(uart_relay_app STATIC)

target_sources(uart_relay_app
  PRIVATE
    ${UART_RELAY_APP_SOURCES}
    include/uart_relay.h
)

target_include_directories(uart_relay_app
  PUBLIC
    include/
)

target_link_libraries(uart_relay_app
  PRIVATE
    hal
    minmea
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Static
)

set_source_files_properties(${UART_RELAY_APP_SOURCES}
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion"
)

add_library(radio_driver_app STATIC)

target_sources(radio_driver_app
  PRIVATE
    ${RADIO_DRIVER_APP_SOURCES}
    include/radio_driver.h
)

target_include_directories(radio_driver_app
  PUBLIC
    include/
)

target_link_libraries(radio_driver_app
  PRIVATE
    sx126x_port
    sx126x
    hal
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Static
)

set_source_files_properties(${RADIO_DRIVER_APP_SOURCES}
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion"
)