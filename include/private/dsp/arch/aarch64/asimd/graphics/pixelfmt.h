/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 11 окт. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_PIXELFMT_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t rgba32_to_bgra32_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x00ff00ff),
                LSP_DSP_VEC4(0x00ff00ff),
            };
        );

        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ldp         q16, q17, [%[XC]]")

                // 32x blocks
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src], 0x00]")    // v0   = R G B A
                __ASM_EMIT("ldp         q2, q3, [%[src], 0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src], 0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src], 0x60]")
                __ASM_EMIT("rev32       v8.8h, v0.8h")              // v8   = B A R G
                __ASM_EMIT("rev32       v9.8h, v1.8h")
                __ASM_EMIT("rev32       v10.8h, v2.8h")
                __ASM_EMIT("rev32       v11.8h, v3.8h")
                __ASM_EMIT("rev32       v12.8h, v4.8h")
                __ASM_EMIT("rev32       v13.8h, v5.8h")
                __ASM_EMIT("rev32       v14.8h, v6.8h")
                __ASM_EMIT("rev32       v15.8h, v7.8h")
                __ASM_EMIT("bit         v0.16b, v8.16b, v16.16b")   // v0   = B G R A
                __ASM_EMIT("bit         v1.16b, v9.16b, v17.16b")
                __ASM_EMIT("bit         v2.16b, v10.16b, v16.16b")
                __ASM_EMIT("bit         v3.16b, v11.16b, v17.16b")
                __ASM_EMIT("bit         v4.16b, v12.16b, v16.16b")
                __ASM_EMIT("bit         v5.16b, v13.16b, v17.16b")
                __ASM_EMIT("bit         v6.16b, v14.16b, v16.16b")
                __ASM_EMIT("bit         v7.16b, v15.16b, v17.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], 0x20]")
                __ASM_EMIT("stp         q4, q5, [%[dst], 0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], 0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[src], %[src], 0x80")
                __ASM_EMIT("add         %[dst], %[dst], 0x80")
                __ASM_EMIT("b.hs        1b")

                // 16x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src], 0x00]")    // v0   = R G B A
                __ASM_EMIT("ldp         q2, q3, [%[src], 0x20]")
                __ASM_EMIT("rev32       v8.8h, v0.8h")              // v8   = B A R G
                __ASM_EMIT("rev32       v9.8h, v1.8h")
                __ASM_EMIT("rev32       v10.8h, v2.8h")
                __ASM_EMIT("rev32       v11.8h, v3.8h")
                __ASM_EMIT("bit         v0.16b, v8.16b, v16.16b")   // v0   = B G R A
                __ASM_EMIT("bit         v1.16b, v9.16b, v17.16b")
                __ASM_EMIT("bit         v2.16b, v10.16b, v16.16b")
                __ASM_EMIT("bit         v3.16b, v11.16b, v17.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], 0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[src], %[src], 0x40")
                __ASM_EMIT("add         %[dst], %[dst], 0x40")

                // 8x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src], 0x00]")    // v0   = R G B A
                __ASM_EMIT("rev32       v8.8h, v0.8h")              // v8   = B A R G
                __ASM_EMIT("rev32       v9.8h, v1.8h")
                __ASM_EMIT("bit         v0.16b, v8.16b, v16.16b")   // v0   = B G R A
                __ASM_EMIT("bit         v1.16b, v9.16b, v17.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst], 0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[src], %[src], 0x20")
                __ASM_EMIT("add         %[dst], %[dst], 0x20")

                // 4x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src], 0x00]")        // v0   = R G B A
                __ASM_EMIT("rev32       v8.8h, v0.8h")              // v8   = B A R G
                __ASM_EMIT("bit         v0.16b, v8.16b, v16.16b")   // v0   = B G R A
                __ASM_EMIT("str         q0, [%[dst], 0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[src], %[src], 0x10")
                __ASM_EMIT("add         %[dst], %[dst], 0x10")

                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")         // v0   = R G B A
                __ASM_EMIT("rev32       v8.8h, v0.8h")              // v8   = B A R G
                __ASM_EMIT("bit         v0.16b, v8.16b, v16.16b")   // v0   = B G R A
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add         %[src], %[src], 0x04")
                __ASM_EMIT("add         %[dst], %[dst], 0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("b.ge        9b")

                // End
                __ASM_EMIT("10:")
                : [src] "+r" (src), [dst] "+r" (dst),
                  [count] "+r" (count)
                : [XC] "r" (&rgba32_to_bgra32_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17"
            );
        }

        IF_ARCH_AARCH64(
            static const uint32_t abgr32_to_bgrff32_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0xff000000)
            };
        );

        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldp         q16, q17, [%[MASK]]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ldp         q18, q19, [%[MASK]]")
                __ASM_EMIT("b.lo        2f")
                // 32x blocks
                __ASM_EMIT("ldp         q20, q21, [%[MASK]]")
                __ASM_EMIT("ldp         q22, q23, [%[MASK]]")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[src], #0x60]")
                __ASM_EMIT("sri         v16.4s, v0.4s, #8")
                __ASM_EMIT("sri         v17.4s, v1.4s, #8")
                __ASM_EMIT("sri         v18.4s, v2.4s, #8")
                __ASM_EMIT("sri         v19.4s, v3.4s, #8")
                __ASM_EMIT("sri         v20.4s, v4.4s, #8")
                __ASM_EMIT("sri         v21.4s, v5.4s, #8")
                __ASM_EMIT("sri         v22.4s, v6.4s, #8")
                __ASM_EMIT("sri         v23.4s, v7.4s, #8")
                __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
                __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
                __ASM_EMIT("stp         q20, q21, [%[dst], #0x40]")
                __ASM_EMIT("stp         q22, q23, [%[dst], #0x60]")
                __ASM_EMIT("add         %[src], %[src], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs        1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("sri         v16.4s, v0.4s, #8")
                __ASM_EMIT("sri         v17.4s, v1.4s, #8")
                __ASM_EMIT("sri         v18.4s, v2.4s, #8")
                __ASM_EMIT("sri         v19.4s, v3.4s, #8")
                __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
                __ASM_EMIT("stp         q18, q19, [%[dst], #0x20]")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("sri         v16.4s, v0.4s, #8")
                __ASM_EMIT("sri         v17.4s, v1.4s, #8")
                __ASM_EMIT("stp         q16, q17, [%[dst], #0x00]")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                __ASM_EMIT("sub         %[count], %[count], #8")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src], #0x00]")
                __ASM_EMIT("sri         v16.4s, v0.4s, #8")
                __ASM_EMIT("str         q16, [%[dst], #0x00]")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                __ASM_EMIT("sub         %[count], %[count], #4")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")
                __ASM_EMIT("sri         v16.4s, v0.4s, #8")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v16.s}[0], [%[dst]]")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [MASK] "r" (&abgr32_to_bgrff32_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        IF_ARCH_AARCH64(
            static const uint32_t pabc32_set_alpha_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x00ffffff),
                LSP_DSP_VEC4(0x00ffffff)
            };
        );

        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            IF_ARCH_AARCH64(
                uint32_t a  = uint32_t(alpha) << 24;
            );

            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld1r        {v16.4s}, [%[a]]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ldp         q18, q19, [%[MASK]]")
                __ASM_EMIT("mov         v17.16b, v16.16b")
                __ASM_EMIT("b.lo        2f")
                // 32x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("bif         v0.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v1.16b, v17.16b, v19.16b")
                __ASM_EMIT("ldp         q4, q5, [%[src], #0x40]")
                __ASM_EMIT("bif         v2.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v3.16b, v17.16b, v19.16b")
                __ASM_EMIT("ldp         q6, q7, [%[src], #0x60]")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("bif         v4.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v5.16b, v17.16b, v19.16b")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("bif         v6.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v7.16b, v17.16b, v19.16b")
                __ASM_EMIT("stp         q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("add         %[src], %[src], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs        1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp         q2, q3, [%[src], #0x20]")
                __ASM_EMIT("bif         v0.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v1.16b, v17.16b, v19.16b")
                __ASM_EMIT("bif         v2.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v3.16b, v17.16b, v19.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("add         %[src], %[src], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[dst], %[dst], #0x40")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldp         q0, q1, [%[src], #0x00]")
                __ASM_EMIT("bif         v0.16b, v16.16b, v18.16b")
                __ASM_EMIT("bif         v1.16b, v17.16b, v19.16b")
                __ASM_EMIT("stp         q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("add         %[src], %[src], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                __ASM_EMIT("sub         %[count], %[count], #8")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ldr         q0, [%[src], #0x00]")
                __ASM_EMIT("bif         v0.16b, v16.16b, v18.16b")
                __ASM_EMIT("str         q0, [%[dst], #0x00]")
                __ASM_EMIT("add         %[src], %[src], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                __ASM_EMIT("sub         %[count], %[count], #4")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]]")
                __ASM_EMIT("bif         v0.16b, v16.16b, v18.16b")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add         %[src], %[src], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [MASK] "r" (&pabc32_set_alpha_const[0]),
                  [a] "r" (&a)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_PIXELFMT_H_ */
