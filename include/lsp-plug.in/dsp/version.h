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
#define LSP_DSP_LIB_MICRO           4

#ifdef LSP_DSP_LIB_BUILTIN
    #define LSP_DSP_LIB_EXPORT
    #define LSP_DSP_LIB_CEXPORT
    #define LSP_DSP_LIB_IMPORT          LSP_SYMBOL_IMPORT
    #define LSP_DSP_LIB_CIMPORT         LSP_CSYMBOL_IMPORT
#else
    #define LSP_DSP_LIB_EXPORT          LSP_SYMBOL_EXPORT
    #define LSP_DSP_LIB_CEXPORT         LSP_CSYMBOL_EXPORT
    #define LSP_DSP_LIB_IMPORT          LSP_SYMBOL_IMPORT
    #define LSP_DSP_LIB_CIMPORT         LSP_CSYMBOL_IMPORT
#endif

#endif /* LSP_PLUG_IN_DSP_VERSION_H_ */
