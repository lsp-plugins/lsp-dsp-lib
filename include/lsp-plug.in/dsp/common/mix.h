/*
 * mix.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_MIX_H_
#define LSP_PLUG_IN_DSP_COMMON_MIX_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Calculate dst[i] = dst[i] * k1 + src[i] * k2
         *
         */
        extern void (* mix2)(float *dst, const float *src, float k1, float k2, size_t count);

        /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2
         *
         */
        extern void (* mix_copy2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

        /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2
         *
         */
        extern void (* mix_add2)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);

        /** Calculate dst[i] = dst[i] * k1 + src1[i] * k2 + src2[i] * k3
         *
         */
        extern void (* mix3)(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);

        /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2 + src3 * k3
         *
         */
        extern void (* mix_copy3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);

        /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2 + src3 * k3
         *
         */
        extern void (* mix_add3)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);

        /** Calculate dst[i] = dst[i] * k1 + src1[i] * k2 + src2[i] * k3 + src3[i] * k4
         *
         */
        extern void (* mix4)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        /** Calculate dst[i] = src1[i] * k1 + src2[i] * k2 + src3 * k3 + src4 * k4
         *
         */
        extern void (* mix_copy4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        /** Calculate dst[i] = dst[i] + src1[i] * k1 + src2[i] * k2 + src3 * k3 + src4 * k4
         *
         */
        extern void (* mix_add4)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_MIX_H_ */
