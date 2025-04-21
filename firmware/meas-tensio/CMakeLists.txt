cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

option(DEBUG "Enable debug output via USART" OFF)

set(HAL_DIR ../../hal)
set(CANLIB_DIR ../avr-can-lib)
set(LIBSTDCPP_INCLUDE_DIR ${HAL_DIR}/include/avr-stdc++)

set(AVR_MCU atmega168p)
set(MCU_FREQ 1000000UL)
set(CMAKE_TOOLCHAIN_FILE "${HAL_DIR}/cmake-avr/generic-gcc-avr.cmake")
add_compile_definitions(F_CPU=${MCU_FREQ})
add_compile_options(
    -fpack-struct
    -fshort-enums
    -Wall
)

project(meas-tensio)


add_subdirectory(${CANLIB_DIR} canlib)
avr_target_include_directories(can
    PUBLIC
        ${CMAKE_SOURCE_DIR} # can_config.h
)
target_compile_options(can${MCU_TYPE_FOR_FILENAME}
    PRIVATE
        -Wstrict-prototypes
)

# measurement with CAN interface
set(meas_hal_modules
    adc_irq.cpp
)
if(DEBUG)
   list(APPEND meas_hal_modules
      usart_irq.cpp
      usart_stdout.c
   )
endif()

string(REGEX REPLACE "([^;]+)" "${HAL_DIR}/src/hal/avr/\\1"
    meas_hal_modules_sources "${meas_hal_modules}") # add path to lib to modules
add_avr_executable(meas
    meas.cpp
    acquire.h
    acquire.cpp
    ../utils/clock.h
    ../utils/clock.cpp
    ../utils/settings.h
    ../utils/settings.cpp
    ${meas_hal_modules_sources}
)
avr_target_include_directories(meas
   PRIVATE
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/../../common
      ${HAL_DIR}/include
      ${CANLIB_DIR}
      ${LIBSTDCPP_INCLUDE_DIR}
)
avr_target_link_libraries(meas
   PRIVATE
      can
)
set_property(
   SOURCE meas.cpp ../utils/settings.cpp
   APPEND
   PROPERTY COMPILE_DEFINITIONS MY_DEVICE_ID=250
)
if(DEBUG)
   set_property(
      SOURCE meas.cpp
      APPEND
      PROPERTY COMPILE_DEFINITIONS DEBUG
   )
endif()

# test app with simple serial interface
set(serial_hal_modules
    adc_irq.cpp
    usart_irq.cpp
    usart_stdout.c
)
string(REGEX REPLACE "([^;]+)" "${HAL_DIR}/src/hal/avr/\\1"
    serial_hal_modules_sources "${serial_hal_modules}") # add path to lib to modules
add_avr_executable(serial
    serial.cpp
    acquire.h
    acquire.cpp
    ${serial_hal_modules_sources}
)
avr_target_include_directories(serial
   PRIVATE
      ${HAL_DIR}/include
      ${LIBSTDCPP_INCLUDE_DIR}
)
