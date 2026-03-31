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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const float ms_minmax_abs[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0.5f)          // 0.5f
            };
        )

        #define MS_MUL(x)       __ASM_EMIT(x)
        #define MS_NOMUL(x)

        #define INC_ON(cmd)     cmd
        #define INC_OFF(cmd)

        #define MS_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            IF_MUL("ldr             q31, [%[CC], #0x00]")               /* v31 = 0.5f */ \
            __ASM_EMIT("b.lo        2f") \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
            __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("ldp         q20, q21, [%[" B "], #0x40]") \
            __ASM_EMIT("ldp         q22, q23, [%[" B "], #0x60]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fsub        v12.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fsub        v13.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fsub        v14.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fsub        v15.4s, v7.4s, v23.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v4.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fadd        v5.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fadd        v6.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fadd        v7.4s, v7.4s, v23.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(m, s) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
            __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
            __ASM_EMIT(OP "         v4.4s, v4.4s, v12.4s") \
            __ASM_EMIT(OP "         v5.4s, v5.4s, v13.4s") \
            __ASM_EMIT(OP "         v6.4s, v6.4s, v14.4s") \
            __ASM_EMIT(OP "         v7.4s, v7.4s, v15.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            IF_MUL("fmul            v4.4s, v4.4s, v31.4s") \
            IF_MUL("fmul            v5.4s, v5.4s, v31.4s") \
            IF_MUL("fmul            v6.4s, v6.4s, v31.4s") \
            IF_MUL("fmul            v7.4s, v7.4s, v31.4s") \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
            __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x80")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
            __ASM_EMIT("b.hs        1b") \
            /* 16x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], %[count], #16")  \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(m, s) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
            __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #16") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(m, s) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], %[count], #4") \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("ldr         q0, [%[" A "], #0x00]")             /* v0 = l */ \
            __ASM_EMIT("ldr         q16, [%[" B "], #0x00]")            /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(m, s) */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
            /* 1x block */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
            __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(m, s) */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], %[count], #1") \
            __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge        9b") \
            __ASM_EMIT("10:")

        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "fmin", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pmax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "fmax", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void lr_pmin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "fmin", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_pmax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_MINMAX_CORE("dst", "a", "b", "fmax", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        #undef MS_MINMAX_CORE

        #define MS_SIGN_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            IF_MUL("ldr             q31, [%[CC], #0x00]")               /* v31 = 0.5f */ \
            __ASM_EMIT("b.lo        2f") \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
            __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("ldp         q20, q21, [%[" B "], #0x40]") \
            __ASM_EMIT("ldp         q22, q23, [%[" B "], #0x60]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fsub        v12.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fsub        v13.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fsub        v14.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fsub        v15.4s, v7.4s, v23.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v4.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fadd        v5.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fadd        v6.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fadd        v7.4s, v7.4s, v23.4s") \
            __ASM_EMIT("facgt       v16.4s, v8.4s, v0.4s")              /* v16 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT("facgt       v17.4s, v9.4s, v1.4s") \
            __ASM_EMIT("facgt       v18.4s, v10.4s, v2.4s") \
            __ASM_EMIT("facgt       v19.4s, v11.4s, v3.4s") \
            __ASM_EMIT("facgt       v20.4s, v12.4s, v4.4s") \
            __ASM_EMIT("facgt       v21.4s, v13.4s, v5.4s") \
            __ASM_EMIT("facgt       v22.4s, v14.4s, v6.4s") \
            __ASM_EMIT("facgt       v23.4s, v15.4s, v7.4s") \
            __ASM_EMIT(OP "         v0.16b, v8.16b, v16.16b")           /* v0 = O = OP(fabsf(s) > fabsf(m)) ? s : m */ \
            __ASM_EMIT(OP "         v1.16b, v9.16b, v17.16b") \
            __ASM_EMIT(OP "         v2.16b, v10.16b, v18.16b") \
            __ASM_EMIT(OP "         v3.16b, v11.16b, v19.16b") \
            __ASM_EMIT(OP "         v4.16b, v12.16b, v20.16b") \
            __ASM_EMIT(OP "         v5.16b, v13.16b, v21.16b") \
            __ASM_EMIT(OP "         v6.16b, v14.16b, v22.16b") \
            __ASM_EMIT(OP "         v7.16b, v15.16b, v23.16b") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            IF_MUL("fmul            v4.4s, v4.4s, v31.4s") \
            IF_MUL("fmul            v5.4s, v5.4s, v31.4s") \
            IF_MUL("fmul            v6.4s, v6.4s, v31.4s") \
            IF_MUL("fmul            v7.4s, v7.4s, v31.4s") \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
            __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x80")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
            __ASM_EMIT("b.hs        1b") \
            /* 16x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], %[count], #16")  \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT("facgt       v16.4s, v8.4s, v0.4s")              /* v16 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT("facgt       v17.4s, v9.4s, v1.4s") \
            __ASM_EMIT("facgt       v18.4s, v10.4s, v2.4s") \
            __ASM_EMIT("facgt       v19.4s, v11.4s, v3.4s") \
            __ASM_EMIT(OP "         v0.16b, v8.16b, v16.16b")           /* v0 = O = OP(fabsf(s) > fabsf(m)) ? s : m */ \
            __ASM_EMIT(OP "         v1.16b, v9.16b, v17.16b") \
            __ASM_EMIT(OP "         v2.16b, v10.16b, v18.16b") \
            __ASM_EMIT(OP "         v3.16b, v11.16b, v19.16b") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #16") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("facgt       v16.4s, v8.4s, v0.4s")              /* v16 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT("facgt       v17.4s, v9.4s, v1.4s") \
            __ASM_EMIT(OP "         v0.16b, v8.16b, v16.16b")           /* v0 = O = OP(fabsf(s) > fabsf(m)) ? s : m */ \
            __ASM_EMIT(OP "         v1.16b, v9.16b, v17.16b") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("ldr         q0, [%[" A "], #0x00]")             /* v0 = l */ \
            __ASM_EMIT("ldr         q16, [%[" B "], #0x00]")            /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("facgt       v16.4s, v8.4s, v0.4s")              /* v16 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT(OP "         v0.16b, v8.16b, v16.16b")           /* v0 = O = OP(fabsf(s) > fabsf(m)) ? s : m */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
            /* 1x block */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
            __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("facgt       v16.4s, v8.4s, v0.4s")              /* v16 = fabsf(s) > fabsf(m) */ \
            __ASM_EMIT(OP "         v0.16b, v8.16b, v16.16b")           /* v0 = O = OP(fabsf(s) > fabsf(m)) ? s : m */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], %[count], #1") \
            __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge        9b") \
            __ASM_EMIT("10:")

        void ms_psmin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "bif", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "bif", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "bit", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "bit", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void lr_psmin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "bif", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "bif", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_psmax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "dst", "src", "bit", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MS_SIGN_MINMAX_CORE("dst", "a", "b", "bit", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        #undef MS_SIGN_MINMAX_CORE

        #define MM_ABS_MINMAX_CORE(DST, A, B, OP, INCA, IF_MUL) \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            IF_MUL("ldr             q31, [%[CC], #0x00]")               /* v31 = 0.5f */ \
            __ASM_EMIT("b.lo        2f") \
            /* 32x blocks */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q4, q5, [%[" A "], #0x40]") \
            __ASM_EMIT("ldp         q6, q7, [%[" A "], #0x60]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("ldp         q20, q21, [%[" B "], #0x40]") \
            __ASM_EMIT("ldp         q22, q23, [%[" B "], #0x60]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fsub        v12.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fsub        v13.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fsub        v14.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fsub        v15.4s, v7.4s, v23.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v4.4s, v4.4s, v20.4s") \
            __ASM_EMIT("fadd        v5.4s, v5.4s, v21.4s") \
            __ASM_EMIT("fadd        v6.4s, v6.4s, v22.4s") \
            __ASM_EMIT("fadd        v7.4s, v7.4s, v23.4s") \
            __ASM_EMIT("fabs        v8.4s, v8.4s")                      /* v8 = fabsf(s) */ \
            __ASM_EMIT("fabs        v9.4s, v9.4s") \
            __ASM_EMIT("fabs        v10.4s, v10.4s") \
            __ASM_EMIT("fabs        v11.4s, v11.4s") \
            __ASM_EMIT("fabs        v12.4s, v12.4s") \
            __ASM_EMIT("fabs        v13.4s, v13.4s") \
            __ASM_EMIT("fabs        v14.4s, v14.4s") \
            __ASM_EMIT("fabs        v15.4s, v15.4s") \
            __ASM_EMIT("fabs        v0.4s, v0.4s")                      /* v0 = fabsf(m) */ \
            __ASM_EMIT("fabs        v1.4s, v1.4s") \
            __ASM_EMIT("fabs        v2.4s, v2.4s") \
            __ASM_EMIT("fabs        v3.4s, v3.4s") \
            __ASM_EMIT("fabs        v4.4s, v4.4s") \
            __ASM_EMIT("fabs        v5.4s, v5.4s") \
            __ASM_EMIT("fabs        v6.4s, v6.4s") \
            __ASM_EMIT("fabs        v7.4s, v7.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
            __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
            __ASM_EMIT(OP "         v4.4s, v4.4s, v12.4s") \
            __ASM_EMIT(OP "         v5.4s, v5.4s, v13.4s") \
            __ASM_EMIT(OP "         v6.4s, v6.4s, v14.4s") \
            __ASM_EMIT(OP "         v7.4s, v7.4s, v15.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            IF_MUL("fmul            v4.4s, v4.4s, v31.4s") \
            IF_MUL("fmul            v5.4s, v5.4s, v31.4s") \
            IF_MUL("fmul            v6.4s, v6.4s, v31.4s") \
            IF_MUL("fmul            v7.4s, v7.4s, v31.4s") \
            __ASM_EMIT("subs        %[count], %[count], #32") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT("stp         q4, q5, [%[" DST "], #0x40]") \
            __ASM_EMIT("stp         q6, q7, [%[" DST "], #0x60]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x80")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x80") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x80") \
            __ASM_EMIT("b.hs        1b") \
            /* 16x block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("adds        %[count], %[count], #16")  \
            __ASM_EMIT("b.lt        4f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q2, q3, [%[" A "], #0x20]") \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("ldp         q18, q19, [%[" B "], #0x20]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fsub        v10.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fsub        v11.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v2.4s, v2.4s, v18.4s") \
            __ASM_EMIT("fadd        v3.4s, v3.4s, v19.4s") \
            __ASM_EMIT("fabs        v8.4s, v8.4s")                      /* v8 = fabsf(s) */ \
            __ASM_EMIT("fabs        v9.4s, v9.4s") \
            __ASM_EMIT("fabs        v10.4s, v10.4s") \
            __ASM_EMIT("fabs        v11.4s, v11.4s") \
            __ASM_EMIT("fabs        v0.4s, v0.4s")                      /* v0 = fabsf(m) */ \
            __ASM_EMIT("fabs        v1.4s, v1.4s") \
            __ASM_EMIT("fabs        v2.4s, v2.4s") \
            __ASM_EMIT("fabs        v3.4s, v3.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            __ASM_EMIT(OP "         v2.4s, v2.4s, v10.4s") \
            __ASM_EMIT(OP "         v3.4s, v3.4s, v11.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            IF_MUL("fmul            v2.4s, v2.4s, v31.4s") \
            IF_MUL("fmul            v3.4s, v3.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #16") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT("stp         q2, q3, [%[" DST "], #0x20]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x40")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x40") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x40") \
            /* 8x block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("adds        %[count], %[count], #8") \
            __ASM_EMIT("b.lt        6f") \
            __ASM_EMIT("ldp         q0, q1, [%[" A "], #0x00]")         /* v0 = l */ \
            __ASM_EMIT("ldp         q16, q17, [%[" B "], #0x00]")       /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fsub        v9.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fadd        v1.4s, v1.4s, v17.4s") \
            __ASM_EMIT("fabs        v8.4s, v8.4s")                      /* v8 = fabsf(s) */ \
            __ASM_EMIT("fabs        v9.4s, v9.4s") \
            __ASM_EMIT("fabs        v0.4s, v0.4s")                      /* v0 = fabsf(m) */ \
            __ASM_EMIT("fabs        v1.4s, v1.4s") \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(fabsf(m), fabsf(s)) */ \
            __ASM_EMIT(OP "         v1.4s, v1.4s, v9.4s") \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            IF_MUL("fmul            v1.4s, v1.4s, v31.4s") \
            __ASM_EMIT("sub         %[count], %[count], #8") \
            __ASM_EMIT("stp         q0, q1, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x20")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x20") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x20") \
            /* 4x block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */ \
            __ASM_EMIT("b.lt        8f") \
            __ASM_EMIT("ldr         q0, [%[" A "], #0x00]")             /* v0 = l */ \
            __ASM_EMIT("ldr         q16, [%[" B "], #0x00]")            /* v12 = r */ \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fabs        v8.4s, v8.4s")                      /* v8 = fabsf(s) */ \
            __ASM_EMIT("fabs        v0.4s, v0.4s")                      /* v0 = fabsf(m) */ \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("sub         %[count], %[count], #4") \
            __ASM_EMIT("str         q0, [%[" DST "], #0x00]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x10")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x10") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x10") \
            /* 1x block */ \
            __ASM_EMIT("8:") \
            __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */ \
            __ASM_EMIT("b.lt        10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("ld1r        {v0.4s}, [%[" A "]]") \
            __ASM_EMIT("ld1r        {v16.4s}, [%[" B "]]") \
            __ASM_EMIT("fsub        v8.4s, v0.4s, v16.4s")              /* v8 = s = l - r */ \
            __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")              /* v0 = m = l + r */ \
            __ASM_EMIT("fabs        v8.4s, v8.4s")                      /* v8 = fabsf(s) */ \
            __ASM_EMIT("fabs        v0.4s, v0.4s")                      /* v0 = fabsf(m) */ \
            __ASM_EMIT(OP "         v0.4s, v0.4s, v8.4s")               /* v0 = O = OP(fabsf(m), fabsf(s)) */ \
            IF_MUL("fmul            v0.4s, v0.4s, v31.4s")              /* v0 = O * 0.5f */ \
            __ASM_EMIT("subs        %[count], %[count], #1") \
            __ASM_EMIT("st1         {v0.s}[0], [%[" DST "]]") \
            __ASM_EMIT(INCA("add    %[" A "], %[" A "], #0x04")) \
            __ASM_EMIT("add         %[" B "], %[" B "], #0x04") \
            __ASM_EMIT("add         %[" DST "], %[" DST "], #0x04") \
            __ASM_EMIT("b.ge        9b") \
            __ASM_EMIT("10:")

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "a", "b", "fmin", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pamax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF, MS_MUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void ms_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "a", "b", "fmax", INC_ON, MS_MUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void lr_pamin2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "dst", "src", "fmin", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (&ms_minmax_abs[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v31"
            );
        }

        void lr_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "a", "b", "fmin", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_pamax2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "dst", "src", "fmax", INC_OFF, MS_NOMUL)
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void lr_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                MM_ABS_MINMAX_CORE("dst", "a", "b", "fmax", INC_ON, MS_NOMUL)
                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        #undef MM_ABS_MINMAX_CORE

        #undef INC_ON
        #undef INC_OFF

        #undef MS_MUL
        #undef MS_NOMUL
    } /* namespace asimd */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PMATH_MS_MINMAX_H_ */
