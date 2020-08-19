/*
 * dsp_c.c
 *
 *  Created on: 19 Aug 2020.
 *      Author: Alexandros Theodotou
 */

#include <lsp-plug.in/dsp/dsp.h>

extern "C"
{

LSP_DSP_LIB_EXPORT
void
lsp_dsp_lib_dsp_init (void)
{
  lsp::dsp::init ();
}

} /* extern "C" */
