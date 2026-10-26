set(SX126X_PORT_SOURCES
    sx126x_hal.c
)

add_library(sx126x_port STATIC)

target_sources(sx126x_port
  PRIVATE
    ${SX126X_PORT_SOURCES}
    include/sx126x_port.h
)

target_include_directories(sx126x_port
  PUBLIC
    include/
)

# The port implements the vendor HAL and stands on the hardware HALs, so both
# travel to whoever links the port (the application).
target_link_libraries(sx126x_port
  PUBLIC
    sx126x
    hal
)

set_source_files_properties(${SX126X_PORT_SOURCES}
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion"
)