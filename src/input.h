#pragma once

#include <raylib.h>

typedef enum {
  BUTTON,
  ANALOG,
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
} Input;


Input input_new(int id, Color color, int x, int y, int s, InputType type, float idle_value, char* name) { 
  Input input;
  input.id = id;
  input.color = color;
  input.x = x;
  input.y = y;
  input.s = s;
  input.type = type;
  input.idle_value = idle_value;
  input.name = name;
  return input;
}
