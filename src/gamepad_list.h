#pragma once

#include <stdio.h>
#include "raylib.h"
#include "../deps/raygui/src/raygui.h"
#include "colors.h"
#include "log.h"
#include "vector.h"

#define MAX_GAMEPAD_ROWS 8
#define SIDEBAR_MIN_WIDTH 150
#define SIDEBAR_MAX_WIDTH 500
#define SIDEBAR_TEXT_PADDING 60
#define SIDEBAR_ICON_WIDTH 36
#define SIDEBAR_FOOTER_HEIGHT 40
#define GAMEPAD_ACTIVITY_ICON ICON_WAVE

typedef struct {
  Vector ids;
  int current;
  int list_scroll;
} GamepadList;

void gamepad_list_init(GamepadList *gl) {
  vec_init(&gl->ids, 0);
  gl->current = -1;
  gl->list_scroll = 0;
}

void gamepad_list_update(GamepadList *gl) {
  for (int id = 0; id <= vec_len(&gl->ids)+1; id++) {
    if (IsGamepadAvailable(id) && vec_has_val(&gl->ids, id) == -1) {
      vec_append(&gl->ids, id);
      gl->current = id;
      print_log(MAIN, TextFormat("Connected gamepad id %d", id));

    } else if (!IsGamepadAvailable(id) && vec_has_val(&gl->ids, id) > -1) {
      vec_remove(&gl->ids, id);

      if (id == gl->current) {
        bool is_reconnected = false;
        for (int new_id = 0; id <= vec_len(&gl->ids); id++) {
          if (IsGamepadAvailable(new_id)) {
            gl->current = new_id;
            is_reconnected = true;
          }
        }
        if (!is_reconnected) gl->current = -1;
      }
      print_log(MAIN, TextFormat("Disconnected gamepad id %d", id));
    }
  }
}

void gamepad_list_handle_keys(GamepadList *gl) {
  if (IsKeyPressed(KEY_DOWN)) {
    int i = vec_has_val(&gl->ids, gl->current);
    int next_i = vec_next(&gl->ids, i);
    gl->current = vec_get(&gl->ids, next_i);
  }
  if (IsKeyPressed(KEY_UP)) {
    int i = vec_has_val(&gl->ids, gl->current);
    int prev_i = vec_prev(&gl->ids, i);
    gl->current = vec_get(&gl->ids, prev_i);
  }
}

static bool gamepad_has_activity(int id) {
  for (int button = 1; button <= GAMEPAD_BUTTON_RIGHT_THUMB; button++) {
    if (IsGamepadButtonDown(id, button)) return true;
  }
  return false;
}

float gamepad_list_sidebar_width(GamepadList *gl) {
  int text_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  float longest = 0;
  for (int i = 0; i < vec_len(&gl->ids); i++) {
    float w = (float)MeasureText(GetGamepadName(vec_get(&gl->ids, i)), text_size);
    if (w > longest) longest = w;
  }

  float width = longest + SIDEBAR_ICON_WIDTH + SIDEBAR_TEXT_PADDING;
  if (width < SIDEBAR_MIN_WIDTH) width = SIDEBAR_MIN_WIDTH;
  if (width > SIDEBAR_MAX_WIDTH) width = SIDEBAR_MAX_WIDTH;
  return width;
}

void gamepad_list_sidebar(GamepadList *gl, Rectangle bounds) {
  int row_count = vec_len(&gl->ids);
  if (row_count > MAX_GAMEPAD_ROWS) row_count = MAX_GAMEPAD_ROWS;
  const char *gamepad_names[MAX_GAMEPAD_ROWS];
  char row_text[MAX_GAMEPAD_ROWS][160];
  for (int i = 0; i < row_count; i++) {
    int id = vec_get(&gl->ids, i);
    const char *name = GetGamepadName(id);
    int icon = gamepad_has_activity(id) ? GAMEPAD_ACTIVITY_ICON : ICON_NONE;
    snprintf(row_text[i], sizeof(row_text[i]), "#%03d#%s", icon, name);
    gamepad_names[i] = row_text[i];
  }
  const char *row_labels = TextJoin((char **)gamepad_names, row_count, ";");

  Rectangle list_bounds = { bounds.x, bounds.y, bounds.width, bounds.height - SIDEBAR_FOOTER_HEIGHT };
  int active_row = vec_has_val(&gl->ids, gl->current);
  GuiListView(list_bounds, row_labels, &gl->list_scroll, &active_row);
  if (active_row >= 0) gl->current = vec_get(&gl->ids, active_row);

  DrawText("raafat.turki@protonmail.com", (int)bounds.x + 10, (int)(bounds.y + bounds.height) - 30, 10, COLOR_TEXT);
  DrawText("muhammedturki@protonmail.com", (int)bounds.x + 10, (int)(bounds.y + bounds.height) - 20, 10, COLOR_TEXT);
}
