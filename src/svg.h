#pragma once

#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "utils.h"

#define NANOSVG_IMPLEMENTATION
#include "../deps/nanosvg/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "../deps/nanosvg/nanosvgrast.h"

#define SVG_MAX_SHAPES 64

typedef struct {
  NSVGshape *shape;
  NSVGpaint fill;
  NSVGpaint stroke;
  float *pts;
  int pts_count;
} SvgShapeSnapshot;

typedef struct {
  NSVGimage *image;
  NSVGrasterizer *rast;
  int width;
  int height;
  unsigned char *pixels;
  SvgShapeSnapshot snapshots[SVG_MAX_SHAPES];
  int snapshot_count;
} SvgController;

SvgController svg_controller_load(const char *file_name, int width, int height) {
  SvgController sc = {0};
  sc.image = nsvgParseFromFile(file_name, "px", 96.0f);
  sc.rast = nsvgCreateRasterizer();
  sc.width = width;
  sc.height = height;
  sc.pixels = malloc(width * height * 4);

  for (NSVGshape *shape = sc.image->shapes; shape != NULL; shape = shape->next) {
    SvgShapeSnapshot *snap = &sc.snapshots[sc.snapshot_count++];
    snap->shape = shape;
    snap->fill = shape->fill;
    snap->stroke = shape->stroke;
    snap->pts_count = shape->paths != NULL ? shape->paths->npts * 2 : 0;
    if (snap->pts_count > 0) {
      snap->pts = malloc(sizeof(float) * snap->pts_count);
      memcpy(snap->pts, shape->paths->pts, sizeof(float) * snap->pts_count);
    }
  }

  return sc;
}

SvgShapeSnapshot *svg_find(SvgController *sc, const char *id) {
  SvgShapeSnapshot *found = NULL;
  for (int i = 0; i < sc->snapshot_count; i++) {
    if (strcmp(sc->snapshots[i].shape->id, id) == 0) found = &sc->snapshots[i];
  }
  return found;
}

void svg_reset(SvgController *sc) {
  for (int i = 0; i < sc->snapshot_count; i++) {
    SvgShapeSnapshot *snap = &sc->snapshots[i];
    snap->shape->fill = snap->fill;
    snap->shape->stroke = snap->stroke;
    if (snap->pts_count > 0) {
      memcpy(snap->shape->paths->pts, snap->pts, sizeof(float) * snap->pts_count);
    }
  }
}

void svg_commit(SvgController *sc) {
  for (int i = 0; i < sc->snapshot_count; i++) {
    sc->snapshots[i].fill = sc->snapshots[i].shape->fill;
    sc->snapshots[i].stroke = sc->snapshots[i].shape->stroke;
  }
}

void svg_set_fill_fraction(SvgController *sc, const char *id, Color color, float t) {
  SvgShapeSnapshot *snap = svg_find(sc, id);
  if (snap == NULL) return;
  t = clamp(t, 0, 1);
  snap->shape->fill.type = NSVG_PAINT_COLOR;
  unsigned int a = (unsigned int)(t * color.a);
  snap->shape->fill.color = NSVG_RGB(color.r, color.g, color.b) | (a << 24);
}

void svg_set_fill(SvgController *sc, const char *id, Color color) {
  svg_set_fill_fraction(sc, id, color, 1.0f);
}

void svg_set_stroke(SvgController *sc, const char *id, Color color) {
  SvgShapeSnapshot *snap = svg_find(sc, id);
  if (snap == NULL) return;
  snap->shape->stroke.type = NSVG_PAINT_COLOR;
  snap->shape->stroke.color = NSVG_RGB(color.r, color.g, color.b) | ((unsigned int)color.a << 24);
}

void svg_hide(SvgController *sc, const char *id) {
  SvgShapeSnapshot *snap = svg_find(sc, id);
  if (snap == NULL) return;
  snap->shape->fill.type = NSVG_PAINT_NONE;
  snap->shape->stroke.type = NSVG_PAINT_NONE;
}

void svg_set_stroke_width(SvgController *sc, float width) {
  for (int i = 0; i < sc->snapshot_count; i++) {
    sc->snapshots[i].shape->strokeWidth = width;
  }
}

void svg_translate(SvgController *sc, const char *id, float dx, float dy) {
  SvgShapeSnapshot *snap = svg_find(sc, id);
  if (snap == NULL || snap->pts_count == 0) return;

  float *pts = snap->shape->paths->pts;
  float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
  for (int i = 0; i < snap->pts_count; i += 2) {
    pts[i] = snap->pts[i] + dx;
    pts[i + 1] = snap->pts[i + 1] + dy;
    if (pts[i] < minx) minx = pts[i];
    if (pts[i] > maxx) maxx = pts[i];
    if (pts[i + 1] < miny) miny = pts[i + 1];
    if (pts[i + 1] > maxy) maxy = pts[i + 1];
  }
  snap->shape->paths->bounds[0] = minx;
  snap->shape->paths->bounds[1] = miny;
  snap->shape->paths->bounds[2] = maxx;
  snap->shape->paths->bounds[3] = maxy;
  snap->shape->bounds[0] = minx;
  snap->shape->bounds[1] = miny;
  snap->shape->bounds[2] = maxx;
  snap->shape->bounds[3] = maxy;
}

void svg_rasterize(SvgController *sc) {
  float scale_w = (float)sc->width / sc->image->width;
  float scale_h = (float)sc->height / sc->image->height;
  float scale = scale_h > scale_w ? scale_w : scale_h;
  nsvgRasterize(sc->rast, sc->image, 0, 0, scale, sc->pixels, sc->width, sc->height, sc->width * 4);
}

void svg_controller_unload(SvgController *sc) {
  for (int i = 0; i < sc->snapshot_count; i++) {
    if (sc->snapshots[i].pts != NULL) free(sc->snapshots[i].pts);
  }
  nsvgDeleteRasterizer(sc->rast);
  nsvgDelete(sc->image);
  free(sc->pixels);
}
