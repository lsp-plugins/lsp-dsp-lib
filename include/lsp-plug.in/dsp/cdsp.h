/*
 * cdsp.h
 *
 *  Created on: 19 Aug 2020.
 *      Author: Alexandros Theodotou
 */

#ifndef LSP_PLUG_IN_DSP_DSP_C_H_
#define LSP_PLUG_IN_DSP_DSP_C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <lsp-plug.in/dsp/version.h>

/* --- 3dmath --- */

#pragma pack(push, 1)
typedef struct lsp_dsp_point3d_t
{
    float       x, y, z, w;
} point3d_t;

typedef struct lsp_dsp_vector3d_t
{
    float       dx, dy, dz, dw;
} vector3d_t;

typedef struct lsp_dsp_color3d_t
{
    float       r, g, b, a;
} color3d_t;

typedef struct lsp_dsp_ray3d_t
{
    point3d_t   z;          // The start point: x, y, z = point coordinates
    vector3d_t  v;          // The spread vector: dx, dy, dz = direction
} ray3d_t;

typedef struct lsp_dsp_bound_box3d_t
{
    point3d_t   p[8];       // Bounding-box contains 8 points
} bound_box3d_t;

typedef struct lsp_dsp_segment3d_t
{
    point3d_t   p[2];
} segment3d_t;

typedef struct lsp_dsp_triangle3d_t
{
    point3d_t   p[3];       // Vertexes: x, y, z = vertex coordinates, w = length of the corresponding edge
    vector3d_t  n;          // Normal: dx, dy, dz = normal coordinates, dw = plane equation coefficient
} triangle3d_t;

typedef struct lsp_dsp_matrix3d_t
{
    float       m[16];      // Matrix data
} matrix3d_t;

typedef struct lsp_dsp_raw_triangle_t
{
    point3d_t   v[3];
} raw_triangle_t;

#pragma pack(pop)

enum lsp_dsp_axis_orientation_t
{
    AO3D_POS_X_FWD_POS_Y_UP,
    AO3D_POS_X_FWD_POS_Z_UP,
    AO3D_POS_X_FWD_NEG_Y_UP,
    AO3D_POS_X_FWD_NEG_Z_UP,
    AO3D_NEG_X_FWD_POS_Y_UP,
    AO3D_NEG_X_FWD_POS_Z_UP,
    AO3D_NEG_X_FWD_NEG_Y_UP,
    AO3D_NEG_X_FWD_NEG_Z_UP,

    AO3D_POS_Y_FWD_POS_X_UP,
    AO3D_POS_Y_FWD_POS_Z_UP,
    AO3D_POS_Y_FWD_NEG_X_UP,
    AO3D_POS_Y_FWD_NEG_Z_UP,
    AO3D_NEG_Y_FWD_POS_X_UP,
    AO3D_NEG_Y_FWD_POS_Z_UP,
    AO3D_NEG_Y_FWD_NEG_X_UP,
    AO3D_NEG_Y_FWD_NEG_Z_UP,

    AO3D_POS_Z_FWD_POS_X_UP,
    AO3D_POS_Z_FWD_POS_Y_UP,
    AO3D_POS_Z_FWD_NEG_X_UP,
    AO3D_POS_Z_FWD_NEG_Y_UP,
    AO3D_NEG_Z_FWD_POS_X_UP,
    AO3D_NEG_Z_FWD_POS_Y_UP,
    AO3D_NEG_Z_FWD_NEG_X_UP,
    AO3D_NEG_Z_FWD_NEG_Y_UP
};

void
lsp_dsp_init_point_xyz (
  lsp_dsp_point3d_t * p,
  float               x,
  float               y,
  float               z);

void
lsp_dsp_init_point (
  lsp_dsp_point3d_t * p,
  lsp_dsp_point3d_t * s);

void
lsp_dsp_normalize_point (
  lsp_dsp_point3d_t * p);

void
lsp_dsp_scale_point1 (
  lsp_dsp_point3d_t * p,
  float               r);

void
lsp_dsp_scale_point2 (
  lsp_dsp_point3d_t * p,
  lsp_dsp_point3d_t * s,
  float               r);

void
lsp_dsp_init_vector_dxyz (
  lsp_dsp_vector3d_t * v,
  float                dx,
  float                dy,
  float                dz);

void
lsp_dsp_init_vector (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * s);

void
lsp_dsp_init_vector_p2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_point3d_t *  p1,
  lsp_dsp_point3d_t *  p2);

