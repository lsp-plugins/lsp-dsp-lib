/*
 * hmath.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_DSP_ARCH_GENERIC_HMATH_H_
#define PRIVATE_DSP_ARCH_GENERIC_HMATH_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float scalar_mul(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += *(a++) * *(b++);
            return result;
        }

        float h_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            while (count--)
                result         += *(src++);
            return result;
        }

        float h_sqr_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            while (count--)
            {
                float tmp       = *(src++);
                result         += tmp * tmp;
            }
            return result;
        }

        float h_abs_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            while (count--)
            {
                float tmp       = *(src++);
                if (tmp < 0.0f)
                    result         -= tmp;
                else
                    result         += tmp;
            }
            return result;
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_GENERIC_HMATH_H_ */
