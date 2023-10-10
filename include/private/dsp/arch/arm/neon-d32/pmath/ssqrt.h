/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SSQRT_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SSQRT_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        #define SSQRT_CORE(DST, SRC, INC) \
            /* 16x blocks */ \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("blo             2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q3}") \
            __ASM_EMIT("vrsqrte.f32     q4, q0")            /* q4  = x0 */ \
            __ASM_EMIT("vrsqrte.f32     q5, q1") \
            __ASM_EMIT("vrsqrte.f32     q6, q2") \
            __ASM_EMIT("vrsqrte.f32     q7, q3") \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8  = R * x0 */ \
            __ASM_EMIT("vmul.f32        q9, q5, q1") \
            __ASM_EMIT("vmul.f32        q10, q6, q2") \
            __ASM_EMIT("vmul.f32        q11, q7, q3") \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vrsqrts.f32     q13, q9, q5") \
            __ASM_EMIT("vrsqrts.f32     q14, q10, q6") \
            __ASM_EMIT("vrsqrts.f32     q15, q11, q7") \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = x1 = x0 * (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vmul.f32        q5, q5, q13") \
            __ASM_EMIT("vmul.f32        q6, q6, q14") \
            __ASM_EMIT("vmul.f32        q7, q7, q15") \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8 = R * x1 */ \
            __ASM_EMIT("vmul.f32        q9, q5, q1") \
            __ASM_EMIT("vmul.f32        q10, q6, q2") \
            __ASM_EMIT("vmul.f32        q11, q7, q3") \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vrsqrts.f32     q13, q9, q5") \
            __ASM_EMIT("vrsqrts.f32     q14, q10, q6") \
            __ASM_EMIT("vrsqrts.f32     q15, q11, q7") \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vmul.f32        q5, q5, q13") \
            __ASM_EMIT("vmul.f32        q6, q6, q14") \
            __ASM_EMIT("vmul.f32        q7, q7, q15") \
            __ASM_EMIT("vmul.f32        q4, q0, q4")        /* q0 = R/sqrt(R) = sqrt(R) */ \
            __ASM_EMIT("vmul.f32        q5, q1, q5") \
            __ASM_EMIT("vmul.f32        q6, q2, q6") \
            __ASM_EMIT("vmul.f32        q7, q3, q7") \
            \
            __ASM_EMIT("veor            q14, q14, q14") \
            __ASM_EMIT("veor            q15, q15, q15") \
            __ASM_EMIT("vcgt.f32        q8, q0, q14") \
            __ASM_EMIT("vcgt.f32        q9, q1, q15") \
            __ASM_EMIT("vcgt.f32        q10, q2, q14") \
            __ASM_EMIT("vcgt.f32        q11, q3, q15") \
            __ASM_EMIT("vand.f32        q0, q4, q8") \
            __ASM_EMIT("vand.f32        q1, q5, q9") \
            __ASM_EMIT("vand.f32        q2, q6, q10") \
            __ASM_EMIT("vand.f32        q3, q7, q11") \
            __ASM_EMIT("subs            %[count], #16") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("bhs             1b") \
            __ASM_EMIT("2:") \
            __ASM_EMIT("veor            q14, q14, q14") \
            __ASM_EMIT("veor            q15, q15, q15") \
            /* 8x block */ \
            __ASM_EMIT("adds            %[count], #8")  \
            __ASM_EMIT("blt             4f") \
            __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0-q1}") \
            __ASM_EMIT("vrsqrte.f32     q4, q0")            /* q4  = x0 */ \
            __ASM_EMIT("vrsqrte.f32     q5, q1") \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8  = R * x0 */ \
            __ASM_EMIT("vmul.f32        q9, q5, q1") \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vrsqrts.f32     q13, q9, q5") \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = x1 = x0 * (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vmul.f32        q5, q5, q13") \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8 = R * x1 */ \
            __ASM_EMIT("vmul.f32        q9, q5, q1") \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vrsqrts.f32     q13, q9, q5") \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vmul.f32        q5, q5, q13") \
            __ASM_EMIT("vmul.f32        q4, q0, q4")        /* q0 = R/sqrt(R) = sqrt(R) */ \
            __ASM_EMIT("vmul.f32        q5, q1, q5") \
            \
            __ASM_EMIT("vcgt.f32        q8, q0, q14") \
            __ASM_EMIT("vcgt.f32        q9, q1, q15") \
            __ASM_EMIT("vand.f32        q0, q4, q8") \
            __ASM_EMIT("vand.f32        q1, q5, q9") \
            __ASM_EMIT("subs            %[count], #8") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0-q1}") \
            __ASM_EMIT("4:") \
            /* 4x block */ \
            __ASM_EMIT("adds            %[count], #4")  \
            __ASM_EMIT("blt             6f") \
            __ASM_EMIT("vldm            %[" SRC "]" INC ", {q0}") \
            __ASM_EMIT("vrsqrte.f32     q4, q0")            /* q4  = x0 */ \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8  = R * x0 */ \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = x1 = x0 * (3 - R * x0 * x0) / 2 */ \
            __ASM_EMIT("vmul.f32        q8, q4, q0")        /* q8 = R * x1 */ \
            __ASM_EMIT("vrsqrts.f32     q12, q8, q4")       /* q12 = (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vmul.f32        q4, q4, q12")       /* q4 = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2 */ \
            __ASM_EMIT("vmul.f32        q4, q0, q4")        /* q0 = R/sqrt(R) = sqrt(R) */ \
            \
            __ASM_EMIT("vcgt.f32        q8, q0, q14") \
            __ASM_EMIT("vand.f32        q0, q4, q8") \
            __ASM_EMIT("subs            %[count], #4") \
            __ASM_EMIT("vstm            %[" DST "]!, {q0}") \
            __ASM_EMIT("6:") \
            /* 1x block */ \
            __ASM_EMIT("adds            %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("blt             8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32         {d0[], d1[]}, [%[" SRC "]]" INC) \
            __ASM_EMIT("vmax.f32        q0, q0, q14") \
            __ASM_EMIT("vsqrt.f32       s0, s0") \
            __ASM_EMIT("subs            %[count], #1") \
            __ASM_EMIT("vst1.32         {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("bge             7b") \
            __ASM_EMIT("8:")

        void ssqrt1(float *dst, size_t count)
        {
            ARCH_ARM_ASM(
                SSQRT_CORE("dst", "dst", "")
                : [dst] "+r" (dst), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        void ssqrt2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM(
                SSQRT_CORE("dst", "src", "!")
                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );
        }

        #undef SSQRT_CORE

    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SSQRT_H_ */
