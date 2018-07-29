#include "crkbd.h"
#include "bootloader.h"
#include "action_layer.h"
#include "action_util.h"
#include "eeconfig.h"
#ifdef PROTOCOL_LUFA
#include "lufa.h"
#include "split_util.h"
#endif
#include "LUFA/Drivers/Peripheral/TWI.h"
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

// #include "../lib/mode_icon_reader.c"
// #include "../lib/layer_state_reader.c"
// #include "../lib/host_led_state_reader.c"
// #include "../lib/logo_reader.c"
// #include "../lib/keylogger.c"
// #include "../lib/timelogger.c"

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _FUNCT 3
#define _ARROW 4
#define _ADJUST 5

#define _______ KC_TRNS
#define XXXXXXX KC_NO

#define NEXTWKS ACTION_MODS_KEY(MOD_LCTL, KC_RGHT)
#define PREVWKS ACTION_MODS_KEY(MOD_LCTL, KC_LEFT)
#define NEXTTAB ACTION_MODS_KEY(MOD_LGUI, KC_RCBR)
#define PREVTAB ACTION_MODS_KEY(MOD_LGUI, KC_LCBR)
#define WINCOPY ACTION_MODS_KEY(MOD_LCTL, KC_C)
#define WINPASTE ACTION_MODS_KEY(MOD_LCTL, KC_V)
#define MACUNDO ACTION_MODS_KEY(MOD_LGUI, KC_Z)
#define MACCUT ACTION_MODS_KEY(MOD_LGUI, KC_X)
#define MACCOPY ACTION_MODS_KEY(MOD_LGUI, KC_C)
#define MACPASTE ACTION_MODS_KEY(MOD_LGUI, KC_V)
#define MACFIND ACTION_MODS_KEY(MOD_LGUI, KC_F)
#define SPTLGHT ACTION_MODS_KEY(MOD_LGUI, KC_SPC)
#define MISSIONCTL ACTION_MODS_KEY(MOD_LCTL, KC_UP)
#define NEXTAPP ACTION_MODS_KEY(MOD_LGUI, KC_TAB)
#define PREVAPP ACTION_MODS_KEY(MOD_LGUI | MOD_LSFT, KC_TAB)

//Macros
#define SEND_KEYMAP_URI 0
#define MAC_COPY_PASTE 1
#define WIN_COPY_PASTE 2

// TAP DANCE
enum {
    TD_CTL_SPL = 0,
    TD_ALT_MISSIONCTL
};


enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  FUNCT,
  ARROW,
  ADJUST,
  RGBRST
  // DYNAMIC_MACRO_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Qwerty
 * ,-----------------------------------------.,-----------------------------------------.
 * | Tab  |   Q  |   W  |   E  |   R  |   T  ||   Y  |   U  |   I  |   O  |   P  |  \   |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |TD_SPL|      |      |      |      |      ||      |      |      |      |   ;  |  '   |
 * | Ctrl |   A  |   S  |   D  |   F  |   G  ||   H  |   J  |   K  |   L  | Arrow| Ctrl |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |  [   |   Z  |      |      |      |      ||      |      |      |      |   /  |   ]  |
 * | Shift| Raise|   X  |   C  |   V  |   B  ||   N  |   M  |   ,  |   .  | Lower| Shift|
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |FUNCT | BS   || Esc  | ARROW|TD_MSN|
 *                      | GUI  |Space | Lower|| Raise| Enter| Alt  |
 *                      `--------------------'`--------------------'
 */

  [_QWERTY] = LAYOUT( \
    KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, \
    TD(TD_CTL_SPL), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, LT(_ARROW, KC_SCLN), CTL_T(KC_QUOT), \
    SFT_T(KC_LBRC), LT(_RAISE, KC_Z), KC_X, KC_C, KC_V, KC_B, \
    KC_N, KC_M, KC_COMM, KC_DOT, LT(_LOWER, KC_SLSH), SFT_T(KC_RBRC), \
    KC_LGUI, LT(_FUNCT, KC_SPC), LT(_LOWER, KC_BSPC), LT(_RAISE, KC_ESC), LT(_ARROW, KC_ENT), TD(TD_ALT_MISSIONCTL)
  ),

