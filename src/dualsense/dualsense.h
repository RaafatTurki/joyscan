#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct JSDualSense JSDualSense;

typedef struct {
  bool connected;

  float gyro[3];
  float accel[3];

  struct {
    bool active;
    float x, y;
  } touch[2];
} JSDualSenseState;

#define JS_PLAYER_LED_LEFT 0x01
#define JS_PLAYER_LED_MIDDLE_LEFT 0x02
#define JS_PLAYER_LED_MIDDLE 0x04
#define JS_PLAYER_LED_MIDDLE_RIGHT 0x08
#define JS_PLAYER_LED_RIGHT 0x10

#ifdef __cplusplus
extern "C" {
#endif

JSDualSense *js_dualsense_create(void);
void js_dualsense_destroy(JSDualSense *device);

bool js_dualsense_update(JSDualSense *device, float delta, JSDualSenseState *out_state);

void js_dualsense_set_lightbar(JSDualSense *device, uint8_t r, uint8_t g, uint8_t b);
void js_dualsense_set_player_leds(JSDualSense *device, uint8_t leds);

#ifdef __cplusplus
}
#endif
