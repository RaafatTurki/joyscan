#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../deps/raygui/src/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define DEBUG_MODE true
#include "controller.h"
#include "gamepad_list.h"

int main(void) {
  int w = 1000;
  int h = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(w, h, "gamepad");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

  controller_init();

  GamepadList gamepads;
  gamepad_list_init(&gamepads);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(CTRL_BG_COLOR);
    w = GetScreenWidth();
    h = GetScreenHeight();

    gamepad_list_update(&gamepads);

    if (gamepads.current == -1) {
      DrawText("No Controller Detected :c", 10, 10, 20, WHITE);
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
