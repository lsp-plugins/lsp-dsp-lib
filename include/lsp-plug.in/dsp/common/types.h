/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_TYPES_H_
#define LSP_PLUG_IN_DSP_COMMON_TYPES_H_

#include <lsp-plug.in/dsp/version.h>

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// Macro definitions
#define LSP_DSP_VEC2(v)                     v, v
#define LSP_DSP_VEC4(v)                     LSP_DSP_VEC2(v), LSP_DSP_VEC2(v)
#define LSP_DSP_VEC8(v)                     LSP_DSP_VEC4(v), LSP_DSP_VEC4(v)
#define LSP_DSP_VEC16(v)                    LSP_DSP_VEC8(v), LSP_DSP_VEC8(v)
#define LSP_DSP_VEC32(v)                    LSP_DSP_VEC16(v), LSP_DSP_VEC16(v)

#define LSP_DSP_VEC2x2(a, b)                a, b, a, b
#define LSP_DSP_VEC4x2(a, b)                LSP_DSP_VEC2x2(a, b), LSP_DSP_VEC2x2(a, b)
#define LSP_DSP_VEC8x2(a, b)                LSP_DSP_VEC4x2(a, b), LSP_DSP_VEC4x2(a, b)
#define LSP_DSP_VEC16x2(a, b)               LSP_DSP_VEC8x2(a, b), LSP_DSP_VEC8x2(a, b)

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

#ifndef __lsp_forced_inline
    #define __lsp_forced_inline                 __attribute__ ((always_inline))
#endif

#ifndef __lsp_aligned16
    #define __lsp_aligned16                     __attribute__ ((aligned (16)))
#endif

#ifndef __lsp_aligned32
    #define __lsp_aligned32                     __attribute__ ((aligned (32)))
#endif

#ifndef __lsp_aligned64
    #define __lsp_aligned64                     __attribute__ ((aligned (64)))
#endif

#ifndef __lsp_packed
    #define __lsp_packed                        __attribute__ ((__packed__))
#endif

#ifndef __lsp_aligned
    #define __lsp_aligned(bytes)                __attribute__ ((aligned (bytes)))
#endif

#endif /* LSP_PLUG_IN_DSP_COMMON_TYPES_H_ */
