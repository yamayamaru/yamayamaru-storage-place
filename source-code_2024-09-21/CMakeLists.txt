cmake_minimum_required(VERSION 3.12)



#set(PICO_PLATFORM rp2040)
#set(PICO_PLATFORM rp2350-arm-s)
set(PICO_PLATFORM rp2350-riscv)
#set(PICO_BOARD pico)
#set(PICO_BOARD picow)
set(PICO_BOARD pico2)
#set(PICO_BOARD pimoroni_pico_plus2_rp2350)





# Pull in PICO SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


project(riscv_rdcycle_usb01_project)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()
add_executable(riscv_rdcycle_usb01 riscv_rdcycle_usb01.c)
target_link_libraries(riscv_rdcycle_usb01 
                              pico_stdlib
)

pico_enable_stdio_usb(riscv_rdcycle_usb01 1)
pico_enable_stdio_uart(riscv_rdcycle_usb01 0)

pico_add_extra_outputs(riscv_rdcycle_usb01)
