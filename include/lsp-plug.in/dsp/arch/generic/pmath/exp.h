/*
 * exp.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_EXP_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_EXP_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void exp1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = ::expf(dst[i]);
        }

        void exp2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = ::expf(src[i]);
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_PMATH_EXP_H_ */
