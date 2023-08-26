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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_TYPES_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_TYPES_H_

#include <lsp-plug.in/dsp/common/types.h>

/*
  DIGITAL FILTER BASICS

     Analog filter cascade:
                t0 + t1*s + t2*s^2
       H[s] = ──────────────────────
                b0 + b1*s + b2*s^2

     Normalized biquad filter:
               b0 + b1*z^-1 + b2*z^-2
       h[z] = ────────────────────────
               1 - a1*z^-1 - a2*z^-2

     The structure of biquad filter:

       s  = input sample
       s' = output sample
       d0, d1 = filter memory

        s   ┌─────┐     ┌───┐                s'
       ──┬─►│x b0 │────►│ + │──────────────┬────►
         │  └─────┘     └───┘              │
         │                ▲                │
         │                │                │
         │             ┌─────┐             │
         │             │  d0 │             │
         │             └─────┘             │
         │                ▲                │
         │                │                │
         │  ┌─────┐     ┌───┐     ┌─────┐  │
         ├─►│x b1 │────►│ + │◄────│x a1 │◄─┤
         │  └─────┘     └───┘     └─────┘  │
         │                ▲                │
         │                │                │
         │             ┌─────┐             │
         │             │  d1 │             │
         │             └─────┘             │
         │                ▲                │
         │                │                │
         │  ┌─────┐     ┌───┐     ┌─────┐  │
         └─►│x b2 │────►│ + │◄────│x a2 │◄─┘
            └─────┘     └───┘     └─────┘

    Static filters consist of biquad filter banks organized in optimal
    for SIMD processing mode. For example, x4 static filter bank will contain
    four biquad cascades:
        ┌───────┬───────┬───────┬───────┐
        │  f[0] │  f[1] │  f[2] │  f[3] │
        └───────┴───────┴───────┴───────┘

    Each biquad filter cascade contains:
        - top coefficients of equation a0..a2;
        - bottom coefficients of equation b0..b1.

    For optimal SIMD processing each coefficient should be packed into distinct
    SIMD register, so here's the data structure of the optimized x4 biquad filter bank:

      Index      +0x00     +0x01     +0x02     +0x03
              ┌─────────┬─────────┬─────────┬─────────┐
       +0x00  │ f[0].b0 │ f[1].b0 │ f[2].b0 │ f[3].b0 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x04  │ f[0].b1 │ f[1].b1 │ f[2].b1 │ f[3].b1 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x08  │ f[0].b2 │ f[1].b2 │ f[2].b2 │ f[3].b2 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x0c  │ f[0].a0 │ f[1].a0 │ f[2].a0 │ f[3].a0 │
              ├─────────┼─────────┼─────────┼─────────┤
       +0x10  │ f[0].a1 │ f[1].a1 │ f[2].a1 │ f[3].a1 │
              └─────────┴─────────┴─────────┴─────────┘

    Dynamic filters consist of shifted biquad filter banks. For example,
    bank of x4 filters for 5 input samples looks like:

      Index     +0x00   +0x01   +0x02   +0x03
              ┌───────┬───────┬───────┬───────┐
       +0x00  │ f[0:0]│   1   │   1   │   1   │
              ├───────┼───────┼───────┼───────┤
       +0x04  │ f[0:1]│ f[1:0]│   1   │   1   │
              ├───────┼───────┼───────┼───────┤
       +0x08  │ f[0:2]│ f[1:1]│ f[2:0]│   1   │
              ├───────┼───────┼───────┼───────┤
       +0x0c  │ f[0:3]│ f[1:2]│ f[2:1]│ f[3:0]│
              ├───────┼───────┼───────┼───────┤
       +0x10  │ f[0:4]│ f[1:3]│ f[2:2]│ f[3:1]│
              ├───────┼───────┼───────┼───────┤
       +0x14  │   1   │ f[1:4]│ f[2:3]│ f[3:2]│
              ├───────┼───────┼───────┼───────┤
       +0x18  │   1   │   1   │ f[2:4]│ f[3:3]│
              ├───────┼───────┼───────┼───────┤
       +0x1c  │   1   │   1   │   1   │ f[3:4]│
              └───────┴───────┴───────┴───────┘

       Each cell is one biquad filter f[i:j] where:
         - i is number of the filter cascade
         - j is the number of filter state for the j'th sample
         - 1 is the cascade that does nothing with input signal

   Then each row can be combined into single x4 filter bank same as for
   static filters and applied to the input sample in a pipeline mode.
 */

