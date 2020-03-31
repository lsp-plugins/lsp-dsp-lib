/*
 * pow.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_POW_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_POW_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /**
         * Compute v[i] = c ^ v[i], the value to be raised should be non-negative
         * @param v power array
         * @param c value to be raised
         * @param count number of elements in array
         */
        extern void (* powcv1)(float *v, float c, size_t count);

        /**
         * Compute dst[i] = c ^ v[i], the value to be raised should be non-negative
         * @param dst output array
         * @param v power array
         * @param c value to be raised
         * @param count number of elements in array
         */
        extern void (* powcv2)(float *dst, const float *v, float c, size_t count);

        /**
         * Compute v[i] = v[i] ^ c, the value to be raised should be non-negative
         * @param v values to be raised
         * @param c power value
         * @param count number of elements in array
         */
        extern void (* powvc1)(float *c, float v, size_t count);

        /**
         * Compute dst[i] = v[i] ^ c, the value to be raised should be non-negative
         * @param dst output array
         * @param v values to be raised
         * @param c power value
         * @param count number of elements in array
         */
        extern void (* powvc2)(float *dst, const float *c, float v, size_t count);

        /**
         * Compute v[i] = v[i] ^ x[i], the value to be raised should be non-negative
         * @param v values to be raised
         * @param x power values
         * @param count number of elements in array
         */
        extern void (* powvx1)(float *v, const float *x, size_t count);

        /**
         * Compute dst[i] = v[i] ^ x[i], the value to be raised should be non-negative
         * @param dst output array
         * @param v values to be raised
         * @param x power values
         * @param count number of elements in array
         */
        extern void (* powvx2)(float *dst, const float *v, const float *x, size_t count);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_POW_H_ */
