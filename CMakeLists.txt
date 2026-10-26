cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# KEEP EVERYTHING THAT WAS GENERATED
# FOR YOU ABOVE the ===== LINE.
#================================
set(PICO_BOARD pico CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(M10 C CXX ASM)
pico_sdk_init()

# The FreeRTOS-Kernel checkout is separate from the SDK. This is the path to
# one machine's checkout: point it at your own FreeRTOS-Kernel directory, for
# example -DFREERTOS_KERNEL_PATH=/path/to/FreeRTOS-Kernel/ on the cmake
# command line. FREERTOS_CONFIG_FILE_DIRECTORY points the kernel at this
# directory's FreeRTOSConfig.h; both must be set before the port is added
# because its library.cmake reads them while defining the kernel targets.
set(FREERTOS_KERNEL_PATH "/Users/jackallenburg/VS_CODE/EmbeddedClass/FreeRTOS-Kernel")
set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_executable(M10)
target_sources(M10 PRIVATE main.c)

# stdio stays off UART0: UART0 is wired to the GPS receiver.
pico_enable_stdio_uart(M10 0)
pico_enable_stdio_usb(M10 1)
pico_add_extra_outputs(M10)

add_subdirectory(lib/minmea)
add_subdirectory(lib/sx126x)
add_subdirectory(hal)
add_subdirectory(port)
add_subdirectory(app)

target_link_libraries(M10 PRIVATE radio_driver_app
                                  uart_relay_app
                                  pico_stdlib
                                  FreeRTOS-Kernel
                                  FreeRTOS-Kernel-Static
                      )

set_source_files_properties(main.c
    PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion"
)