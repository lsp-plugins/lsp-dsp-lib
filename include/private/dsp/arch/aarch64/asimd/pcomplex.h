/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_AARCH64(float *ptr1, *ptr2);
            ARCH_AARCH64_ASM
            (
                // x8 blocks
                __ASM_EMIT("subs        %[count], %[count], #8")
                __ASM_EMIT("add         %[src2], %[src1], #0x20")
                __ASM_EMIT("add         %[dst2], %[dst1], #0x20")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src1]]")           // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[dst1]]")           // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src2]]")           // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[dst2]]")           // v26  = dr2, v27 = di2
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("add         %[src1], %[src1], #0x40")
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("fmul        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("add         %[src2], %[src2], #0x40")
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmls        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("fmla        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst1]]")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst2]]")
                __ASM_EMIT("subs        %[count], %[count], #8")
                __ASM_EMIT("add         %[dst1], %[dst1], #0x40")
                __ASM_EMIT("add         %[dst2], %[dst2], #0x40")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("2:")
                // x4 blocks
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src1]]")           // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[dst1]]")           // v24  = dr1, v25 = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("add         %[src1], %[src1], #0x20")
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst1]]")
                __ASM_EMIT("add         %[dst1], %[dst1], #0x20")
                __ASM_EMIT("4:")
                // x1 block
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("ld2         {v16.s, v17.s}[0], [%[src1]]")          // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.s, v25.s}[0], [%[dst1]]")          // v24  = dr1, v25 = di1
                __ASM_EMIT("fmul        v0.2s, v16.2s, v24.2s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.2s, v24.2s, v17.2s")                 // v1   = si*dr
                __ASM_EMIT("add         %[src1], %[src1], #0x08")
                __ASM_EMIT("fmls        v0.2s, v17.2s, v25.2s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmla        v1.2s, v16.2s, v25.2s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst1]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[dst1], %[dst1], #0x08")
                __ASM_EMIT("b.ge        5b")
                __ASM_EMIT("6:")

                : [dst1] "+r" (dst), [src1] "+r" (src),
                  [dst2] "=&r" (ptr1), [src2] "=&r" (ptr2),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")

                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src1]], #0x20")    // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src2]], #0x20")    // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src1]], #0x20")    // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[src2]], #0x20")    // v26  = dr2, v27 = di2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src1]], #0x20")    // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v28.4s, v29.4s}, [%[src2]], #0x20")    // v28  = dr3, v29 = di3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src1]], #0x20")    // v22  = sr4, v23 = si4
                __ASM_EMIT("ld2         {v30.4s, v31.4s}, [%[src2]], #0x20")    // v30  = dr4, v31 = di4

                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmul        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("fmul        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmul        v5.4s, v28.4s, v21.4s")
                __ASM_EMIT("fmul        v7.4s, v30.4s, v23.4s")
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmls        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmls        v4.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmls        v6.4s, v23.4s, v31.4s")
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("fmla        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla        v5.4s, v20.4s, v29.4s")
                __ASM_EMIT("fmla        v7.4s, v22.4s, v31.4s")

                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")

                // x8 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src1]], #0x20")    // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src2]], #0x20")    // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src1]], #0x20")    // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[src2]], #0x20")    // v26  = dr2, v27 = di2

                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("fmul        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmls        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("fmla        v3.4s, v18.4s, v27.4s")

                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src1]], #0x20")    // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src2]], #0x20")    // v24  = dr1, v25 = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")

                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[src1]], #0x08")    // v16  = sr1, v17  = si1
                __ASM_EMIT("ld2r        {v24.4s, v25.4s}, [%[src2]], #0x08")    // v24  = dr1, v25  = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v24.4s, v17.4s")                 // v1   = si*dr
                __ASM_EMIT("fmls        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr - si*di
                __ASM_EMIT("fmla        v1.4s, v16.4s, v25.4s")                 // v1   = si*dr + sr*di
                __ASM_EMIT("str         s0, [%[dst]], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s1, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
                  [count] "+r" (count)
                : [kdiv] "r" (&complex_div_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", /* "v9", "v10", "v11", */ "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        static const float pcomplex_div_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(1.0f)
        };

        void pcomplex_div2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldr         q8, [%[kdiv]]")
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[dst]], #0x20")     // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[dst]], #0x20")     // v26  = dr2, v27 = di2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v28.4s, v29.4s}, [%[dst]], #0x20")     // v28  = dr3, v29 = di3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src]], #0x20")     // v22  = sr4, v23 = si4
                __ASM_EMIT("ld2         {v30.4s, v31.4s}, [%[dst]], #0x20")     // v30  = dr4, v31 = di4
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmul        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmul        v5.4s, v20.4s, v29.4s")
                __ASM_EMIT("fmul        v7.4s, v22.4s, v31.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v23.4s, v31.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmls        v5.4s, v28.4s, v21.4s")
                __ASM_EMIT("fmls        v7.4s, v30.4s, v23.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v20.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v22.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v20.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v22.4s, v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fdiv        v14.4s, v8.4s, v20.4s")
                __ASM_EMIT("fdiv        v15.4s, v8.4s, v22.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v14.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v15.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("fmul        v5.4s, v5.4s, v14.4s")
                __ASM_EMIT("fmul        v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")                 // dst -= 0x80
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[dst]], #0x20")     // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[dst]], #0x20")     // v26  = dr2, v27 = di2
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")                 // dst -= 0x40
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("4:")
                // x4 blocks
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[dst]]")            // v24  = dr1, v25 = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[src]], #0x08")     // v16  = sr1, v17  = si1
                __ASM_EMIT("ld2r        {v24.4s, v25.4s}, [%[dst]]")            // v24  = dr1, v25  = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("b.ge        7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kdiv] "r" (&pcomplex_div_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", /* "v9", "v10", "v11", */ "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_rdiv2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldr         q8, [%[kdiv]]")
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src]], #0x20")     // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[src]], #0x20")     // v26  = dr2, v27 = di2
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v28.4s, v29.4s}, [%[src]], #0x20")     // v28  = dr3, v29 = di3
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[dst]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v30.4s, v31.4s}, [%[src]], #0x20")     // v30  = dr4, v31 = di4
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[dst]], #0x20")     // v22  = sr4, v23 = si4
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmul        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmul        v5.4s, v20.4s, v29.4s")
                __ASM_EMIT("fmul        v7.4s, v22.4s, v31.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v23.4s, v31.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmls        v5.4s, v28.4s, v21.4s")
                __ASM_EMIT("fmls        v7.4s, v30.4s, v23.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v20.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v22.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v20.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v22.4s, v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fdiv        v14.4s, v8.4s, v20.4s")
                __ASM_EMIT("fdiv        v15.4s, v8.4s, v22.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v14.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v15.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("fmul        v5.4s, v5.4s, v14.4s")
                __ASM_EMIT("fmul        v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")                 // dst -= 0x80
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src]], #0x20")     // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[src]], #0x20")     // v26  = dr2, v27 = di2
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")                 // dst -= 0x40
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("4:")
                // x4 blocks
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src]], #0x20")     // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17 = si1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v24.4s, v25.4s}, [%[src]], #0x08")     // v24  = dr1, v25  = di1
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17  = si1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("b.ge        7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kdiv] "r" (&pcomplex_div_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", /* "v9", "v10", "v11", */ "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ldr         q8, [%[kdiv]]")
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[b]], #0x20")       // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[t]], #0x20")       // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[b]], #0x20")       // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[t]], #0x20")       // v26  = dr2, v27 = di2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[b]], #0x20")       // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v28.4s, v29.4s}, [%[t]], #0x20")       // v28  = dr3, v29 = di3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[b]], #0x20")       // v22  = sr4, v23 = si4
                __ASM_EMIT("ld2         {v30.4s, v31.4s}, [%[t]], #0x20")       // v30  = dr4, v31 = di4
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v4.4s, v20.4s, v28.4s")
                __ASM_EMIT("fmul        v6.4s, v22.4s, v30.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmul        v5.4s, v20.4s, v29.4s")
                __ASM_EMIT("fmul        v7.4s, v22.4s, v31.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla        v4.4s, v21.4s, v29.4s")
                __ASM_EMIT("fmla        v6.4s, v23.4s, v31.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmls        v5.4s, v28.4s, v21.4s")
                __ASM_EMIT("fmls        v7.4s, v30.4s, v23.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v20.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v22.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v20.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v22.4s, v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fdiv        v14.4s, v8.4s, v20.4s")
                __ASM_EMIT("fdiv        v15.4s, v8.4s, v22.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v14.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v15.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("fmul        v5.4s, v5.4s, v14.4s")
                __ASM_EMIT("fmul        v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("2:")
                // x8 blocks
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[b]], #0x20")       // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[t]], #0x20")       // v24  = dr1, v25 = di1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[b]], #0x20")       // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[t]], #0x20")       // v26  = dr2, v27 = di2
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v2.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmul        v3.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmla        v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmls        v3.4s, v26.4s, v19.4s")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fdiv        v13.4s, v8.4s, v18.4s")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("4:")
                // x4 blocks
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[b]], #0x20")       // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[t]], #0x20")       // v24  = dr1, v25 = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[b]], #0x08")       // v16  = sr1, v17  = si1
                __ASM_EMIT("ld2r        {v24.4s, v25.4s}, [%[t]], #0x08")       // v24  = dr1, v25  = di1
                __ASM_EMIT("fmul        v0.4s, v16.4s, v24.4s")                 // v0   = sr*dr
                __ASM_EMIT("fmul        v1.4s, v16.4s, v25.4s")                 // v1   = sr*di
                __ASM_EMIT("fmla        v0.4s, v17.4s, v25.4s")                 // v0   = sr*dr + si*di
                __ASM_EMIT("fmls        v1.4s, v24.4s, v17.4s")                 // v1   = sr*di-si*dr
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16  = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*sr+si*si
                __ASM_EMIT("fdiv        v12.4s, v8.4s, v16.4s")                 // v12  = R = 1/(sr*sr + si*si)
                __ASM_EMIT("fmul        v0.4s, v0.4s, v12.4s")                  // v0   = (sr*dr + si*di) / (sr*sr+si*si)
                __ASM_EMIT("fmul        v1.4s, v1.4s, v12.4s")                  // v1   = (sr*di - si*dr) / (sr*sr+si*si)
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("b.ge        7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [t] "+r" (t), [b] "+r" (b),
                  [count] "+r" (count)
                : [kdiv] "r" (&pcomplex_div_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", /* "v9", "v10", "v11", */ "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")

                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src]], #0x20")     // v22  = sr4, v23 = si4

                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16   = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v20.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v22.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*dr + si*di
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v20.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v22.4s, v23.4s, v23.4s")
                __ASM_EMIT("fsqrt       v0.4s, v16.4s")
                __ASM_EMIT("fsqrt       v1.4s, v18.4s")
                __ASM_EMIT("fsqrt       v2.4s, v20.4s")
                __ASM_EMIT("fsqrt       v3.4s, v22.4s")

                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st1         {v0.4s-v3.4s}, [%[dst]], #0x40")
                __ASM_EMIT("b.hs        1b")

                // x8 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2

                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16   = sr*sr
                __ASM_EMIT("fmul        v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*dr + si*di
                __ASM_EMIT("fmla        v18.4s, v19.4s, v19.4s")
                __ASM_EMIT("fsqrt       v0.4s, v16.4s")
                __ASM_EMIT("fsqrt       v1.4s, v18.4s")

                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st1         {v0.4s, v1.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1

                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16   = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*dr + si*di
                __ASM_EMIT("fsqrt       v0.4s, v16.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st1         {v0.4s}, [%[dst]], #0x10")

                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[src]], #0x08")     // v16  = sr1, v17  = si1
                __ASM_EMIT("fmul        v16.4s, v16.4s, v16.4s")                // v16   = sr*sr
                __ASM_EMIT("fmla        v16.4s, v17.4s, v17.4s")                // v16  = sr*dr + si*di
                __ASM_EMIT("fsqrt       v0.4s, v16.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s0, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_rcp1(float *dst, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")

                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[dst]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[dst]], #0x20")     // v22  = sr4, v23 = si4

                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v26.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v27.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v26.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v27.4s, v23.4s, v23.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fneg        v21.4s, v21.4s")
                __ASM_EMIT("fneg        v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v20.4s, v20.4s, v26.4s")
                __ASM_EMIT("fdiv        v22.4s, v22.4s, v27.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")
                __ASM_EMIT("fdiv        v21.4s, v21.4s, v26.4s")
                __ASM_EMIT("fdiv        v23.4s, v23.4s, v27.4s")

                __ASM_EMIT("sub         %[dst], %[dst], 0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v20.4s, v21.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v22.4s, v23.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")

                // x8 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2

                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")

                __ASM_EMIT("sub         %[dst], %[dst], 0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17 = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")

                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17  = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("str         s16, [%[dst]], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s17, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_rcp2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")

                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src]], #0x20")     // v22  = sr4, v23 = si4

                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v26.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v27.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v26.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v27.4s, v23.4s, v23.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fneg        v21.4s, v21.4s")
                __ASM_EMIT("fneg        v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v20.4s, v20.4s, v26.4s")
                __ASM_EMIT("fdiv        v22.4s, v22.4s, v27.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")
                __ASM_EMIT("fdiv        v21.4s, v21.4s, v26.4s")
                __ASM_EMIT("fdiv        v23.4s, v23.4s, v27.4s")

                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v20.4s, v21.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v22.4s, v23.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")

                // x8 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")     // v18  = sr2, v19 = si2

                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")

                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")

                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[src]], #0x08")     // v16  = sr1, v17  = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("str         s16, [%[dst]], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s17, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")

                // x32 blocks
                __ASM_EMIT("eor         v25.16b, v25.16b, v25.16b")
                __ASM_EMIT("eor         v27.16b, v27.16b, v27.16b")
                __ASM_EMIT("eor         v29.16b, v29.16b, v29.16b")
                __ASM_EMIT("eor         v31.16b, v31.16b, v31.16b")

                __ASM_EMIT("1:")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("ld1         {v20.4s-v23.4s}, [%[src]], #0x40")

                __ASM_EMIT("mov         v30.16b, v23.16b")
                __ASM_EMIT("mov         v28.16b, v22.16b")
                __ASM_EMIT("mov         v26.16b, v21.16b")
                __ASM_EMIT("mov         v24.16b, v20.16b")
                __ASM_EMIT("mov         v22.16b, v19.16b")
                __ASM_EMIT("mov         v20.16b, v18.16b")
                __ASM_EMIT("mov         v18.16b, v17.16b")
                __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
                __ASM_EMIT("eor         v19.16b, v19.16b, v19.16b")
                __ASM_EMIT("eor         v21.16b, v21.16b, v21.16b")
                __ASM_EMIT("eor         v23.16b, v23.16b, v23.16b")

                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v20.4s, v21.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v22.4s, v23.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v24.4s, v25.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v26.4s, v27.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v28.4s, v29.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v30.4s, v31.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")

                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")

                __ASM_EMIT("mov         v22.16b, v19.16b")
                __ASM_EMIT("mov         v20.16b, v18.16b")
                __ASM_EMIT("mov         v18.16b, v17.16b")
                __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
                __ASM_EMIT("eor         v19.16b, v19.16b, v19.16b")
                __ASM_EMIT("eor         v21.16b, v21.16b, v21.16b")
                __ASM_EMIT("eor         v23.16b, v23.16b, v23.16b")

                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v20.4s, v21.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v22.4s, v23.4s}, [%[dst]], #0x20")

                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld1         {v16.4s, v17.4s}, [%[src]], #0x20")

                __ASM_EMIT("mov         v18.16b, v17.16b")
                __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
                __ASM_EMIT("eor         v19.16b, v19.16b, v19.16b")

                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("mov         v18.16b, v17.16b")
                __ASM_EMIT("eor         v17.16b, v17.16b, v17.16b")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")

                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ldr         s0, [%[src]], #0x04")
                __ASM_EMIT("str         s0, [%[dst]], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s31, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")

                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_c2r(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("b.lo        2f")

                // x32 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v24.4s, v25.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v26.4s, v27.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v28.4s, v29.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v30.4s, v31.4s}, [%[src]], #0x20")

                __ASM_EMIT("mov         v17.16b, v18.16b")
                __ASM_EMIT("mov         v18.16b, v20.16b")
                __ASM_EMIT("mov         v19.16b, v22.16b")
                __ASM_EMIT("mov         v20.16b, v24.16b")
                __ASM_EMIT("mov         v21.16b, v26.16b")
                __ASM_EMIT("mov         v22.16b, v28.16b")
                __ASM_EMIT("mov         v23.16b, v30.16b")

                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst]], #0x40")
                __ASM_EMIT("st1         {v20.4s-v23.4s}, [%[dst]], #0x40")
                __ASM_EMIT("b.hs        1b")

                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[src]], #0x20")

                __ASM_EMIT("mov         v17.16b, v18.16b")
                __ASM_EMIT("mov         v18.16b, v20.16b")
                __ASM_EMIT("mov         v19.16b, v22.16b")

                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("st1         {v16.4s-v19.4s}, [%[dst]], #0x40")

                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[src]], #0x20")
                __ASM_EMIT("mov         v17.16b, v18.16b")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st1         {v16.4s, v17.4s}, [%[dst]], #0x20")

                // x4 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("mov         v17.16b, v18.16b")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st1         {v16.4s}, [%[dst]], #0x10")

                // x1 blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld2         {v16.s, v17.s}[0], [%[src]], #0x08")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("str         s16, [%[dst]], #0x04")
                __ASM_EMIT("b.ge        9b")

                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fadd        v4.4s, v4.4s, v18.4s")
                __ASM_EMIT("fadd        v6.4s, v6.4s, v19.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]], #0x04")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("fsub        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fsub        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fsub        v4.4s, v4.4s, v18.4s")
                __ASM_EMIT("fsub        v6.4s, v6.4s, v19.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("fsub        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fsub        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("fsub        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]], #0x04")
                __ASM_EMIT("fsub        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("fsub        v0.4s, v16.4s, v0.4s")
                __ASM_EMIT("fneg        v1.4s, v1.4s")
                __ASM_EMIT("fsub        v2.4s, v17.4s, v2.4s")
                __ASM_EMIT("fneg        v3.4s, v3.4s")
                __ASM_EMIT("fsub        v4.4s, v18.4s, v4.4s")
                __ASM_EMIT("fneg        v5.4s, v5.4s")
                __ASM_EMIT("fsub        v6.4s, v19.4s, v6.4s")
                __ASM_EMIT("fneg        v7.4s, v7.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("fsub        v0.4s, v16.4s, v0.4s")
                __ASM_EMIT("fneg        v1.4s, v1.4s")
                __ASM_EMIT("fsub        v2.4s, v17.4s, v2.4s")
                __ASM_EMIT("fneg        v3.4s, v3.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("fsub        v0.4s, v16.4s, v0.4s")
                __ASM_EMIT("fneg        v1.4s, v1.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]], #0x04")
                __ASM_EMIT("fsub        v0.4s, v16.4s, v0.4s")
                __ASM_EMIT("fneg        v1.4s, v1.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v18.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v19.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("fmul        v3.4s, v3.4s, v17.4s")
                __ASM_EMIT("fmul        v5.4s, v5.4s, v18.4s")
                __ASM_EMIT("fmul        v7.4s, v7.4s, v19.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld1         {v16.4s-v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("fmul        v3.4s, v3.4s, v17.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]], #0x04")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void pcomplex_r2c_div2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld1         {v16.4s-v19.4s}, [%[src]], #0x40")
                __ASM_EMIT("frecpe      v0.4s, v16.4s")                     /* v0.4s = s2 */
                __ASM_EMIT("frecpe      v1.4s, v17.4s")
                __ASM_EMIT("frecpe      v2.4s, v18.4s")
                __ASM_EMIT("frecpe      v3.4s, v19.4s")
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2) */
                __ASM_EMIT("frecps      v5.4s, v1.4s, v17.4s")
                __ASM_EMIT("frecps      v6.4s, v2.4s, v18.4s")
                __ASM_EMIT("frecps      v7.4s, v3.4s, v19.4s")
                __ASM_EMIT("fmul        v0.4s, v4.4s, v0.4s")               /* v0.4s = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul        v1.4s, v5.4s, v1.4s")
                __ASM_EMIT("fmul        v2.4s, v6.4s, v2.4s")
                __ASM_EMIT("fmul        v3.4s, v7.4s, v3.4s")
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2') */
                __ASM_EMIT("frecps      v5.4s, v1.4s, v17.4s")
                __ASM_EMIT("frecps      v6.4s, v2.4s, v18.4s")
                __ASM_EMIT("frecps      v7.4s, v3.4s, v19.4s")
                __ASM_EMIT("fmul        v16.4s, v4.4s, v0.4s")              /* v16s = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul        v17.4s, v5.4s, v1.4s")
                __ASM_EMIT("fmul        v18.4s, v6.4s, v2.4s")
                __ASM_EMIT("fmul        v19.4s, v7.4s, v3.4s")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v18.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v19.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("fmul        v3.4s, v3.4s, v17.4s")
                __ASM_EMIT("fmul        v5.4s, v5.4s, v18.4s")
                __ASM_EMIT("fmul        v7.4s, v7.4s, v19.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x80")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v4.4s, v5.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v6.4s, v7.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld1         {v16.4s-v17.4s}, [%[src]], #0x20")
                __ASM_EMIT("frecpe      v0.4s, v16.4s")                     /* v0.4s = s2 */
                __ASM_EMIT("frecpe      v1.4s, v17.4s")
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2) */
                __ASM_EMIT("frecps      v5.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmul        v0.4s, v4.4s, v0.4s")               /* v0.4s = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul        v1.4s, v5.4s, v1.4s")
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2') */
                __ASM_EMIT("frecps      v5.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmul        v16.4s, v4.4s, v0.4s")              /* v16s = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul        v17.4s, v5.4s, v1.4s")
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("ld2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v2.4s, v2.4s, v17.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("fmul        v3.4s, v3.4s, v17.4s")
                __ASM_EMIT("sub         %[dst], %[dst], #0x40")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v2.4s, v3.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld1         {v16.4s}, [%[src]], #0x10")
                __ASM_EMIT("frecpe      v0.4s, v16.4s")                     /* v0.4s = s2 */
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2) */
                __ASM_EMIT("fmul        v0.4s, v4.4s, v0.4s")               /* v0.4s = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2') */
                __ASM_EMIT("fmul        v16.4s, v4.4s, v0.4s")              /* v16s = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("ld2         {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("st2         {v0.4s, v1.4s}, [%[dst]], #0x20")
                // x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r        {v16.4s}, [%[src]], #0x04")
                __ASM_EMIT("frecpe      v0.4s, v16.4s")                     /* v0.4s = s2 */
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2) */
                __ASM_EMIT("fmul        v0.4s, v4.4s, v0.4s")               /* v0.4s = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps      v4.4s, v0.4s, v16.4s")              /* v4.4s = (2 - R*s2') */
                __ASM_EMIT("fmul        v16.4s, v4.4s, v0.4s")              /* v16s = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("ld2r        {v0.4s, v1.4s}, [%[dst]]")
                __ASM_EMIT("fmul        v0.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmul        v1.4s, v1.4s, v16.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v0.s, v1.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19"
            );
        }

        void pcomplex_r2c_rdiv2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // x16 blocks
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("ld2         {v20.4s, v21.4s}, [%[dst]], #0x20")     // v20  = sr3, v21 = si3
                __ASM_EMIT("ld2         {v22.4s, v23.4s}, [%[dst]], #0x20")     // v22  = sr4, v23 = si4

                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul        v26.4s, v20.4s, v20.4s")
                __ASM_EMIT("fmul        v27.4s, v22.4s, v22.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmla        v26.4s, v21.4s, v21.4s")
                __ASM_EMIT("fmla        v27.4s, v23.4s, v23.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fneg        v21.4s, v21.4s")
                __ASM_EMIT("fneg        v23.4s, v23.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v20.4s, v20.4s, v26.4s")
                __ASM_EMIT("fdiv        v22.4s, v22.4s, v27.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")
                __ASM_EMIT("fdiv        v21.4s, v21.4s, v26.4s")
                __ASM_EMIT("fdiv        v23.4s, v23.4s, v27.4s")

                __ASM_EMIT("ld1         {v0.4s-v3.4s}, [%[src]], #0x40")
                __ASM_EMIT("sub         %[dst], %[dst], 0x80")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmul        v18.4s, v18.4s, v1.4s")
                __ASM_EMIT("fmul        v20.4s, v20.4s, v2.4s")
                __ASM_EMIT("fmul        v22.4s, v22.4s, v3.4s")
                __ASM_EMIT("fmul        v17.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmul        v19.4s, v19.4s, v1.4s")
                __ASM_EMIT("fmul        v21.4s, v21.4s, v2.4s")
                __ASM_EMIT("fmul        v23.4s, v23.4s, v3.4s")
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v20.4s, v21.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v22.4s, v23.4s}, [%[dst]], #0x20")
                __ASM_EMIT("b.hs        1b")

                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]], #0x20")     // v16  = sr1, v17 = si1
                __ASM_EMIT("ld2         {v18.4s, v19.4s}, [%[dst]], #0x20")     // v18  = sr2, v19 = si2
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmul        v25.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fmla        v25.4s, v19.4s, v19.4s")
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fneg        v19.4s, v19.4s")
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v18.4s, v18.4s, v25.4s")
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v19.4s, v19.4s, v25.4s")
                __ASM_EMIT("ld1         {v0.4s-v1.4s}, [%[src]], #0x20")
                __ASM_EMIT("sub         %[dst], %[dst], 0x40")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmul        v18.4s, v18.4s, v1.4s")
                __ASM_EMIT("fmul        v17.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmul        v19.4s, v19.4s, v1.4s")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                __ASM_EMIT("st2         {v18.4s, v19.4s}, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ld2         {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17 = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("ld1         {v0.4s}, [%[src]], #0x10")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmul        v17.4s, v17.4s, v0.4s")
                __ASM_EMIT("st2         {v16.4s, v17.4s}, [%[dst]], #0x20")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld2r        {v16.4s, v17.4s}, [%[dst]]")            // v16  = sr1, v17  = si1
                __ASM_EMIT("fmul        v24.4s, v16.4s, v16.4s")                // v24   = sr*sr
                __ASM_EMIT("fmla        v24.4s, v17.4s, v17.4s")                // v24  = sr*dr + si*di
                __ASM_EMIT("fneg        v17.4s, v17.4s")                        // v17  = -si
                __ASM_EMIT("fdiv        v16.4s, v16.4s, v24.4s")                // v16  = sr/(sr*dr + si*di)
                __ASM_EMIT("fdiv        v17.4s, v17.4s, v24.4s")                // v17  = -si/(sr*dr + si*di)
                __ASM_EMIT("ld1r        {v0.4s}, [%[src]], #0x04")
                __ASM_EMIT("fmul        v16.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmul        v17.4s, v17.4s, v0.4s")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st2         {v16.s, v17.s}[0], [%[dst]], #0x08")
                __ASM_EMIT("b.ge        7b")

                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  // "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", // Avoid usage if possible
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static float pcomplex_corr_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-36f),
                LSP_DSP_VEC4(1e-36f)
            };
        );

        void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            /*
             * src1[i] = a + j*b, src2[i] = c + j*d
             * den  = (a*a + b*b)*(c*c + d*d)
             * nom  = a*c + b*d
             * corr[i] = (den > threshold) ? nom / sqrt(den) : 0.0
             */
            ARCH_AARCH64_ASM
            (
                // x8 blocks
                __ASM_EMIT("ldp             q14, q15, [%[CC]]")                 /* v14-v15 = threshold */
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2             {v0.4s, v1.4s}, [%[src1]], #0x20")  /* v0   = A0, v1 = B0 */
                __ASM_EMIT("ld2             {v2.4s, v3.4s}, [%[src2]], #0x20")  /* v2   = C0, v3 = D0 */
                __ASM_EMIT("ld2             {v4.4s, v5.4s}, [%[src1]], #0x20")  /* v4   = A1, v5 = B1 */
                __ASM_EMIT("ld2             {v6.4s, v7.4s}, [%[src2]], #0x20")  /* v6   = C1, v7 = D1 */
                __ASM_EMIT("fmul            v8.4s, v0.4s, v2.4s")               /* v8   = A0*C0 */
                __ASM_EMIT("fmul            v9.4s, v4.4s, v6.4s")               /* v9   = A1*C1 */
                __ASM_EMIT("fmul            v10.4s, v0.4s, v0.4s")              /* v10  = A0*A0 */
                __ASM_EMIT("fmul            v11.4s, v4.4s, v4.4s")              /* v11  = A1*A1 */
                __ASM_EMIT("fmul            v12.4s, v2.4s, v2.4s")              /* v12  = C0*C0 */
                __ASM_EMIT("fmul            v13.4s, v6.4s, v6.4s")              /* v13  = C1*C1 */
                __ASM_EMIT("fmla            v8.4s, v1.4s, v3.4s")               /* v8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("fmla            v9.4s, v5.4s, v7.4s")               /* v9   = nom1 = A1*C1 + B1*D1 */
                __ASM_EMIT("fmla            v10.4s, v1.4s, v1.4s")              /* v10  = A0*A0 + B0*B0 */
                __ASM_EMIT("fmla            v11.4s, v5.4s, v5.4s")              /* v11  = A1*A1 + B1*B1 */
                __ASM_EMIT("fmla            v12.4s, v3.4s, v3.4s")              /* v12  = C0*C0 + D0*D0 */
                __ASM_EMIT("fmla            v13.4s, v7.4s, v7.4s")              /* v13  = C1*C1 + D1*D1 */
                __ASM_EMIT("fmul            v0.4s, v10.4s, v12.4s")             /* v0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("fmul            v1.4s, v11.4s, v13.4s")             /* v1   = den1 = (A1*A1 + B1*B1)*(C1*C1 + D1*D1) */
                __ASM_EMIT("frsqrte         v2.4s, v0.4s")                      /* v2   = x0 */
                __ASM_EMIT("frsqrte         v3.4s, v1.4s")
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x0 */
                __ASM_EMIT("fmul            v5.4s, v3.4s, v1.4s")
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6   = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("frsqrts         v7.4s, v5.4s, v3.4s")
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x1 */
                __ASM_EMIT("fmul            v5.4s, v3.4s, v1.4s")
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6    = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("frsqrts         v7.4s, v5.4s, v3.4s")
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2    = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")               /* v3    = 1/den1 */
                __ASM_EMIT("fcmge           v0.4s, v0.4s, v14.4s")              /* v0    = (den0 >= threshold) */
                __ASM_EMIT("fcmge           v1.4s, v1.4s, v15.4s")              /* v1    = (den1 >= threshold) */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v8.4s")               /* v2    = nom0/den0 */
                __ASM_EMIT("fmul            v3.4s, v3.4s, v9.4s")               /* v3    = nom1/den1 */
                __ASM_EMIT("and             v0.16b, v0.16b, v2.16b")            /* v0    = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("and             v1.16b, v1.16b, v3.16b")            /* v1    = (den1 >= threshold) ? nom1/den1 : 0.0f */
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs            1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld2             {v0.4s, v1.4s}, [%[src1]], #0x20")  /* v0   = A0, v1 = B0 */
                __ASM_EMIT("ld2             {v2.4s, v3.4s}, [%[src2]], #0x20")  /* v2   = C0, v3 = D0 */
                __ASM_EMIT("fmul            v8.4s, v0.4s, v2.4s")               /* v8   = A0*C0 */
                __ASM_EMIT("fmul            v10.4s, v0.4s, v0.4s")              /* v10  = A0*A0 */
                __ASM_EMIT("fmul            v12.4s, v2.4s, v2.4s")              /* v12  = C0*C0 */
                __ASM_EMIT("fmla            v8.4s, v1.4s, v3.4s")               /* v8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("fmla            v10.4s, v1.4s, v1.4s")              /* v10  = A0*A0 + B0*B0 */
                __ASM_EMIT("fmla            v12.4s, v3.4s, v3.4s")              /* v12  = C0*C0 + D0*D0 */
                __ASM_EMIT("fmul            v0.4s, v10.4s, v12.4s")             /* v0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("frsqrte         v2.4s, v0.4s")                      /* v2   = x0 */
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x0 */
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6   = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x1 */
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6    = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2    = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fcmge           v0.4s, v0.4s, v14.4s")              /* v0    = (den0 >= threshold) */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v8.4s")               /* v2    = nom0/den0 */
                __ASM_EMIT("and             v0.16b, v0.16b, v2.16b")            /* v0    = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                // x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("ld2r            {v0.4s, v1.4s}, [%[src1]], #0x08")  /* v0   = A0, v1 = B0 */
                __ASM_EMIT("ld2r            {v2.4s, v3.4s}, [%[src2]], #0x08")  /* v2   = C0, v3 = D0 */
                __ASM_EMIT("fmul            v8.4s, v0.4s, v2.4s")               /* v8   = A0*C0 */
                __ASM_EMIT("fmul            v10.4s, v0.4s, v0.4s")              /* v10  = A0*A0 */
                __ASM_EMIT("fmul            v12.4s, v2.4s, v2.4s")              /* v12  = C0*C0 */
                __ASM_EMIT("fmla            v8.4s, v1.4s, v3.4s")               /* v8   = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("fmla            v10.4s, v1.4s, v1.4s")              /* v10  = A0*A0 + B0*B0 */
                __ASM_EMIT("fmla            v12.4s, v3.4s, v3.4s")              /* v12  = C0*C0 + D0*D0 */
                __ASM_EMIT("fmul            v0.4s, v10.4s, v12.4s")             /* v0   = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("frsqrte         v2.4s, v0.4s")                      /* v2   = x0 */
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x0 */
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6   = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul            v4.4s, v2.4s, v0.4s")               /* v4   = R * x1 */
                __ASM_EMIT("frsqrts         v6.4s, v4.4s, v2.4s")               /* v6    = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")               /* v2    = 1/den0 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fcmge           v0.4s, v0.4s, v14.4s")              /* v0    = (den0 >= threshold) */
                __ASM_EMIT("fmul            v2.4s, v2.4s, v8.4s")               /* v2    = nom0/den0 */
                __ASM_EMIT("and             v0.16b, v0.16b, v2.16b")            /* v0    = (den0 >= threshold) ? nom0/den0 : 0.0f */
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            5b")
                // End
                __ASM_EMIT("6:")

                : [dst] "+r" (dst_corr), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
                : [CC] "r" (&pcomplex_corr_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PCOMPLEX_H_ */
