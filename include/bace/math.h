#ifndef _H_MATH
#define _H_MATH

#include <math.h> // IWYU pragma: export

#include "base.h"

#define rad_to_deg_f32(rad) (rad * 180.0f / PI32)
#define rad_to_deg_f64(rad) (rad * 180.0 / PI64)

#define deg_to_rad_f32(deg) (deg * PI32 / 180.0f)
#define deg_to_rad_f64(deg) (deg * PI64 / 180.0)

// vector types
#define Vec2(ty)                                                                         \
  typedef union Vec2##ty {                                                               \
    struct {                                                                             \
      ty x;                                                                              \
      ty y;                                                                              \
    };                                                                                   \
    ty v[2];                                                                             \
  } Vec2##ty

Vec2(f32);
Vec2(i16);
Vec2(i32);
Vec2(i64);

#define Vec3(ty)                                                                         \
  typedef union Vec3##ty {                                                               \
    struct {                                                                             \
      ty x;                                                                              \
      ty y;                                                                              \
      ty z;                                                                              \
    };                                                                                   \
    ty v[3];                                                                             \
  } Vec3##ty

Vec3(f32);
Vec3(i32);

#define Vec4(ty)                                                                         \
  typedef union Vec4##ty {                                                               \
    struct {                                                                             \
      ty x;                                                                              \
      ty y;                                                                              \
      ty z;                                                                              \
      ty w;                                                                              \
    };                                                                                   \
    ty v[4];                                                                             \
  } Vec4##ty

Vec4(f32);
Vec4(i32);

// matrices
typedef struct Mat3x3f32 {
  f32 v[3][3];
} Mat3x3f32;

typedef struct Mat4x4f32 {
  f32 v[4][4];
} Mat4x4f32;

f32 lerp_f32(f32 a, f32 b, f32 t);
f64 lerp_f64(f64 a, f64 b, f64 t);

// vec ops
#define v2f32(x, y) vec_2f32((x), (y))
Vec2f32 vec_2f32(f32 x, f32 y);
Vec2f32 add_2f32(Vec2f32 a, Vec2f32 b);
Vec2f32 sub_2f32(Vec2f32 a, Vec2f32 b);
Vec2f32 mul_2f32(Vec2f32 a, Vec2f32 b);
Vec2f32 div_2f32(Vec2f32 a, Vec2f32 b);
Vec2f32 scale_2f32(Vec2f32 v, f32 s);
Vec2f32 lerp_2f32(Vec2f32 a, Vec2f32 b, f32 t);
f32 dot_2f32(Vec2f32 a, Vec2f32 b);
f32 length_squared_2f32(Vec2f32 v);
f32 length_2f32(Vec2f32 v);
Vec2f32 normalize_2f32(Vec2f32 v);

#define v2i16(x, y) vec_2i16((x), (y))
Vec2i16 vec_2i16(i16 x, i16 y);
Vec2i16 add_2i16(Vec2i16 a, Vec2i16 b);
Vec2i16 sub_2i16(Vec2i16 a, Vec2i16 b);
Vec2i16 mul_2i16(Vec2i16 a, Vec2i16 b);
Vec2i16 div_2i16(Vec2i16 a, Vec2i16 b);
Vec2i16 scale_2i16(Vec2i16 v, i16 s);
Vec2i16 lerp_2i16(Vec2i16 a, Vec2i16 b, f32 t);
i16 dot_2i16(Vec2i16 a, Vec2i16 b);
i16 length_squared_2i16(Vec2i16 v);
i16 length_2i16(Vec2i16 v);

#define v2i32(x, y) vec_2i32((x), (y))
Vec2i32 vec_2i32(i32 x, i32 y);
Vec2i32 add_2i32(Vec2i32 a, Vec2i32 b);
Vec2i32 sub_2i32(Vec2i32 a, Vec2i32 b);
Vec2i32 mul_2i32(Vec2i32 a, Vec2i32 b);
Vec2i32 div_2i32(Vec2i32 a, Vec2i32 b);
Vec2i32 scale_2i32(Vec2i32 v, i32 s);
Vec2i32 lerp_2i32(Vec2i32 a, Vec2i32 b, f32 t);
i32 dot_2i32(Vec2i32 a, Vec2i32 b);
i32 length_squared_2i32(Vec2i32 v);
i32 length_2i32(Vec2i32 v);

#define v2i64(x, y) vec_2i64((x), (y))
Vec2i64 vec_2i64(i64 x, i64 y);
Vec2i64 add_2i64(Vec2i64 a, Vec2i64 b);
Vec2i64 sub_2i64(Vec2i64 a, Vec2i64 b);
Vec2i64 mul_2i64(Vec2i64 a, Vec2i64 b);
Vec2i64 div_2i64(Vec2i64 a, Vec2i64 b);
Vec2i64 scale_2i64(Vec2i64 v, i64 s);
Vec2i64 lerp_2i64(Vec2i64 a, Vec2i64 b, f32 t);
i64 dot_2i64(Vec2i64 a, Vec2i64 b);
i64 length_squared_2i64(Vec2i64 v);
i64 length_2i64(Vec2i64 v);

