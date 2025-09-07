/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 David Alvarez Rosa (david.alvarezrosa.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <pico/time.h>

#include "bsp/board_api.h"
#include "class/hid/hid.h"
#include "usb_descriptors.hpp"

auto tud_mount_cb() -> void {}
auto tud_umount_cb() -> void {}
auto tud_suspend_cb(bool remote_wakeup_en) -> void {}
auto tud_resume_cb() -> void {}
auto tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t* buffer,
                           uint16_t reqlen) -> uint16_t {
  return {};
}
auto tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize) -> void {}

struct KeyStroke {
  uint8_t modifier;
  uint8_t keycode;
};

KeyStroke text[] = {
    {0, HID_KEY_CAPS_LOCK}, {0, HID_KEY_T},     {0, HID_KEY_CAPS_LOCK},
    {0, HID_KEY_E},         {0, HID_KEY_X},     {0, HID_KEY_T},
    {0, HID_KEY_SPACE},     {0, HID_KEY_F},     {0, HID_KEY_R},
    {0, HID_KEY_O},         {0, HID_KEY_M},     {0, HID_KEY_SPACE},
    {0, HID_KEY_F},         {0, HID_KEY_A},     {0, HID_KEY_K},
    {0, HID_KEY_E},         {0, HID_KEY_SPACE}, {0, HID_KEY_K},
    {0, HID_KEY_E},         {0, HID_KEY_Y},     {0, HID_KEY_B},
    {0, HID_KEY_O},         {0, HID_KEY_A},     {0, HID_KEY_R},
    {0, HID_KEY_D},         {0, HID_KEY_ENTER}, {0, HID_KEY_ENTER}};

auto send_hid_report() -> void {
  if (!tud_hid_ready()) {
    return;
  }

  static bool first = true;
  if (first) {
    sleep_ms(1000);
    first = false;
  }

  static auto step = 0;

  uint8_t keycode[6] = {0};
  keycode[0] = text[step].keycode;
  tud_hid_keyboard_report(static_cast<uint8_t>(ReportId::REPORT_ID_KEYBOARD), 0,
                          keycode);
  // Key release
  tud_task();
  keycode[0] = 0;
  tud_hid_keyboard_report(static_cast<uint8_t>(ReportId::REPORT_ID_KEYBOARD), 0,
                          keycode);

  ++step;
  if (step == sizeof(text) / sizeof(text[0])) {
    step = 0;
  }
}

auto hid_task() -> void {
  const uint32_t interval_ms = 75;
  static uint32_t start_ms = 0;
  static bool running = true;
  static bool was_btn = false;

  if (board_millis() - start_ms < interval_ms) {
    return;
  }
  start_ms += interval_ms;

  auto btn = board_button_read();

  if (btn) {
    if (!was_btn) {
      running = !running;
    }
    was_btn = true;
  } else {
    was_btn = false;
  }

  if (tud_suspended()) {
    tud_remote_wakeup();
  } else if (running) {
    send_hid_report();
  }
}

auto main(void) -> int {
  board_init();

  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  sleep_ms(500);

  while (true) {
    tud_task();
    hid_task();
  }
}
