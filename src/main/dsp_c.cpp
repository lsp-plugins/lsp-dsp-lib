/*
 * dsp_c.c
 *
 *  Created on: 19 Aug 2020.
 *      Author: Alexandros Theodotou
 */

#include <cstdlib>

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/dsp/dsp_c.h>

extern "C"
{

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

#define FILL_FUNC(val) \
LSP_DSP_LIB_EXPORT \
void \
lsp_dsp_fill_##val ( \
  float * dst, \
  size_t  count) \
{ \
  lsp::dsp::fill_##val (dst, count); \
}

FILL_FUNC (zero)
FILL_FUNC (one)
FILL_FUNC (minus_one)

#undef FILL_FUNC

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

} /* extern "C" */
