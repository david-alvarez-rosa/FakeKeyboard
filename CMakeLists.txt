cmake_minimum_required(VERSION 3.13)

include(~/dev/pico-sdk/pico_sdk_init.cmake)
project(FakeKeyboardMouse)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(fake_keyboard)
target_sources(fake_keyboard PUBLIC src/fake_keyboard.cpp
                                    src/usb_descriptors.cpp)
target_include_directories(fake_keyboard PRIVATE include)
target_link_libraries(fake_keyboard PUBLIC pico_stdlib pico_unique_id
                                           tinyusb_device tinyusb_board)
pico_add_extra_outputs(fake_keyboard)

add_executable(fake_mouse)
target_sources(fake_mouse PUBLIC src/fake_mouse.cpp src/usb_descriptors.cpp)
target_include_directories(fake_mouse PRIVATE include)
target_link_libraries(fake_mouse PUBLIC pico_stdlib pico_unique_id
                                        tinyusb_device tinyusb_board)
pico_add_extra_outputs(fake_mouse)

add_executable(fake_keyboard_mouse)
target_sources(fake_keyboard_mouse PUBLIC src/fake_keyboard_mouse.cpp
                                          src/usb_descriptors.cpp)
target_include_directories(fake_keyboard_mouse PRIVATE include)
target_link_libraries(fake_keyboard_mouse PUBLIC pico_stdlib pico_unique_id
                                                 tinyusb_device tinyusb_board)
pico_add_extra_outputs(fake_keyboard_mouse)