/* Lower
 * ,-----------------------------------------.,-----------------------------------------.
 * |   ~  |   !  |   @  |   #  |   $  |   %  ||   ^  |   &  |   *  |   (  |   )  | Bksp |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      |      |      |W FIND|      ||      |   _  |   +  |   {  |   }  | Del  |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |W UNDO|W CUT |W COPY|WPASTE|      ||      |      |      |      |XXXXXX|      |
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |      |XXXXXX||      |      |      |
 *                      `--------------------'`--------------------'
 */
  [_LOWER] = LAYOUT( \
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC, \
    _______, _______, _______, _______, MACFIND, _______, _______, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_DEL, \
    _______, MACUNDO, MACCUT,  MACCOPY, MACPASTE, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

/* Raise
 * ,-----------------------------------------.,-----------------------------------------.
 * |   `  |   1  |   2  |   3  |   4  |   5  ||   6  |   7  |   8  |   9  |   0  | Bksp |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      |      |      |W FIND|      ||      |   -  |   =  |   [  |   ]  | Del  |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |XXXXXX|W CUT |W COPY|WPASTE|      ||      |      |      |      |      |      |
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |      |      ||XXXXXX|      |      |
 *                      `--------------------'`--------------------'
 */
  [_RAISE] = LAYOUT( \
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
    _______, _______, _______, _______, MACFIND, _______, _______, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_DEL, \
    _______, _______, MACCUT,  MACCOPY, MACPASTE, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______ \

  ),

/* Function
 * ,-----------------------------------------.,-----------------------------------------.
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  ||  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      |M C/P |M COPY|MPASTE|      ||      |      |      |      |      |      |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      |W C/P |W COPY|WPASTE|      ||      |      |      |KEYMAP|      |      |
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |XXXXXX|      ||      |      |      |
 *                      `--------------------'`--------------------'
 */
  [_FUNCT] = LAYOUT( \
    KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, \
    _______, _______, M(MAC_COPY_PASTE), MACCOPY, MACPASTE, _______, \
    _______, _______, _______, _______, _______, _______, \
    _______, _______, M(WIN_COPY_PASTE), WINCOPY, WINPASTE, _______, \
    _______, _______, _______, M(SEND_KEYMAP_URI), _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

/* Arrow
 * ,-----------------------------------------.,-----------------------------------------.
 * |      |      |      |  Up  |      |      ||      |      |      |      |      |      |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      | Left | Down | Right|      || Left | Down |  Up  | Right|XXXXXX|      |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |      |      |      |      |      |      ||      |      |      |      |      |      |
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |      |      ||      |XXXXXX|      |
 *                      `--------------------'`--------------------'
 */
  [_ARROW] = LAYOUT( \
    _______, _______, _______, KC_UP, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______ \
  ),

/* Adjust
 * ,-----------------------------------------.,-----------------------------------------.
 * |RESET |      |      |      |      |      ||      |      |      |      |      |      |
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |RGB on|RGBmod|RGBRST|      |      |      ||      |      |      |      |RGB v-|RGB v+|
 * |------+------+------+------+------+------||------+------+------+------+------+------|
 * |RGB h-|RGB h+|      |      |      |      ||      |      |      |      |RGB s-|RGB s+|
 * `------+------+------+------+------+------||------+------+------+------+------+------'
 *                      |      |XXXXXX|      ||      |XXXXXX|      |
 *                      `--------------------'`--------------------'
 */
  [_ADJUST] = LAYOUT( \
    RESET,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    RGB_TOG, RGB_MOD, RGBRST,  _______, _______, _______, _______, _______, _______, _______, RGB_VAD, RGB_VAI, \
    RGB_HUD, RGB_HUI, _______, _______, _______, _______, _______, _______, _______, _______, RGB_SAD, RGB_SAI, \
    _______, _______, _______, _______, _______, _______ \
  )
};

static uint16_t start;
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch(id) {
        case SEND_KEYMAP_URI:
            if (record->event.pressed) {
                SEND_STRING("https://github.com/rinx/qmk_firmware_crkbd_rinx/blob/master/keymap.c");
            }
            break;
        case MAC_COPY_PASTE:
            if (record->event.pressed) {
                start = timer_read();
            } else {
                if (timer_elapsed(start) >= TAPPING_TERM)
                    return MACRO(D(LGUI), T(C), U(LGUI), END);
                else
                    return MACRO(D(LGUI), T(V), U(LGUI), END);
            }
            break;
        case WIN_COPY_PASTE:
            if (record->event.pressed) {
                start = timer_read();
            } else {
                if (timer_elapsed(start) >= TAPPING_TERM)
                    return MACRO(D(LCTL), T(C), U(LCTL), END);
                else
                    return MACRO(D(LCTL), T(V), U(LCTL), END);
            }
            break;
    }
    return MACRO_NONE;
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_CTL_SPL] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, SPTLGHT),
  [TD_ALT_MISSIONCTL] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, MISSIONCTL)
};

