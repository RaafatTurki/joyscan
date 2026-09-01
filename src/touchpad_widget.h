#pragma once

#include "raylib.h"
#include "colors.h"
#include "layout.h"
#include "dualsense/dualsense.h"

#define TOUCHPAD_ASPECT (1920.0f / 1080.0f)
#define TOUCHPAD_MAX_WIDTH 260
#define TOUCHPAD_ROUNDNESS 0.25f
#define TOUCHPAD_DOT_RADIUS 9

void touchpad_widget_build(void) {
  CLAY({
    .id = layout_id("Touchpad"),
    .layout = { .sizing = { CLAY_SIZING_GROW(0, TOUCHPAD_MAX_WIDTH), CLAY_SIZING_GROW(0, TOUCHPAD_MAX_WIDTH / TOUCHPAD_ASPECT) } },
    .aspectRatio = { TOUCHPAD_ASPECT },
  }) {}
}

void touchpad_widget_draw(const JSDualSenseState *state) {
  Rectangle bounds = layout_rect("Touchpad");
  if (bounds.width <= 0) return;

  DrawRectangleRounded(bounds, TOUCHPAD_ROUNDNESS, 12, COLOR_BG);
  DrawRectangleRoundedLinesEx(bounds, TOUCHPAD_ROUNDNESS, 12, 2, COLOR_OUTLINE);

  for (int i = 0; i < 2; i++) {
    if (!state->touch[i].active) continue;

    Vector2 dot = {
      bounds.x + state->touch[i].x * bounds.width,
      bounds.y + state->touch[i].y * bounds.height,
    };
    DrawCircleV(dot, TOUCHPAD_DOT_RADIUS, COLOR_PRESS);
    DrawCircleLinesV(dot, TOUCHPAD_DOT_RADIUS, COLOR_OUTLINE);
  }
}
