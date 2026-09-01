#pragma once

#include <stdlib.h>
#include <string.h>
#include "raylib.h"

#define CLAY_IMPLEMENTATION
#include "../deps/clay/clay.h"

#include "log.h"

static void layout_error_handler(Clay_ErrorData error) {
  print_log(MAIN, TextFormat("Clay error: %s", error.errorText.chars));
}

void layout_init(void) {
  uint64_t memory_size = Clay_MinMemorySize();
  Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(memory_size, malloc(memory_size));
  Clay_Initialize(arena, (Clay_Dimensions){0, 0}, (Clay_ErrorHandler){layout_error_handler, 0});
}

void layout_begin(int w, int h) {
  Clay_SetLayoutDimensions((Clay_Dimensions){(float)w, (float)h});
  Clay_BeginLayout();
}

void layout_end(void) {
  Clay_EndLayout();
}

static Rectangle layout_from_bounding_box(Clay_BoundingBox box) {
  return (Rectangle){box.x, box.y, box.width, box.height};
}

static Clay_String layout_clay_string(const char *id) {
  return (Clay_String){.isStaticallyAllocated = false, .length = (int32_t)strlen(id), .chars = id};
}

Clay_ElementId layout_id(const char *id) {
  return Clay_GetElementIdWithIndex(layout_clay_string(id), 0);
}

Clay_ElementId layout_id_i(const char *id, int index) {
  return Clay_GetElementIdWithIndex(layout_clay_string(id), (uint32_t)index);
}

Rectangle layout_rect(const char *id) {
  Clay_ElementData data = Clay_GetElementData(Clay_GetElementId(layout_clay_string(id)));
  return data.found ? layout_from_bounding_box(data.boundingBox) : (Rectangle){0};
}

Rectangle layout_rect_i(const char *id, int index) {
  Clay_ElementData data = Clay_GetElementData(Clay_GetElementIdWithIndex(layout_clay_string(id), (uint32_t)index));
  return data.found ? layout_from_bounding_box(data.boundingBox) : (Rectangle){0};
}