#define v3f32(x, y) vec_3f32((x), (y))
Vec3f32 vec_3f32(f32 x, f32 y, f32 z);
Vec3f32 cross_3f32(Vec3f32 a, Vec3f32 b);
Vec3f32 add_3f32(Vec3f32 a, Vec3f32 b);
Vec3f32 sub_3f32(Vec3f32 a, Vec3f32 b);
Vec3f32 mul_3f32(Vec3f32 a, Vec3f32 b);
Vec3f32 div_3f32(Vec3f32 a, Vec3f32 b);
Vec3f32 scale_3f32(Vec3f32 v, f32 s);
Vec3f32 lerp_3f32(Vec3f32 a, Vec3f32 b, f32 t);
f32 dot_3f32(Vec3f32 a, Vec3f32 b);
f32 length_squared_3f32(Vec3f32 v);
f32 length_3f32(Vec3f32 v);
Vec3f32 normalize_3f32(Vec3f32 v);
Vec3f32 transform_3f32(Vec3f32 v, Mat3x3f32 m);

#define v3i32(x, y) vec_3i32((x), (y))
Vec3i32 vec_3i32(i32 x, i32 y, i32 z);
Vec3i32 cross_3i32(Vec3i32 a, Vec3i32 b);
Vec3i32 add_3i32(Vec3i32 a, Vec3i32 b);
Vec3i32 sub_3i32(Vec3i32 a, Vec3i32 b);
Vec3i32 mul_3i32(Vec3i32 a, Vec3i32 b);
Vec3i32 div_3i32(Vec3i32 a, Vec3i32 b);
Vec3i32 scale_3i32(Vec3i32 v, i32 s);
Vec3i32 lerp_3i32(Vec3i32 a, Vec3i32 b, f32 t);
i32 dot_3i32(Vec3i32 a, Vec3i32 b);
i32 length_squared_3i32(Vec3i32 v);
i32 length_3i32(Vec3i32 v);

#define v4f32(x, y) vec_4f32((x), (y))
Vec4f32 vec_4f32(f32 x, f32 y, f32 z, f32 w);
Vec4f32 add_4f32(Vec4f32 a, Vec4f32 b);
Vec4f32 sub_4f32(Vec4f32 a, Vec4f32 b);
Vec4f32 mul_4f32(Vec4f32 a, Vec4f32 b);
Vec4f32 div_4f32(Vec4f32 a, Vec4f32 b);
Vec4f32 scale_4f32(Vec4f32 v, f32 s);
Vec4f32 lerp_4f32(Vec4f32 a, Vec4f32 b, f32 t);
f32 dot_4f32(Vec4f32 a, Vec4f32 b);
f32 length_squared_4f32(Vec4f32 v);
f32 length_4f32(Vec4f32 v);
Vec4f32 normalize_4f32(Vec4f32 v);

#define v4i32(x, y) vec_4i32((x), (y))
Vec4i32 vec_4i32(i32 x, i32 y, i32 z, i32 w);
Vec4i32 add_4i32(Vec4i32 a, Vec4i32 b);
Vec4i32 sub_4i32(Vec4i32 a, Vec4i32 b);
Vec4i32 mul_4i32(Vec4i32 a, Vec4i32 b);
Vec4i32 div_4i32(Vec4i32 a, Vec4i32 b);
Vec4i32 scale_4i32(Vec4i32 v, i32 s);
Vec4i32 lerp_4i32(Vec4i32 a, Vec4i32 b, f32 t);
i32 dot_4i32(Vec4i32 a, Vec4i32 b);
i32 length_squared_4i32(Vec4i32 v);
i32 length_4i32(Vec4i32 v);

// matrix ops
Mat3x3f32 mat_3x3f32(f32 diagonal);
Mat3x3f32 make_translate_3x3f32(Vec2f32 delta);
Mat3x3f32 make_scale_3x3f32(Vec2f32 scale);
Mat3x3f32 mul_3x3f32(Mat3x3f32 a, Mat3x3f32 b);

Mat4x4f32 mat_4x4f32(f32 diagonal);
Mat4x4f32 make_translate_4x4f32(Vec3f32 delta);
Mat4x4f32 make_scale_4x4f32(Vec3f32 scale);
Mat4x4f32 make_perspective_4x4f32(f32 fov, f32 aspect_ratio, f32 near_z, f32 far_z);
Mat4x4f32 make_orthographic_4x4f32(f32 left, f32 right, f32 bottom, f32 top, f32 near_z,
                                   f32 far_z);
Mat4x4f32 make_look_at_4x4f32(Vec3f32 eye, Vec3f32 center, Vec3f32 up);
Mat4x4f32 make_rotate_4x4f32(Vec3f32 axis, f32 turns);
Mat4x4f32 mul_4x4f32(Mat4x4f32 a, Mat4x4f32 b);
Mat4x4f32 scale_4x4f32(Mat4x4f32 m, f32 scale);
Mat4x4f32 inverse_4x4f32(Mat4x4f32 m);
Mat4x4f32 derotate_4x4f32(Mat4x4f32 mat);
Mat4x4f32 transpose_4x4f32(Mat4x4f32 mat);

// color ops
Vec3f32 rgb_to_hsv(Vec3f32 rgb);
Vec3f32 hsv_to_rgb(Vec3f32 hsv);
Vec4f32 rgba_to_hsva(Vec4f32 rgba);
Vec4f32 hsva_to_rgba(Vec4f32 hsva);

Vec3f32 linear_to_srgb(Vec3f32 linear);
Vec3f32 srgb_to_linear(Vec3f32 srgb);
Vec4f32 linear_to_srgba(Vec4f32 linear);
Vec4f32 srgba_to_linear(Vec4f32 srgba);

Vec3f32 oklab_to_linear(Vec3f32 oklab);
Vec3f32 linear_to_oklab(Vec3f32 linear);
Vec4f32 oklab_to_lineara(Vec4f32 oklab);
Vec4f32 lineara_to_oklab(Vec4f32 linear);

Vec4f32 u32_to_rgba(u32 hex);
u32 rgba_to_u32(Vec4f32 hex);

#endif // !_H_MATH
