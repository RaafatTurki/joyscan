#pragma once

#include <stdio.h>
#include "raylib.h"
#include "../deps/raygui/src/raygui.h"
#include "colors.h"
#include "layout.h"

#define MINI_ITEM_WIDTH 60
#define MINI_ITEM_HEIGHT 96
#define MINI_METER_WIDTH 10
#define MINI_METER_HEIGHT 50
#define GROUP_GAP 8
#define GROUP_PADDING 30

#define BUTTON_LIST_MIN_CTRL_WIDTH 250
#define BUTTON_LIST_MIN_CTRL_HEIGHT 230

typedef struct {
  int button;
  const char *name;
} GamepadButtonEntry;

static const GamepadButtonEntry BUTTON_LIST_DPAD[] = {
  { GAMEPAD_BUTTON_LEFT_FACE_DOWN,  "Down" },
  { GAMEPAD_BUTTON_LEFT_FACE_LEFT,  "Left" },
  { GAMEPAD_BUTTON_LEFT_FACE_UP,    "Up" },
  { GAMEPAD_BUTTON_LEFT_FACE_RIGHT, "Right" },
};

static const GamepadButtonEntry BUTTON_LIST_FACE[] = {
  { GAMEPAD_BUTTON_RIGHT_FACE_LEFT,  "X" },
  { GAMEPAD_BUTTON_RIGHT_FACE_UP,    "Y" },
  { GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, "B" },
  { GAMEPAD_BUTTON_RIGHT_FACE_DOWN,  "A" },
};

static const GamepadButtonEntry BUTTON_LIST_LEFT_SHOULDER[] = {
  { GAMEPAD_BUTTON_LEFT_TRIGGER_2, "L2" },
  { GAMEPAD_BUTTON_LEFT_TRIGGER_1, "L1" },
};

static const GamepadButtonEntry BUTTON_LIST_RIGHT_SHOULDER[] = {
  { GAMEPAD_BUTTON_RIGHT_TRIGGER_1, "R1" },
  { GAMEPAD_BUTTON_RIGHT_TRIGGER_2, "R2" },
};

static const GamepadButtonEntry BUTTON_LIST_MIDDLE[] = {
  { GAMEPAD_BUTTON_MIDDLE_LEFT,  "Select" },
  { GAMEPAD_BUTTON_MIDDLE,       "Home" },
  { GAMEPAD_BUTTON_MIDDLE_RIGHT, "Start" },
};

#define BUTTON_LIST_COUNT(list) (int)(sizeof(list) / sizeof(list[0]))

static float gamepad_button_fraction(int id, int button) {
  if (button == GAMEPAD_BUTTON_LEFT_TRIGGER_2 || button == GAMEPAD_BUTTON_RIGHT_TRIGGER_2) {
    int axis = (button == GAMEPAD_BUTTON_LEFT_TRIGGER_2) ? GAMEPAD_AXIS_LEFT_TRIGGER : GAMEPAD_AXIS_RIGHT_TRIGGER;
    float fraction = (GetGamepadAxisMovement(id, axis) + 1) / 2;
    if (IsGamepadButtonDown(id, button)) fraction = 1.0f;
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;
    return fraction;
  }
  return IsGamepadButtonDown(id, button) ? 1.0f : 0.0f;
}

static void draw_vertical_meter(Rectangle bounds, float fraction) {
  DrawRectangleLinesEx(bounds, 1, COLOR_OUTLINE);
  float fill_height = bounds.height * fraction;
  Rectangle fill = { bounds.x, bounds.y + bounds.height - fill_height, bounds.width, fill_height };
  DrawRectangleRec(fill, COLOR_PRESS);
}

static void button_item_build(const char *item_id, int index) {
  CLAY({
    .id = layout_id_i(item_id, index),
    .layout = { .sizing = { CLAY_SIZING_FIXED(MINI_ITEM_WIDTH), CLAY_SIZING_FIXED(MINI_ITEM_HEIGHT) } },
  }) {}
}

static void button_group_build(const char *group_id, const char *item_id, int count) {
  CLAY({
    .id = layout_id(group_id),
    .layout = { .sizing = { CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0) }, .childGap = GROUP_GAP },
  }) {
    for (int i = 0; i < count; i++) button_item_build(item_id, i);
  }
}

