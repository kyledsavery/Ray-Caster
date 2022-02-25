#include "v3math.h"

void v3_from_points(float *dst, float *a, float *b) {
  dst[0] = b[0] - a[0];
  dst[1] = b[1] - a[1];
  dst[2] = b[2] - a[2];
}

void v3_add(float *dst, float *a, float *b) {
  dst[0] = a[0] + b[0];
  dst[1] = a[1] + b[1];
  dst[2] = a[2] + b[2];
}

void v3_subtract(float *dst, float *a, float *b) {
  dst[0] = a[0] - b[0];
  dst[1] = a[1] - b[1];
  dst[2] = a[2] - b[2];
}

float v3_dot_product(float *a, float *b) {
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

void v3_cross_product(float *dst, float *a, float *b) {
  float i_component = (a[1]*b[2]) - (a[2]*b[1]);
  float j_component = -1 * ((a[0]*b[2]) - (a[2]*b[0]));
  float k_component = (a[0]*b[1]) - (a[1]*b[0]);
  dst[0] = i_component;
  dst[1] = j_component;
  dst[2] = k_component;
}

void v3_scale(float *dst, float s) {
  dst[0] *= s;
  dst[1] *= s;
  dst[2] *= s;
}

float v3_angle(float *a, float *b) {
  float mag_a = v3_length(a);
  float mag_b = v3_length(b);
  assert(mag_a != 0.0 && mag_b != 0.0);
  float cos_theta = v3_dot_product(a, b) / (mag_a*mag_b);
  return acos(cos_theta);
}

float v3_angle_quick(float *a, float *b) {
  float mag_a = v3_length(a);
  float mag_b = v3_length(b);
  assert(mag_a != 0.0 && mag_b != 0.0);
  return v3_dot_product(a, b) / (mag_a*mag_b);
}

void v3_reflect(float *dst, float *v, float *n) {
  float temp[] = {n[0], n[1], n[2]};
  v3_scale(temp, (2 * v3_dot_product(temp, v)));
  v3_subtract(dst, v, temp);
}

float v3_length(float *a) {
  return sqrt((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]));
}

void v3_normalize(float *dst, float *a) {
  float mag_a = v3_length(a);
  assert(mag_a != 0.0);
  dst[0] = a[0] / mag_a;
  dst[1] = a[1] / mag_a;
  dst[2] = a[2] / mag_a;
}
