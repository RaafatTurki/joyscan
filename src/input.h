#pragma once

#include <raylib.h>

typedef enum {
  INPUT_BUTTON,
  INPUT_ANALOG,
} InputType;

typedef struct {
  int id;
  Color color;
  int x;
  int y;
  int s;
  InputType type;
  char *name;
  float idle_value;
  char *svg_id;
} Input;


Input input_new(int id, Color color, int x, int y, int s, InputType type, float idle_value, char* name, char *svg_id) {
  Input input;
  input.id = id;
  input.color = color;
  input.x = x;
  input.y = y;
  input.s = s;
  input.type = type;
  input.idle_value = idle_value;
  input.name = name;
  input.svg_id = svg_id;
  return input;
}
