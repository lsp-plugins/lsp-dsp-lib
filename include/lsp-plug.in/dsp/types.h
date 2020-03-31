/*
 * types.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_VERSION_H_
#define LSP_PLUG_IN_DSP_VERSION_H_

#include <lsp-plug.in/common/types.h>

// Define version of headers
#define LSP_DSP_LIB_MAJOR           1
#define LSP_DSP_LIB_MINOR           0
#define LSP_DSP_LIB_MICRO           0

// Macro definitions
#define LSP_DSP_VEC2(v)     v, v
#define LSP_DSP_VEC4(v)     v, v, v, v
#define LSP_DSP_VEC8(v)     v, v, v, v, v, v, v, v
#define LSP_DSP_VEC16(v)    v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v
#define LSP_DSP_VEC32(v)    v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v

// Different constants
#define LSP_DSP_AMPLIFICATION_THRESH        1e-8

// Float saturation limits
#define LSP_DSP_FLOAT_SAT_P_NAN             0.0f
#define LSP_DSP_FLOAT_SAT_N_NAN             0.0f
#define LSP_DSP_FLOAT_SAT_P_INF             1e+10f
#define LSP_DSP_FLOAT_SAT_N_INF             -1e+10f
#define LSP_DSP_FLOAT_SAT_P_NAN_I           0
#define LSP_DSP_FLOAT_SAT_N_NAN_I           0
#define LSP_DSP_FLOAT_SAT_P_INF_I           0x501502f9
#define LSP_DSP_FLOAT_SAT_N_INF_I           0xd01502f9

#endif /* LSP_PLUG_IN_DSP_VERSION_H_ */
