#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

void hid_task(void);

int main(void) {
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

void tud_mount_cb(void) {}
void tud_umount_cb(void) {}
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }
void tud_resume_cb(void) {}

static void send_hid_report(uint8_t report_id, uint32_t btn) {
  if (!tud_hid_ready()) return;

  switch (report_id) {
    case REPORT_ID_KEYBOARD: {
      static bool has_keyboard_key = false;

      if (btn) {
        uint8_t keycode[6] = {0};
        keycode[0] = HID_KEY_A;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
        has_keyboard_key = true;
      } else {
        if (has_keyboard_key)
          tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    } break;

    case REPORT_ID_MOUSE: {
      int8_t const delta = 5;
      tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
    } break;
  }
}

void hid_task(void) {
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms) return;
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  if (tud_suspended() && btn) {
    tud_remote_wakeup();
  } else {
    send_hid_report(REPORT_ID_KEYBOARD, btn);
  }
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report,
                                uint16_t len) {
  uint8_t next_report_id = report[0] + 1u;

  if (next_report_id < REPORT_ID_COUNT) {
    send_hid_report(next_report_id, board_button_read());
  }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen) {
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize) {}
