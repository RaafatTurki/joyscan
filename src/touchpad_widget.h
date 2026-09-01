#pragma once

#include <stdio.h>
#include "raylib.h"
#include "colors.h"
#include "layout.h"
#include "dualsense/dualsense.h"

#define TOUCHPAD_ASPECT (1920.0f / 1080.0f)
#define TOUCHPAD_MAX_WIDTH 260
#define TOUCHPAD_ROUNDNESS 0.25f
#define TOUCHPAD_DOT_RADIUS 9
#define TOUCHPAD_LABEL_HEIGHT 36
#define TOUCHPAD_LABEL_TEXT_SIZE 14

void touchpad_widget_build(void) {
  CLAY({
    .id = layout_id("TouchpadPanel"),
    .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .childGap = 4, .childAlignment = { .x = CLAY_ALIGN_X_CENTER } },
  }) {
    CLAY({
      .id = layout_id("Touchpad"),
      .layout = { .sizing = { CLAY_SIZING_GROW(0, TOUCHPAD_MAX_WIDTH), CLAY_SIZING_GROW(0, TOUCHPAD_MAX_WIDTH / TOUCHPAD_ASPECT) } },
      .aspectRatio = { TOUCHPAD_ASPECT },
    }) {}

    CLAY({
      .id = layout_id("TouchpadLabels"),
      .layout = { .sizing = { CLAY_SIZING_FIXED(TOUCHPAD_MAX_WIDTH), CLAY_SIZING_FIXED(TOUCHPAD_LABEL_HEIGHT) } },
    }) {}
  }
}

static void touchpad_widget_draw_label(Rectangle bounds, int line, const char *text) {
  int text_width = MeasureText(text, TOUCHPAD_LABEL_TEXT_SIZE);
  DrawText(
    text,
    (int)(bounds.x + (bounds.width - text_width) / 2.0f),
    (int)(bounds.y + line * (TOUCHPAD_LABEL_TEXT_SIZE + 2)),
    TOUCHPAD_LABEL_TEXT_SIZE,
    COLOR_TEXT
  );
}

void touchpad_widget_draw(const JSDualSenseState *state) {
  Rectangle bounds = layout_rect("Touchpad");
  if (bounds.width <= 0) return;

  Color outline = state->touchpad_pressed ? COLOR_PRESS : COLOR_OUTLINE;
  DrawRectangleRounded(bounds, TOUCHPAD_ROUNDNESS, 12, COLOR_BG);
  DrawRectangleRoundedLinesEx(bounds, TOUCHPAD_ROUNDNESS, 12, 2, outline);

  for (int i = 0; i < 2; i++) {
    if (!state->touch[i].active) continue;

    Vector2 dot = {
      bounds.x + state->touch[i].x * bounds.width,
      bounds.y + state->touch[i].y * bounds.height,
    };
    DrawCircleV(dot, TOUCHPAD_DOT_RADIUS, COLOR_PRESS);
    DrawCircleLinesV(dot, TOUCHPAD_DOT_RADIUS, COLOR_OUTLINE);
  }

  Rectangle labels_bounds = layout_rect("TouchpadLabels");
  char line[64];
  for (int i = 0; i < 2; i++) {
    if (state->touch[i].active) {
      snprintf(line, sizeof(line), "P%d  x:%.2f y:%.2f", i + 1, state->touch[i].x, state->touch[i].y);
    } else {
      snprintf(line, sizeof(line), "P%d  --", i + 1);
    }
    touchpad_widget_draw_label(labels_bounds, i, line);
  }
}
