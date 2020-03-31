/*
 * pmath.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void abs_normalized(float *dst, const float *src, size_t count)
        {
            // Calculate absolute values
            dsp::abs2(dst, src, count);

            // Find the maximum value
            float max = dsp::max(dst, count);

            // Divide if it is possible
            if (max != 0.0f)
                dsp::mul_k2(dst, 1.0f / max, count);
        }

        void normalize(float *dst, const float *src, size_t count)
        {
            // Find minimum and maximum
            float max = 0.0f, min = 0.0f;
            dsp::minmax(src, count, &min, &max);

            // Determine maximum possible value
            max = (max > 0.0f) ? max : -max;
            min = (min > 0.0f) ? min : -min;
            if (max < min)
                max = min;

            // Normalize OR copy
            if (max > 0.0f)
                dsp::mul_k3(dst, src, 1.0f / max, count);
            else
                dsp::copy(dst, src, count);
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_H_ */
