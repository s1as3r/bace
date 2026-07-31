#include <math.h>

#include "bace/math.h"

f32 lerp_f32(f32 a, f32 b, f32 t) {
  return a + (b - a) * clamp(t, 0.0f, 1.0f);
}

f64 lerp_f64(f64 a, f64 b, f64 t) {
  return a + (b - a) * clamp(t, 0.0, 1.0);
}

// vec ops
// Vec2f32
Vec2f32 vec_2f32(f32 x, f32 y) {
  return (Vec2f32){.x = x, .y = y};
}

Vec2f32 add_2f32(Vec2f32 a, Vec2f32 b) {
  return (Vec2f32){.x = a.x + b.x, .y = a.y + b.y};
}

Vec2f32 sub_2f32(Vec2f32 a, Vec2f32 b) {
  return (Vec2f32){.x = a.x - b.x, .y = a.y - b.y};
}

Vec2f32 mul_2f32(Vec2f32 a, Vec2f32 b) {
  return (Vec2f32){.x = a.x * b.x, .y = a.y * b.y};
}

Vec2f32 div_2f32(Vec2f32 a, Vec2f32 b) {
  return (Vec2f32){.x = a.x / b.x, .y = a.y / b.y};
}

Vec2f32 scale_2f32(Vec2f32 v, f32 s) {
  return (Vec2f32){.x = v.x * s, .y = v.y * s};
}

Vec2f32 lerp_2f32(Vec2f32 a, Vec2f32 b, f32 t) {
  return (Vec2f32){.x = lerp_f32(a.x, b.x, t), .y = lerp_f32(a.y, b.y, t)};
}

f32 dot_2f32(Vec2f32 a, Vec2f32 b) {
  return a.x * b.x + a.y * b.y;
}

f32 length_squared_2f32(Vec2f32 v) {
  return v.x * v.x + v.y * v.y;
}

