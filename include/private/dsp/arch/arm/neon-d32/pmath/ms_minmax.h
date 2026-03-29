/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 мар. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        IF_ARCH_ARM(
            static const float ms_minmax_abs[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0.5f)          // 0.5f
            };
        )

        #define MS_MUL(x)       __ASM_EMIT(x)
        #define MS_NOMUL(x)

        #define MS_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], #16") \
            IF_MUL("vldm            %[CC], {q12}")              /* q12 = 0.5f */ \
            __ASM_EMIT("blo         2f") \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") /* q0 = l */ \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q11}")       /* q8 = r */ \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vsub.f32    q6, q2, q10") \
            __ASM_EMIT("vsub.f32    q7, q3, q11") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT("vadd.f32    q2, q2, q10") \
            __ASM_EMIT("vadd.f32    q3, q3, q11") \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(m, s) */ \
            __ASM_EMIT(OP ".f32     q1, q1, q5") \
            __ASM_EMIT(OP ".f32     q2, q2, q6") \
            __ASM_EMIT(OP ".f32     q3, q3, q7") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            IF_MUL("vmul.f32        q2, q2, q12") \
            IF_MUL("vmul.f32        q3, q3, q12") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("bhs         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #8") \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") /* q0 = l */ \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q9}")        /* q8 = r */ \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(m, s) */ \
            __ASM_EMIT(OP ".f32     q1, q1, q5") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            __ASM_EMIT("sub         %[count], #8") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #4") \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}")    /* q0 = l */ \
            __ASM_EMIT("vldm        %[" B "]!, {q8}")           /* q8 = r */ \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(m, s) */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], #4") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
            /* 1x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #3") \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(m, s) */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("bge         7b") \
            __ASM_EMIT("8:")

        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vmin", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "vmin", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pmax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vmax", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "vmax", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }


        void lr_pmin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vmin", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "vmin", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pmax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "vmax", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "vmax", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        #undef MS_MINMAX_CORE

        #define MS_SIGN_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], #16") \
            IF_MUL("vldm            %[CC], {q12}")              /* q12 = 0.5f */ \
            __ASM_EMIT("blo         2f") \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") /* q0   = m */ \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q11}")       /* q8   = s */ \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vsub.f32    q6, q2, q10") \
            __ASM_EMIT("vsub.f32    q7, q3, q11") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT("vadd.f32    q2, q2, q10") \
            __ASM_EMIT("vadd.f32    q3, q3, q11") \
            __ASM_EMIT("vacgt.f32   q8,  q4, q0")               /* q8 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT("vacgt.f32   q9,  q5, q1") \
            __ASM_EMIT("vacgt.f32   q10, q6, q2") \
            __ASM_EMIT("vacgt.f32   q11, q7, q3") \
            __ASM_EMIT(OP "         q0,  q4, q8")               /* q0 = O */ \
            __ASM_EMIT(OP "         q1,  q5, q9") \
            __ASM_EMIT(OP "         q2,  q6, q10") \
            __ASM_EMIT(OP "         q3,  q7, q11") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            IF_MUL("vmul.f32        q2, q2, q12") \
            IF_MUL("vmul.f32        q3, q3, q12") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("bhs         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #8") \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT("vacgt.f32   q8,  q4, q0")               /* q8 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT("vacgt.f32   q9,  q5, q1") \
            __ASM_EMIT(OP "         q0,  q4, q8")               /* q0 = O */ \
            __ASM_EMIT(OP "         q1,  q5, q9") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            __ASM_EMIT("sub         %[count], #8") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #4") \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}") \
            __ASM_EMIT("vldm        %[" B "]!, {q8}") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vacgt.f32   q8,  q4, q0")               /* q8 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT(OP "         q0,  q4, q8")               /* q0 = O */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], #4") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
            /* 1x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #3") \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vacgt.f32   q8,  q4, q0")               /* q8 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT(OP "         q0,  q4, q8")               /* q0 = O */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("bge         7b") \
            __ASM_EMIT("8:")

        void ms_psmin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "vbif", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "vbif", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "vbit", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "vbit", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void lr_psmin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "vbif", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "vbif", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_psmax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "vbit", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "vbit", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        #undef MS_SIGN_MINMAX_CORE

        #define MS_ABS_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], #16") \
            IF_MUL("vldm            %[CC], {q12}")              /* q12 = 0.5f */ \
            __ASM_EMIT("blo         2f") \
            /* 16x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q3}") \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q11}") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vsub.f32    q6, q2, q10") \
            __ASM_EMIT("vsub.f32    q7, q3, q11") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT("vadd.f32    q2, q2, q10") \
            __ASM_EMIT("vadd.f32    q3, q3, q11") \
            __ASM_EMIT("vabs.f32    q4, q4")                    /* q4 = fabsf(s) */ \
            __ASM_EMIT("vabs.f32    q5, q5") \
            __ASM_EMIT("vabs.f32    q6, q6") \
            __ASM_EMIT("vabs.f32    q7, q7") \
            __ASM_EMIT("vabs.f32    q0, q0")                    /* q0 = fabsf(m) */ \
            __ASM_EMIT("vabs.f32    q1, q1") \
            __ASM_EMIT("vabs.f32    q2, q2") \
            __ASM_EMIT("vabs.f32    q3, q3") \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP ".f32     q1, q1, q5") \
            __ASM_EMIT(OP ".f32     q2, q2, q6") \
            __ASM_EMIT(OP ".f32     q3, q3, q7") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            IF_MUL("vmul.f32        q2, q2, q12") \
            IF_MUL("vmul.f32        q3, q3, q12") \
            __ASM_EMIT("subs        %[count], #16") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q3}") \
            __ASM_EMIT("bhs         1b") \
            /* 8x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], #8") \
            __ASM_EMIT("blt         4f") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0-q1}") \
            __ASM_EMIT("vldm        %[" B "]!, {q8-q9}") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vsub.f32    q5, q1, q9") \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vadd.f32    q1, q1, q9") \
            __ASM_EMIT("vabs.f32    q4, q4")                    /* q4 = fabsf(s) */ \
            __ASM_EMIT("vabs.f32    q5, q5") \
            __ASM_EMIT("vabs.f32    q0, q0")                    /* q0 = fabsf(m) */ \
            __ASM_EMIT("vabs.f32    q1, q1") \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP ".f32     q1, q1, q5") \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            IF_MUL("vmul.f32        q1, q1, q12") \
            __ASM_EMIT("sub         %[count], #8") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0-q1}") \
            /* 4x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], #4") \
            __ASM_EMIT("blt         6f") \
            __ASM_EMIT("vldm        %[" B "]!, {q8}") \
            __ASM_EMIT("vldm        %[" A "]" INCA ", {q0}") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vabs.f32    q4, q4")                    /* q4 = fabsf(s) */ \
            __ASM_EMIT("vabs.f32    q0, q0")                    /* q0 = fabsf(m) */ \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], #4") \
            __ASM_EMIT("vstm        %[" DST "]!, {q0}") \
            /* 1x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], #3") \
            __ASM_EMIT("blt         8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("vld1.32     {d0[], d1[]}, [%[" A "]]" INCA) \
            __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[" B "]]!") \
            __ASM_EMIT("vsub.f32    q4, q0, q8")                /* q4 = s = l - r */ \
            __ASM_EMIT("vadd.f32    q0, q0, q8")                /* q0 = m = l + r */ \
            __ASM_EMIT("vabs.f32    q4, q4")                    /* q4 = fabsf(s) */ \
            __ASM_EMIT("vabs.f32    q0, q0")                    /* q0 = fabsf(m) */ \
            __ASM_EMIT(OP ".f32     q0, q0, q4")                /* q0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("vmul.f32        q0, q0, q12")               /* q0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], #1") \
            __ASM_EMIT("vst1.32     {d0[0]}, [%[" DST "]]!") \
            __ASM_EMIT("bge         7b") \
            __ASM_EMIT("8:")

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmin", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmin", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pamax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmax", "", MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void ms_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmax", "!", MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11",
                  "q12"
            );
        }

        void lr_pamin2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmin", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmin", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pamax2(float *dst, const float *src, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "dst", "src", "vmax", "", MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        void lr_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM
            (
                MS_ABS_MINMAX_CORE("dst", "a", "b", "vmax", "!", MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q0", "q1", "q2", "q3",
                  "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11"
            );
        }

        #undef MS_ABS_MINMAX_CORE

        #undef MS_MUL
        #undef MS_NOMUL
    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_PMATH_MS_MINMAX_H_ */
