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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SQR_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SQR_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        #define SQR_CORE(DST, SRC, INC) \
            /* 32x blocks */ \
            __ASM_EMIT("subs        %[count], #32") \
            __ASM_EMIT("blo         2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" SRC "]" INC ", {q0-q7}") \
            __ASM_EMIT("vmul.f32    q0, q0, q0") \
            __ASM_EMIT("vmul.f32    q1, q1, q1") \
            __ASM_EMIT("vmul.f32    q2, q2, q2") \
            __ASM_EMIT("vmul.f32    q3, q3, q3") \
            __ASM_EMIT("vmul.f32    q4, q4, q4") \
            __ASM_EMIT("vmul.f32    q5, q5, q5") \
            __ASM_EMIT("vmul.f32    q6, q6, q6") \
            __ASM_EMIT("vmul.f32    q7, q7, q7") \
            __ASM_EMIT("subs        %[count], #32") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q7}") \
            __ASM_EMIT("bhs         1b") \
            __ASM_EMIT("2:") \
            /* 16x block */ \
            __ASM_EMIT("adds        %[count], #16")  \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" SRC "]" INC ", {q0-q3}") \
            __ASM_EMIT("vmul.f32    q0, q0, q0") \
            __ASM_EMIT("vmul.f32    q1, q1, q1") \
            __ASM_EMIT("vmul.f32    q2, q2, q2") \
            __ASM_EMIT("vmul.f32    q3, q3, q3") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("4:") \
            /* 8x block */ \
            __ASM_EMIT("adds        %[count], #8")  \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" SRC "]" INC ", {q0-q1}") \
            __ASM_EMIT("vmul.f32    q0, q0, q0") \
            __ASM_EMIT("vmul.f32    q1, q1, q1") \
            __ASM_EMIT("subs        %[count], #8") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
            __ASM_EMIT("6:") \
            /* 4x block */ \
            __ASM_EMIT("adds        %[count], #4")  \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("vldm        %[" SRC "]" INC ", {q0}") \
            __ASM_EMIT("vmul.f32    q0, q0, q0") \
            __ASM_EMIT("subs        %[count], #4") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
            __ASM_EMIT("8:") \
            /* 1x block */ \
            __ASM_EMIT("adds        %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("blt         10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" SRC "]]" INC) \
            __ASM_EMIT("vmul.f32    q0, q0, q0") \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("bge         9b") \
            __ASM_EMIT("10:")

        void sqr1(float *dst, size_t count)
        {
            ARCH_ARM_ASM(
                SQR_CORE("dst", "dst", "")
                : [dst] "+r" (dst), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7"
            );
        }

        void sqr2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM(
                SQR_CORE("dst", "src", "!")
                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7"
            );
        }

        #undef SQR_CORE

    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_SQR_H_ */
