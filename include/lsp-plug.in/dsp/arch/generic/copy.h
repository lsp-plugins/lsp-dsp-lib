/*
 * copy.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_COPY_H_
#define LSP_PLUG_IN_DSP_ARCH_GENERIC_COPY_H_

#ifndef LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void copy(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            while (count--)
                *(dst++)    = *(src++);
        }

        void move(float *dst, const float *src, size_t count)
        {
            if (dst == src)
                return;
            if (dst < src)
            {
                while (count--)
                    *(dst++)    = *(src++);
            }
            else
            {
                dst += count;
                src += count;
                while (count--)
                    *(--dst)    = *(--src);
            }
        }

        void fill(float *dst, float value, size_t count)
        {
            while (count--)
                *(dst++) = value;
        }

        void fill_zero(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 0.0f;
        }

        void fill_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = 1.0f;
        }

        void fill_minus_one(float *dst, size_t count)
        {
            while (count--)
                *(dst++) = -1.0f;
        }

        void reverse1(float *dst, size_t count)
        {
            float *src      = &dst[count];
            count >>= 1;
            while (count--)
            {
                float   tmp = *dst;
                *(dst++)    = *(--src);
                *src        = tmp;
            }
        }

        void reverse2(float *dst, const float *src, size_t count)
        {
            if (dst != src)
            {
                src         = &src[count];
                while (count--)
                    *(dst++)    = *(--src);
            }
            else
                reverse1(dst, count);
        }
    }
}

#endif /* LSP_PLUG_IN_DSP_ARCH_GENERIC_COPY_H_ */
