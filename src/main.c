#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../deps/raygui/src/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define DEBUG_MODE false

#include "layout.h"
#include "controller.h"
#include "gamepad_list.h"
#include "button_list.h"
#include "dualsense/dualsense.h"
#include "touchpad_widget.h"

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

  GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
  GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 40);
  GuiSetIconScale(2);
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
  layout_init();
  controller_init();

  GamepadList gamepads;
  gamepad_list_init(&gamepads);

  JSDualSense *ds = js_dualsense_create();
  JSDualSenseState ds_state = {0};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(COLOR_BG);
    w = GetScreenWidth();
    h = GetScreenHeight();

    gamepad_list_update(&gamepads);

    if (ds && !js_dualsense_update(ds, GetFrameTime(), &ds_state)) {
      js_dualsense_destroy(ds);
      ds = NULL;
    }

    if (gamepads.current == -1) {
      DrawText("No Controller Detected :c", 10, 10, 20, COLOR_TEXT);
    } else {
      float sidebar_width = gamepad_list_sidebar_width(&gamepads);
      Rectangle content_area = {sidebar_width, 0, (float)w - sidebar_width, (float)h};
      bool fits = button_list_fits(content_area);
      bool show_touchpad = fits && ds != NULL;

      layout_begin(w, h);
      CLAY({
        .id = layout_id("Root"),
        .layout = { .sizing = { CLAY_SIZING_FIXED((float)w), CLAY_SIZING_FIXED((float)h) } },
      }) {
        CLAY({
          .id = layout_id("Sidebar"),
          .layout = { .sizing = { CLAY_SIZING_FIXED(sidebar_width), CLAY_SIZING_GROW(0) } },
        }) {}
        CLAY({
          .id = layout_id("Content"),
          .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(GROUP_PADDING),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
          },
        }) {
          if (fits) {
            button_list_build(show_touchpad);
          } else {
            CLAY({
              .id = layout_id("Controller"),
              .layout = { .sizing = { CLAY_SIZING_GROW(0, CTRL_SVG_WIDTH), CLAY_SIZING_GROW(0, CTRL_SVG_HEIGHT) } },
              .aspectRatio = { CTRL_SVG_WIDTH / (float)CTRL_SVG_HEIGHT },
            }) {}
          }
        }
      }
      layout_end();

      gamepad_list_sidebar(&gamepads, layout_rect("Sidebar"));
      Rectangle ctrl_rect = fits ? button_list_draw(gamepads.current) : layout_rect("Controller");
      display_gamepad(gamepads.current, ctrl_rect);
      if (show_touchpad) touchpad_widget_draw(&ds_state);

      if (DEBUG_MODE) {
        const char *dbg_ids[] = {"MiddleRow", "ControllerColumn", "ShoulderRow", "BottomRow", "Dpad", "Face", "Controller", "Touchpad"};
        for (int i = 0; i < 8; i++) {
          Rectangle r = layout_rect(dbg_ids[i]);
          DrawRectangleLinesEx(r, 2, RED);
          if (IsKeyPressed(KEY_P)) printf("%s: %.1f %.1f %.1f %.1f\n", dbg_ids[i], r.x, r.y, r.width, r.height);
        }
      }
    }

    gamepad_list_handle_keys(&gamepads);

    EndDrawing();
  }

  js_dualsense_destroy(ds);
  controller_deinit();
  CloseWindow();

  return 0;
}
