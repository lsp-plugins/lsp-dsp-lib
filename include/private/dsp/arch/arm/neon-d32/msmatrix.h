/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const float msmatrix_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0.5f)
            };
        );

        void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("vld1.32     {q12}, [%[X_HALF]]")        // q12 = 0.5
                __ASM_EMIT("blo         2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[left]!, {q0-q3}")         // q0 = l
                __ASM_EMIT("vldm        %[right]!, {q4-q7}")        // q4 = r
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = l - r
                __ASM_EMIT("vsub.f32    q9, q1, q5")
                __ASM_EMIT("vsub.f32    q10, q2, q6")
                __ASM_EMIT("vsub.f32    q11, q3, q7")
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l + r
                __ASM_EMIT("vadd.f32    q1, q1, q5")
                __ASM_EMIT("vadd.f32    q2, q2, q6")
                __ASM_EMIT("vadd.f32    q3, q3, q7")
                __ASM_EMIT("vmul.f32    q4, q8, q12")               // q4 = (l - r) * 0.5f
                __ASM_EMIT("vmul.f32    q5, q9, q12")
                __ASM_EMIT("vmul.f32    q6, q10, q12")
                __ASM_EMIT("vmul.f32    q7, q11, q12")
                __ASM_EMIT("vmul.f32    q0, q0, q12")               // q0 = (l + r) * 0.5f
                __ASM_EMIT("vmul.f32    q1, q1, q12")
                __ASM_EMIT("vmul.f32    q2, q2, q12")
                __ASM_EMIT("vmul.f32    q3, q3, q12")
                __ASM_EMIT("vstm        %[mid]!, {q0-q3}")
                __ASM_EMIT("vstm        %[side]!, {q4-q7}")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")              // 16 - 8
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[left]!, {q0-q1}")         // q0 = l
                __ASM_EMIT("vldm        %[right]!, {q4-q5}")        // q4 = r
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = l - r
                __ASM_EMIT("vsub.f32    q9, q1, q5")
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l + r
                __ASM_EMIT("vadd.f32    q1, q1, q5")
                __ASM_EMIT("vmul.f32    q4, q8, q12")               // q4 = (l - r) * 0.5f
                __ASM_EMIT("vmul.f32    q5, q9, q12")
                __ASM_EMIT("vmul.f32    q0, q0, q12")               // q0 = (l + r) * 0.5f
                __ASM_EMIT("vmul.f32    q1, q1, q12")
                __ASM_EMIT("vstm        %[mid]!, {q0-q1}")
                __ASM_EMIT("vstm        %[side]!, {q4-q5}")
                __ASM_EMIT("sub         %[count], #8")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")              // 8 - 4
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[left]!, {q0}")            // q0 = l
                __ASM_EMIT("vldm        %[right]!, {q4}")           // q4 = r
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = l - r
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l + r
                __ASM_EMIT("vmul.f32    q4, q8, q12")               // q4 = (l - r) * 0.5f
                __ASM_EMIT("vmul.f32    q0, q0, q12")               // q0 = (l + r) * 0.5f
                __ASM_EMIT("vstm        %[mid]!, {q0}")
                __ASM_EMIT("vstm        %[side]!, {q4}")
                __ASM_EMIT("sub         %[count], #4")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")              // 4 - 1
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[left]]!")
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[right]]!")
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = l - r
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l + r
                __ASM_EMIT("vmul.f32    q4, q8, q12")               // q4 = (l - r) * 0.5f
                __ASM_EMIT("vmul.f32    q0, q0, q12")               // q0 = (l + r) * 0.5f
                __ASM_EMIT("vst1.32     {d0[0]}, [%[mid]]!")
                __ASM_EMIT("vst1.32     {d8[0]}, [%[side]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         7b")
                __ASM_EMIT("8:")

                : [mid] "+r" (m), [side] "+r" (s),
                  [left] "+r" (l), [right] "+r" (r),
                  [count] "+r" (count)
                : [X_HALF] "r" (&msmatrix_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3" , "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12"
            );
        }

        void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
        {
            ARCH_ARM_ASM
            (
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("blo         2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[mid]!, {q0-q3}")          // q0 = m
                __ASM_EMIT("vldm        %[side]!, {q4-q7}")         // q4 = s
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = r = m - s
                __ASM_EMIT("vsub.f32    q9, q1, q5")
                __ASM_EMIT("vsub.f32    q10, q2, q6")
                __ASM_EMIT("vsub.f32    q11, q3, q7")
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l = m + s
                __ASM_EMIT("vadd.f32    q1, q1, q5")
                __ASM_EMIT("vadd.f32    q2, q2, q6")
                __ASM_EMIT("vadd.f32    q3, q3, q7")
                __ASM_EMIT("vstm        %[left]!, {q0-q3}")
                __ASM_EMIT("vstm        %[right]!, {q8-q11}")
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("bhs         1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], #8")              // 16 - 8
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[mid]!, {q0-q1}")          // q0 = m
                __ASM_EMIT("vldm        %[side]!, {q4-q5}")         // q4 = s
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = r = m - s
                __ASM_EMIT("vsub.f32    q9, q1, q5")
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l = m + s
                __ASM_EMIT("vadd.f32    q1, q1, q5")
                __ASM_EMIT("vstm        %[left]!, {q0-q1}")
                __ASM_EMIT("vstm        %[right]!, {q8-q9}")
                __ASM_EMIT("sub         %[count], #8")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], #4")              // 8 - 4
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[mid]!, {q0}")             // q0 = m
                __ASM_EMIT("vldm        %[side]!, {q4}")            // q4 = s
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = r = m - s
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l = m + s
                __ASM_EMIT("vstm        %[left]!, {q0}")
                __ASM_EMIT("vstm        %[right]!, {q8}")
                __ASM_EMIT("sub         %[count], #4")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], #3")              // 4 - 1
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[mid]]!")
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[side]]!")
                __ASM_EMIT("vsub.f32    q8, q0, q4")                // q8 = r = m - s
                __ASM_EMIT("vadd.f32    q0, q0, q4")                // q0 = l = m + s
                __ASM_EMIT("vst1.32     {d0[0]}, [%[left]]!")
                __ASM_EMIT("vst1.32     {d16[0]}, [%[right]]!")
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         7b")
                __ASM_EMIT("8:")

                : [mid] "+r" (m), [side] "+r" (s),
                  [left] "+r" (l), [right] "+r" (r),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        #define LR_CVT_BODY(d, l, r, op)    \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("vld1.32     {q8}, [%[X_HALF]]")         /* q8 = 0.5 */  \
            __ASM_EMIT("blo         2f") \
            /* x16 blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" l "]!, {q0-q3}")        /* q0 = l */ \
            __ASM_EMIT("vldm        %[" r "]!, {q4-q7}")        /* q4 = r */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT(op ".f32     q1, q1, q5") \
            __ASM_EMIT(op ".f32     q2, q2, q6") \
            __ASM_EMIT(op ".f32     q3, q3, q7") \
            __ASM_EMIT("vmul.f32    q0, q0, q8")                /* q0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("vmul.f32    q1, q1, q8") \
            __ASM_EMIT("vmul.f32    q2, q2, q8") \
            __ASM_EMIT("vmul.f32    q3, q3, q8") \
            __ASM_EMIT("vstm        %[" d "]!, {q0-q3}") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("bhs         1b") \
            /* x8 block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #8")              /* 16 - 8 */ \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" l "]!, {q0-q1}")        /* q0 = l */ \
            __ASM_EMIT("vldm        %[" r "]!, {q4-q5}")        /* q4 = r */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT(op ".f32     q1, q1, q5") \
            __ASM_EMIT("vmul.f32    q0, q0, q8")                /* q0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("vmul.f32    q1, q1, q8") \
            __ASM_EMIT("vstm        %[" d "]!, {q0-q1}") \
            __ASM_EMIT("sub         %[count], #8") \
            /* x4 block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #4")              /* 8 - 4 */ \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" l "]!, {q0}")           /* q0 = l */ \
            __ASM_EMIT("vldm        %[" r "]!, {q4}")           /* q4 = r */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT("vmul.f32    q0, q0, q8")                /* q0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("vstm        %[" d "]!, {q0}") \
            __ASM_EMIT("sub         %[count], #4") \
            /* x1 blocks */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #3")              /* 4 - 1 */ \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" l "]]!") \
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" r "]]!") \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT("vmul.f32    q0, q0, q8")                /* q0 = (l <+-> r) * 0.5f */ \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" d "]]!") \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("bge         7b") \
            __ASM_EMIT("8:")


        void lr_to_mid(float *m, const float *l, const float *r, size_t count)
        {
            ARCH_ARM_ASM
            (
                LR_CVT_BODY("mid", "left", "right", "vadd")
                : [mid] "+r" (m),
                  [left] "+r" (l), [right] "+r" (r),
                  [count] "+r" (count)
                : [X_HALF] "r" (&msmatrix_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8"
            );
        }

        void lr_to_side(float *s, const float *l, const float *r, size_t count)
        {
            ARCH_ARM_ASM
            (
                LR_CVT_BODY("side", "left", "right", "vsub")
                : [side] "+r" (s),
                  [left] "+r" (l), [right] "+r" (r),
                  [count] "+r" (count)
                : [X_HALF] "r" (&msmatrix_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8"
            );
        }

        #undef LR_CVT_BODY

        #define MS_CVT_BODY(d, m, s, op)    \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("blo         2f") \
            /* x16 blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" m "]!, {q0-q3}")        /* q0 = m */ \
            __ASM_EMIT("vldm        %[" s "]!, {q4-q7}")        /* q4 = s */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = m <+-> s */ \
            __ASM_EMIT(op ".f32     q1, q1, q5") \
            __ASM_EMIT(op ".f32     q2, q2, q6") \
            __ASM_EMIT(op ".f32     q3, q3, q7") \
            __ASM_EMIT("vstm        %[" d "]!, {q0-q3}") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("bhs         1b") \
            /* x8 block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #8")              /* 16 - 8 */ \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" m "]!, {q0-q1}")        /* q0 = m */ \
            __ASM_EMIT("vldm        %[" s "]!, {q4-q5}")        /* q4 = s */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = m <+-> s */ \
            __ASM_EMIT(op ".f32     q1, q1, q5") \
            __ASM_EMIT("vstm        %[" d "]!, {q0-q1}") \
            __ASM_EMIT("sub         %[count], #8") \
            /* x4 block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #4")              /* 8 - 4 */ \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" m "]!, {q0}")           /* q0 = l */ \
            __ASM_EMIT("vldm        %[" s "]!, {q4}")           /* q4 = r */ \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT("vstm        %[" d "]!, {q0}") \
            __ASM_EMIT("sub         %[count], #4") \
            /* x1 blocks */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #3")              /* 4 - 1 */ \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" m "]]!") \
            __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[" s "]]!") \
            __ASM_EMIT(op ".f32     q0, q0, q4")                /* q0 = l <+-> r */ \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" d "]]!") \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("bge         7b") \
            __ASM_EMIT("8:")

        void ms_to_left(float *l, const float *m, const float *s, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_CVT_BODY("left", "mid", "side", "vadd")
                : [left] "+r" (l),
                  [mid] "+r" (m), [side] "+r" (s),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7"
            );
        }

        void ms_to_right(float *r, const float *m, const float *s, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_CVT_BODY("right", "mid", "side", "vsub")
                : [right] "+r" (r),
                  [mid] "+r" (m), [side] "+r" (s),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7"
            );
        }

    #undef MS_CVT_BODY
    } /* namespace neon_d32 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_MSMATRIX_H_ */
