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

#include <algorithm>
#include <cmath>
#include <cstdlib>

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

auto send_hid_report() -> void {
  if (!tud_hid_ready()) {
    return;
  }

  constexpr int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
  constexpr float CENTERING_STRENGTH = 0.05f;
  static int mouse_x = SCREEN_WIDTH / 2;
  static int mouse_y = SCREEN_HEIGHT / 2;

  static int8_t prev_dx = 0, prev_dy = 0;
  float angle = atan2f(prev_dy, prev_dx) + ((rand() % 61 - 30) * M_PI / 180.0f);
  int step = (rand() % 4 == 0) ? (rand() % 40 + 20) : (rand() % 16 + 4);
  float natural_dx = cosf(angle) * step;
  float natural_dy = sinf(angle) * step;

  prev_dx = (prev_dx + static_cast<int8_t>(natural_dx)) / 2;
  prev_dy = (prev_dy + static_cast<int8_t>(natural_dy)) / 2;

  int cx = SCREEN_WIDTH / 2, cy = SCREEN_HEIGHT / 2;
  float bias_dx = (cx - mouse_x) * CENTERING_STRENGTH;
  float bias_dy = (cy - mouse_y) * CENTERING_STRENGTH;

  float move_dx = natural_dx + bias_dx;
  float move_dy = natural_dy + bias_dy;

  int8_t delta_x = std::clamp(static_cast<int>(roundf(move_dx)), -127, 127);
  int8_t delta_y = std::clamp(static_cast<int>(roundf(move_dy)), -127, 127);

  mouse_x += delta_x;
  mouse_y += delta_y;
  mouse_x = std::clamp(mouse_x, 0, SCREEN_WIDTH - 1);
  mouse_y = std::clamp(mouse_y, 0, SCREEN_HEIGHT - 1);

  tud_hid_mouse_report(static_cast<uint>(ReportId::REPORT_ID_MOUSE), 0x00,
                       delta_x, delta_y, 0, 0);
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

  while (true) {
    tud_task();
    hid_task();
  }
}
