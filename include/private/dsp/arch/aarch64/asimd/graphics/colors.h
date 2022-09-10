/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 сент. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        #define FILL4_CORE \
            __ASM_EMIT("ldr         q0, [%[c4]]")       /* v0 = c0 c1 c2 c3 */ \
            __ASM_EMIT("mov         v1.16b, v0.16b")    /* v1 = c0 c1 c2 c3 */ \
            \
            /* 16x blocks */ \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("b.lo        2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x40]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x60]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x80]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xa0]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xc0]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0xe0]") \
            __ASM_EMIT("subs        %[count], %[count], #16") \
            __ASM_EMIT("add         %[dst], %[dst], #0x100") \
            __ASM_EMIT("b.hs         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x40]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x60]") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("add         %[dst], %[dst], #0x80") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], %[count], #4") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x20]") \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("add         %[dst], %[dst], #0x40") \
            /* 2x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], %[count], #2") \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]") \
            __ASM_EMIT("sub         %[count], %[count], #2") \
            __ASM_EMIT("add         %[dst], %[dst], #0x20") \
            /* 1x block */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("adds        %[count], %[count], #1") \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("str         q0, [%[dst], 0x00]") \
            __ASM_EMIT("10:")

        void fill_rgba(float *dst, float r, float g, float b, float a, size_t count)
        {
            IF_ARCH_AARCH64(
                float c4[4] __lsp_aligned16;
                c4[0] = r;
                c4[1] = g;
                c4[2] = b;
                c4[3] = a;
            )

            ARCH_AARCH64_ASM
            (
                FILL4_CORE
                : [dst] "+r" (dst), [count] "+r" (count)
                : [c4] "r" (&c4[0])
                : "cc", "memory",
                  "v0", "v1"
            );
        }

        void fill_hsla(float *dst, float h, float s, float l, float a, size_t count)
        {
            IF_ARCH_AARCH64(
                float c4[4] __lsp_aligned16;
                c4[0] = h;
                c4[1] = s;
                c4[2] = l;
                c4[3] = a;
            );

            ARCH_AARCH64_ASM
            (
                FILL4_CORE
                : [dst] "+r" (dst), [count] "+r" (count)
                : [c4] "r" (&c4[0])
                : "cc", "memory",
                  "v0", "v1"
            );
        }

        #undef FILL4_CORE
    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_COLORS_H_ */
