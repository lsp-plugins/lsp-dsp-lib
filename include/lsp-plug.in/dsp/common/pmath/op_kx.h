/*
 * op_kx.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_

#include <lsp-plug.in/dsp/common/types.h>

namespace lsp
{
    namespace dsp
    {
        /**
         * Add constant value to the data
         * @param dst destination
         * @param k constant value to add
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* add_k2)(float *dst, float k, size_t count);

        /**
         * Subtract constant value from the data
         * @param dst destination
         * @param k constant value to subtract
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* sub_k2)(float *dst, float k, size_t count);

        /**
         * Subtract data from constant value
         * @param dst destination
         * @param k constant value to subtract
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rsub_k2)(float *dst, float k, size_t count);

        /**
         * Divide data by constant value
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* div_k2)(float *dst, float k, size_t count);

        /**
         * Divide constant value by data
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rdiv_k2)(float *dst, float k, size_t count);

        /**
         * Compute remainder of dst/k: dst = dst - k * int(dst/k)
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* mod_k2)(float *dst, float k, size_t count);

        /**
         * Compute remainder of k/dst: dst = k - dst * int(k/dst)
         * @param dst destination/divisor
         * @param k constant value to use as divident
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rmod_k2)(float *dst, float k, size_t count);

        /** Scale: dst[i] = dst[i] * k
         *
         * @param dst destination
         * @param k multiplier
         * @param count number of elements
         */
        LSP_DSP_LIB_IMPORT
        void (* mul_k2)(float *dst, float k, size_t count);

        /**
         * Add constant value to the data
         * @param dst destination
         * @param src source
         * @param k constant value to add
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* add_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Subtract constant value from the data
         * @param dst destination
         * @param src source
         * @param k constant value to subtract
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* sub_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Subtract value data from constant value
         * @param dst destination
         * @param src source
         * @param k constant value to subtract
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rsub_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Divide data by constant value
         * @param dst destination
         * @param src source
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* div_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Divide constant value by data
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rdiv_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Compute remainder of src/k: dst = src - k * int(src/k)
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* mod_k3)(float *dst, const float *src, float k, size_t count);

        /**
         * Compute remainder of k/src: dst = k - src * int(k/src)
         * @param dst destination
         * @param k constant value to use as divisor
         * @param count number of elements of destination to modify
         */
        LSP_DSP_LIB_IMPORT
        void (* rmod_k3)(float *dst, const float *src, float k, size_t count);

        /** Scale: dst[i] = src[i] * k
         *
         * @param dst destination
         * @param src source
         * @param k multiplier
         * @param count number of elements
         */
        LSP_DSP_LIB_IMPORT
        void (* mul_k3)(float *dst, const float *src, float k, size_t count);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_ */
