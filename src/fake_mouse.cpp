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

#include "bsp/board_api.h"
#include "usb_descriptors.h"

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

auto send_hid_report(uint32_t btn) -> void {
  if (!tud_hid_ready()) {
    return;
  }

  if (btn) {
    int8_t const delta = 5;
    tud_hid_mouse_report(static_cast<uint>(ReportId::REPORT_ID_MOUSE), 0x00,
                         delta, delta, 0, 0);
  }
}

auto hid_task() -> void {
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms) {
    return;
  }
  start_ms += interval_ms;

  auto btn = board_button_read();

  if (tud_suspended() && btn) {
    tud_remote_wakeup();
  } else {
    send_hid_report(btn);
  }
}

auto main(void) -> int {
  board_init();

  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (true) {
    tud_task();
    hid_task();
  }
}