/*
  BILINEAR TRANSFORMATION

    Original filter chain:

               t[0] + t[1] * p + t[2] * p^2
      H(p) =  ------------------------------
               b[0] + b[1] * p + b[2] * p^2

    Bilinear transform:

      x    = z^-1

      kf   = 1 / tan(pi * frequency / sample_rate) - frequency shift factor

                   1 - x
      p    = kf * -------   - analog -> digital bilinear transform expression
                   1 + x

    Applied bilinear transform:

               (t[0] + t[1]*kf + t[2]*kf^2) + 2*(t[0] - t[2]*kf^2)*x + (t[0] - t[1]*kf + t[2]*kf^2)*x^2
      H[x] =  -----------------------------------------------------------------------------------------
               (b[0] + b[1]*kf + b[2]*kf^2) + 2*(b[0] - b[2]*kf^2)*x + (b[0] - b[1]*kf + b[2]*kf^2)*x^2

    Finally:

      T    =   { t[0], t[1]*kf, t[2]*kf*kf }
      B    =   { b[0], b[1]*kf, b[2]*kf*kf }

               (T[0] + T[1] + T[2]) + 2*(T[0] - T[2])*z^-1 + (T[0] - T[1] + T[2])*z^-2
      H[z] =  -------------------------------------------------------------------------
               (B[0] + B[1] + B[2]) + 2*(B[0] - B[2])*z^-1 + (B[0] - B[1] + B[2])*z^-2
 */

/*
  MATCHED Z TRANSFORMATION

    Original filter chain:

               t[0] + t[1] * p + t[2] * p^2     k1 * (p + a[0]) * (p + a[1])
      H(p) =  ------------------------------ = -----------------------------
               b[0] + b[1] * p + b[2] * p^2     k2 * (p + b[0]) * (p + b[1])

      a[0], a[1], b[0], b[1] may not exist, so there are series of solutions

    Matched Z-transform:

      T    = discretization period

      x    = z^-1

      p + a = 1 - x*exp(-a * T)

      kf   = 1 / f, f = filter frequency

    After the Matched Z-transform the Frequency Response of the filter has to be normalized!

*/

/**
 * These constants define the offset of filter constants relative to the memory in biquad_t structure,
 * filter alignment and maximum number of memory elements
 */
#define LSP_DSP_BIQUAD_XN_OFF           0x40
#define LSP_DSP_BIQUAD_XN_SOFF          "0x40"
#define LSP_DSP_BIQUAD_ALIGN            0x40
#define LSP_DSP_BIQUAD_D_ITEMS          16

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)

/**
 * Analog filter cascade transfer function:
 *
 *              t0 + t1*s + t2*s^2
 *     H[s] = ──────────────────────
 *              b0 + b1*s + b2*s^2
 */
typedef struct LSP_DSP_LIB_TYPE(f_cascade_t)
{
    float       t[4];       // Top part of polynom (zeros): T[p] = t[0] + t[1] * p + t[2] * p^2
    float       b[4];       // Bottom part of polynom (poles): B[p] = b[0] + b[1] * p + b[2] * p^2
} LSP_DSP_LIB_TYPE(f_cascade_t);

/**
 * Biquad filter bank for 1 digital biquad filter
 * Non-used elements should be filled with zeros
 */
typedef struct LSP_DSP_LIB_TYPE(biquad_x1_t)
{
    float   b0, b1, b2;     //  b0 b1 b2
    float   a1, a2;         //  a1 a2
    float   p0, p1, p2;     //  padding (not used), SHOULD be zero
} LSP_DSP_LIB_TYPE(biquad_x1_t);

/**
 * Biquad filter bank for 2 digital biquad filters
 * Non-used elements should be filled with zeros
 */
typedef struct LSP_DSP_LIB_TYPE(biquad_x2_t)
{
    float   b0[2];
    float   b1[2];
    float   b2[2];
    float   a1[2];
    float   a2[2];
    float   p[2];           // padding (not used), SHOULD be zero
} LSP_DSP_LIB_TYPE(biquad_x2_t);

/**
 * Biquad filter bank for 4 digital biquad filters
 */
typedef struct LSP_DSP_LIB_TYPE(biquad_x4_t)
{
    float   b0[4];
    float   b1[4];
    float   b2[4];
    float   a1[4];
    float   a2[4];
} LSP_DSP_LIB_TYPE(biquad_x4_t);

/**
 * Biquad filter bank for 8 digital biquad filters
 */
typedef struct LSP_DSP_LIB_TYPE(biquad_x8_t)
{
    float   b0[8];
    float   b1[8];
    float   b2[8];
    float   a1[8];
    float   a2[8];
} LSP_DSP_LIB_TYPE(biquad_x8_t);

/**
 * This is main filter structure with memory elements
 * It should be aligned at least to 16-byte boundary due to
 * alignment restrictions of some different hardware architectures
 * For best purpose it should be aligned to 64-byte boundary
 */
typedef struct LSP_DSP_LIB_TYPE(biquad_t)
{
    float   d[LSP_DSP_BIQUAD_D_ITEMS];
    union
    {
        LSP_DSP_LIB_TYPE(biquad_x1_t) x1;
        LSP_DSP_LIB_TYPE(biquad_x2_t) x2;
        LSP_DSP_LIB_TYPE(biquad_x4_t) x4;
        LSP_DSP_LIB_TYPE(biquad_x8_t) x8;
    };
    float   __pad[8];
} __lsp_aligned(LSP_DSP_BIQUAD_ALIGN) LSP_DSP_LIB_TYPE(biquad_t);

#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_TYPES_H_ */
