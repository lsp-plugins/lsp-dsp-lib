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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        void axis_apply_lin1(float *x, const float *v, float zero, float norm, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld1r        {v24.4s}, [%[zero]]")           // v24 = zero
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("ld1r        {v25.4s}, [%[norm]]")           // v25 = norm
                __ASM_EMIT("blo         2f")
                /* 32x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q0, q1, [%[x], #0x00]")         // v0   = x
                __ASM_EMIT("ldp         q2, q3, [%[x], #0x20]")
                __ASM_EMIT("ldp         q4, q5, [%[x], #0x40]")
                __ASM_EMIT("ldp         q6, q7, [%[x], #0x60]")
                __ASM_EMIT("ldp         q16, q17, [%[v], #0x00]")       // v16  = v
                __ASM_EMIT("ldp         q18, q19, [%[v], #0x20]")
                __ASM_EMIT("ldp         q20, q21, [%[v], #0x40]")
                __ASM_EMIT("ldp         q22, q23, [%[v], #0x60]")
                __ASM_EMIT("fadd        v16.4s, v16.4s, v24.4s")        // v16 = v + zero
                __ASM_EMIT("fadd        v17.4s, v17.4s, v24.4s")
                __ASM_EMIT("fadd        v18.4s, v18.4s, v24.4s")
                __ASM_EMIT("fadd        v19.4s, v19.4s, v24.4s")
                __ASM_EMIT("fadd        v20.4s, v20.4s, v24.4s")
                __ASM_EMIT("fadd        v21.4s, v21.4s, v24.4s")
                __ASM_EMIT("fadd        v22.4s, v22.4s, v24.4s")
                __ASM_EMIT("fadd        v23.4s, v23.4s, v24.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v25.4s")         // v0   = x + (v + zero) * norm
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla        v4.4s, v20.4s, v25.4s")
                __ASM_EMIT("fmla        v5.4s, v21.4s, v25.4s")
                __ASM_EMIT("fmla        v6.4s, v22.4s, v25.4s")
                __ASM_EMIT("fmla        v7.4s, v23.4s, v25.4s")
                __ASM_EMIT("stp         q0, q1, [%[x], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[x], #0x20]")
                __ASM_EMIT("stp         q4, q5, [%[x], #0x40]")
                __ASM_EMIT("stp         q6, q7, [%[x], #0x60]")
                __ASM_EMIT("subs        %[count], %[count], #32")
                __ASM_EMIT("add         %[x], %[x], #0x80")
                __ASM_EMIT("add         %[v], %[v], #0x80")
                __ASM_EMIT("bhs         1b")
                /* 16x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("adds        %[count], %[count], #16") /* 32-16 */
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("ldp         q0, q1, [%[x], #0x00]")
                __ASM_EMIT("ldp         q2, q3, [%[x], #0x20]")
                __ASM_EMIT("ldp         q16, q17, [%[v], #0x00]")
                __ASM_EMIT("ldp         q18, q19, [%[v], #0x20]")
                __ASM_EMIT("fadd        v16.4s, v16.4s, v24.4s")        // v16 = v + zero
                __ASM_EMIT("fadd        v17.4s, v17.4s, v24.4s")
                __ASM_EMIT("fadd        v18.4s, v18.4s, v24.4s")
                __ASM_EMIT("fadd        v19.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v25.4s")         // v0   = x + (v + zero) * norm
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla        v2.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla        v3.4s, v19.4s, v25.4s")
                __ASM_EMIT("stp         q0, q1, [%[x], #0x00]")
                __ASM_EMIT("stp         q2, q3, [%[x], #0x20]")
                __ASM_EMIT("sub         %[count], %[count], #16")
                __ASM_EMIT("add         %[x], %[x], #0x40")
                __ASM_EMIT("add         %[v], %[v], #0x40")
                /* 8x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("adds        %[count], %[count], #8") /* 16-8 */
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("ldp         q0, q1, [%[x], #0x00]")
                __ASM_EMIT("ldp         q16, q17, [%[v], #0x00]")
                __ASM_EMIT("fadd        v16.4s, v16.4s, v24.4s")        // v16 = v + zero
                __ASM_EMIT("fadd        v17.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla        v0.4s, v16.4s, v25.4s")         // v0   = x + (v + zero) * norm
                __ASM_EMIT("fmla        v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("stp         q0, q1, [%[x], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[x], %[x], #0x20")
                __ASM_EMIT("add         %[v], %[v], #0x20")
                /* 4x block */
                __ASM_EMIT("6:")
                __ASM_EMIT("adds        %[count], %[count], #4") /* 8 - 4 */
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("ldr         q0, [%[x], #0x00]")
                __ASM_EMIT("ldr         q16, [%[v], #0x00]")
                __ASM_EMIT("fadd        v16.4s, v16.4s, v24.4s")        // v16 = v + zero
                __ASM_EMIT("fmla        v0.4s, v16.4s, v25.4s")         // v0   = x + (v + zero) * norm
                __ASM_EMIT("str         q0, [%[x], #0x00]")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[x], %[x], #0x10")
                __ASM_EMIT("add         %[v], %[v], #0x10")
                /* 1x block */
                __ASM_EMIT("8:")
                __ASM_EMIT("adds        %[count], %[count], #3") /* 4 - 3 */
                __ASM_EMIT("blt         10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1r        {v0.4s}, [%[x]]")
                __ASM_EMIT("ld1r        {v16.4s}, [%[v]]")
                __ASM_EMIT("fadd        v16.4s, v16.4s, v24.4s")        // v16 = v + zero
                __ASM_EMIT("fmla        v0.4s, v16.4s, v25.4s")         // v0   = x + (v + zero) * norm
                __ASM_EMIT("st1         {v0.s}[0], [%[x]]")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[x], %[x], #0x04")
                __ASM_EMIT("add         %[v], %[v], #0x04")
                __ASM_EMIT("bge         9b")
                __ASM_EMIT("10:")
                : [x] "+r" (x), [v] "+r" (v),
                  [count] "+r" (count)
                : [zero] "r" (&zero),
                  [norm] "r" (&norm)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25"
            );
        }

        IF_ARCH_AARCH64(
            static const uint32_t LOG_IARGS[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x007fffff),           // X_MANT
                LSP_DSP_VEC4(0x0000007f)            // X_MMASK
            };

            static const float LOG_FARGS[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-8),                  // X_AMP
                LSP_DSP_VEC4(0.5f),                  // X_HALF
                LSP_DSP_VEC4(M_SQRT1_2),             // SQRT1_2
                LSP_DSP_VEC4(1.0f),                  // ONE
                LSP_DSP_VEC4(7.0376836292E-2),       // C0
                LSP_DSP_VEC4(-1.1514610310E-1),      // C1
                LSP_DSP_VEC4(1.1676998740E-1),       // C2
                LSP_DSP_VEC4(-1.2420140846E-1),      // C3
                LSP_DSP_VEC4(+1.4249322787E-1),      // C4
                LSP_DSP_VEC4(-1.6668057665E-1),      // C5
                LSP_DSP_VEC4(+2.0000714765E-1),      // C6
                LSP_DSP_VEC4(-2.4999993993E-1),      // C7
                LSP_DSP_VEC4(+3.3333331174E-1),      // C8
                LSP_DSP_VEC4(-0.5f),                 // C9
                LSP_DSP_VEC4(M_LN2 - 2.12194440e-4)  // LXE + LN2
            };
        )

        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count)
        {
            IF_ARCH_AARCH64(
                float params[2] __lsp_aligned16;
                params[0] = zero;
                params[1] = norm_x;
            );

            //---------------------------------------------------------------
            // Prepare constants
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld2r            {v24.4s, v25.4s}, [%[params]]") // v24  = zero, v25  = norm_x
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")

                // Do 16x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q4, q5, [%[v], 0x00]")          // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q6, q7, [%[v], 0x20]")          // v6   = v[8..11], v7 = v[12..15]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x40")             // v   += 16
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fabs            v6.4s, v6.4s")
                __ASM_EMIT("fabs            v7.4s, v7.4s")
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmax            v5.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmax            v6.4s, v6.4s, v12.4s")
                __ASM_EMIT("fmax            v7.4s, v7.4s, v12.4s")
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                __ASM_EMIT("fmul            v1.4s, v5.4s, v24.4s")
                __ASM_EMIT("fmul            v2.4s, v6.4s, v24.4s")
                __ASM_EMIT("fmul            v3.4s, v7.4s, v24.4s")
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("ushr            v5.4s, v1.4s, #23")
                __ASM_EMIT("ushr            v6.4s, v2.4s, #23")
                __ASM_EMIT("ushr            v7.4s, v3.4s, #23")
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("and             v9.16b, v1.16b, v14.16b")
                __ASM_EMIT("and             v10.16b, v2.16b, v14.16b")
                __ASM_EMIT("and             v11.16b, v3.16b, v14.16b")
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("sub             v5.4s, v5.4s, v15.4s")
                __ASM_EMIT("sub             v6.4s, v6.4s, v15.4s")
                __ASM_EMIT("sub             v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("orr             v9.16b, v9.16b, v13.16b")
                __ASM_EMIT("orr             v10.16b, v10.16b, v13.16b")
                __ASM_EMIT("orr             v11.16b, v11.16b, v13.16b")
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                __ASM_EMIT("scvtf           v5.4s, v5.4s")
                __ASM_EMIT("scvtf           v6.4s, v6.4s")
                __ASM_EMIT("scvtf           v7.4s, v7.4s")
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v1.4s, v9.4s, v12.4s")
                __ASM_EMIT("fcmge           v2.4s, v10.4s, v12.4s")
                __ASM_EMIT("fcmge           v3.4s, v11.4s, v12.4s")
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("bic             v1.16b, v9.16b, v1.16b")
                __ASM_EMIT("bic             v2.16b, v10.16b, v2.16b")
                __ASM_EMIT("bic             v3.16b, v11.16b, v3.16b")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fadd            v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fadd            v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fcmge           v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fcmge           v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("fsub            v1.4s, v1.4s, v13.4s")
                __ASM_EMIT("fsub            v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fsub            v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("and             v9.16b, v13.16b, v9.16b")
                __ASM_EMIT("and             v10.16b, v13.16b, v10.16b")
                __ASM_EMIT("and             v11.16b, v13.16b, v11.16b")
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v5.4s, v5.4s, v9.4s")
                __ASM_EMIT("fadd            v6.4s, v6.4s, v10.4s")
                __ASM_EMIT("fadd            v7.4s, v7.4s, v11.4s")
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fmul            v9.4s, v1.4s, v14.4s")
                __ASM_EMIT("fmul            v10.4s, v2.4s, v14.4s")
                __ASM_EMIT("fmul            v11.4s, v3.4s, v14.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v14.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v14.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v9.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmla            v10.4s, v6.4s, v12.4s")
                __ASM_EMIT("fmla            v11.4s, v7.4s, v12.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v3.4s")

                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldp             q4, q5, [%[x], 0x00]")          // v4-v5 = x
                __ASM_EMIT("ldp             q6, q7, [%[x], 0x20]")          // v6-v7 = x
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v5.4s, v9.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v10.4s, v25.4s")
                __ASM_EMIT("fmla            v7.4s, v11.4s, v25.4s")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q4, q5, [%[x], 0x00]")
                __ASM_EMIT("stp             q6, q7, [%[x], 0x20]")
                __ASM_EMIT("add             %[x], %[x], #0x40")
                __ASM_EMIT("b.ge            1b")

                // Do 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q4, q5, [%[v], 0x00]")          // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x20")             // v   += 8
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmax            v5.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                __ASM_EMIT("fmul            v1.4s, v5.4s, v24.4s")
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("ushr            v5.4s, v1.4s, #23")
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("and             v9.16b, v1.16b, v14.16b")
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("sub             v5.4s, v5.4s, v15.4s")
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("orr             v9.16b, v9.16b, v13.16b")
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                __ASM_EMIT("scvtf           v5.4s, v5.4s")
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v1.4s, v9.4s, v12.4s")
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("bic             v1.16b, v9.16b, v1.16b")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("fsub            v1.4s, v1.4s, v13.4s")
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("and             v9.16b, v13.16b, v9.16b")
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v5.4s, v5.4s, v9.4s")
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fmul            v9.4s, v1.4s, v14.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v9.4s, v5.4s, v12.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v1.4s")
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldp             q4, q5, [%[x], 0x00]")          // v4-v5 = x
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v5.4s, v9.4s, v25.4s")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q4, q5, [%[x], 0x00]")
                __ASM_EMIT("add             %[x], %[x], #0x20")

                // Do x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("ldr             q4, [%[v], 0x00]")              // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x10")             // v   += 4
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldr             q4, [%[x], 0x00]")              // v4   = x
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("str             q4, [%[x], 0x00]")
                __ASM_EMIT("add             %[x], %[x], #0x10")

                // Do x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")

                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v4.4s}, [%[v]]")               // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x04")             // v   ++
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")          // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ld1r            {v4.4s}, [%[x]]")               // v4   = x
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4   = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v4.s}[0], [%[x]]")
                __ASM_EMIT("add             %[x], %[x], #0x04")
                __ASM_EMIT("b.ge            7b")

                __ASM_EMIT("8:")

                : [v] "+r" (v), [x] "+r" (x),
                  [count] "+r" (count)
                : [params] "r" (&params[0]),
                  [iptr] "r" (&LOG_IARGS[0]),
                  [fptr] "r" (&LOG_FARGS[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v24", "v25"
            );
        }

        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
        {
            IF_ARCH_AARCH64(
                float params[3] __lsp_aligned16;
                params[0] = zero;
                params[1] = norm_x;
                params[2] = norm_y;
            );

            //---------------------------------------------------------------
            // Prepare constants
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld3r            {v24.4s, v25.4s, v26.4s}, [%[params]]")   // v24  = zero
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")

                // Do 16x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q4, q5, [%[v], 0x00]")          // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q6, q7, [%[v], 0x20]")          // v6   = v[8..11], v7 = v[12..15]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x40")             // v   += 16
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fabs            v6.4s, v6.4s")
                __ASM_EMIT("fabs            v7.4s, v7.4s")
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmax            v5.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmax            v6.4s, v6.4s, v12.4s")
                __ASM_EMIT("fmax            v7.4s, v7.4s, v12.4s")
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                __ASM_EMIT("fmul            v1.4s, v5.4s, v24.4s")
                __ASM_EMIT("fmul            v2.4s, v6.4s, v24.4s")
                __ASM_EMIT("fmul            v3.4s, v7.4s, v24.4s")
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("ushr            v5.4s, v1.4s, #23")
                __ASM_EMIT("ushr            v6.4s, v2.4s, #23")
                __ASM_EMIT("ushr            v7.4s, v3.4s, #23")
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("and             v9.16b, v1.16b, v14.16b")
                __ASM_EMIT("and             v10.16b, v2.16b, v14.16b")
                __ASM_EMIT("and             v11.16b, v3.16b, v14.16b")
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("sub             v5.4s, v5.4s, v15.4s")
                __ASM_EMIT("sub             v6.4s, v6.4s, v15.4s")
                __ASM_EMIT("sub             v7.4s, v7.4s, v15.4s")
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("orr             v9.16b, v9.16b, v13.16b")
                __ASM_EMIT("orr             v10.16b, v10.16b, v13.16b")
                __ASM_EMIT("orr             v11.16b, v11.16b, v13.16b")
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                __ASM_EMIT("scvtf           v5.4s, v5.4s")
                __ASM_EMIT("scvtf           v6.4s, v6.4s")
                __ASM_EMIT("scvtf           v7.4s, v7.4s")
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v1.4s, v9.4s, v12.4s")
                __ASM_EMIT("fcmge           v2.4s, v10.4s, v12.4s")
                __ASM_EMIT("fcmge           v3.4s, v11.4s, v12.4s")
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("bic             v1.16b, v9.16b, v1.16b")
                __ASM_EMIT("bic             v2.16b, v10.16b, v2.16b")
                __ASM_EMIT("bic             v3.16b, v11.16b, v3.16b")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fadd            v2.4s, v2.4s, v10.4s")
                __ASM_EMIT("fadd            v3.4s, v3.4s, v11.4s")
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fcmge           v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fcmge           v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("fsub            v1.4s, v1.4s, v13.4s")
                __ASM_EMIT("fsub            v2.4s, v2.4s, v13.4s")
                __ASM_EMIT("fsub            v3.4s, v3.4s, v13.4s")
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("and             v9.16b, v13.16b, v9.16b")
                __ASM_EMIT("and             v10.16b, v13.16b, v10.16b")
                __ASM_EMIT("and             v11.16b, v13.16b, v11.16b")
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v5.4s, v5.4s, v9.4s")
                __ASM_EMIT("fadd            v6.4s, v6.4s, v10.4s")
                __ASM_EMIT("fadd            v7.4s, v7.4s, v11.4s")
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fmul            v9.4s, v1.4s, v14.4s")
                __ASM_EMIT("fmul            v10.4s, v2.4s, v14.4s")
                __ASM_EMIT("fmul            v11.4s, v3.4s, v14.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v14.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v12.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v13.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v14.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v15.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v15.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmul            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fmul            v11.4s, v11.4s, v3.4s")
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v9.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmla            v10.4s, v6.4s, v12.4s")
                __ASM_EMIT("fmla            v11.4s, v7.4s, v12.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v10.4s, v10.4s, v2.4s")
                __ASM_EMIT("fadd            v11.4s, v11.4s, v3.4s")

                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldp             q4, q5, [%[x], 0x00]")          // v4-v5 = x
                __ASM_EMIT("ldp             q6, q7, [%[x], 0x20]")          // v6-v7 = x
                __ASM_EMIT("ldp             q0, q1, [%[y], 0x00]")          // v0-v1 = y
                __ASM_EMIT("ldp             q2, q3, [%[y], 0x20]")          // v2-v3 = y
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v5.4s, v9.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v10.4s, v25.4s")
                __ASM_EMIT("fmla            v7.4s, v11.4s, v25.4s")
                __ASM_EMIT("fmla            v0.4s, v8.4s, v26.4s")          // v0 = y + log(abs(v*zero)) * norm_y
                __ASM_EMIT("fmla            v1.4s, v9.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v10.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v11.4s, v26.4s")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q4, q5, [%[x], 0x00]")
                __ASM_EMIT("stp             q6, q7, [%[x], 0x20]")
                __ASM_EMIT("stp             q0, q1, [%[y], 0x00]")
                __ASM_EMIT("stp             q2, q3, [%[y], 0x20]")
                __ASM_EMIT("add             %[x], %[x], #0x40")
                __ASM_EMIT("add             %[y], %[y], #0x40")
                __ASM_EMIT("b.ge            1b")

                // Do 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q4, q5, [%[v], 0x00]")          // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x20")             // v   += 8
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmax            v5.4s, v5.4s, v12.4s")
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                __ASM_EMIT("fmul            v1.4s, v5.4s, v24.4s")
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("ushr            v5.4s, v1.4s, #23")
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("and             v9.16b, v1.16b, v14.16b")
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("sub             v5.4s, v5.4s, v15.4s")
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("orr             v9.16b, v9.16b, v13.16b")
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                __ASM_EMIT("scvtf           v5.4s, v5.4s")
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v1.4s, v9.4s, v12.4s")
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("bic             v1.16b, v9.16b, v1.16b")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v1.4s, v1.4s, v9.4s")
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fcmge           v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("fsub            v1.4s, v1.4s, v13.4s")
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("and             v9.16b, v13.16b, v9.16b")
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v5.4s, v5.4s, v9.4s")
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fmul            v9.4s, v1.4s, v14.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v12.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v13.4s")
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v14.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v9.4s, v9.4s, v15.4s")
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmul            v9.4s, v9.4s, v1.4s")
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v9.4s, v5.4s, v12.4s")
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                __ASM_EMIT("fadd            v9.4s, v9.4s, v1.4s")
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldp             q4, q5, [%[x], 0x00]")          // v4-v5 = x
                __ASM_EMIT("ldp             q0, q1, [%[y], 0x00]")          // v0-v1 = y
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v5.4s, v9.4s, v25.4s")
                __ASM_EMIT("fmla            v0.4s, v8.4s, v26.4s")          // v0 = y + log(abs(v*zero)) * norm_y
                __ASM_EMIT("fmla            v1.4s, v9.4s, v26.4s")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("stp             q4, q5, [%[x], 0x00]")
                __ASM_EMIT("stp             q0, q1, [%[y], 0x00]")
                __ASM_EMIT("add             %[x], %[x], #0x20")
                __ASM_EMIT("add             %[y], %[y], #0x20")

                // Do x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("ldr             q4, [%[v], 0x00]")              // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x10")             // v   += 4
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ldr             q4, [%[x], 0x00]")              // v4-v5 = x
                __ASM_EMIT("ldr             q0, [%[y], 0x00]")              // v0-v1 = y
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4 = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v0.4s, v8.4s, v26.4s")          // v0 = y + log(abs(v*zero)) * norm_y
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("str             q4, [%[x], 0x00]")
                __ASM_EMIT("str             q0, [%[y], 0x00]")
                __ASM_EMIT("add             %[x], %[x], #0x10")
                __ASM_EMIT("add             %[y], %[y], #0x10")

                // Do x1 blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")

                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v4.4s}, [%[v]]")               // v4   = v[0..3], v5 = v[4..7]
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x00]")     // v12  = X_AMP, v13 = 0.5f
                __ASM_EMIT("add             %[v], %[v], #0x04")             // v   ++
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  // v4   = abs(v)
                __ASM_EMIT("fmax            v4.4s, v4.4s, v12.4s")          // v12  = max(X_AMP, abs(v)), ignores denormalized values
                __ASM_EMIT("fmul            v0.4s, v4.4s, v24.4s")          // v0   = X = max(X_AMP, abs(v)) * zero
                // Step 2: parse float value
                __ASM_EMIT("ldp             q14, q15, [%[iptr], 0x00]")     // v14  = MANT_MASK, v15 = FRAC_SUB = 127
                __ASM_EMIT("ushr            v4.4s, v0.4s, #23")             // v4   = frac(X)
                __ASM_EMIT("and             v8.16b, v0.16b, v14.16b")       // v8   = mant(X)
                __ASM_EMIT("sub             v4.4s, v4.4s, v15.4s")          // v4   = frac(X) - 127
                __ASM_EMIT("orr             v8.16b, v8.16b, v13.16b")       // v8   = V = mant(X) + 0.5
                __ASM_EMIT("scvtf           v4.4s, v4.4s")                  // v4   = E = float(frac(X) - 127)
                // Prepare logarithm approximation calculations
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x20]")     // v12  = sqrt(1/2), v13 = 1.0
                __ASM_EMIT("fcmge           v0.4s, v8.4s, v12.4s")          // v0   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("bic             v0.16b, v8.16b, v0.16b")        // v0   = V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           // v0   = V + V * [ V < sqrt(1/2) ]
                __ASM_EMIT("fcmge           v8.4s, v8.4s, v12.4s")          // v8   = [ V >= sqrt(1/2) ]
                __ASM_EMIT("fsub            v0.4s, v0.4s, v13.4s")          // v0   = A = V + V * [ V < sqrt(1/2) ] - 1.0
                __ASM_EMIT("and             v8.16b, v13.16b, v8.16b")       // v8   = 1.0 & [ V >= sqrt(1/2) ]
                __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s")           // v4   = B = E + 1.0 & [ V >= sqrt(1/2) ]
                // Calculate logarithmic values
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x40]")     // v14  = L0, v15 = L1
                __ASM_EMIT("fmul            v8.4s, v0.4s, v14.4s")          // v8   = L0*A
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L1+L0*A
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0x60]")     // v12  = L2, v13 = L3
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L1+L0*A)
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L2+A*(L1+L0*A)
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L2+A*(L1+L0*A))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L3+A*(L2+A*(L1+L0*A))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0x80]")     // v14  = L4, v15 = L5
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L4+A*(L3+A*(L2+A*(L1+L0*A)))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))
                __ASM_EMIT("ldp             q12, q13, [%[fptr], 0xa0]")     // v12  = L6, v13 = L7
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v12.4s")          // v8   = L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v13.4s")          // v8   = L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))
                __ASM_EMIT("ldp             q14, q15, [%[fptr], 0xc0]")     // v14  = L8, v15 = L9
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v14.4s")          // v8   = L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v15.4s")          // v8   = L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("ldr             q12, [%[fptr], 0xe0]")          // v12  = LXE + LN2
                __ASM_EMIT("fmul            v8.4s, v8.4s, v0.4s")           // v8   = A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fmla            v8.4s, v4.4s, v12.4s")          // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A)))))))))
                __ASM_EMIT("fadd            v8.4s, v8.4s, v0.4s")           // v8   = B*(LXE + LN2) + A*A*(L9+A*(L8+A*(L7+A*(L6+A*(L5+A*(L4+A*(L3+A*(L2+A*(L1+L0*A))))))))) + A
                // Apply values to axes, q0 = log(abs(v*zero))
                __ASM_EMIT("ld1r            {v4.4s}, [%[x]]")               // v4   = x
                __ASM_EMIT("ld1r            {v0.4s}, [%[y]]")               // v0   = y
                __ASM_EMIT("fmla            v4.4s, v8.4s, v25.4s")          // v4   = x + log(abs(v*zero)) * norm_x
                __ASM_EMIT("fmla            v0.4s, v8.4s, v26.4s")          // v0   = y + log(abs(v*zero)) * norm_y
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v4.s}[0], [%[x]]")
                __ASM_EMIT("st1             {v0.s}[0], [%[y]]")
                __ASM_EMIT("add             %[x], %[x], #0x04")
                __ASM_EMIT("add             %[y], %[y], #0x04")
                __ASM_EMIT("b.ge            7b")

                __ASM_EMIT("8:")

                : [v] "+r" (v), [x] "+r" (x), [y] "+r" (y),
                  [count] "+r" (count)
                : [params] "r" (&params[0]),
                  [iptr] "r" (&LOG_IARGS[0]),
                  [fptr] "r" (&LOG_FARGS[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15",
                  "v24", "v25", "v26"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_ */
