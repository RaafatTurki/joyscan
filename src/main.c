#include <math.h>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../deps/raygui/src/raygui.h"

#define DEBUG_MODE true
#include "input.h"
#include "log.h"
#include "vector.h"
#include "utils.h"
#include "svg.h"

#define CTRL_SVG_WIDTH 550
#define CTRL_SVG_HEIGHT 503

#define TAB_BAR_HEIGHT 32
#define MAX_GAMEPAD_TABS 8

#define STICK_SLIDE 9.5f

static const char *CTRL_PRESSABLE_IDS[] = {
  "BTop", "BRight", "BBottom", "BLeft",
  "DUp", "DRight", "DDown", "DLeft",
  "LMeta", "RMeta", "Home",
  "L1", "R1", "L2", "R2",
  "LeftStick", "RightStick",
};
#define CTRL_PRESSABLE_ID_COUNT (sizeof(CTRL_PRESSABLE_IDS) / sizeof(CTRL_PRESSABLE_IDS[0]))
#define CTRL_OUTLINE_COLOR (Color){78, 89, 111, 255}
#define CTRL_BG_COLOR BLACK
#define CTRL_PRESS_COLOR WHITE

SvgController ctrl_svg;
Texture2D ctrl_tex;

Vector2 left_stick_smooth = {0, 0};
Vector2 right_stick_smooth = {0, 0};

int gamepad_tab_hscroll = 0;

static bool ctrl_id_is_pressable(const char *id) {
  for (int i = 0; i < CTRL_PRESSABLE_ID_COUNT; i++) {
    if (strcmp(id, CTRL_PRESSABLE_IDS[i]) == 0) return true;
  }
  return false;
}

void style_controller_svg(SvgController *sc) {
  svg_set_stroke_width(sc, 3.0f);

  for (int i = 0; i < CTRL_PRESSABLE_ID_COUNT; i++) {
    svg_hide(sc, CTRL_PRESSABLE_IDS[i]);
    svg_set_stroke(sc, CTRL_PRESSABLE_IDS[i], CTRL_PRESS_COLOR);
  }

  svg_hide(sc, "LStickDot");
  svg_hide(sc, "RStickDot");

  for (int i = 0; i < sc->snapshot_count; i++) {
    NSVGshape *shape = sc->snapshots[i].shape;
    if (ctrl_id_is_pressable(shape->id)) continue;
    if (strcmp(shape->id, "LStickDot") == 0 || strcmp(shape->id, "RStickDot") == 0) continue;

    if (strcmp(shape->id, "Outline") == 0) {
      shape->fill.type = NSVG_PAINT_COLOR;
      shape->fill.color = NSVG_RGB(CTRL_BG_COLOR.r, CTRL_BG_COLOR.g, CTRL_BG_COLOR.b) | ((unsigned int)CTRL_BG_COLOR.a << 24);
    }
    if (shape->stroke.type != NSVG_PAINT_NONE) {
      shape->stroke.type = NSVG_PAINT_COLOR;
      shape->stroke.color = NSVG_RGB(CTRL_OUTLINE_COLOR.r, CTRL_OUTLINE_COLOR.g, CTRL_OUTLINE_COLOR.b) | ((unsigned int)CTRL_OUTLINE_COLOR.a << 24);
    }
  }

  svg_commit(sc);
}

