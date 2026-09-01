#pragma once

#include <math.h>
#include <string.h>
#include "raylib.h"
#include "colors.h"
#include "input.h"
#include "utils.h"
#include "svg.h"

#define CTRL_SVG_WIDTH 550
#define CTRL_SVG_HEIGHT 503

#define STICK_SLIDE 9.5f

static const char *CTRL_PRESSABLE_IDS[] = {
  "BTop", "BRight", "BBottom", "BLeft",
  "DUp", "DRight", "DDown", "DLeft",
  "LMeta", "RMeta", "Home",
  "L1", "R1", "L2", "R2",
  "LeftStick", "RightStick",
};
#define CTRL_PRESSABLE_ID_COUNT (sizeof(CTRL_PRESSABLE_IDS) / sizeof(CTRL_PRESSABLE_IDS[0]))

SvgController ctrl_svg;
Texture2D ctrl_tex;

Vector2 left_stick_smooth = {0, 0};
Vector2 right_stick_smooth = {0, 0};

static bool ctrl_id_is_pressable(const char *id) {
  for (size_t i = 0; i < CTRL_PRESSABLE_ID_COUNT; i++) {
    if (strcmp(id, CTRL_PRESSABLE_IDS[i]) == 0) return true;
  }
  return false;
}

void style_controller_svg(SvgController *sc) {
  svg_set_stroke_width(sc, 3.0f);

  for (size_t i = 0; i < CTRL_PRESSABLE_ID_COUNT; i++) {
    svg_hide(sc, CTRL_PRESSABLE_IDS[i]);
    svg_set_stroke(sc, CTRL_PRESSABLE_IDS[i], COLOR_PRESS);
  }

  svg_hide(sc, "LStickDot");
  svg_hide(sc, "RStickDot");

  for (int i = 0; i < sc->snapshot_count; i++) {
    NSVGshape *shape = sc->snapshots[i].shape;
    if (ctrl_id_is_pressable(shape->id)) continue;
    if (strcmp(shape->id, "LStickDot") == 0 || strcmp(shape->id, "RStickDot") == 0) continue;

    if (strcmp(shape->id, "Outline") == 0) {
      shape->fill.type = NSVG_PAINT_COLOR;
      shape->fill.color = NSVG_RGB(COLOR_BG.r, COLOR_BG.g, COLOR_BG.b) | ((unsigned int)COLOR_BG.a << 24);
    }
    if (shape->stroke.type != NSVG_PAINT_NONE) {
      shape->stroke.type = NSVG_PAINT_COLOR;
      shape->stroke.color = NSVG_RGB(COLOR_OUTLINE.r, COLOR_OUTLINE.g, COLOR_OUTLINE.b) | ((unsigned int)COLOR_OUTLINE.a << 24);
    }
  }

  svg_commit(sc);
}

void controller_init(void) {
  ctrl_svg = svg_controller_load("assets/controller.svg", CTRL_SVG_WIDTH, CTRL_SVG_HEIGHT);
  style_controller_svg(&ctrl_svg);
  Image ctrl_img = {
    .data = ctrl_svg.pixels,
    .width = ctrl_svg.width,
    .height = ctrl_svg.height,
    .mipmaps = 1,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
  };
  ctrl_tex = LoadTextureFromImage(ctrl_img);
  SetTextureFilter(ctrl_tex, TEXTURE_FILTER_BILINEAR);
}

void controller_deinit(void) {
  svg_controller_unload(&ctrl_svg);
  UnloadTexture(ctrl_tex);
}

