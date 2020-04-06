/*
 * fmop_kx.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Calculate dst[i] = dst[i] + src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmadd_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] - src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmsub_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = src[i] * k - dst[i]
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmrsub_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] * src[i] * k
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmmul_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = dst[i] / (src[i] * k)
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmdiv_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = (src[i] * k) / dst[i]
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmrdiv_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate remainder: dst[i] = dst[i] - (src[i]*k)*int(dst[i]/(src[i]*k))
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmmod_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate reverse remainder: dst[i] = src[i] - (dst[i]*k)*int(src[i]/(dst[i]*k))
         *
         * @param dst destination array
         * @param src source array
         * @param k multiplier
         * @param count number of elements
         */
        extern void (* fmrmod_k3)(float *dst, const float *src, float k, size_t count);

        /** Calculate dst[i] = src1[i] + src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmadd_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] - src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmsub_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src2[i] * k - src1[i]
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmrsub_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] * src2[i] * k
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmmul_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = src1[i] / (src2[i] * k)
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmdiv_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate dst[i] = (src2[i] * k) / src1[i]
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmrdiv_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate remainder: dst[i] = src1[i] - (src2[i]*k)*int(src1[i]/(src2[i]*k))
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmmod_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);

        /** Calculate reverse remainder: dst[i] = src2[i]*k - src1[i]*int((src2[i]*k)/src1[i])
         *
         * @param dst destination array
         * @param src1 source array 1
         * @param src2 source array 2
         * @param k multiplier for elements of array 2
         * @param count number of elements
         */
        extern void (* fmrmod_k4)(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_ */
