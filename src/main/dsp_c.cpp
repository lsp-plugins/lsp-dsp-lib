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
lsp_dsp_lib_dsp_init (void)
{
  lsp::dsp::init ();
}

LSP_DSP_LIB_EXPORT
lsp_dsp_lib_info_t *
lsp_dsp_lib_dsp_info_new (void)
{
  lsp::dsp::info_t * info = lsp::dsp::info ();
  return (lsp_dsp_lib_info_t *) info;
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_lib_dsp_info_free (
  lsp_dsp_lib_info_t * info)
{
  ::free ((lsp::dsp::info_t *) info);
}

LSP_DSP_LIB_EXPORT
void
lsp_dsp_lib_dsp_start (
  lsp_dsp_lib_context_t * c_ctx)
{
  lsp::dsp::context_t * ctx = (lsp::dsp::context_t *) c_ctx;
  lsp::dsp::start (ctx);
}

} /* extern "C" */
