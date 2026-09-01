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
      gamepad_list_tab_bar(&gamepads, (Rectangle){0, 0, (float)w, TAB_BAR_HEIGHT});
      display_gamepad(gamepads.current);
    }

    gamepad_list_handle_keys(&gamepads);

    DrawText("raafat.turki@protonmail.com", 10, h-30, 10, WHITE);
    DrawText("muhammedturki@protonmail.com", 10, h-20, 10, WHITE);
    EndDrawing();
  }

  controller_deinit();
  CloseWindow();

  return 0;
}
