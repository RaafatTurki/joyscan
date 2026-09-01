#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../deps/raygui/src/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define DEBUG_MODE true
#include "controller.h"
#include "gamepad_list.h"

void apply_gui_theme(void) {
  GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(COLOR_OUTLINE));
  GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(COLOR_BG));
  GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(COLOR_TEXT));
  GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(COLOR_PRESS));
  GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(COLOR_OUTLINE));
  GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(COLOR_TEXT));
  GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(COLOR_PRESS));
  GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(COLOR_PRESS));
  GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(COLOR_BG));
  GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(COLOR_OUTLINE));
  GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(COLOR_BG));
}

int main(void) {
  int w = 1000;
  int h = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(w, h, "gamepad");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

  apply_gui_theme();
  controller_init();

  GamepadList gamepads;
  gamepad_list_init(&gamepads);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(COLOR_BG);
    w = GetScreenWidth();
    h = GetScreenHeight();

    gamepad_list_update(&gamepads);

    if (gamepads.current == -1) {
      DrawText("No Controller Detected :c", 10, 10, 20, COLOR_TEXT);
    } else {
      gamepad_list_sidebar(&gamepads, (Rectangle){0, 0, SIDEBAR_WIDTH, (float)h});
      Rectangle ctrl_area = {SIDEBAR_WIDTH, 0, (float)w - SIDEBAR_WIDTH, (float)h};
      display_gamepad(gamepads.current, ctrl_area);
    }

    gamepad_list_handle_keys(&gamepads);

    EndDrawing();
  }

  controller_deinit();
  CloseWindow();

  return 0;
}