static void button_item_draw(int id, const char *item_id, int index, const GamepadButtonEntry *entry) {
  Rectangle bounds = layout_rect_i(item_id, index);
  int text_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  float fraction = gamepad_button_fraction(id, entry->button);

  int name_width = MeasureText(entry->name, text_size);
  DrawText(entry->name, (int)(bounds.x + (bounds.width - name_width) / 2.0f), (int)bounds.y, text_size, COLOR_TEXT);

  Rectangle meter_bounds = {
    bounds.x + (bounds.width - MINI_METER_WIDTH) / 2.0f,
    bounds.y + text_size + 4,
    MINI_METER_WIDTH,
    MINI_METER_HEIGHT,
  };
  draw_vertical_meter(meter_bounds, fraction);

  char value_text[8];
  snprintf(value_text, sizeof(value_text), "%.2f", fraction);
  int value_width = MeasureText(value_text, text_size);
  DrawText(value_text, (int)(bounds.x + (bounds.width - value_width) / 2.0f), (int)(meter_bounds.y + meter_bounds.height + 4), text_size, COLOR_TEXT);
}

static void button_group_draw(int id, const char *item_id, const GamepadButtonEntry *entries, int count) {
  for (int i = 0; i < count; i++) button_item_draw(id, item_id, i, &entries[i]);
}

bool button_list_fits(Rectangle content_area) {
  float dpad_face_width = BUTTON_LIST_COUNT(BUTTON_LIST_DPAD) * MINI_ITEM_WIDTH + (BUTTON_LIST_COUNT(BUTTON_LIST_DPAD) - 1) * GROUP_GAP;
  float min_width = 2 * (dpad_face_width + GROUP_PADDING) + BUTTON_LIST_MIN_CTRL_WIDTH;
  float min_height = 2 * (MINI_ITEM_HEIGHT + GROUP_PADDING) + BUTTON_LIST_MIN_CTRL_HEIGHT;
  return content_area.width >= min_width && content_area.height >= min_height;
}

void button_list_build(void) {
  CLAY({
    .id = layout_id("MiddleRow"),
    .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) }, .childGap = GROUP_PADDING, .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } },
  }) {
    button_group_build("Dpad", "DpadItem", BUTTON_LIST_COUNT(BUTTON_LIST_DPAD));

    CLAY({
      .id = layout_id("ControllerColumn"),
      .layout = {
        .sizing = { CLAY_SIZING_GROW(0, CTRL_SVG_WIDTH), CLAY_SIZING_GROW(0) },
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
        .childGap = GROUP_PADDING,
        .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
      },
    }) {
      CLAY({
        .id = layout_id("ShoulderRow"),
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) } },
      }) {
        button_group_build("LShoulder", "LShoulderItem", BUTTON_LIST_COUNT(BUTTON_LIST_LEFT_SHOULDER));
        CLAY({ .id = layout_id("ShoulderSpacer"), .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) } } }) {}
        button_group_build("RShoulder", "RShoulderItem", BUTTON_LIST_COUNT(BUTTON_LIST_RIGHT_SHOULDER));
      }

      CLAY({
        .id = layout_id("Controller"),
        .layout = { .sizing = { CLAY_SIZING_GROW(0, CTRL_SVG_WIDTH), CLAY_SIZING_GROW(0, CTRL_SVG_HEIGHT) } },
        .aspectRatio = { CTRL_SVG_WIDTH / (float)CTRL_SVG_HEIGHT },
      }) {}

      CLAY({
        .id = layout_id("BottomRow"),
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) }, .childAlignment = { .x = CLAY_ALIGN_X_CENTER } },
      }) {
        button_group_build("Middle", "MiddleItem", BUTTON_LIST_COUNT(BUTTON_LIST_MIDDLE));
      }
    }

    button_group_build("Face", "FaceItem", BUTTON_LIST_COUNT(BUTTON_LIST_FACE));
  }
}

Rectangle button_list_draw(int id) {
  button_group_draw(id, "DpadItem", BUTTON_LIST_DPAD, BUTTON_LIST_COUNT(BUTTON_LIST_DPAD));
  button_group_draw(id, "FaceItem", BUTTON_LIST_FACE, BUTTON_LIST_COUNT(BUTTON_LIST_FACE));
  button_group_draw(id, "LShoulderItem", BUTTON_LIST_LEFT_SHOULDER, BUTTON_LIST_COUNT(BUTTON_LIST_LEFT_SHOULDER));
  button_group_draw(id, "RShoulderItem", BUTTON_LIST_RIGHT_SHOULDER, BUTTON_LIST_COUNT(BUTTON_LIST_RIGHT_SHOULDER));
  button_group_draw(id, "MiddleItem", BUTTON_LIST_MIDDLE, BUTTON_LIST_COUNT(BUTTON_LIST_MIDDLE));
  return layout_rect("Controller");
}