f32 length_2f32(Vec2f32 v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

Vec2f32 normalize_2f32(Vec2f32 v) {
  return scale_2f32(v, 1.f / length_2f32(v));
}

// Vec2i16
Vec2i16 vec_2i16(i16 x, i16 y) {
  return (Vec2i16){.x = x, .y = y};
}

Vec2i16 add_2i16(Vec2i16 a, Vec2i16 b) {
  return (Vec2i16){.x = (i16)(a.x + b.x), .y = (i16)(a.y + b.y)};
}

Vec2i16 sub_2i16(Vec2i16 a, Vec2i16 b) {
  return (Vec2i16){.x = (i16)(a.x - b.x), .y = (i16)(a.y - b.y)};
}

Vec2i16 mul_2i16(Vec2i16 a, Vec2i16 b) {
  return (Vec2i16){.x = (i16)(a.x * b.x), .y = (i16)(a.y * b.y)};
}

Vec2i16 div_2i16(Vec2i16 a, Vec2i16 b) {
  return (Vec2i16){.x = (i16)(a.x / b.x), .y = (i16)(a.y / b.y)};
}

Vec2i16 scale_2i16(Vec2i16 v, i16 s) {
  return (Vec2i16){.x = (i16)(v.x * s), .y = (i16)(v.y * s)};
}

Vec2i16 lerp_2i16(Vec2i16 a, Vec2i16 b, f32 t) {
  return (Vec2i16){
      .x = (i16)lerp_f32((f32)a.x, (f32)b.x, t),
      .y = (i16)lerp_f32((f32)a.y, (f32)b.y, t),
  };
}

i16 dot_2i16(Vec2i16 a, Vec2i16 b) {
  return (i16)(a.x * b.x + a.y * b.y);
}

i16 length_squared_2i16(Vec2i16 v) {
  return (i16)(v.x * v.x + v.y * v.y);
}

i16 length_2i16(Vec2i16 v) {
  return (i16)sqrtf((f32)(v.x * v.x + v.y * v.y));
}

// Vec2i32
Vec2i32 vec_2i32(i32 x, i32 y) {
  return (Vec2i32){.x = x, .y = y};
}

Vec2i32 add_2i32(Vec2i32 a, Vec2i32 b) {
  return (Vec2i32){.x = a.x + b.x, .y = a.y + b.y};
}

Vec2i32 sub_2i32(Vec2i32 a, Vec2i32 b) {
  return (Vec2i32){.x = a.x - b.x, .y = a.y - b.y};
}

Vec2i32 mul_2i32(Vec2i32 a, Vec2i32 b) {
  return (Vec2i32){.x = a.x * b.x, .y = a.y * b.y};
}

Vec2i32 div_2i32(Vec2i32 a, Vec2i32 b) {
  return (Vec2i32){.x = a.x / b.x, .y = a.y / b.y};
}

Vec2i32 scale_2i32(Vec2i32 v, i32 s) {
  return (Vec2i32){.x = v.x * s, .y = v.y * s};
}

Vec2i32 lerp_2i32(Vec2i32 a, Vec2i32 b, f32 t) {
  return (Vec2i32){
      .x = (i32)lerp_f32((f32)a.x, (f32)b.x, t),
      .y = (i32)lerp_f32((f32)a.y, (f32)b.y, t),
  };
}

i32 dot_2i32(Vec2i32 a, Vec2i32 b) {
  return a.x * b.x + a.y * b.y;
}

i32 length_squared_2i32(Vec2i32 v) {
  return v.x * v.x + v.y * v.y;
}

i32 length_2i32(Vec2i32 v) {
  return (i32)sqrtf((f32)v.x * (f32)v.x + (f32)v.y * (f32)v.y);
}

// Vec2i64
Vec2i64 vec_2i64(i64 x, i64 y) {
  return (Vec2i64){.x = x, .y = y};
}

Vec2i64 add_2i64(Vec2i64 a, Vec2i64 b) {
  return (Vec2i64){.x = a.x + b.x, .y = a.y + b.y};
}

Vec2i64 sub_2i64(Vec2i64 a, Vec2i64 b) {
  return (Vec2i64){.x = a.x - b.x, .y = a.y - b.y};
}

Vec2i64 mul_2i64(Vec2i64 a, Vec2i64 b) {
  return (Vec2i64){.x = a.x * b.x, .y = a.y * b.y};
}

Vec2i64 div_2i64(Vec2i64 a, Vec2i64 b) {
  return (Vec2i64){.x = a.x / b.x, .y = a.y / b.y};
}

Vec2i64 scale_2i64(Vec2i64 v, i64 s) {
  return (Vec2i64){.x = v.x * s, .y = v.y * s};
}

Vec2i64 lerp_2i64(Vec2i64 a, Vec2i64 b, f32 t) {
  return (Vec2i64){
      .x = (i64)lerp_f32((f32)a.x, (f32)b.x, t),
      .y = (i64)lerp_f32((f32)a.y, (f32)b.y, t),
  };
}

i64 dot_2i64(Vec2i64 a, Vec2i64 b) {
  return a.x * b.x + a.y * b.y;
}

i64 length_squared_2i64(Vec2i64 v) {
  return v.x * v.x + v.y * v.y;
}

i64 length_2i64(Vec2i64 v) {
  return (i64)sqrt((f64)(v.x * v.x + v.y * v.y));
}

// Vec3f32
Vec3f32 vec_3f32(f32 x, f32 y, f32 z) {
  return (Vec3f32){.x = x, .y = y, .z = z};
}

Vec3f32 cross_3f32(Vec3f32 a, Vec3f32 b) {
  return (Vec3f32){
      .x = a.y * b.z - a.z * b.y,
      .y = a.z * b.x - a.x * b.z,
      .z = a.x * b.y - a.y * b.x,
  };
}

Vec3f32 add_3f32(Vec3f32 a, Vec3f32 b) {
  return (Vec3f32){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Vec3f32 sub_3f32(Vec3f32 a, Vec3f32 b) {
  return (Vec3f32){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

Vec3f32 mul_3f32(Vec3f32 a, Vec3f32 b) {
  return (Vec3f32){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

Vec3f32 div_3f32(Vec3f32 a, Vec3f32 b) {
  return (Vec3f32){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z};
}

Vec3f32 scale_3f32(Vec3f32 v, f32 s) {
  return (Vec3f32){.x = v.x * s, .y = v.y * s, .z = v.z * s};
}

Vec3f32 lerp_3f32(Vec3f32 a, Vec3f32 b, f32 t) {
  return (Vec3f32){
      .x = lerp_f32(a.x, b.x, t),
      .y = lerp_f32(a.y, b.y, t),
      .z = lerp_f32(a.z, b.z, t),
  };
}

f32 dot_3f32(Vec3f32 a, Vec3f32 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 length_squared_3f32(Vec3f32 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

f32 length_3f32(Vec3f32 v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3f32 normalize_3f32(Vec3f32 v) {
  return scale_3f32(v, 1.f / length_3f32(v));
}

Vec3f32 transform_3f32(Vec3f32 v, Mat3x3f32 m) {
  Vec3f32 result;
  result.x = v.x * m.v[0][0] + v.y * m.v[1][0] + v.z * m.v[2][0];
  result.y = v.x * m.v[0][1] + v.y * m.v[1][1] + v.z * m.v[2][1];
  result.z = v.x * m.v[0][2] + v.y * m.v[1][2] + v.z * m.v[2][2];
  return result;
}

// Vec3i32
Vec3i32 vec_3i32(i32 x, i32 y, i32 z) {
  return (Vec3i32){.x = x, .y = y, .z = z};
}

Vec3i32 cross_3i32(Vec3i32 a, Vec3i32 b) {
  return (Vec3i32){
      .x = a.y * b.z - a.z * b.y,
      .y = a.z * b.x - a.x * b.z,
      .z = a.x * b.y - a.y * b.x,
  };
}

Vec3i32 add_3i32(Vec3i32 a, Vec3i32 b) {
  return (Vec3i32){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Vec3i32 sub_3i32(Vec3i32 a, Vec3i32 b) {
  return (Vec3i32){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

Vec3i32 mul_3i32(Vec3i32 a, Vec3i32 b) {
  return (Vec3i32){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

Vec3i32 div_3i32(Vec3i32 a, Vec3i32 b) {
  return (Vec3i32){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z};
}

Vec3i32 scale_3i32(Vec3i32 v, i32 s) {
  return (Vec3i32){.x = v.x * s, .y = v.y * s, .z = v.z * s};
}

Vec3i32 lerp_3i32(Vec3i32 a, Vec3i32 b, f32 t) {
  return (Vec3i32){
      .x = (i32)lerp_f32((f32)a.x, (f32)b.x, t),
      .y = (i32)lerp_f32((f32)a.y, (f32)b.y, t),
      .z = (i32)lerp_f32((f32)a.z, (f32)b.z, t),
  };
}

i32 dot_3i32(Vec3i32 a, Vec3i32 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

i32 length_squared_3i32(Vec3i32 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

i32 length_3i32(Vec3i32 v) {
  return (i32)sqrtf((f32)(v.x * v.x + v.y * v.y + v.z * v.z));
}

// Vec4f32
Vec4f32 vec_4f32(f32 x, f32 y, f32 z, f32 w) {
  return (Vec4f32){.x = x, .y = y, .z = z, .w = w};
}

Vec4f32 add_4f32(Vec4f32 a, Vec4f32 b) {
  return (Vec4f32){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w};
}

Vec4f32 sub_4f32(Vec4f32 a, Vec4f32 b) {
  return (Vec4f32){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w};
}

Vec4f32 mul_4f32(Vec4f32 a, Vec4f32 b) {
  return (Vec4f32){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z, .w = a.w * b.w};
}

Vec4f32 div_4f32(Vec4f32 a, Vec4f32 b) {
  return (Vec4f32){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z, .w = a.w / b.w};
}

Vec4f32 scale_4f32(Vec4f32 v, f32 s) {
  return (Vec4f32){.x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s};
}

Vec4f32 lerp_4f32(Vec4f32 a, Vec4f32 b, f32 t) {
  return (Vec4f32){
      .x = lerp_f32(a.x, b.x, t),
      .y = lerp_f32(a.y, b.y, t),
      .z = lerp_f32(a.z, b.z, t),
      .w = lerp_f32(a.w, b.w, t),
  };
}

f32 dot_4f32(Vec4f32 a, Vec4f32 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

f32 length_squared_4f32(Vec4f32 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

f32 length_4f32(Vec4f32 v) {
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

Vec4f32 normalize_4f32(Vec4f32 v) {
  return scale_4f32(v, 1.f / length_4f32(v));
}

// Vec4i32
Vec4i32 vec_4i32(i32 x, i32 y, i32 z, i32 w) {
  return (Vec4i32){.x = x, .y = y, .z = z, .w = w};
}

Vec4i32 add_4i32(Vec4i32 a, Vec4i32 b) {
  return (Vec4i32){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w};
}

Vec4i32 sub_4i32(Vec4i32 a, Vec4i32 b) {
  return (Vec4i32){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w};
}

Vec4i32 mul_4i32(Vec4i32 a, Vec4i32 b) {
  return (Vec4i32){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z, .w = a.w * b.w};
}

Vec4i32 div_4i32(Vec4i32 a, Vec4i32 b) {
  return (Vec4i32){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z, .w = a.w / b.w};
}

Vec4i32 scale_4i32(Vec4i32 v, i32 s) {
  return (Vec4i32){.x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s};
}

Vec4i32 lerp_4i32(Vec4i32 a, Vec4i32 b, f32 t) {
  return (Vec4i32){
      .x = (i32)lerp_f32((f32)a.x, (f32)b.x, t),
      .y = (i32)lerp_f32((f32)a.y, (f32)b.y, t),
      .z = (i32)lerp_f32((f32)a.z, (f32)b.z, t),
      .w = (i32)lerp_f32((f32)a.w, (f32)b.w, t),
  };
}

i32 dot_4i32(Vec4i32 a, Vec4i32 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

i32 length_squared_4i32(Vec4i32 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

i32 length_4i32(Vec4i32 v) {
  return (i32)sqrtf((f32)(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
}

// matrix ops
Mat3x3f32 mat_3x3f32(f32 diagonal) {
  Mat3x3f32 result = {0};
  result.v[0][0] = diagonal;
  result.v[1][1] = diagonal;
  result.v[2][2] = diagonal;
  return result;
}

Mat3x3f32 make_translate_3x3f32(Vec2f32 delta) {
  Mat3x3f32 mat = mat_3x3f32(1.f);
  mat.v[2][0] = delta.x;
  mat.v[2][1] = delta.y;
  return mat;
}

Mat3x3f32 make_scale_3x3f32(Vec2f32 scale) {
  Mat3x3f32 mat = mat_3x3f32(1.f);
  mat.v[0][0] = scale.x;
  mat.v[1][1] = scale.y;
  return mat;
}

Mat3x3f32 mul_3x3f32(Mat3x3f32 a, Mat3x3f32 b) {
  Mat3x3f32 c = {0};
  for (i32 j = 0; j < 3; j += 1) {
    for (i32 i = 0; i < 3; i += 1) {
      c.v[i][j] = (a.v[0][j] * b.v[i][0] + a.v[1][j] * b.v[i][1] + a.v[2][j] * b.v[i][2]);
    }
  }
  return c;
}

Mat4x4f32 mat_4x4f32(f32 diagonal) {
  Mat4x4f32 result = {0};
  result.v[0][0] = diagonal;
  result.v[1][1] = diagonal;
  result.v[2][2] = diagonal;
  result.v[3][3] = diagonal;
  return result;
}

Mat4x4f32 make_translate_4x4f32(Vec3f32 delta) {
  Mat4x4f32 result = mat_4x4f32(1.f);
  result.v[3][0] = delta.x;
  result.v[3][1] = delta.y;
  result.v[3][2] = delta.z;
  return result;
}

Mat4x4f32 make_scale_4x4f32(Vec3f32 scale) {
  Mat4x4f32 result = mat_4x4f32(1.f);
  result.v[0][0] = scale.x;
  result.v[1][1] = scale.y;
  result.v[2][2] = scale.z;
  return result;
}

Mat4x4f32 make_perspective_4x4f32(f32 fov, f32 aspect_ratio, f32 near_z, f32 far_z) {
  Mat4x4f32 result = mat_4x4f32(1.f);
  f32 tan_theta_over_2 = tanf(fov / 2);
  result.v[0][0] = 1.f / tan_theta_over_2;
  result.v[1][1] = aspect_ratio / tan_theta_over_2;
  result.v[2][3] = 1.f;
  result.v[2][2] = -(near_z + far_z) / (near_z - far_z);
  result.v[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
  result.v[3][3] = 0.f;
  return result;
}

Mat4x4f32 make_orthographic_4x4f32(f32 left, f32 right, f32 bottom, f32 top, f32 near_z,
                                   f32 far_z) {
  Mat4x4f32 result = mat_4x4f32(1.f);

  result.v[0][0] = 2.f / (right - left);
  result.v[1][1] = 2.f / (top - bottom);
  result.v[2][2] = 2.f / (far_z - near_z);
  result.v[3][3] = 1.f;

  result.v[3][0] = (left + right) / (left - right);
  result.v[3][1] = (bottom + top) / (bottom - top);
  result.v[3][2] = (near_z + far_z) / (near_z - far_z);

  return result;
}

Mat4x4f32 make_look_at_4x4f32(Vec3f32 eye, Vec3f32 center, Vec3f32 up) {
  Mat4x4f32 result;
  Vec3f32 f = normalize_3f32(sub_3f32(eye, center));
  Vec3f32 s = normalize_3f32(cross_3f32(f, up));
  Vec3f32 u = cross_3f32(s, f);
  result.v[0][0] = s.x;
  result.v[0][1] = u.x;
  result.v[0][2] = -f.x;
  result.v[0][3] = 0.0f;
  result.v[1][0] = s.y;
  result.v[1][1] = u.y;
  result.v[1][2] = -f.y;
  result.v[1][3] = 0.0f;
  result.v[2][0] = s.z;
  result.v[2][1] = u.z;
  result.v[2][2] = -f.z;
  result.v[2][3] = 0.0f;
  result.v[3][0] = -dot_3f32(s, eye);
  result.v[3][1] = -dot_3f32(u, eye);
  result.v[3][2] = dot_3f32(f, eye);
  result.v[3][3] = 1.0f;
  return result;
}

Mat4x4f32 make_rotate_4x4f32(Vec3f32 axis, f32 rad) {
  Mat4x4f32 result = mat_4x4f32(1.f);
  axis = normalize_3f32(axis);
  f32 sin_theta = sinf(rad);
  f32 cos_theta = cosf(rad);
  f32 cos_value = 1.f - cos_theta;
  result.v[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  result.v[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
  result.v[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);
  result.v[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
  result.v[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  result.v[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);
  result.v[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
  result.v[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
  result.v[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return result;
}

Mat4x4f32 mul_4x4f32(Mat4x4f32 a, Mat4x4f32 b) {
  Mat4x4f32 c = {0};
  for (i32 j = 0; j < 4; j += 1) {
    for (i32 i = 0; i < 4; i += 1) {
      c.v[i][j] = (a.v[0][j] * b.v[i][0] + a.v[1][j] * b.v[i][1] + a.v[2][j] * b.v[i][2] +
                   a.v[3][j] * b.v[i][3]);
    }
  }
  return c;
}

Mat4x4f32 scale_4x4f32(Mat4x4f32 m, f32 scale) {
  for (i32 j = 0; j < 4; j += 1) {
    for (i32 i = 0; i < 4; i += 1) {
      m.v[i][j] *= scale;
    }
  }
  return m;
}

Mat4x4f32 inverse_4x4f32(Mat4x4f32 m) {
  f32 coef00 = m.v[2][2] * m.v[3][3] - m.v[3][2] * m.v[2][3];
  f32 coef02 = m.v[1][2] * m.v[3][3] - m.v[3][2] * m.v[1][3];
  f32 coef03 = m.v[1][2] * m.v[2][3] - m.v[2][2] * m.v[1][3];
  f32 coef04 = m.v[2][1] * m.v[3][3] - m.v[3][1] * m.v[2][3];
  f32 coef06 = m.v[1][1] * m.v[3][3] - m.v[3][1] * m.v[1][3];
  f32 coef07 = m.v[1][1] * m.v[2][3] - m.v[2][1] * m.v[1][3];
  f32 coef08 = m.v[2][1] * m.v[3][2] - m.v[3][1] * m.v[2][2];
  f32 coef10 = m.v[1][1] * m.v[3][2] - m.v[3][1] * m.v[1][2];
  f32 coef11 = m.v[1][1] * m.v[2][2] - m.v[2][1] * m.v[1][2];
  f32 coef12 = m.v[2][0] * m.v[3][3] - m.v[3][0] * m.v[2][3];
  f32 coef14 = m.v[1][0] * m.v[3][3] - m.v[3][0] * m.v[1][3];
  f32 coef15 = m.v[1][0] * m.v[2][3] - m.v[2][0] * m.v[1][3];
  f32 coef16 = m.v[2][0] * m.v[3][2] - m.v[3][0] * m.v[2][2];
  f32 coef18 = m.v[1][0] * m.v[3][2] - m.v[3][0] * m.v[1][2];
  f32 coef19 = m.v[1][0] * m.v[2][2] - m.v[2][0] * m.v[1][2];
  f32 coef20 = m.v[2][0] * m.v[3][1] - m.v[3][0] * m.v[2][1];
  f32 coef22 = m.v[1][0] * m.v[3][1] - m.v[3][0] * m.v[1][1];
  f32 coef23 = m.v[1][0] * m.v[2][1] - m.v[2][0] * m.v[1][1];

  Vec4f32 fac0 = vec_4f32(coef00, coef00, coef02, coef03);
  Vec4f32 fac1 = vec_4f32(coef04, coef04, coef06, coef07);
  Vec4f32 fac2 = vec_4f32(coef08, coef08, coef10, coef11);
  Vec4f32 fac3 = vec_4f32(coef12, coef12, coef14, coef15);
  Vec4f32 fac4 = vec_4f32(coef16, coef16, coef18, coef19);
  Vec4f32 fac5 = vec_4f32(coef20, coef20, coef22, coef23);

  Vec4f32 vec0 = vec_4f32(m.v[1][0], m.v[0][0], m.v[0][0], m.v[0][0]);
  Vec4f32 vec1 = vec_4f32(m.v[1][1], m.v[0][1], m.v[0][1], m.v[0][1]);
  Vec4f32 vec2 = vec_4f32(m.v[1][2], m.v[0][2], m.v[0][2], m.v[0][2]);
  Vec4f32 vec3 = vec_4f32(m.v[1][3], m.v[0][3], m.v[0][3], m.v[0][3]);

  Vec4f32 inv0 = add_4f32(sub_4f32(mul_4f32(vec1, fac0), mul_4f32(vec2, fac1)),
                          mul_4f32(vec3, fac2));
  Vec4f32 inv1 = add_4f32(sub_4f32(mul_4f32(vec0, fac0), mul_4f32(vec2, fac3)),
                          mul_4f32(vec3, fac4));
  Vec4f32 inv2 = add_4f32(sub_4f32(mul_4f32(vec0, fac1), mul_4f32(vec1, fac3)),
                          mul_4f32(vec3, fac5));
  Vec4f32 inv3 = add_4f32(sub_4f32(mul_4f32(vec0, fac2), mul_4f32(vec1, fac4)),
                          mul_4f32(vec2, fac5));

  Vec4f32 sign_a = vec_4f32(+1, -1, +1, -1);
  Vec4f32 sign_b = vec_4f32(-1, +1, -1, +1);

  f32 inv0_v[4] = {inv0.x, inv0.y, inv0.z, inv0.w};
  f32 inv1_v[4] = {inv1.x, inv1.y, inv1.z, inv1.w};
  f32 inv2_v[4] = {inv2.x, inv2.y, inv2.z, inv2.w};
  f32 inv3_v[4] = {inv3.x, inv3.y, inv3.z, inv3.w};
  f32 sign_a_v[4] = {sign_a.x, sign_a.y, sign_a.z, sign_a.w};
  f32 sign_b_v[4] = {sign_b.x, sign_b.y, sign_b.z, sign_b.w};

  Mat4x4f32 inverse;
  for (u32 i = 0; i < 4; i += 1) {
    inverse.v[0][i] = inv0_v[i] * sign_a_v[i];
    inverse.v[1][i] = inv1_v[i] * sign_b_v[i];
    inverse.v[2][i] = inv2_v[i] * sign_a_v[i];
    inverse.v[3][i] = inv3_v[i] * sign_b_v[i];
  }

  Vec4f32 row0 =
      vec_4f32(inverse.v[0][0], inverse.v[1][0], inverse.v[2][0], inverse.v[3][0]);
  Vec4f32 m0 = vec_4f32(m.v[0][0], m.v[0][1], m.v[0][2], m.v[0][3]);
  Vec4f32 dot0 = mul_4f32(m0, row0);
  f32 dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

  f32 one_over_det = 1 / dot1;

  return scale_4x4f32(inverse, one_over_det);
}

Mat4x4f32 derotate_4x4f32(Mat4x4f32 mat) {
  Vec3f32 scale = vec_3f32(length_3f32(vec_3f32(mat.v[0][0], mat.v[0][1], mat.v[0][2])),
                           length_3f32(vec_3f32(mat.v[1][0], mat.v[1][1], mat.v[1][2])),
                           length_3f32(vec_3f32(mat.v[2][0], mat.v[2][1], mat.v[2][2])));
  mat.v[0][0] = scale.x;
  mat.v[1][0] = 0.f;
  mat.v[2][0] = 0.f;
  mat.v[0][1] = 0.f;
  mat.v[1][1] = scale.y;
  mat.v[2][1] = 0.f;
  mat.v[0][2] = 0.f;
  mat.v[1][2] = 0.f;
  mat.v[2][2] = scale.z;
  return mat;
}

Mat4x4f32 transpose_4x4f32(Mat4x4f32 mat) {
  Mat4x4f32 result;
  result.v[0][0] = mat.v[0][0];
  result.v[0][1] = mat.v[1][0];
  result.v[0][2] = mat.v[2][0];
  result.v[0][3] = mat.v[3][0];
  result.v[1][0] = mat.v[0][1];
  result.v[1][1] = mat.v[1][1];
  result.v[1][2] = mat.v[2][1];
  result.v[1][3] = mat.v[3][1];
  result.v[2][0] = mat.v[0][2];
  result.v[2][1] = mat.v[1][2];
  result.v[2][2] = mat.v[2][2];
  result.v[2][3] = mat.v[3][2];
  result.v[3][0] = mat.v[0][3];
  result.v[3][1] = mat.v[1][3];
  result.v[3][2] = mat.v[2][3];
  result.v[3][3] = mat.v[3][3];
  return result;
}

// color ops
Vec3f32 rgb_to_hsv(Vec3f32 rgb) {
  f32 c_max = fmaxf(rgb.x, fmaxf(rgb.y, rgb.z));
  f32 c_min = fminf(rgb.x, fminf(rgb.y, rgb.z));
  f32 delta = c_max - c_min;
  f32 h = ((delta == 0.f)     ? 0.f
           : (c_max == rgb.x) ? fmodf((rgb.y - rgb.z) / delta + 6.f, 6.f)
           : (c_max == rgb.y) ? (rgb.z - rgb.x) / delta + 2.f
           : (c_max == rgb.z) ? (rgb.x - rgb.y) / delta + 4.f
                              : 0.f);
  f32 s = (c_max == 0.f) ? 0.f : (delta / c_max);
  f32 v = c_max;
  return vec_3f32(h / 6.f, s, v);
}

Vec3f32 hsv_to_rgb(Vec3f32 hsv) {
  f32 h = fmodf(hsv.x * 360.f, 360.f);
  f32 s = hsv.y;
  f32 v = hsv.z;

  f32 c = v * s;
  f32 x = c * (1.f - fabsf(fmodf(h / 60.f, 2.f) - 1.f));
  f32 m = v - c;

  f32 r = 0;
  f32 g = 0;
  f32 b = 0;

  if ((h >= 0.f && h < 60.f) || (h >= 360.f && h < 420.f)) {
    r = c;
    g = x;
    b = 0;
  } else if (h >= 60.f && h < 120.f) {
    r = x;
    g = c;
    b = 0;
  } else if (h >= 120.f && h < 180.f) {
    r = 0;
    g = c;
    b = x;
  } else if (h >= 180.f && h < 240.f) {
    r = 0;
    g = x;
    b = c;
  } else if (h >= 240.f && h < 300.f) {
    r = x;
    g = 0;
    b = c;
  } else if ((h >= 300.f && h <= 360.f) || (h >= -60.f && h <= 0.f)) {
    r = c;
    g = 0;
    b = x;
  }

  return vec_3f32(r + m, g + m, b + m);
}

Vec4f32 rgba_to_hsva(Vec4f32 rgba) {
  Vec3f32 rgb = vec_3f32(rgba.x, rgba.y, rgba.z);
  Vec3f32 hsv = rgb_to_hsv(rgb);
  return vec_4f32(hsv.x, hsv.y, hsv.z, rgba.w);
}

Vec4f32 hsva_to_rgba(Vec4f32 hsva) {
  Vec3f32 hsv = vec_3f32(hsva.x, hsva.y, hsva.z);
  Vec3f32 rgb = hsv_to_rgb(hsv);
  return vec_4f32(rgb.x, rgb.y, rgb.z, hsva.w);
}

Vec3f32 linear_to_srgb(Vec3f32 linear) {
  Vec3f32 result;
  result.x = (linear.x < 0.00313066844250063f)
                 ? linear.x * 12.92f
                 : 1.055f * powf(linear.x, 1.f / 2.4f) - 0.055f;
  result.y = (linear.y < 0.00313066844250063f)
                 ? linear.y * 12.92f
                 : 1.055f * powf(linear.y, 1.f / 2.4f) - 0.055f;
  result.z = (linear.z < 0.00313066844250063f)
                 ? linear.z * 12.92f
                 : 1.055f * powf(linear.z, 1.f / 2.4f) - 0.055f;
  return result;
}

Vec3f32 srgb_to_linear(Vec3f32 srgb) {
  Vec3f32 result;
  result.x = (srgb.x < 0.0404482362771082f) ? srgb.x / 12.92f
                                            : powf((srgb.x + 0.055f) / 1.055f, 2.4f);
  result.y = (srgb.y < 0.0404482362771082f) ? srgb.y / 12.92f
                                            : powf((srgb.y + 0.055f) / 1.055f, 2.4f);
  result.z = (srgb.z < 0.0404482362771082f) ? srgb.z / 12.92f
                                            : powf((srgb.z + 0.055f) / 1.055f, 2.4f);
  return result;
}

Vec4f32 linear_to_srgba(Vec4f32 linear) {
  Vec3f32 srgb = linear_to_srgb(vec_3f32(linear.x, linear.y, linear.z));
  return vec_4f32(srgb.x, srgb.y, srgb.z, linear.w);
}

Vec4f32 srgba_to_linear(Vec4f32 srgba) {
  Vec3f32 linear = srgb_to_linear(vec_3f32(srgba.x, srgba.y, srgba.z));
  return vec_4f32(linear.x, linear.y, linear.z, srgba.w);
}

Vec3f32 oklab_to_linear(Vec3f32 oklab) {
  f32 l_ = oklab.x + 0.3963377774f * oklab.y + 0.2158037573f * oklab.z;
  f32 m_ = oklab.x - 0.1055613458f * oklab.y - 0.0638541728f * oklab.z;
  f32 s_ = oklab.x - 0.0894841775f * oklab.y - 1.2914855480f * oklab.z;
  f32 l = l_ * l_ * l_;
  f32 m = m_ * m_ * m_;
  f32 s = s_ * s_ * s_;
  Vec3f32 result;
  result.x = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
  result.y = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
  result.z = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
  return result;
}

Vec3f32 linear_to_oklab(Vec3f32 linear) {
  f32 l =
      (0.4122214708f * linear.x + 0.5363325363f * linear.y + 0.0514459929f * linear.z);
  f32 m =
      (0.2119034982f * linear.x + 0.6806995451f * linear.y + 0.1073969566f * linear.z);
  f32 s =
      (0.0883024619f * linear.x + 0.2817188376f * linear.y + 0.6299787005f * linear.z);
  f32 l_ = cbrtf(l);
  f32 m_ = cbrtf(m);
  f32 s_ = cbrtf(s);
  Vec3f32 result;
  result.x = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  result.y = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  result.z = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;
  return result;
}

Vec4f32 oklab_to_lineara(Vec4f32 oklab) {
  Vec3f32 linear = oklab_to_linear(vec_3f32(oklab.x, oklab.y, oklab.z));
  return vec_4f32(linear.x, linear.y, linear.z, oklab.w);
}

Vec4f32 lineara_to_oklab(Vec4f32 linear) {
  Vec3f32 oklab = linear_to_oklab(vec_3f32(linear.x, linear.y, linear.z));
  return vec_4f32(oklab.x, oklab.y, oklab.z, linear.w);
}

Vec4f32 u32_to_rgba(u32 hex) {
  return vec_4f32(((hex & 0xff000000) >> 24) / 255.f, ((hex & 0x00ff0000) >> 16) / 255.f,
                  ((hex & 0x0000ff00) >> 8) / 255.f, ((hex & 0x000000ff) >> 0) / 255.f);
}

u32 rgba_to_u32(Vec4f32 hex) {
  u32 result = 0;
  result |= ((u32)((u8)(hex.x * 255.f))) << 24;
  result |= ((u32)((u8)(hex.y * 255.f))) << 16;
  result |= ((u32)((u8)(hex.z * 255.f))) << 8;
  result |= ((u32)((u8)(hex.w * 255.f))) << 0;
  return result;
}
