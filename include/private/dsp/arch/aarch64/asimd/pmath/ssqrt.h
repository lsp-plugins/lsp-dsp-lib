/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_SSQRT_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_SSQRT_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        #define SSQRT_INC(...) __VA_ARGS__
        #define SSQRT_NOINC(...)

        #define SSQRT_CORE(DST, SRC, INC) \
            /* 32x blocks */ \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            __ASM_EMIT("eor         v16.16b, v16.16b, v16.16b") \
            __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b") \
            __ASM_EMIT("b.lo        2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
            __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
            __ASM_EMIT("ldp         q4, q5, [%[" SRC "], #0x40]") \
            __ASM_EMIT("ldp         q6, q7, [%[" SRC "], #0x60]") \
            __ASM_EMIT("fmax        v0.4s, v0.4s, v16.4s") \
            __ASM_EMIT("fmax        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fmax        v2.4s, v2.4s, v16.4s") \
            __ASM_EMIT("fmax        v3.4s, v3.4s, v17.4s") \
            __ASM_EMIT("fmax        v4.4s, v4.4s, v16.4s") \
            __ASM_EMIT("fmax        v5.4s, v5.4s, v17.4s") \
            __ASM_EMIT("fmax        v6.4s, v6.4s, v16.4s") \
            __ASM_EMIT("fmax        v7.4s, v7.4s, v17.4s") \
            __ASM_EMIT("fsqrt       v0.4s, v0.4s") \
            __ASM_EMIT("fsqrt       v1.4s, v1.4s") \
            __ASM_EMIT("fsqrt       v2.4s, v2.4s") \
            __ASM_EMIT("fsqrt       v3.4s, v3.4s") \
            __ASM_EMIT("fsqrt       v4.4s, v4.4s") \
            __ASM_EMIT("fsqrt       v5.4s, v5.4s") \
            __ASM_EMIT("fsqrt       v6.4s, v6.4s") \
            __ASM_EMIT("fsqrt       v7.4s, v7.4s") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
            __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
            INC(__ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x80")) \
            __ASM_EMIT("b.hs        1b") \
            __ASM_EMIT("2:") \
            /* 16x block */ \
            __ASM_EMIT("adds        %[count], %[count], #16") /* 32 - 16 */ \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
            __ASM_EMIT("ldp         q2, q3, [%[" SRC "], #0x20]") \
            __ASM_EMIT("fmax        v0.4s, v0.4s, v16.4s") \
            __ASM_EMIT("fmax        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fmax        v2.4s, v2.4s, v16.4s") \
            __ASM_EMIT("fmax        v3.4s, v3.4s, v17.4s") \
            __ASM_EMIT("fsqrt       v0.4s, v0.4s") \
            __ASM_EMIT("fsqrt       v1.4s, v1.4s") \
            __ASM_EMIT("fsqrt       v2.4s, v2.4s") \
            __ASM_EMIT("fsqrt       v3.4s, v3.4s") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT("sub         %[count], %[count], #16") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
            INC(__ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x40")) \
            __ASM_EMIT("4:") \
            /* 8x block */ \
            __ASM_EMIT("adds        %[count], %[count], #8") /* 16 - 8 */ \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("ldp         q0, q1, [%[" SRC "], #0x00]") \
            __ASM_EMIT("fmax        v0.4s, v0.4s, v16.4s") \
            __ASM_EMIT("fmax        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsqrt       v0.4s, v0.4s") \
            __ASM_EMIT("fsqrt       v1.4s, v1.4s") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
            INC(__ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x20")) \
            __ASM_EMIT("6:") \
            /* 4x block */ \
            __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("ldr         q0, [%[" SRC "], #0x00]") \
            __ASM_EMIT("fmax        v0.4s, v0.4s, v16.4s") \
            __ASM_EMIT("fsqrt       v0.4s, v0.4s") \
            __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
            INC(__ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x10")) \
            __ASM_EMIT("8:") \
            /* 1x block */ \
            __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("ld1r        {v0.4s}, [%[" SRC "]]") \
            __ASM_EMIT("fmax        v0.4s, v0.4s, v16.4s") \
            __ASM_EMIT("fsqrt       v0.4s, v0.4s") \
            __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT("subs        %[count], %[count], #1") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
            INC(__ASM_EMIT("add         %[" SRC "], %[" SRC "], #0x04")) \
            __ASM_EMIT("bge         9b") \
            __ASM_EMIT("10:")

        void ssqrt1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM(
                SSQRT_CORE("dst", "dst", SSQRT_NOINC)
                : [dst] "+r" (dst),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17"
            );
        }

        void ssqrt2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                SSQRT_CORE("dst", "src", SSQRT_INC)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17"
            );
        }

        #undef SSQRT_CORE
        #undef SSQRT_NOINC
        #undef SSQRT_INC

    } /* namespace asimd */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_SSQRT_H_ */