// define variables for reactive RGB
bool TOG_STATUS = false;
bool INIT_STATUS = true;
int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // if (record->event.pressed) {
  //   set_keylog(keycode, record);
  //   set_timelog();
  // }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
      } else {
        layer_off(_LOWER);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
      } else {
        layer_off(_RAISE);
      }
      return false;
      break;
    case FUNCT:
      if (record->event.pressed) {
        layer_on(_FUNCT);
        update_tri_layer_RGB(_FUNCT, _ARROW, _ADJUST);
      } else {
        layer_off(_FUNCT);
        update_tri_layer_RGB(_FUNCT, _ARROW, _ADJUST);
      }
      return false;
      break;
    case ARROW:
      if (record->event.pressed) {
        layer_on(_ARROW);
        update_tri_layer_RGB(_FUNCT, _ARROW, _ADJUST);
      } else {
        layer_off(_ARROW);
        update_tri_layer_RGB(_FUNCT, _ARROW, _ADJUST);
      }
      return false;
      break;
    case ADJUST:
        if (record->event.pressed) {
          layer_on(_ADJUST);
        } else {
          layer_off(_ADJUST);
        }
        return false;
        break;
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
      break;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
  }
  return true;
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 800000));
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

void matrix_scan_user(void) {
    #ifdef RGBLIGHT_ENABLE
        uint8_t layer = biton32(layer_state);
        switch (layer) {
            case _LOWER:
                if (TOG_STATUS) {
                } else {
                    RGB_current_mode = rgblight_config.mode;
                    TOG_STATUS = true;
                    // rgblight_mode(RGB_current_mode);
                }
                break;
            case _RAISE:
                if (TOG_STATUS) {
                } else {
                    RGB_current_mode = rgblight_config.mode;
                    TOG_STATUS = true;
                    // rgblight_mode(RGB_current_mode);
                }
                break;
            case _FUNCT:
                if (TOG_STATUS) {
                } else {
                    RGB_current_mode = rgblight_config.mode;
                    TOG_STATUS = true;
                    rgblight_mode(27);
                }
                break;
            case _ARROW:
                if (TOG_STATUS) {
                } else {
                    RGB_current_mode = rgblight_config.mode;
                    TOG_STATUS = true;
                    rgblight_mode(16);
                }
                break;
            case _ADJUST:
                // if (TOG_STATUS) {
                // } else {
                //     RGB_current_mode = rgblight_config.mode;
                //     TOG_STATUS = true;
                //     rgblight_mode(15);
                // }
                break;
            default:
                TOG_STATUS = false;
                rgblight_mode(RGB_current_mode);
                break;
        }
    #endif
    iota_gfx_task();
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void render_status(struct CharacterMatrix *matrix) {

  static char logo[]={
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
    0};
  matrix_write(matrix, logo);

  // Define layers here, Have not worked out how to have text displayed for each layer. Copy down the number you see and add a case for it below
  char buf[40];
  snprintf(buf,sizeof(buf), "Und-%ld", layer_state);

  uint8_t layer = biton32(layer_state);
  matrix_write_P(matrix, PSTR("Layer:"));
    switch (layer) {
        case _QWERTY:
            matrix_write_P(matrix, PSTR("QWERTY"));
            break;
        case _LOWER:
            matrix_write_P(matrix, PSTR("Lower "));
            break;
        case _RAISE:
            matrix_write_P(matrix, PSTR("Raise "));
            break;
        case _FUNCT:
            matrix_write_P(matrix, PSTR("Funct."));
            break;
        case _ARROW:
            matrix_write_P(matrix, PSTR("Arrow "));
            break;
        case _ADJUST:
            matrix_write_P(matrix, PSTR("Adjust"));
            break;
        default:
            matrix_write(matrix, buf);
            break;
    }

    #ifdef RGBLIGHT_ENABLE
        if (INIT_STATUS) {
            RGB_current_mode = rgblight_config.mode;
            INIT_STATUS = false;
        }
        char rgbbuf[20];
        snprintf(rgbbuf,sizeof(rgbbuf), "|RGB:%d", RGB_current_mode);
        matrix_write(matrix, rgbbuf);
    #endif
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  if(is_master){
    render_status(&matrix);
  }
  matrix_update(&display, &matrix);
}

#endif
