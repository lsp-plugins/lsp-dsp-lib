/*
 * hdotp.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_HMATH_HDOTP_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_HMATH_HDOTP_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float h_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += *(a++) * *(b++);
            return result;
        }

        float h_abs_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
                result += ::fabs(*(a++)) * ::fabs(*(b++));
            return result;
        }

        float h_sqr_dotp(const float *a, const float *b, size_t count)
        {
            float result = 0;
            while (count--)
            {
                float xa = *a++, xb = *b++;
                result += xa*xa * xb*xb;
            }
            return result;
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_HMATH_HDOTP_H_ */
