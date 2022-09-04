/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 окт. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_PIXELFMT_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            IF_ARCH_ARM(
                uint32_t mask;
                uint32_t t1, t2;
            );

            ARCH_ARM_ASM(
                // 64x blocks
                __ASM_EMIT("subs        %[count], #64")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
                __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
                __ASM_EMIT("vswp        d0, d2")
                __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vswp        d4, d6")
                __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vswp        d8, d10")
                __ASM_EMIT("vld4.8      {q8-q9}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vswp        d12, d14")
                __ASM_EMIT("vld4.8      {q10-q11}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
                __ASM_EMIT("vswp        d16, d18")
                __ASM_EMIT("vld4.8      {q12-q13}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q8-q9}, [%[dst]]!")
                __ASM_EMIT("vswp        d20, d22")
                __ASM_EMIT("vld4.8      {q14-q15}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q10-q11}, [%[dst]]!")
                __ASM_EMIT("vswp        d24, d26")
                __ASM_EMIT("vswp        d28, d30")
                __ASM_EMIT("vst4.8      {q12-q13}, [%[dst]]!")
                __ASM_EMIT("vst4.8      {q14-q15}, [%[dst]]!")
                __ASM_EMIT("subs        %[count], #64")
                __ASM_EMIT("bhs         1b")

                // 32x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #32")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
                __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
                __ASM_EMIT("vswp        d0, d2")
                __ASM_EMIT("vld4.8      {q4-q5}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vswp        d4, d6")
                __ASM_EMIT("vld4.8      {q6-q7}, [%[src]]!")
                __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
                __ASM_EMIT("vswp        d8, d10")
                __ASM_EMIT("vswp        d12, d14")
                __ASM_EMIT("vst4.8      {q4-q5}, [%[dst]]!")
                __ASM_EMIT("vst4.8      {q6-q7}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #32")

                // 16x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
                __ASM_EMIT("vld4.8      {q2-q3}, [%[src]]!")
                __ASM_EMIT("vswp        d0, d2")
                __ASM_EMIT("vswp        d4, d6")
                __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
                __ASM_EMIT("vst4.8      {q2-q3}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #16")

                // 8x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vld4.8      {q0-q1}, [%[src]]!") // d0 = R, d1 = G, d2 = B, d3 = A
                __ASM_EMIT("vswp        d0, d2")
                __ASM_EMIT("vst4.8      {q0-q1}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #8")

                // 4x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("vld2.8      {q0}, [%[src]]!") // d0 = RB, d1 = GA
                __ASM_EMIT("vshl.i16    d2, d0, #8")
                __ASM_EMIT("vshr.u16    d0, d0, #8")
                __ASM_EMIT("vorr        d0, d2")
                __ASM_EMIT("vst2.8      {q0}, [%[dst]]!")
                __ASM_EMIT("sub         %[count], #4")

                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         12f")
                __ASM_EMIT("mov         %[mask], #0xff")
                __ASM_EMIT("orr         %[mask], #0xff0000")           // mask = ff 00 ff 00
                __ASM_EMIT("11:")
                __ASM_EMIT("ldr         %[t1], [%[src]], #4")               // t1 = R G B A
                __ASM_EMIT("and         %[t2], %[t1], %[mask]")             // t2 = R 0 B 0
                __ASM_EMIT("and         %[t1], %[t1], %[mask], lsl #8")     // t1 = 0 G 0 A
                __ASM_EMIT("orr         %[t1], %[t1], %[t2], ror #16")      // t1 = B G R A
                __ASM_EMIT("str         %[t1], [%[dst]], #4")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         11b")

                __ASM_EMIT("12:")
                : [src] "+r" (src), [dst] "+r" (dst), [count] "+r" (count),
                  [t1] "=&r" (t1), [t2] "=&r" (t2), [mask] "=&r" (mask)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        IF_ARCH_ARM(
            static const uint32_t abgr32_to_bgrff32_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0xff000000),
                LSP_DSP_VEC8(0xff000000),
                LSP_DSP_VEC8(0xff000000),
                LSP_DSP_VEC8(0xff000000)
            };
        );

        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("vldm        %[MASK], {q8-q15}")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("blo         2f")
                // 32x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src]!, {q0-q7}")
                __ASM_EMIT("vsri.32     q8, q0, #8")
                __ASM_EMIT("vsri.32     q9, q1, #8")
                __ASM_EMIT("vsri.32     q10, q2, #8")
                __ASM_EMIT("vsri.32     q11, q3, #8")
                __ASM_EMIT("vsri.32     q12, q4, #8")
                __ASM_EMIT("vsri.32     q13, q5, #8")
                __ASM_EMIT("vsri.32     q14, q6, #8")
                __ASM_EMIT("vsri.32     q15, q7, #8")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q8-q15}")
                __ASM_EMIT("bhs         1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src]!, {q0-q3}")
                __ASM_EMIT("vsri.32     q8, q0, #8")
                __ASM_EMIT("vsri.32     q9, q1, #8")
                __ASM_EMIT("vsri.32     q10, q2, #8")
                __ASM_EMIT("vsri.32     q11, q3, #8")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q8-q11}")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src]!, {q0-q1}")
                __ASM_EMIT("vsri.32     q8, q0, #8")
                __ASM_EMIT("vsri.32     q9, q1, #8")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q8-q9}")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[src]!, {q0}")
                __ASM_EMIT("vsri.32     q8, q0, #8")
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q8}")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")
                __ASM_EMIT("vsri.32     q8, q0, #8")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d16[0]}, [%[dst]]!")
                __ASM_EMIT("bge         9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [MASK] "r" (&abgr32_to_bgrff32_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        IF_ARCH_ARM(
            static const uint32_t pabc32_set_alpha_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x00ffffff),
                LSP_DSP_VEC4(0x00ffffff)
            };
        );

        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            IF_ARCH_ARM(
                uint32_t a  = uint32_t(alpha) << 24;
            );

            ARCH_ARM_ASM
            (
                __ASM_EMIT("vld1.f32    {d16[], d17[]}, [%[a]]")            // q8   = alpha
                __ASM_EMIT("vldm        %[MASK], {q10-q11}")                // q10  = q11 = mask
                __ASM_EMIT("vmov        q9, q8")                            // q9   = alpha

                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("blo         2f")
                // 32x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[src]!, {q0-q7}")
                __ASM_EMIT("vbif        q0, q8, q10")
                __ASM_EMIT("vbif        q1, q9, q11")
                __ASM_EMIT("vbif        q2, q8, q10")
                __ASM_EMIT("vbif        q3, q9, q11")
                __ASM_EMIT("vbif        q4, q8, q10")
                __ASM_EMIT("vbif        q5, q9, q11")
                __ASM_EMIT("vbif        q6, q8, q10")
                __ASM_EMIT("vbif        q7, q9, q11")
                __ASM_EMIT("subs        %[count], #32")
                __ASM_EMIT("vstm        %[dst]!, {q0-q7}")
                __ASM_EMIT("bhs         1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #16")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[src]!, {q0-q3}")
                __ASM_EMIT("vbif        q0, q8, q10")
                __ASM_EMIT("vbif        q1, q9, q11")
                __ASM_EMIT("vbif        q2, q8, q10")
                __ASM_EMIT("vbif        q3, q9, q11")
                __ASM_EMIT("sub         %[count], #16")
                __ASM_EMIT("vstm        %[dst]!, {q0-q3}")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[src]!, {q0-q1}")
                __ASM_EMIT("vbif        q0, q8, q10")
                __ASM_EMIT("vbif        q1, q9, q11")
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q0-q1}")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("vldm        %[src]!, {q0}")
                __ASM_EMIT("vbif        q0, q8, q10")
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q0}")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[src]]!")
                __ASM_EMIT("vbif        q0, q8, q10")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d0[0]}, [%[dst]]!")
                __ASM_EMIT("bge         9b")
                // End
                __ASM_EMIT("10:")

                : [dst] "+r"(dst), [src] "+r"(src),
                  [count] "+r" (count)
                : [MASK] "r" (&pabc32_set_alpha_const[0]),
                  [a] "r" (&a)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_GRAPHICS_PIXELFMT_H_ */
