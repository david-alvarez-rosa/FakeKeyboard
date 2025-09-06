cmake_minimum_required(VERSION 3.13)

include(~/dev/pico-sdk/pico_sdk_init.cmake)
project(FakeKeyboard C CXX ASM)

# set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(fake_keyboard)
target_sources(
  fake_keyboard PUBLIC ${CMAKE_CURRENT_LIST_DIR}/main.c
                           ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c)
target_include_directories(fake_keyboard PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(fake_keyboard PUBLIC pico_stdlib pico_unique_id
                                               tinyusb_device tinyusb_board)
pico_add_extra_outputs(fake_keyboard)
