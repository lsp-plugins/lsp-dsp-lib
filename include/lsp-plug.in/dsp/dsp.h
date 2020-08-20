/*
 * dsp.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_DSP_H_
#define LSP_PLUG_IN_DSP_DSP_H_

#include <lsp-plug.in/dsp/version.h>

#ifndef LSP_DSP_LIB_SYMBOL
    #ifdef __cplusplus
        #define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
            namespace lsp { \
                namespace dsp { \
                    LSP_DSP_LIB_CPPIMPORT ret (* name)(__VA_ARGS__); \
                } \
            }
    #else
        #define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
            LSP_DSP_LIB_CIMPORT ret (* LSP_DSP_LIB_MANGLE(name))(__VA_ARGS__);
    #endif /* __cplusplus */
#endif /* LSP_DSP_LIB_SYMBOL */

// Include all partial definitions
#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/3dmath.h>
#include <lsp-plug.in/dsp/common/coding.h>
#include <lsp-plug.in/dsp/common/complex.h>
#include <lsp-plug.in/dsp/common/context.h>
#include <lsp-plug.in/dsp/common/convolution.h>
#include <lsp-plug.in/dsp/common/copy.h>
#include <lsp-plug.in/dsp/common/fastconv.h>
#include <lsp-plug.in/dsp/common/fft.h>
#include <lsp-plug.in/dsp/common/filters.h>
#include <lsp-plug.in/dsp/common/float.h>
#include <lsp-plug.in/dsp/common/graphics.h>
#include <lsp-plug.in/dsp/common/hmath.h>
#include <lsp-plug.in/dsp/common/misc.h>
#include <lsp-plug.in/dsp/common/mix.h>
#include <lsp-plug.in/dsp/common/msmatrix.h>
#include <lsp-plug.in/dsp/common/pcomplex.h>
#include <lsp-plug.in/dsp/common/pmath.h>
#include <lsp-plug.in/dsp/common/resampling.h>
#include <lsp-plug.in/dsp/common/search.h>
#include <lsp-plug.in/dsp/common/smath.h>

#undef LSP_DSP_LIB_CXX_IFACE

#endif /* LSP_PLUG_IN_DSP_DSP_H_ */