Rectangle display_gamepad(int id, Rectangle area) {
  Input LX = input_new(GAMEPAD_AXIS_LEFT_X,              YELLOW, 1, 18, 20, INPUT_ANALOG, 0, "Left X Axis", NULL);
  Input LY = input_new(GAMEPAD_AXIS_LEFT_Y,              YELLOW, 1, 19, 20, INPUT_ANALOG, 0, "Left Y Axis", NULL);
  Input RX = input_new(GAMEPAD_AXIS_RIGHT_X,             YELLOW, 1, 20, 20, INPUT_ANALOG, 0, "Right X Axis", NULL);
  Input RY = input_new(GAMEPAD_AXIS_RIGHT_Y,             YELLOW, 1, 21, 20, INPUT_ANALOG, 0, "Right Y Axis", NULL);

  Input inputs[] = {
    input_new(GAMEPAD_BUTTON_MIDDLE_RIGHT,      RED,    1, 1, 20, INPUT_BUTTON, 0, "Start", "RMeta"),
    input_new(GAMEPAD_BUTTON_MIDDLE_LEFT,       RED,    1, 2, 20, INPUT_BUTTON, 0, "Select", "LMeta"),
    input_new(GAMEPAD_BUTTON_MIDDLE,            GREEN,  1, 3, 20, INPUT_BUTTON, 0, "Home", "Home"),

    input_new(GAMEPAD_BUTTON_RIGHT_FACE_UP,     GOLD,   1, 4, 20, INPUT_BUTTON, 0, "Y", "BTop"),
    input_new(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,  LIME,   1, 5, 20, INPUT_BUTTON, 0, "B", "BRight"),
    input_new(GAMEPAD_BUTTON_RIGHT_FACE_DOWN,   BLUE,   1, 6, 20, INPUT_BUTTON, 0, "A", "BBottom"),
    input_new(GAMEPAD_BUTTON_RIGHT_FACE_LEFT,   MAROON, 1, 7, 20, INPUT_BUTTON, 0, "X", "BLeft"),

    input_new(GAMEPAD_BUTTON_LEFT_FACE_UP,      RED,    1, 8, 20, INPUT_BUTTON, 0, "Up", "DUp"),
    input_new(GAMEPAD_BUTTON_LEFT_FACE_DOWN,    RED,    1, 9, 20, INPUT_BUTTON, 0, "Down", "DDown"),
    input_new(GAMEPAD_BUTTON_LEFT_FACE_LEFT,    RED,    1, 10, 20, INPUT_BUTTON, 0, "Left", "DLeft"),
    input_new(GAMEPAD_BUTTON_LEFT_FACE_RIGHT,   RED,    1, 11, 20, INPUT_BUTTON, 0, "Right", "DRight"),

    input_new(GAMEPAD_BUTTON_RIGHT_THUMB,       RED,    1, 12, 20, INPUT_BUTTON, 0, "Right Thumb", "RStickDot"),
    input_new(GAMEPAD_BUTTON_LEFT_THUMB,        RED,    1, 13, 20, INPUT_BUTTON, 0, "Left Thumb", "LStickDot"),

    input_new(GAMEPAD_BUTTON_LEFT_TRIGGER_1,    RED,    1, 14, 20, INPUT_BUTTON, 0, "Left Trigger 1", "L1"),
    input_new(GAMEPAD_BUTTON_LEFT_TRIGGER_2,    RED,    1, 15, 20, INPUT_BUTTON, 0, "Left Trigger 2", "L2"),
    input_new(GAMEPAD_BUTTON_RIGHT_TRIGGER_1,   RED,    1, 16, 20, INPUT_BUTTON, 0, "Right Trigger 1", "R1"),
    input_new(GAMEPAD_BUTTON_RIGHT_TRIGGER_2,   RED,    1, 17, 20, INPUT_BUTTON, 0, "Right Trigger 2", "R2"),

    LX,
    LY,
    RX,
    RY,

    input_new(GAMEPAD_AXIS_RIGHT_TRIGGER,       YELLOW, 1, 22, 20, INPUT_ANALOG, -1, "Right Trigger Axis", NULL),
    input_new(GAMEPAD_AXIS_LEFT_TRIGGER,        YELLOW, 1, 23, 20, INPUT_ANALOG, -1, "Left Trigger Axis", NULL),
  };

  const int INPUTS_COUNT = sizeof inputs / sizeof inputs[0];

  svg_reset(&ctrl_svg);

  for (int i = 0; i < INPUTS_COUNT; i++) {
    Input input = inputs[i];
    bool is_trigger = input.svg_id != NULL && (strcmp(input.svg_id, "L2") == 0 || strcmp(input.svg_id, "R2") == 0);
    bool is_stick_dot = input.svg_id != NULL && (strcmp(input.svg_id, "LStickDot") == 0 || strcmp(input.svg_id, "RStickDot") == 0);
    if (input.type == INPUT_BUTTON && input.svg_id != NULL && !is_trigger && IsGamepadButtonDown(id, input.id)) {
      svg_set_fill(&ctrl_svg, input.svg_id, COLOR_PRESS);
      if (is_stick_dot) svg_set_stroke(&ctrl_svg, input.svg_id, COLOR_BG);
    }
  }

  float lt_fraction = (GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_TRIGGER) + 1) / 2;
  float rt_fraction = (GetGamepadAxisMovement(id, GAMEPAD_AXIS_RIGHT_TRIGGER) + 1) / 2;
  if (IsGamepadButtonDown(id, GAMEPAD_BUTTON_LEFT_TRIGGER_2)) lt_fraction = 1;
  if (IsGamepadButtonDown(id, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) rt_fraction = 1;
  svg_set_fill_fraction(&ctrl_svg, "L2", COLOR_PRESS, lt_fraction);
  svg_set_fill_fraction(&ctrl_svg, "R2", COLOR_PRESS, rt_fraction);

  Vector2 lv = (Vector2){GetGamepadAxisMovement(id, LX.id), GetGamepadAxisMovement(id, LY.id)};
  Vector2 rv = (Vector2){GetGamepadAxisMovement(id, RX.id), GetGamepadAxisMovement(id, RY.id)};

  left_stick_smooth.x = lerp(left_stick_smooth.x, lv.x, 0.5);
  left_stick_smooth.y = lerp(left_stick_smooth.y, lv.y, 0.5);
  right_stick_smooth.x = lerp(right_stick_smooth.x, rv.x, 0.5);
  right_stick_smooth.y = lerp(right_stick_smooth.y, rv.y, 0.5);

  svg_translate(&ctrl_svg, "LeftStick", left_stick_smooth.x * STICK_SLIDE, left_stick_smooth.y * STICK_SLIDE);
  svg_translate(&ctrl_svg, "LStickDot", left_stick_smooth.x * STICK_SLIDE, left_stick_smooth.y * STICK_SLIDE);
  svg_translate(&ctrl_svg, "RightStick", right_stick_smooth.x * STICK_SLIDE, right_stick_smooth.y * STICK_SLIDE);
  svg_translate(&ctrl_svg, "RStickDot", right_stick_smooth.x * STICK_SLIDE, right_stick_smooth.y * STICK_SLIDE);

  float left_stick_mag = sqrtf(left_stick_smooth.x*left_stick_smooth.x + left_stick_smooth.y*left_stick_smooth.y);
  svg_set_fill_fraction(&ctrl_svg, "LeftStick", COLOR_PRESS, left_stick_mag);

  float right_stick_mag = sqrtf(right_stick_smooth.x*right_stick_smooth.x + right_stick_smooth.y*right_stick_smooth.y);
  svg_set_fill_fraction(&ctrl_svg, "RightStick", COLOR_PRESS, right_stick_mag);

  svg_rasterize(&ctrl_svg);
  UpdateTexture(ctrl_tex, ctrl_svg.pixels);

  float scale = area.width / CTRL_SVG_WIDTH;
  Vector2 ctrl_pos = {roundf(area.x), roundf(area.y)};
  DrawTextureEx(ctrl_tex, ctrl_pos, 0, scale, COLOR_TEXT);

  return (Rectangle){ctrl_pos.x, ctrl_pos.y, area.width, area.height};
}