void display_gamepad(int id) {
  // defining the buttons
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
      svg_set_fill(&ctrl_svg, input.svg_id, CTRL_PRESS_COLOR);
      if (is_stick_dot) svg_set_stroke(&ctrl_svg, input.svg_id, CTRL_BG_COLOR);
    }
  }

  float lt_fraction = (GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_TRIGGER) + 1) / 2;
  float rt_fraction = (GetGamepadAxisMovement(id, GAMEPAD_AXIS_RIGHT_TRIGGER) + 1) / 2;
  if (IsGamepadButtonDown(id, GAMEPAD_BUTTON_LEFT_TRIGGER_2)) lt_fraction = 1;
  if (IsGamepadButtonDown(id, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) rt_fraction = 1;
  svg_set_fill_fraction(&ctrl_svg, "L2", CTRL_PRESS_COLOR, lt_fraction);
  svg_set_fill_fraction(&ctrl_svg, "R2", CTRL_PRESS_COLOR, rt_fraction);

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
  svg_set_fill_fraction(&ctrl_svg, "LeftStick", CTRL_PRESS_COLOR, left_stick_mag);

  float right_stick_mag = sqrtf(right_stick_smooth.x*right_stick_smooth.x + right_stick_smooth.y*right_stick_smooth.y);
  svg_set_fill_fraction(&ctrl_svg, "RightStick", CTRL_PRESS_COLOR, right_stick_mag);

  svg_rasterize(&ctrl_svg);
  UpdateTexture(ctrl_tex, ctrl_svg.pixels);
  int ctrl_x = (GetScreenWidth() - CTRL_SVG_WIDTH) / 2;
  int ctrl_y = (GetScreenHeight() - CTRL_SVG_HEIGHT) / 2;
  DrawTexture(ctrl_tex, ctrl_x, ctrl_y, WHITE);
}

int main(void) {
  int w = 1000;
  int h = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(w, h, "gamepad");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

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

  Vector gamepad_ids;
  vec_init(&gamepad_ids, 0);
  int gamepad_id_curr = -1;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(CTRL_BG_COLOR);
    w = GetScreenWidth();
    h = GetScreenHeight();

    // detect connected and disconnected devices
    for (int id = 0; id <= vec_len(&gamepad_ids)+1; id++) {
      if (IsGamepadAvailable(id) && vec_has_val(&gamepad_ids, id) == -1) {
        vec_append(&gamepad_ids, id);
        gamepad_id_curr = id;
        print_log(MAIN, TextFormat("Connected gamepad id %d", id));

      } else if (!IsGamepadAvailable(id) && vec_has_val(&gamepad_ids, id) > -1) {
        vec_remove(&gamepad_ids, id);

        if (id == gamepad_id_curr) {
          bool is_reconnected = false;
          for (int new_id = 0; id <= vec_len(&gamepad_ids); id++) {
            if (IsGamepadAvailable(new_id)) {
              gamepad_id_curr = new_id;
              is_reconnected = true;
            }
          }
          if (!is_reconnected) gamepad_id_curr = -1;
        }
        print_log(MAIN, TextFormat("Disconnected gamepad id %d", id));
      }
    }

    // drawing everything
    if (gamepad_id_curr == -1) {
      DrawText("No Controller Detected :c", 10, 10, 20, WHITE);
    } else {
      int tab_count = vec_len(&gamepad_ids);
      if (tab_count > MAX_GAMEPAD_TABS) tab_count = MAX_GAMEPAD_TABS;
      const char *gamepad_names[MAX_GAMEPAD_TABS];
      for (int i = 0; i < tab_count; i++) {
        gamepad_names[i] = GetGamepadName(vec_get(&gamepad_ids, i));
      }
      const char *tab_labels = TextJoin((char **)gamepad_names, tab_count, ";");

      int active_tab = vec_has_val(&gamepad_ids, gamepad_id_curr);
      GuiTabBar((Rectangle){0, 0, (float)w, TAB_BAR_HEIGHT}, tab_labels, &gamepad_tab_hscroll, &active_tab);
      gamepad_id_curr = vec_get(&gamepad_ids, active_tab);

      display_gamepad(gamepad_id_curr);
    }

    // gamepad switching
    if (IsKeyPressed(KEY_DOWN)) {
      int i = vec_has_val(&gamepad_ids, gamepad_id_curr);
      int next_i = vec_next(&gamepad_ids, i);
      gamepad_id_curr = vec_get(&gamepad_ids, next_i);
    }
    if (IsKeyPressed(KEY_UP)) {
      int i = vec_has_val(&gamepad_ids, gamepad_id_curr);
      int prev_i = vec_prev(&gamepad_ids, i);
      gamepad_id_curr = vec_get(&gamepad_ids, prev_i);
    }

    DrawText("raafat.turki@protonmail.com", 10, h-30, 10, WHITE);
    DrawText("muhammedturki@protonmail.com", 10, h-20, 10, WHITE);
    EndDrawing();
  }

  svg_controller_unload(&ctrl_svg);
  UnloadTexture(ctrl_tex);
  CloseWindow();

  return 0;
}
