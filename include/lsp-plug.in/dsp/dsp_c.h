/*
 * dsp_c.h
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

typedef struct lsp_dsp_info_t
{
    const char     *arch;       /* Architecture information */
    const char     *cpu;        /* CPU information */
    const char     *model;      /* CPU model information */
    const char     *features;   /* CPU features */
} lsp_dsp_info_t;

/**
 * DSP context to store and restore machine state
 */
typedef struct lsp_dsp_context_t
{
    uint32_t        top;
    uint32_t        data[15];
} lsp_dsp_context_t;

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
