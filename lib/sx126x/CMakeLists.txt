# The Semtech SX126x driver is third-party code: build it as its own static
# library target and do not subject it to our strict project warnings.
add_library(sx126x STATIC)

target_sources(sx126x
    PRIVATE
        sx126x.c
        sx126x_driver_version.c
)

# SYSTEM PUBLIC: the path still travels, but our strict warnings skip this
# third-party header.
target_include_directories(sx126x SYSTEM PUBLIC .)

target_compile_options(sx126x PRIVATE -Wall -Wextra)