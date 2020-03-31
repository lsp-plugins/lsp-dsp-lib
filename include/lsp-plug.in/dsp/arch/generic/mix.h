/*
 * mix.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_MIX_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_MIX_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void mix2(float *dst, const float *src, float k1, float k2, size_t count)
        {
            while (count--)
            {
                *dst = *(dst) * k1 + *(src++) * k2;
                dst     ++;
            }
        }

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) * k1 + *(src2++) * k2;
        }

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count)
        {
            while (count--)
                *(dst++) += *(src1++) * k1 + *(src2++) * k2;
        }

        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count)
        {
            while (count--)
            {
                *dst = *(dst) * k1 + *(src1++) * k2 + *(src2++) * k3;
                dst     ++;
            }
        }

        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3;
        }

        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count)
        {
            while (count--)
                *(dst++) += *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3;
        }

        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count)
        {
            while (count--)
            {
                *dst = *(dst) * k1 + *(src1++) * k2 + *(src2++) * k3 + *(src3++) * k4;
                dst     ++;
            }
        }

        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
        {
            while (count--)
                *(dst++) = *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3 + *(src4++) * k4;
        }

        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count)
        {
            while (count--)
                *(dst++) += *(src1++) * k1 + *(src2++) * k2 + *(src3++) * k3 + *(src4++) * k4;
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_MIX_H_ */
