#pragma once

#include "raylib.h"
#include "../deps/raygui/src/raygui.h"
#include "log.h"
#include "vector.h"

#define MAX_GAMEPAD_TABS 8
#define TAB_BAR_HEIGHT 32

typedef struct {
  Vector ids;
  int current;
  int tab_hscroll;
} GamepadList;

void gamepad_list_init(GamepadList *gl) {
  vec_init(&gl->ids, 0);
  gl->current = -1;
  gl->tab_hscroll = 0;
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

void gamepad_list_tab_bar(GamepadList *gl, Rectangle bounds) {
  int tab_count = vec_len(&gl->ids);
  if (tab_count > MAX_GAMEPAD_TABS) tab_count = MAX_GAMEPAD_TABS;
  const char *gamepad_names[MAX_GAMEPAD_TABS];
  for (int i = 0; i < tab_count; i++) {
    gamepad_names[i] = GetGamepadName(vec_get(&gl->ids, i));
  }
  const char *tab_labels = TextJoin((char **)gamepad_names, tab_count, ";");

  int active_tab = vec_has_val(&gl->ids, gl->current);
  GuiTabBar(bounds, tab_labels, &gl->tab_hscroll, &active_tab);
  gl->current = vec_get(&gl->ids, active_tab);
}
