double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}