void
lsp_dsp_init_vector_pv (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_point3d_t *  pv);

void
lsp_dsp_init_normal3d_xyz (
  lsp_dsp_vector3d_t * v,
  float                x1,
  float                y1,
  float                z1,
  float                x2,
  float                y2,
  float                z2);

void
lsp_dsp_init_normal3d_dxyz (
  lsp_dsp_vector3d_t * v,
  float                dx,
  float                dy,
  float                dz);

void
lsp_dsp_init_normal3d (
  lsp_dsp_vector3d_t * p,
  lsp_dsp_vector3d_t * s);

void
lsp_dsp_normalize_vector (
  lsp_dsp_vector3d_t * m);

void
lsp_dsp_normalize_vector2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * src);

void
lsp_dsp_flip_vector_v1 (
  lsp_dsp_vector3d_t * v);

void
lsp_dsp_flip_vector_v2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * sv);

void
lsp_dsp_scale_vector1 (
  lsp_dsp_vector3d_t * v,
  float                r);

void
lsp_dsp_scale_vector2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * s,
  float                r);

void
lsp_dsp_vector_mul_v2 (
  lsp_dsp_vector3d_t * r,
  lsp_dsp_vector3d_t * v1,
  lsp_dsp_vector3d_t * v2);

void
lsp_dsp_vector_mul_vv (
  lsp_dsp_vector3d_t * r,
  lsp_dsp_vector3d_t * vv);

/* TODO finish 3dmath */

/* --- coding --- */

size_t
lsp_dsp_base64_enc (
  void *       dst,
  size_t *     dst_left,
  const void * src,
  size_t *     src_left);

ssize_t
lsp_dsp_base64_dec (
  void *       dst,
  size_t *     dst_left,
  const void * src,
  size_t *     src_left);

/* --- context --- */

typedef struct lsp_dsp_info_t
{
    const char     *arch;       /* Architecture information */
    const char     *cpu;        /* CPU information */
    const char     *model;      /* CPU model information */
    const char     *features;   /* CPU features */
} lsp_dsp_info_t;

#pragma pack(push,1)
/**
 * DSP context to store and restore machine state
 */
typedef struct lsp_dsp_context_t
{
    uint32_t        top;
    uint32_t        data[15];
} lsp_dsp_context_t;
#pragma pack(pop)

void
lsp_dsp_init (void);

lsp_dsp_info_t *
lsp_dsp_info_new (void);

void
lsp_dsp_info_free (
  lsp_dsp_info_t * info);

void
lsp_dsp_start (
  lsp_dsp_context_t * c_ctx);

void
lsp_dsp_finish (
  lsp_dsp_context_t * c_ctx);

/* --- copy --- */

void
lsp_dsp_copy (
  float *       dst,
  const float * src,
  size_t        count);

void
lsp_dsp_move (
  float *       dst,
  const float * src,
  size_t        count);

void
lsp_dsp_fill (
  float * dst,
  float   value,
  size_t  count);

void
lsp_dsp_fill_zero (
  float * dst,
  size_t  count);

void
lsp_dsp_fill_one (
  float * dst,
  size_t  count);

void
lsp_dsp_fill_minus_one (
  float * dst,
  size_t  count);

void
lsp_dsp_reverse1 (
  float * dst,
  size_t  count);

void
lsp_dsp_reverse2 (
  float *       dst,
  const float * src,
  size_t        count);

/* --- mix --- */

void
lsp_dsp_mix2 (
  float *       dst,
  const float * src,
  float         k1,
  float         k2,
  size_t        count);

void
lsp_dsp_mix_copy2 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  size_t        count);

void
lsp_dsp_mix_add2 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  size_t        count);

void
lsp_dsp_mix3 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  float         k3,
  size_t        count);

void
lsp_dsp_mix_copy3 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  float         k1,
  float         k2,
  float         k3,
  size_t        count);

void
lsp_dsp_mix_add3 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  float         k1,
  float         k2,
  float         k3,
  size_t        count);

void
lsp_dsp_mix4 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  float         k1,
  float         k2,
  float         k3,
  float         k4,
  size_t        count);

void
lsp_dsp_mix_copy4 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  const float * src4,
  float         k1,
  float         k2,
  float         k3,
  float         k4,
  size_t        count);

void
lsp_dsp_mix_add4 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  const float * src4,
  float         k1,
  float         k2,
  float         k3,
  float         k4,
  size_t        count);

#ifdef __cplusplus
}
#endif

#endif /* LSP_PLUG_IN_DSP_DSP_C_H_ */
