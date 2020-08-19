/*
 * version.h
 *
 *  Created on: 14 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_VERSION_H_
#define LSP_PLUG_IN_DSP_VERSION_H_

// Define version of headers
#define LSP_DSP_LIB_MAJOR           0
#define LSP_DSP_LIB_MINOR           5
#define LSP_DSP_LIB_MICRO           5

#ifdef LSP_DSP_LIB_BUILTIN
    #define LSP_DSP_LIB_CPPEXPORT
    #define LSP_DSP_LIB_CEXPORT
#else
    #define LSP_DSP_LIB_CPPEXPORT       __attribute__((visibility("default")))
    #define LSP_DSP_LIB_CEXPORT         __attribute__((visibility("default")))
#endif

#define LSP_DSP_LIB_CPPIMPORT       extern
#define LSP_DSP_LIB_CIMPORT         extern "C"

#define LSP_DSP_LIB_MANGLE(name)    lsp_dsp_ ## name

#endif /* LSP_PLUG_IN_DSP_VERSION_H_ */
