/*
 * dsp_c.c
 *
 *  Created on: 19 Aug 2020.
 *      Author: Alexandros Theodotou
 */

#include <cstdlib>

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/dsp/cdsp.h>

extern "C"
{

/* --- 3dmath --- */

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_point_xyz (
  lsp_dsp_point3d_t * p,
  float               x,
  float               y,
  float               z)
{
  lsp::dsp::init_point_xyz (
    (lsp::dsp::point3d_t *) p, x, y, z);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_point (
  lsp_dsp_point3d_t * p,
  lsp_dsp_point3d_t * s)
{
  lsp::dsp::init_point (
    (lsp::dsp::point3d_t *) p,
    (lsp::dsp::point3d_t *) s);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_normalize_point (
  lsp_dsp_point3d_t * p)
{
  lsp::dsp::normalize_point (
    (lsp::dsp::point3d_t *) p);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_scale_point1 (
  lsp_dsp_point3d_t * p,
  float               r)
{
  lsp::dsp::scale_point1 (
    (lsp::dsp::point3d_t *) p, r);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_scale_point2 (
  lsp_dsp_point3d_t * p,
  lsp_dsp_point3d_t * s,
  float               r)
{
  lsp::dsp::scale_point2 (
    (lsp::dsp::point3d_t *) p,
    (lsp::dsp::point3d_t *) s, r);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_vector_dxyz (
  lsp_dsp_vector3d_t * v,
  float                dx,
  float                dy,
  float                dz)
{
  lsp::dsp::init_vector_dxyz (
    (lsp::dsp::vector3d_t *) v, dx, dy, dz);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_vector (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * s)
{
  lsp::dsp::init_vector (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::vector3d_t *) s);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_vector_p2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_point3d_t *  p1,
  lsp_dsp_point3d_t *  p2)
{
  lsp::dsp::init_vector_p2 (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::point3d_t *) p1,
    (lsp::dsp::point3d_t *) p2);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_vector_pv (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_point3d_t *  pv)
{
  lsp::dsp::init_vector_pv (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::point3d_t *) pv);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_normal3d_xyz (
  lsp_dsp_vector3d_t * v,
  float                x1,
  float                y1,
  float                z1,
  float                x2,
  float                y2,
  float                z2)
{
  lsp::dsp::init_normal3d_xyz (
    (lsp::dsp::vector3d_t *) v, x1, y1, z1, x2, y2, z2);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_normal3d_dxyz (
  lsp_dsp_vector3d_t * v,
  float                dx,
  float                dy,
  float                dz)
{
  lsp::dsp::init_normal3d_dxyz (
    (lsp::dsp::vector3d_t *) v, dx, dy, dz);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init_normal3d (
  lsp_dsp_vector3d_t * p,
  lsp_dsp_vector3d_t * s)
{
  lsp::dsp::init_normal3d (
    (lsp::dsp::vector3d_t *) p,
    (lsp::dsp::vector3d_t *) s);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_normalize_vector (
  lsp_dsp_vector3d_t * m)
{
  lsp::dsp::normalize_vector (
    (lsp::dsp::vector3d_t *) m);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_normalize_vector2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * src)
{
  lsp::dsp::normalize_vector2 (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::vector3d_t *) src);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_flip_vector_v1 (
  lsp_dsp_vector3d_t * v)
{
  lsp::dsp::flip_vector_v1 (
    (lsp::dsp::vector3d_t *) v);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_flip_vector_v2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * sv)
{
  lsp::dsp::flip_vector_v2 (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::vector3d_t *) sv);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_scale_vector1 (
  lsp_dsp_vector3d_t * v,
  float                r)
{
  lsp::dsp::scale_vector1 (
    (lsp::dsp::vector3d_t *) v, r);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_scale_vector2 (
  lsp_dsp_vector3d_t * v,
  lsp_dsp_vector3d_t * s,
  float                r)
{
  lsp::dsp::scale_vector2 (
    (lsp::dsp::vector3d_t *) v,
    (lsp::dsp::vector3d_t *) s, r);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_vector_mul_v2 (
  lsp_dsp_vector3d_t * r,
  lsp_dsp_vector3d_t * v1,
  lsp_dsp_vector3d_t * v2)
{
  lsp::dsp::vector_mul_v2 (
    (lsp::dsp::vector3d_t *) r,
    (lsp::dsp::vector3d_t *) v1,
    (lsp::dsp::vector3d_t *) v2);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_vector_mul_vv (
  lsp_dsp_vector3d_t * r,
  lsp_dsp_vector3d_t * vv)
{
  lsp::dsp::vector_mul_vv (
    (lsp::dsp::vector3d_t *) r,
    (lsp::dsp::vector3d_t *) vv);
}

/* --- coding --- */

LSP_DSP_LIB_EXPORT
size_t
lsp_dsp_base64_enc (
  void *       dst,
  size_t *     dst_left,
  const void * src,
  size_t *     src_left)
{
  return lsp::dsp::base64_enc (dst, dst_left, src, src_left);
}

LSP_DSP_LIB_EXPORT
ssize_t
lsp_dsp_base64_dec (
  void *       dst,
  size_t *     dst_left,
  const void * src,
  size_t *     src_left)
{
  return lsp::dsp::base64_dec (dst, dst_left, src, src_left);
}

/* --- context --- */

LSP_DSP_LIB_EXPORT
void
lsp_dsp_init (void)
{
  lsp::dsp::init ();
}

LSP_DSP_LIB_EXPORT
lsp_dsp_info_t *
lsp_dsp_info_new (void)
{
  lsp::dsp::info_t * info = lsp::dsp::info ();
  return (lsp_dsp_info_t *) info;
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_info_free (
  lsp_dsp_info_t * info)
{
  ::free ((lsp::dsp::info_t *) info);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_start (
  lsp_dsp_context_t * c_ctx)
{
  lsp::dsp::context_t * ctx = (lsp::dsp::context_t *) c_ctx;
  lsp::dsp::start (ctx);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_finish (
  lsp_dsp_context_t * c_ctx)
{
  lsp::dsp::context_t * ctx = (lsp::dsp::context_t *) c_ctx;
  lsp::dsp::finish (ctx);
}

/* --- copy --- */

LSP_DSP_LIB_EXPORT
void
lsp_dsp_copy (
  float *       dst,
  const float * src,
  size_t        count)
{
  lsp::dsp::copy (dst, src, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_move (
  float *       dst,
  const float * src,
  size_t        count)
{
  lsp::dsp::move (dst, src, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_fill (
  float * dst,
  float   value,
  size_t  count)
{
  lsp::dsp::fill (dst, value, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_fill_zero (
  float * dst,
  size_t  count)
{
  lsp::dsp::fill_zero (dst, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_fill_one (
  float * dst,
  size_t  count)
{
  lsp::dsp::fill_one (dst, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_fill_minus_one (
  float * dst,
  size_t  count)
{
  lsp::dsp::fill_minus_one (dst, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_reverse1 (
  float * dst,
  size_t  count)
{
  lsp::dsp::reverse1 (dst, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_reverse2 (
  float *       dst,
  const float * src,
  size_t        count)
{
  lsp::dsp::reverse2 (dst, src, count);
}

/* --- mix --- */

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix2 (
  float *       dst,
  const float * src,
  float         k1,
  float         k2,
  size_t        count)
{
  lsp::dsp::mix2 (dst, src, k1, k2, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix_copy2 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  size_t        count)
{
  lsp::dsp::mix_copy2 (dst, src1, src2, k1, k2, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix_add2 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  size_t        count)
{
  lsp::dsp::mix_add2 (dst, src1, src2, k1, k2, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix3 (
  float *       dst,
  const float * src1,
  const float * src2,
  float         k1,
  float         k2,
  float         k3,
  size_t        count)
{
  lsp::dsp::mix3 (dst, src1, src2, k1, k2, k3, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix_copy3 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  float         k1,
  float         k2,
  float         k3,
  size_t        count)
{
  lsp::dsp::mix_copy3 (
    dst, src1, src2, src3, k1, k2, k3, count);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_mix_add3 (
  float *       dst,
  const float * src1,
  const float * src2,
  const float * src3,
  float         k1,
  float         k2,
  float         k3,
  size_t        count)
{
  lsp::dsp::mix_add3 (
    dst, src1, src2, src3, k1, k2, k3, count);
}

LSP_DSP_LIB_EXPORT
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
  size_t        count)
{
  lsp::dsp::mix4 (
    dst, src1, src2, src3, k1, k2, k3, k4, count);
}

LSP_DSP_LIB_EXPORT
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
  size_t        count)
{
  lsp::dsp::mix_copy4 (
    dst, src1, src2, src3, src4, k1, k2, k3, k4, count);
}

LSP_DSP_LIB_EXPORT
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
  size_t        count)
{
  lsp::dsp::mix_add4 (
    dst, src1, src2, src3, src4, k1, k2, k3, k4, count);
}

} /* extern "C" */
