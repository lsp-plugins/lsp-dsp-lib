/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 4 авг. 2026 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_FILL_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_FILL_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {

        static const dsp::f_cascade_t fcascade_normal[2] __lsp_aligned64 =
        {
            {
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f, 0.0f },
            },
            {
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f, 0.0f },
            }
        };

        #define FCASCADE_FILL_X1(base, dst) \
            __ASM_EMIT("vmovups     %%ymm0, " base " + 0x00(%[" dst "])") \

        #define FCASCADE_FILL_X2(base, dst) \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x00(%[" dst "])") \

        #define FCASCADE_FILL_X4(base, dst) \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x00(%[" dst "])") \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x40(%[" dst "])") \

        #define FCASCADE_FILL_X8(base, dst) \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x00(%[" dst "])") \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x40(%[" dst "])") \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0x80(%[" dst "])") \
            __ASM_EMIT("vmovups     %%zmm0, " base " + 0xc0(%[" dst "])")

        void fcascade_fill_x4(dsp::f_cascade_t *h, dsp::f_cascade_t *t)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovaps      %[CC], %%zmm0")         // ymm0 = F
                // +0x000: row 1
                FCASCADE_FILL_X1("0x000 + 0x20", "h")
                FCASCADE_FILL_X2("0x000 + 0x40", "h")
                // +0x080: row 2
                FCASCADE_FILL_X2("0x080 + 0x40", "h")
                // +0x100: row 3
                FCASCADE_FILL_X1("0x100 + 0x60", "h")

                // +0x000: row -1
                FCASCADE_FILL_X1("0x000 + 0x00", "t")
                // +0x080: row -2
                FCASCADE_FILL_X2("0x080 + 0x00", "t")
                // +0x100: row -3
                FCASCADE_FILL_X2("0x100 + 0x00", "t")
                FCASCADE_FILL_X1("0x100 + 0x40", "t")
                :
                : [h] "r" (h), [t] "r" (t),
                  [CC] "m" (fcascade_normal)
                : "memory",
                  "%xmm0"
            );
        }

        void fcascade_fill_x8(dsp::f_cascade_t *h, dsp::f_cascade_t *t)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovaps      %[CC], %%zmm0")         // ymm0 = F
                // +0x000: row 1
                FCASCADE_FILL_X1("0x000 + 0x20", "h")
                FCASCADE_FILL_X2("0x000 + 0x40", "h")
                FCASCADE_FILL_X4("0x000 + 0x80", "h")
                // +0x100: row 2
                FCASCADE_FILL_X2("0x100 + 0x40", "h")
                FCASCADE_FILL_X4("0x100 + 0x80", "h")
                // +0x200: row 3
                FCASCADE_FILL_X1("0x200 + 0x60", "h")
                FCASCADE_FILL_X4("0x200 + 0x80", "h")
                // +0x300: row 4
                FCASCADE_FILL_X4("0x300 + 0x80", "h")
                // +0x400: row 5
                FCASCADE_FILL_X1("0x400 + 0xa0", "h")
                FCASCADE_FILL_X2("0x400 + 0xc0", "h")
                // +0x500: row 6
                FCASCADE_FILL_X2("0x500 + 0xc0", "h")
                // +0x600: row 7
                FCASCADE_FILL_X1("0x600 + 0xe0", "h")

                // +0x000: row -1
                FCASCADE_FILL_X1("0x000 + 0x00", "t")
                // +0x100: row -2
                FCASCADE_FILL_X2("0x100 + 0x00", "t")
                // +0x200: row -3
                FCASCADE_FILL_X2("0x200 + 0x00", "t")
                FCASCADE_FILL_X1("0x200 + 0x40", "t")
                // +0x300: row -4
                FCASCADE_FILL_X4("0x300 + 0x00", "t")
                // +0x400: row -5
                FCASCADE_FILL_X4("0x400 + 0x00", "t")
                FCASCADE_FILL_X1("0x400 + 0x80", "t")
                // +0x500: row -6
                FCASCADE_FILL_X4("0x500 + 0x00", "t")
                FCASCADE_FILL_X2("0x500 + 0x80", "t")
                // +0x600: row -7
                FCASCADE_FILL_X4("0x600 + 0x00", "t")
                FCASCADE_FILL_X2("0x600 + 0x80", "t")
                FCASCADE_FILL_X1("0x600 + 0xc0", "t")
                :
                : [h] "r" (h), [t] "r" (t),
                  [CC] "m" (fcascade_normal)
                : "memory",
                  "%xmm0"
            );
        }

        void fcascade_fill_x16(dsp::f_cascade_t *h, dsp::f_cascade_t *t)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovaps      %[CC], %%zmm0")         // ymm0 = F
                // +0x0000: row 1
                FCASCADE_FILL_X1("0x0000 + 0x020", "h")
                FCASCADE_FILL_X2("0x0000 + 0x040", "h")
                FCASCADE_FILL_X4("0x0000 + 0x080", "h")
                FCASCADE_FILL_X8("0x0000 + 0x100", "h")
                // +0x0200: row 2
                FCASCADE_FILL_X2("0x0200 + 0x040", "h")
                FCASCADE_FILL_X4("0x0200 + 0x080", "h")
                FCASCADE_FILL_X8("0x0200 + 0x100", "h")
                // +0x0400: row 3
                FCASCADE_FILL_X1("0x0400 + 0x060", "h")
                FCASCADE_FILL_X4("0x0400 + 0x080", "h")
                FCASCADE_FILL_X8("0x0400 + 0x100", "h")
                // +0x0600: row 4
                FCASCADE_FILL_X4("0x0600 + 0x080", "h")
                FCASCADE_FILL_X8("0x0600 + 0x100", "h")
                // +0x0800: row 5
                FCASCADE_FILL_X1("0x0800 + 0x0a0", "h")
                FCASCADE_FILL_X2("0x0800 + 0x0c0", "h")
                FCASCADE_FILL_X8("0x0800 + 0x100", "h")
                // +0x0a00: row 6
                FCASCADE_FILL_X2("0x0a00 + 0x0c0", "h")
                FCASCADE_FILL_X8("0x0a00 + 0x100", "h")
                // +0x0c00: row 7
                FCASCADE_FILL_X1("0x0c00 + 0x0e0", "h")
                FCASCADE_FILL_X8("0x0c00 + 0x100", "h")
                // +0x0e00: row 8
                FCASCADE_FILL_X8("0x0e00 + 0x100", "h")

                // +0x1000: row 9
                FCASCADE_FILL_X1("0x1000 + 0x120", "h")
                FCASCADE_FILL_X2("0x1000 + 0x140", "h")
                FCASCADE_FILL_X4("0x1000 + 0x180", "h")
                // +0x1200: row 10
                FCASCADE_FILL_X2("0x1200 + 0x140", "h")
                FCASCADE_FILL_X4("0x1200 + 0x180", "h")
                // +0x1400: row 11
                FCASCADE_FILL_X1("0x1400 + 0x160", "h")
                FCASCADE_FILL_X4("0x1400 + 0x180", "h")
                // +0x1600: row 12
                FCASCADE_FILL_X4("0x1600 + 0x180", "h")
                // +0x1800: row 13
                FCASCADE_FILL_X1("0x1800 + 0x1a0", "h")
                FCASCADE_FILL_X2("0x1800 + 0x1c0", "h")
                // +0x1a00: row 14
                FCASCADE_FILL_X2("0x1a00 + 0x1c0", "h")
                // +0x1c00: row 15
                FCASCADE_FILL_X1("0x1c00 + 0x1e0", "h")

                // +0x0000: row -1
                FCASCADE_FILL_X1("0x0000 + 0x000", "t")
                // +0x0200: row -2
                FCASCADE_FILL_X2("0x0200 + 0x000", "t")
                // +0x0400: row -3
                FCASCADE_FILL_X2("0x0400 + 0x000", "t")
                FCASCADE_FILL_X1("0x0400 + 0x040", "t")
                // +0x0600: row -4
                FCASCADE_FILL_X4("0x0600 + 0x000", "t")
                // +0x0800: row -5
                FCASCADE_FILL_X4("0x0800 + 0x000", "t")
                FCASCADE_FILL_X1("0x0800 + 0x080", "t")
                // +0x0a00: row -6
                FCASCADE_FILL_X4("0x0a00 + 0x000", "t")
                FCASCADE_FILL_X2("0x0a00 + 0x080", "t")
                // +0x0c00: row -7
                FCASCADE_FILL_X4("0x0c00 + 0x000", "t")
                FCASCADE_FILL_X2("0x0c00 + 0x080", "t")
                FCASCADE_FILL_X1("0x0c00 + 0x0c0", "t")
                // +0x0e00: row -8
                FCASCADE_FILL_X8("0x0e00 + 0x000", "t")
                // +0x1000: row -1
                FCASCADE_FILL_X8("0x1000 + 0x000", "t")
                FCASCADE_FILL_X1("0x1000 + 0x100", "t")
                // +0x1200: row -2
                FCASCADE_FILL_X8("0x1200 + 0x000", "t")
                FCASCADE_FILL_X2("0x1200 + 0x100", "t")
                // +0x1400: row -3
                FCASCADE_FILL_X8("0x1400 + 0x000", "t")
                FCASCADE_FILL_X2("0x1400 + 0x100", "t")
                FCASCADE_FILL_X1("0x1400 + 0x140", "t")
                // +0x1600: row -4
                FCASCADE_FILL_X8("0x1600 + 0x000", "t")
                FCASCADE_FILL_X4("0x1600 + 0x100", "t")
                // +0x1800: row -5
                FCASCADE_FILL_X8("0x1800 + 0x000", "t")
                FCASCADE_FILL_X4("0x1800 + 0x100", "t")
                FCASCADE_FILL_X1("0x1800 + 0x180", "t")
                // +0x1a00: row -6
                FCASCADE_FILL_X8("0x1a00 + 0x000", "t")
                FCASCADE_FILL_X4("0x1a00 + 0x100", "t")
                FCASCADE_FILL_X2("0x1a00 + 0x180", "t")
                // +0x1c00: row -7
                FCASCADE_FILL_X8("0x1c00 + 0x000", "t")
                FCASCADE_FILL_X4("0x1c00 + 0x100", "t")
                FCASCADE_FILL_X2("0x1c00 + 0x180", "t")
                FCASCADE_FILL_X1("0x1c00 + 0x1c0", "t")
                :
                : [h] "r" (h), [t] "r" (t),
                  [CC] "m" (fcascade_normal)
                : "memory",
                  "%xmm0"
            );
        }


    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_FILL_H_ */
