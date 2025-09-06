cmake_minimum_required(VERSION 3.13)

include(~/dev/pico-sdk/pico_sdk_init.cmake)
project(HelloWorld C CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(FakeKeyboard main.cpp)
pico_enable_stdio_usb(FakeKeyboard 1)
pico_enable_stdio_uart(FakeKeyboard 0)
target_link_libraries(FakeKeyboard pico_stdlib)
pico_add_extra_outputs(FakeKeyboard)
