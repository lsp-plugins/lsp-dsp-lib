/*
 * interpolation.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void smooth_cubic_linear(float *dst, float start, float stop, size_t count)
        {
            float dy = stop - start;
            float nx = 1.0f / (count + 1); // Normalizing x

            for (size_t i=0; i<count; ++i)
            {
                float x = i * nx;
                *(dst++) = start + dy * x*x * (3.0f - 2.0f * x);
            }
        }

        void smooth_cubic_log(float *dst, float start, float stop, size_t count)
        {
            float dy = logf(stop/start);
            float nx = 1.0f / (count + 1); // Normalizing x

            for (size_t i=0; i<count; ++i)
            {
                float x = i * nx;
                *(dst++) = start * expf(dy * x*x * (3.0f - 2.0f * x));
            }
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_ */
