/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 нояб. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_PAN_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_PAN_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const float depan_lin_const_f[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(1e-18f)
            };
        );

        void depan_lin(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = fabsf(l[i]);
                const float sr  = fabsf(r[i]);
                const float den = sl + sr;
                dst[i]          = (den >= 1e-18f) ? sr / den : dfl;
            */
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("dup             v31.4s, %[dfl].s[0]")           /* v31  = dfl */
                __ASM_EMIT("ldp             q28, q29, [%[CC], #0x00]")      /* v28, v29 = thresh */
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                /* 16x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[l], #0x00]")         /* v0   = l */
                __ASM_EMIT("ldp             q2, q3, [%[l], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[r], #0x00]")         /* v4   = r */
                __ASM_EMIT("ldp             q6, q7, [%[r], #0x20]")
                __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = fabsf(l) */
                __ASM_EMIT("fabs            v1.4s, v1.4s")
                __ASM_EMIT("fabs            v2.4s, v2.4s")
                __ASM_EMIT("fabs            v3.4s, v3.4s")
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  /* v4   = fabsf(r) */
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fabs            v6.4s, v6.4s")
                __ASM_EMIT("fabs            v7.4s, v7.4s")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fadd            v2.4s, v2.4s, v6.4s")
                __ASM_EMIT("fadd            v3.4s, v3.4s, v7.4s")
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("fcmge           v25.4s, v1.4s, v29.4s")
                __ASM_EMIT("fcmge           v26.4s, v2.4s, v28.4s")
                __ASM_EMIT("fcmge           v27.4s, v3.4s, v29.4s")
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecpe          v17.4s, v1.4s")
                __ASM_EMIT("frecpe          v18.4s, v2.4s")
                __ASM_EMIT("frecpe          v19.4s, v3.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s")
                __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s")
                __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s")
                __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v1.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v2.4s, v22.4s, v18.4s")
                __ASM_EMIT("fmul            v3.4s, v23.4s, v19.4s")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = fabsf(r) / den */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")
                __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("bif             v1.16b, v31.16b, v25.16b")
                __ASM_EMIT("bif             v2.16b, v31.16b, v26.16b")
                __ASM_EMIT("bif             v3.16b, v31.16b, v27.16b")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[l], %[l], #0x40")
                __ASM_EMIT("add             %[r], %[r], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                /* 8x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[l], #0x00]")         /* v0   = l */
                __ASM_EMIT("ldp             q4, q5, [%[r], #0x00]")         /* v4   = r */
                __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = fabsf(l) */
                __ASM_EMIT("fabs            v1.4s, v1.4s")
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  /* v4   = fabsf(r) */
                __ASM_EMIT("fabs            v5.4s, v5.4s")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("fcmge           v25.4s, v1.4s, v29.4s")
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecpe          v17.4s, v1.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v1.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = fabsf(r) / den */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("bif             v1.16b, v31.16b, v25.16b")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[l], %[l], #0x20")
                __ASM_EMIT("add             %[r], %[r], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                /* 4x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q0, [%[l], #0x00]")             /* v0   = l */
                __ASM_EMIT("ldr             q4, [%[r], #0x00]")             /* v4   = r */
                __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = fabsf(l) */
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  /* v4   = fabsf(r) */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = fabsf(r) / den */
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[l], %[l], #0x10")
                __ASM_EMIT("add             %[r], %[r], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                /* 1x blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v0.4s}, [%[l]]")               /* v0   = l */
                __ASM_EMIT("ld1r            {v4.4s}, [%[r]]")               /* v4   = r */
                __ASM_EMIT("fabs            v0.4s, v0.4s")                  /* v0   = fabsf(l) */
                __ASM_EMIT("fabs            v4.4s, v4.4s")                  /* v4   = fabsf(r) */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = fabsf(l) + fabsf(r) */
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = fabsf(r) / den */
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[l], %[l], #0x04")
                __ASM_EMIT("add             %[r], %[r], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [l] "+r" (l), [r] "+r" (r),
                  [count] "+r" (count),
                  [dfl] "+w" (dfl)
                : [CC] "r" (&depan_lin_const_f[0])
                : "cc", "memory",
                  /* "v0", */ "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float depan_eqpow_const_f[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(1e-36f)
            };
        );

        void depan_eqpow(float *dst, const float *l, const float *r, float dfl, size_t count)
        {
            /*
                const float sl  = l[i] * l[i];
                const float sr  = r[i] * r[i];
                const float den = sl + sr;
                dst[i]          = (den >= 1e-36f) ? sr / den : dfl;
            */
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("dup             v31.4s, %[dfl].s[0]")           /* v31  = dfl */
                __ASM_EMIT("ldp             q28, q29, [%[CC], #0x00]")      /* v28, v29 = thresh */
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                /* 16x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[l], #0x00]")         /* v0   = l */
                __ASM_EMIT("ldp             q2, q3, [%[l], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[r], #0x00]")         /* v4   = r */
                __ASM_EMIT("ldp             q6, q7, [%[r], #0x20]")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v0.4s")           /* v0   = l*l */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v1.4s")
                __ASM_EMIT("fmul            v2.4s, v2.4s, v2.4s")
                __ASM_EMIT("fmul            v3.4s, v3.4s, v3.4s")
                __ASM_EMIT("fmul            v4.4s, v4.4s, v4.4s")           /* v4   = r*r */
                __ASM_EMIT("fmul            v5.4s, v5.4s, v5.4s")
                __ASM_EMIT("fmul            v6.4s, v6.4s, v6.4s")
                __ASM_EMIT("fmul            v7.4s, v7.4s, v7.4s")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = l*l + r*r */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fadd            v2.4s, v2.4s, v6.4s")
                __ASM_EMIT("fadd            v3.4s, v3.4s, v7.4s")
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("fcmge           v25.4s, v1.4s, v29.4s")
                __ASM_EMIT("fcmge           v26.4s, v2.4s, v28.4s")
                __ASM_EMIT("fcmge           v27.4s, v3.4s, v29.4s")
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecpe          v17.4s, v1.4s")
                __ASM_EMIT("frecpe          v18.4s, v2.4s")
                __ASM_EMIT("frecpe          v19.4s, v3.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s")
                __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v18.4s, v22.4s, v18.4s")
                __ASM_EMIT("fmul            v19.4s, v23.4s, v19.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("frecps          v22.4s, v18.4s, v2.4s")
                __ASM_EMIT("frecps          v23.4s, v19.4s, v3.4s")
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v1.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v2.4s, v22.4s, v18.4s")
                __ASM_EMIT("fmul            v3.4s, v23.4s, v19.4s")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = r*r / den */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fmul            v2.4s, v2.4s, v6.4s")
                __ASM_EMIT("fmul            v3.4s, v3.4s, v7.4s")
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("bif             v1.16b, v31.16b, v25.16b")
                __ASM_EMIT("bif             v2.16b, v31.16b, v26.16b")
                __ASM_EMIT("bif             v3.16b, v31.16b, v27.16b")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[l], %[l], #0x40")
                __ASM_EMIT("add             %[r], %[r], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                /* 8x block */
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[l], #0x00]")         /* v0   = l */
                __ASM_EMIT("ldp             q4, q5, [%[r], #0x00]")         /* v4   = r */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v0.4s")           /* v0   = l*l */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v1.4s")
                __ASM_EMIT("fmul            v4.4s, v4.4s, v4.4s")           /* v4   = r*r */
                __ASM_EMIT("fmul            v5.4s, v5.4s, v5.4s")
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = l*l + r*r */
                __ASM_EMIT("fadd            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("fcmge           v25.4s, v1.4s, v29.4s")
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecpe          v17.4s, v1.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("fmul            v17.4s, v21.4s, v17.4s")
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("frecps          v21.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v1.4s, v21.4s, v17.4s")
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = r*r / den */
                __ASM_EMIT("fmul            v1.4s, v1.4s, v5.4s")
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("bif             v1.16b, v31.16b, v25.16b")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[l], %[l], #0x20")
                __ASM_EMIT("add             %[r], %[r], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                /* 4x block */
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q0, [%[l], #0x00]")             /* v0   = l */
                __ASM_EMIT("ldr             q4, [%[r], #0x00]")             /* v4   = r */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v0.4s")           /* v0   = l*l */
                __ASM_EMIT("fmul            v4.4s, v4.4s, v4.4s")           /* v4   = r*r */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = l*l + r*r */
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = r*r / den */
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("str             q0, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[l], %[l], #0x10")
                __ASM_EMIT("add             %[r], %[r], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                /* 1x blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v0.4s}, [%[l]]")               /* v0   = l */
                __ASM_EMIT("ld1r            {v4.4s}, [%[r]]")               /* v4   = r */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v0.4s")           /* v0   = l*l */
                __ASM_EMIT("fmul            v4.4s, v4.4s, v4.4s")           /* v4   = r*r */
                __ASM_EMIT("fadd            v0.4s, v0.4s, v4.4s")           /* v0   = den = l*l + r*r */
                __ASM_EMIT("fcmge           v24.4s, v0.4s, v28.4s")         /* v24  = den >= thresh */
                __ASM_EMIT("frecpe          v16.4s, v0.4s")                 /* v16  = s2 */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2) */
                __ASM_EMIT("fmul            v16.4s, v20.4s, v16.4s")        /* v16  = s2' = s2 * (2 - R*s2) */
                __ASM_EMIT("frecps          v20.4s, v16.4s, v0.4s")         /* v20  = (2 - R*s2') */
                __ASM_EMIT("fmul            v0.4s, v20.4s, v16.4s")         /* v0   = s2" = s2' * (2 - R*s2) = 1/s2 */
                __ASM_EMIT("fmul            v0.4s, v0.4s, v4.4s")           /* v0   = pan  = r*r / den */
                __ASM_EMIT("bif             v0.16b, v31.16b, v24.16b")      /* v0   = (den >= thresh) ? pan : dfl */
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[l], %[l], #0x04")
                __ASM_EMIT("add             %[r], %[r], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [l] "+r" (l), [r] "+r" (r),
                  [count] "+r" (count),
                  [dfl] "+w" (dfl)
                : [CC] "r" (&depan_eqpow_const_f[0])
                : "cc", "memory",
                  /* "v0", */ "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_PAN_H_ */
