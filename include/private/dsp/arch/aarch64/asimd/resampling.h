/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_RESAMPLING_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_RESAMPLING_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const float lanczos_2x2[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0636843520278618f,   // k1
                +0.0000000000000000f,
                +0.5731591682507563f,   // k0

                +1.0000000000000000f,
                +0.5731591682507563f,   // k0
                +0.0000000000000000f,
                -0.0636843520278618f    // k1
            };
        )

        void lanczos_resample_2x2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q22, q23, [%[kernel]]")     // v22 = 0 k1 0 k0, v23 = 1 k0 0 k1
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[src], #0x00]") // v16 = s0 s1 s2 s3, v17 = s4 s5 s6 s7

                __ASM_EMIT("dup             v18.4s, v16.s[0]")
                __ASM_EMIT("dup             v20.4s, v17.s[0]")
                __ASM_EMIT("dup             v19.4s, v16.s[2]")
                __ASM_EMIT("dup             v21.4s, v17.s[2]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("str             q4, [%[dst], #0x40]")

                __ASM_EMIT("dup             v18.4s, v16.s[1]")
                __ASM_EMIT("dup             v20.4s, v17.s[1]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("dup             v19.4s, v16.s[3]")
                __ASM_EMIT("dup             v21.4s, v17.s[3]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("str             q4, [%[dst], #0x40]")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x38")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 4x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")

                __ASM_EMIT("ldr             q16, [%[src]]")             // v16 = s0 s1 s2 s3
                __ASM_EMIT("dup             v18.4s, v16.s[0]")
                __ASM_EMIT("dup             v19.4s, v16.s[2]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("str             q2, [%[dst], #0x20]")

                __ASM_EMIT("dup             v18.4s, v16.s[1]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("dup             v19.4s, v16.s[3]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("str             q2, [%[dst], #0x20]")

                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("4:")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v18.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("b.ge            7b")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_2x2[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_2x3[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                +0.0243170840741611f,   // k2
                +0.0000000000000000f,
                -0.1350949115231170f,   // k1

                +0.0000000000000000f,
                +0.6079271018540265f,   // k0
                +1.0000000000000000f,
                +0.6079271018540265f,   // k0

                +0.0000000000000000f,
                -0.1350949115231170f,   // k1
                +0.0000000000000000f,
                +0.0243170840741611f    // k2
            };
        )

        void lanczos_resample_2x3(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q22, q23, [%[kernel]]")
                __ASM_EMIT("ldr             q24, [%[kernel], #0x20]")
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[src], #0x00]") // v16 = s0 s1 s2 s3, v17 = s4 s5 s6 s7

                __ASM_EMIT("dup             v18.4s, v16.s[0]")
                __ASM_EMIT("dup             v20.4s, v17.s[0]")
                __ASM_EMIT("dup             v19.4s, v16.s[2]")
                __ASM_EMIT("dup             v21.4s, v17.s[2]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v4.4s, v20.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v21.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")

                __ASM_EMIT("dup             v18.4s, v16.s[1]")
                __ASM_EMIT("dup             v20.4s, v17.s[1]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("dup             v19.4s, v16.s[3]")
                __ASM_EMIT("dup             v21.4s, v17.s[3]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v20.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v21.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v20.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v21.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v4.4s, v20.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v21.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x38")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 4x block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")

                __ASM_EMIT("ldr             q16, [%[src]]")             // v16 = s0 s1 s2 s3
                __ASM_EMIT("dup             v18.4s, v16.s[0]")
                __ASM_EMIT("dup             v19.4s, v16.s[2]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")

                __ASM_EMIT("dup             v18.4s, v16.s[1]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("dup             v19.4s, v16.s[3]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")

                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("4:")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v18.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst]]")
                __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst]]")
                __ASM_EMIT("str             q2, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("b.ge            7b")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_2x3[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4",
                  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
                  "v24"
            );
        }

        IF_ARCH_AARCH64
        (
            static const float lanczos_2x4[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0126608778212387f,
                +0.0000000000000000f,
                +0.0599094833772629f,

                +0.0000000000000000f,
                -0.1664152316035080f,
                +0.0000000000000000f,
                +0.6203830132406946f,

                +1.0000000000000000f,
                +0.6203830132406946f,
                +0.0000000000000000f,
                -0.1664152316035080f,

                +0.0000000000000000f,
                +0.0599094833772629f,
                +0.0000000000000000f,
                -0.0126608778212387f
            };
        )

        void lanczos_resample_2x4(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                // Prepare
                __ASM_EMIT("ldp             q0, q1, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[kernel], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo            2f")

                // 8x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x00]")       // q4 = s0 s1 s2 s3, q5 = s4 s5 s6 s7

                // Even cycle: prepare
                __ASM_EMIT("ldp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("dup             v16.4s, v4.s[0]")               // v16 = s0
                __ASM_EMIT("ldp             q24, q25, [%[dst], #0x40]")
                __ASM_EMIT("dup             v17.4s, v4.s[2]")               // v17 = s2
                __ASM_EMIT("dup             v18.4s, v5.s[0]")               // v18 = s4
                __ASM_EMIT("ldr             q26, [%[dst], #0x60]")
                __ASM_EMIT("dup             v19.4s, v5.s[2]")               // v19 = s6

                // Even cycle: convolve
                __ASM_EMIT("fmla            v20.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmla            v21.4s, v16.4s, v1.4s")
                __ASM_EMIT("fmla            v22.4s, v16.4s, v2.4s")
                __ASM_EMIT("fmla            v23.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmla            v21.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmla            v22.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla            v23.4s, v17.4s, v2.4s")
                __ASM_EMIT("fmla            v24.4s, v17.4s, v3.4s")
                __ASM_EMIT("fmla            v22.4s, v18.4s, v0.4s")
                __ASM_EMIT("fmla            v23.4s, v18.4s, v1.4s")
                __ASM_EMIT("fmla            v24.4s, v18.4s, v2.4s")
                __ASM_EMIT("fmla            v25.4s, v18.4s, v3.4s")
                __ASM_EMIT("fmla            v23.4s, v19.4s, v0.4s")
                __ASM_EMIT("fmla            v24.4s, v19.4s, v1.4s")
                __ASM_EMIT("fmla            v25.4s, v19.4s, v2.4s")
                __ASM_EMIT("fmla            v26.4s, v19.4s, v3.4s")

                __ASM_EMIT("stp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("stp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("stp             q24, q25, [%[dst], #0x40]")
                __ASM_EMIT("str             q26, [%[dst], #0x60]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")

                // Odd cycle: prepare
                __ASM_EMIT("ldp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("dup             v16.4s, v4.s[1]")               // v16 = s1
                __ASM_EMIT("ldp             q24, q25, [%[dst], #0x40]")
                __ASM_EMIT("dup             v17.4s, v4.s[3]")               // v17 = s3
                __ASM_EMIT("dup             v18.4s, v5.s[1]")               // v18 = s5
                __ASM_EMIT("ldr             q26, [%[dst], #0x60]")
                __ASM_EMIT("dup             v19.4s, v5.s[3]")               // v19 = s7

                // Odd cycle: convolve
                __ASM_EMIT("fmla            v20.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmla            v21.4s, v16.4s, v1.4s")
                __ASM_EMIT("fmla            v22.4s, v16.4s, v2.4s")
                __ASM_EMIT("fmla            v23.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmla            v21.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmla            v22.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla            v23.4s, v17.4s, v2.4s")
                __ASM_EMIT("fmla            v24.4s, v17.4s, v3.4s")
                __ASM_EMIT("fmla            v22.4s, v18.4s, v0.4s")
                __ASM_EMIT("fmla            v23.4s, v18.4s, v1.4s")
                __ASM_EMIT("fmla            v24.4s, v18.4s, v2.4s")
                __ASM_EMIT("fmla            v25.4s, v18.4s, v3.4s")
                __ASM_EMIT("fmla            v23.4s, v19.4s, v0.4s")
                __ASM_EMIT("fmla            v24.4s, v19.4s, v1.4s")
                __ASM_EMIT("fmla            v25.4s, v19.4s, v2.4s")
                __ASM_EMIT("fmla            v26.4s, v19.4s, v3.4s")

                __ASM_EMIT("stp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("stp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("stp             q24, q25, [%[dst], #0x40]")
                __ASM_EMIT("str             q26, [%[dst], #0x60]")

                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("add             %[dst], %[dst], #0x38")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("b.hs            1b")

                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q4, [%[src]]")

                // Even cycle: prepare
                __ASM_EMIT("ldp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("dup             v16.4s, v4.s[0]")               // v16 = s0
                __ASM_EMIT("ldp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("dup             v17.4s, v4.s[2]")               // v17 = s2
                __ASM_EMIT("ldr             q24, [%[dst], #0x40]")
                // Even cycle: convolve
                __ASM_EMIT("fmla            v20.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmla            v21.4s, v16.4s, v1.4s")
                __ASM_EMIT("fmla            v22.4s, v16.4s, v2.4s")
                __ASM_EMIT("fmla            v23.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmla            v21.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmla            v22.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla            v23.4s, v17.4s, v2.4s")
                __ASM_EMIT("fmla            v24.4s, v17.4s, v3.4s")

                __ASM_EMIT("stp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("stp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("str             q24, [%[dst], #0x40]")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")

                // Odd cycle: prepare
                __ASM_EMIT("ldp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("dup             v16.4s, v4.s[1]")               // v16 = s1
                __ASM_EMIT("ldp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("dup             v17.4s, v4.s[3]")               // v17 = s3
                __ASM_EMIT("ldr             q24, [%[dst], #0x40]")

                // Even cycle: convolve
                __ASM_EMIT("fmla            v20.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmla            v21.4s, v16.4s, v1.4s")
                __ASM_EMIT("fmla            v22.4s, v16.4s, v2.4s")
                __ASM_EMIT("fmla            v23.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmla            v21.4s, v17.4s, v0.4s")
                __ASM_EMIT("fmla            v22.4s, v17.4s, v1.4s")
                __ASM_EMIT("fmla            v23.4s, v17.4s, v2.4s")
                __ASM_EMIT("fmla            v24.4s, v17.4s, v3.4s")

                __ASM_EMIT("stp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("stp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("str             q24, [%[dst], #0x40]")

                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")

                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ldp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v20.4s, v16.4s, v0.4s")
                __ASM_EMIT("fmla            v21.4s, v16.4s, v1.4s")
                __ASM_EMIT("fmla            v22.4s, v16.4s, v2.4s")
                __ASM_EMIT("fmla            v23.4s, v16.4s, v3.4s")
                __ASM_EMIT("stp             q20, q21, [%[dst], #0x00]")
                __ASM_EMIT("stp             q22, q23, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[dst], %[dst], #0x08")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("b.ge            7b")

                __ASM_EMIT("6:")
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_2x4[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_3x2[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0315888188312782f,
                -0.0854897486982225f,
                +0.0000000000000000f,

                +0.3419589947928900f,
                +0.7897204707819555f,
                +1.0000000000000000f,
                +0.7897204707819555f,

                +0.3419589947928900f,
                +0.0000000000000000f,
                -0.0854897486982225f,
                -0.0315888188312782f,

                // Shifted by 1 left
                -0.0315888188312782f,
                -0.0854897486982225f,
                +0.0000000000000000f,
                +0.3419589947928900f,

                +0.7897204707819555f,
                +1.0000000000000000f,
                +0.7897204707819555f,
                +0.3419589947928900f,

                +0.0000000000000000f,
                -0.0854897486982225f,
                -0.0315888188312782f,
                +0.0000000000000000f
            };
        )

        void lanczos_resample_3x2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
                // 2x blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v4.4s, v5.4s}, [%[src]]") // v4 = s0, v5 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v4.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v4.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v4.4s, v26.4s")
                __ASM_EMIT("fmla            v1.4s, v5.4s, v27.4s")
                __ASM_EMIT("fmla            v2.4s, v5.4s, v28.4s")
                __ASM_EMIT("fmla            v3.4s, v5.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 1x block
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld1r            {v4.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldr             q2, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v4.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v4.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v4.4s, v26.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("str             q2, [%[dst], #0x20]")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_3x2[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5",
                  "v24", "v25", "v26", "v27", "v28", "v29"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_3x3[] __lsp_aligned16 =
            {
                // Part 1
                +0.0000000000000000f,
                +0.0126609519658153f,
                +0.0310789306368038f,
                +0.0000000000000000f,

                -0.0933267410806225f,
                -0.1458230329384726f,
                +0.0000000000000000f,
                +0.3807169003008463f,

                +0.8103009258121772f,
                +1.0000000000000000f,
                +0.8103009258121772f,
                +0.3807169003008463f,

                +0.0000000000000000f,
                -0.1458230329384726f,
                -0.0933267410806225f,
                +0.0000000000000000f,

                +0.0310789306368038f,
                +0.0126609519658153f,
                +0.0000000000000000f,
                +0.0000000000000000f,

                // Part 2
                +0.0126609519658153f,
                +0.0310789306368038f,
                +0.0000000000000000f,
                -0.0933267410806225f,

                -0.1458230329384726f,
                +0.0000000000000000f,
                +0.3807169003008463f,
                +0.8103009258121772f,

                +1.0000000000000000f,
                +0.8103009258121772f,
                +0.3807169003008463f,
                +0.0000000000000000f,

                -0.1458230329384726f,
                -0.0933267410806225f,
                -0.0000000000000000f,
                +0.0310789306368038f,

                +0.0126609519658153f,
                +0.0000000000000000f,
                +0.0000000000000000f,
                +0.0000000000000000f
            };
        )

        void lanczos_resample_3x3(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x60]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0x80]")
                // 2x blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v6.4s, v7.4s}, [%[src]]") // v4 = s0, v5 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v6.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v6.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v6.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v6.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v6.4s, v26.4s")
                __ASM_EMIT("fmla            v1.4s, v7.4s, v27.4s")
                __ASM_EMIT("fmla            v2.4s, v7.4s, v28.4s")
                __ASM_EMIT("fmla            v3.4s, v7.4s, v29.4s")
                __ASM_EMIT("fmla            v4.4s, v7.4s, v30.4s")
                __ASM_EMIT("fmla            v5.4s, v7.4s, v31.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 1x block
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld1r            {v6.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v6.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v6.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v6.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v6.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v6.4s, v26.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("str             q4, [%[dst], #0x40]")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_3x3[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5",
                  "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_3x4[] __lsp_aligned16 =
            {
                // Part 1 (unshifted)
                +0.0000000000000000f,
                -0.0067568495254777f,
                -0.0157944094156391f,
                +0.0000000000000000f,

                +0.0427448743491113f,
                +0.0622703182267308f,
                +0.0000000000000000f,
                -0.1220498237243924f,

                -0.1709794973964449f,
                +0.0000000000000000f,
                +0.3948602353909778f,
                +0.8175787925827955f,

                +1.0000000000000000f,
                +0.8175787925827955f,
                +0.3948602353909778f,
                +0.0000000000000000f,

                -0.1709794973964449f,
                -0.1220498237243924f,
                +0.0000000000000000f,
                +0.0622703182267308f,

                +0.0427448743491113f,
                +0.0000000000000000f,
                -0.0157944094156391f,
                -0.0067568495254777f,

                // Part 2 (shifted)
                -0.0067568495254777f,
                -0.0157944094156391f,
                +0.0000000000000000f,
                +0.0427448743491113f,

                +0.0622703182267308f,
                +0.0000000000000000f,
                -0.1220498237243924f,
                -0.1709794973964449f,

                +0.0000000000000000f,
                +0.3948602353909778f,
                +0.8175787925827955f,
                +1.0000000000000000f,

                +0.8175787925827955f,
                +0.3948602353909778f,
                +0.0000000000000000f,
                -0.1709794973964449f,

                -0.1220498237243924f,
                +0.0000000000000000f,
                +0.0622703182267308f,
                +0.0427448743491113f,

                +0.0000000000000000f,
                -0.0157944094156391f,
                -0.0067568495254777f,
                +0.0000000000000000f
            };
        )

        void lanczos_resample_3x4(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q20, q21, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x60]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x80]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0xa0]")
                // 2x blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v18.4s, v19.4s}, [%[src]]")    // v18 = s0, v19 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldr             q6, [%[dst], #0x60]")

                __ASM_EMIT("fmla            v0.4s, v18.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v29.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v30.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("str             q6, [%[dst], #0x60]")

                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.hs            1b")

                // 1x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld1r            {v18.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v25.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_3x4[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6",
                  "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_4x2[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0179051851263444f,
                -0.0636843520278618f,
                -0.0847248039068907f,

                +0.0000000000000000f,
                +0.2353466775191407f,
                +0.5731591682507563f,
                +0.8773540711908775f,

                +1.0000000000000000f,
                +0.8773540711908775f,
                +0.5731591682507563f,
                +0.2353466775191407f,

                +0.0000000000000000f,
                -0.0847248039068907f,
                -0.0636843520278618f,
                -0.0179051851263444f
            };
        )

        void lanczos_resample_4x2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldr             q6, [%[dst], #0x60]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("str             q6, [%[dst], #0x60]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 2x block
                __ASM_EMIT("adds            %[count], %[count], #2")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldr             q4, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("str             q4, [%[dst], #0x40]")
                __ASM_EMIT("sub             %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                // 1x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_4x2[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5",
                  "v16", "v17", "v18", "v19",
                  "v24", "v25", "v26", "v27"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_4x3[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                +0.0073559260471942f,
                +0.0243170840741611f,
                +0.0300210914495816f,

                +0.0000000000000000f,
                -0.0677913359005429f,
                -0.1350949115231170f,
                -0.1328710183650640f,

                +0.0000000000000000f,
                +0.2701898230462341f,
                +0.6079271018540265f,
                +0.8900670517104946f,

                +1.0000000000000000f,
                +0.8900670517104946f,
                +0.6079271018540265f,
                +0.2701898230462341f,

                +0.0000000000000000f,
                -0.1328710183650640f,
                -0.1350949115231170f,
                -0.0677913359005429f,

                +0.0000000000000000f,
                +0.0300210914495816f,
                +0.0243170840741611f,
                +0.0073559260471942f
            };
        )

        void lanczos_resample_4x3(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v29.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v19.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("str             q8, [%[dst], #0x80]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 2x block
                __ASM_EMIT("adds            %[count], %[count], #2")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldr             q6, [%[dst], #0x60]")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("str             q6, [%[dst], #0x60]")
                __ASM_EMIT("sub             %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                // 1x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_4x3[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8",
                  "v16", "v17", "v18", "v19",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_4x4[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0039757442382413f,
                -0.0126608778212387f,
                -0.0150736176408234f,

                +0.0000000000000000f,
                +0.0315083921595442f,
                +0.0599094833772629f,
                +0.0555206000541729f,

                +0.0000000000000000f,
                -0.0917789511099593f,
                -0.1664152316035080f,
                -0.1525006180521938f,

                +0.0000000000000000f,
                +0.2830490423665725f,
                +0.6203830132406946f,
                +0.8945424536042901f,

                +1.0000000000000000f,
                +0.8945424536042901f,
                +0.6203830132406946f,
                +0.2830490423665725f,

                +0.0000000000000000f,
                -0.1525006180521938f,
                -0.1664152316035080f,
                -0.0917789511099593f,

                +0.0000000000000000f,
                +0.0555206000541729f,
                +0.0599094833772629f,
                +0.0315083921595442f,

                +0.0000000000000000f,
                -0.0150736176408234f,
                -0.0126608778212387f,
                -0.0039757442382413f
            };
        )

        void lanczos_resample_4x4(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0x60]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldr             q10, [%[dst], #0xa0]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v31.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v18.4s, v30.4s")
                __ASM_EMIT("fmla            v9.4s, v18.4s, v31.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v8.4s, v19.4s, v29.4s")
                __ASM_EMIT("fmla            v9.4s, v19.4s, v30.4s")
                __ASM_EMIT("fmla            v10.4s, v19.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("str             q10, [%[dst], #0xa0]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 2x block
                __ASM_EMIT("adds            %[count], %[count], #2")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldr             q8, [%[dst], #0x80]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("str             q8, [%[dst], #0x80]")

                __ASM_EMIT("sub             %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                // 1x block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_4x4[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10",
                  "v16", "v17", "v18", "v19",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_6x2[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0078021377848166f,
                -0.0315888188312782f,
                -0.0636843520278618f,

                -0.0854897486982225f,
                -0.0719035699814534f,
                +0.0000000000000000f,
                +0.1409309971636486f,

                +0.3419589947928900f,
                +0.5731591682507563f,
                +0.7897204707819555f,
                +0.9440586719628122f,

                +1.0000000000000000f,
                +0.9440586719628122f,
                +0.7897204707819555f,
                +0.5731591682507563f,

                +0.3419589947928900f,
                +0.1409309971636486f,
                +0.0000000000000000f,
                -0.0719035699814534f,

                -0.0854897486982225f,
                -0.0636843520278618f,
                -0.0315888188312782f,
                -0.0078021377848166f
            };
        )

        void lanczos_resample_6x2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v18.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("str             q8, [%[dst], #0x80]")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
                __ASM_EMIT("fmla            v0.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v19.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("str             q8, [%[dst], #0x80]")

                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x48")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.ge            3b")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_6x2[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8",
                  "v16", "v17", "v18", "v19",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_6x3[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                +0.0032875048460955f,
                +0.0126609519658153f,
                +0.0243170840741611f,

                +0.0310789306368038f,
                +0.0248005479513036f,
                -0.0000000000000000f,
                -0.0424907562338176f,

                -0.0933267410806225f,
                -0.1350949115231170f,
                -0.1458230329384726f,
                -0.1049261531488149f,

                +0.0000000000000000f,
                +0.1676517041508127f,
                +0.3807169003008463f,
                +0.6079271018540265f,

                +0.8103009258121772f,
                +0.9500889005216107f,
                +1.0000000000000000f,
                +0.9500889005216107f,

                +0.8103009258121772f,
                +0.6079271018540265f,
                +0.3807169003008463f,
                +0.1676517041508127f,

                +0.0000000000000000f,
                -0.1049261531488149f,
                -0.1458230329384726f,
                -0.1350949115231170f,

                -0.0933267410806225f,
                -0.0424907562338176f,
                -0.0000000000000000f,
                +0.0248005479513036f,

                +0.0310789306368038f,
                +0.0243170840741611f,
                +0.0126609519658153f,
                +0.0032875048460955f
            };
        )

        void lanczos_resample_6x3(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x60]")
                __ASM_EMIT("ldr             q30, [%[kernel], #0x80]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v9.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v18.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v11.4s, v18.4s, v30.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("fmla            v0.4s, v17.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v9.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v19.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v11.4s, v19.4s, v30.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")

                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x48")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldr             q8, [%[dst], #0x80]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v16.4s, v30.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("str             q8, [%[dst], #0x80]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.ge            3b")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_6x3[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v16", "v17", "v18", "v19",
                  "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_6x4[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0018000092949500f,
                -0.0067568495254777f,
                -0.0126608778212387f,

                -0.0157944094156391f,
                -0.0123019137260206f,
                +0.0000000000000000f,
                +0.0200263389720192f,

                +0.0427448743491113f,
                +0.0599094833772629f,
                +0.0622703182267308f,
                +0.0427971267140625f,

                +0.0000000000000000f,
                -0.0597744992948478f,
                -0.1220498237243924f,
                -0.1664152316035080f,

                -0.1709794973964449f,
                -0.1181145298553785f,
                +0.0000000000000000f,
                +0.1776396342037379f,

                +0.3948602353909778f,
                +0.6203830132406946f,
                +0.8175787925827955f,
                +0.9522049170285306f,

                +1.0000000000000000f,
                +0.9522049170285306f,
                +0.8175787925827955f,
                +0.6203830132406946f,

                +0.3948602353909778f,
                +0.1776396342037379f,
                +0.0000000000000000f,
                -0.1181145298553785f,

                -0.1709794973964449f,
                -0.1664152316035080f,
                -0.1220498237243924f,
                -0.0597744992948478f,

                +0.0000000000000000f,
                +0.0427971267140625f,
                +0.0622703182267308f,
                +0.0599094833772629f,

                +0.0427448743491113f,
                +0.0200263389720192f,
                +0.0000000000000000f,
                -0.0123019137260206f,

                -0.0157944094156391f,
                -0.0126608778212387f,
                -0.0067568495254777f,
                -0.0018000092949500f
            };
        )

        void lanczos_resample_6x4(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q20, q21, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x60]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x80]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0xa0]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3

                // Even part
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v20.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v21.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v22.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v23.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0xa0]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0xc0]")
                __ASM_EMIT("ldr             q6, [%[dst], #0xe0]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v0.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v1.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v29.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v30.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v31.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x80]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0xa0]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0xc0]")
                __ASM_EMIT("str             q6, [%[dst], #0xe0]")

                __ASM_EMIT("add             %[dst], %[dst], #0x18")

                // Odd part
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("fmla            v0.4s, v17.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v20.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v21.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v22.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v23.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v24.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0xa0]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0xc0]")
                __ASM_EMIT("ldr             q6, [%[dst], #0xe0]")
                __ASM_EMIT("fmla            v0.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v1.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v31.4s")
                __ASM_EMIT("fmla            v0.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v1.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v2.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v3.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v4.4s, v19.4s, v29.4s")
                __ASM_EMIT("fmla            v5.4s, v19.4s, v30.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v31.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x80]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0xa0]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0xc0]")
                __ASM_EMIT("str             q6, [%[dst], #0xe0]")

                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x48")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0xa0]")
                __ASM_EMIT("fmla            v0.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v31.4s")
                __ASM_EMIT("stp             q0, q1, [%[dst], #0x60]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x80]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0xa0]")

                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x04")
                __ASM_EMIT("add             %[dst], %[dst], #0x18")
                __ASM_EMIT("b.ge            3b")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_6x4[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_8x2[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0043033145538298f,
                -0.0179051851263444f,
                -0.0393892611124141f,

                -0.0636843520278618f,
                -0.0823353965569232f,
                -0.0847248039068907f,
                -0.0600950644541902f,

                +0.0000000000000000f,
                +0.0993408208324369f,
                +0.2353466775191407f,
                +0.3985033193355084f,

                +0.5731591682507563f,
                +0.7396427919997760f,
                +0.8773540711908775f,
                +0.9682457746117045f,

                +1.0000000000000000f,
                +0.9682457746117045f,
                +0.8773540711908775f,
                +0.7396427919997760f,

                +0.5731591682507563f,
                +0.3985033193355084f,
                +0.2353466775191407f,
                +0.0993408208324369f,

                +0.0000000000000000f,
                -0.0600950644541902f,
                -0.0847248039068907f,
                -0.0823353965569232f,

                -0.0636843520278618f,
                -0.0393892611124141f,
                -0.0179051851263444f,
                -0.0043033145538298f
            };
        )

        void lanczos_resample_8x2(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0x60]")
                // 4x blocks
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld4r            {v16.4s, v17.4s, v18.4s, v19.4s}, [%[src]]") // v16 = s0, v17 = s1, v18 = s2, v19 = s3
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("ldp             q12, q13, [%[dst], #0xc0]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v18.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v18.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v18.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v18.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v18.4s, v28.4s")
                __ASM_EMIT("fmla            v9.4s, v18.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v10.4s, v18.4s, v30.4s")
                __ASM_EMIT("fmla            v11.4s, v18.4s, v31.4s")

                __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v19.4s, v24.4s")
                __ASM_EMIT("fmla            v7.4s, v19.4s, v25.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v19.4s, v26.4s")
                __ASM_EMIT("fmla            v9.4s, v19.4s, v27.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v19.4s, v28.4s")
                __ASM_EMIT("fmla            v11.4s, v19.4s, v29.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v9.4s, v17.4s, v31.4s")
                __ASM_EMIT("fmla            v12.4s, v19.4s, v30.4s")
                __ASM_EMIT("fmla            v13.4s, v19.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("stp             q12, q13, [%[dst], #0xc0]")
                __ASM_EMIT("subs            %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // 2x block
                __ASM_EMIT("adds            %[count], %[count], #2")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v9.4s, v17.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("sub             %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("4:")
                // 1x block
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_8x2[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13",
                  "v16", "v17", "v18", "v19",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_8x3[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                +0.0018368899607481f,
                +0.0073559260471942f,
                +0.0155961678435580f,

                +0.0243170840741611f,
                +0.0303079634725070f,
                +0.0300210914495816f,
                +0.0204366616947175f,

                +0.0000000000000000f,
                -0.0305684889733737f,
                -0.0677913359005429f,
                -0.1054383717904384f,

                -0.1350949115231170f,
                -0.1472651639056537f,
                -0.1328710183650640f,
                -0.0849124693704824f,

                +0.0000000000000000f,
                +0.1205345965259870f,
                +0.2701898230462341f,
                +0.4376469925430009f,

                +0.6079271018540265f,
                +0.7642122243343417f,
                +0.8900670517104946f,
                +0.9717147892357163f,

                +1.0000000000000000f,
                +0.9717147892357163f,
                +0.8900670517104946f,
                +0.7642122243343417f,

                +0.6079271018540265f,
                +0.4376469925430009f,
                +0.2701898230462341f,
                +0.1205345965259870f,

                +0.0000000000000000f,
                -0.0849124693704824f,
                -0.1328710183650640f,
                -0.1472651639056537f,

                -0.1350949115231170f,
                -0.1054383717904384f,
                -0.0677913359005429f,
                -0.0305684889733737f,

                +0.0000000000000000f,
                +0.0204366616947175f,
                +0.0300210914495816f,
                +0.0303079634725070f,

                +0.0243170840741611f,
                +0.0155961678435580f,
                +0.0073559260471942f,
                +0.0018368899607481f
            };
        )

        void lanczos_resample_8x3(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q20, q21, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0x60]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0x80]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0xa0]")
                // 2x blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v16.4s, v17.4s}, [%[src]]") // v16 = s0, v17 = s1
                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("ldp             q12, q13, [%[dst], #0xc0]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v17.4s, v20.4s")
                __ASM_EMIT("fmla            v3.4s, v17.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v17.4s, v22.4s")
                __ASM_EMIT("fmla            v5.4s, v17.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v17.4s, v24.4s")
                __ASM_EMIT("fmla            v7.4s, v17.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v17.4s, v26.4s")
                __ASM_EMIT("fmla            v9.4s, v17.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v9.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v17.4s, v28.4s")
                __ASM_EMIT("fmla            v11.4s, v17.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v11.4s, v16.4s, v31.4s")
                __ASM_EMIT("fmla            v12.4s, v17.4s, v30.4s")
                __ASM_EMIT("fmla            v13.4s, v17.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("stp             q12, q13, [%[dst], #0xc0]")
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")

                // 1x block
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld1r            {v16.4s}, [%[src]]")

                __ASM_EMIT("ldp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q10, q11, [%[dst], #0xa0]")

                __ASM_EMIT("fmla            v0.4s, v16.4s, v20.4s")
                __ASM_EMIT("fmla            v1.4s, v16.4s, v21.4s")
                __ASM_EMIT("fmla            v2.4s, v16.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v16.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v16.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v16.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v16.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v16.4s, v27.4s")
                __ASM_EMIT("fmla            v8.4s, v16.4s, v28.4s")
                __ASM_EMIT("fmla            v9.4s, v16.4s, v29.4s")
                __ASM_EMIT("fmla            v10.4s, v16.4s, v30.4s")
                __ASM_EMIT("fmla            v11.4s, v16.4s, v31.4s")

                __ASM_EMIT("stp             q0, q1, [%[dst], #0x00]")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x20]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x40]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x60]")
                __ASM_EMIT("stp             q8, q9, [%[dst], #0x80]")
                __ASM_EMIT("stp             q10, q11, [%[dst], #0xa0]")
                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_8x3[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13",
                  "v16", "v17",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        IF_ARCH_AARCH64(
            static const float lanczos_8x4[] __lsp_aligned16 =
            {
                +0.0000000000000000f,
                -0.0010124148822791f,
                -0.0039757442382413f,
                -0.0082714887261119f,

                -0.0126608778212387f,
                -0.0154958216565010f,
                -0.0150736176408234f,
                -0.0100753105205530f,

                +0.0000000000000000f,
                +0.0145047275409824f,
                +0.0315083921595442f,
                +0.0479233082326825f,

                +0.0599094833772629f,
                +0.0635233253590927f,
                +0.0555206000541729f,
                +0.0341810767869351f,

                +0.0000000000000000f,
                -0.0439036941841078f,
                -0.0917789511099593f,
                -0.1356918370096595f,

                -0.1664152316035080f,
                -0.1746626357901899f,
                -0.1525006180521938f,
                -0.0947284057923417f,

                +0.0000000000000000f,
                +0.1285116137825641f,
                +0.2830490423665725f,
                +0.4518581595035692f,

                +0.6203830132406946f,
                +0.7729246687400148f,
                +0.8945424536042901f,
                +0.9729307018702211f,

                +1.0000000000000000f,
                +0.9729307018702211f,
                +0.8945424536042901f,
                +0.7729246687400148f,

                +0.6203830132406946f,
                +0.4518581595035692f,
                +0.2830490423665725f,
                +0.1285116137825641f,

                +0.0000000000000000f,
                -0.0947284057923417f,
                -0.1525006180521938f,
                -0.1746626357901899f,

                -0.1664152316035080f,
                -0.1356918370096595f,
                -0.0917789511099593f,
                -0.0439036941841078f,

                +0.0000000000000000f,
                +0.0341810767869351f,
                +0.0555206000541729f,
                +0.0635233253590927f,

                +0.0599094833772629f,
                +0.0479233082326825f,
                +0.0315083921595442f,
                +0.0145047275409824f,

                +0.0000000000000000f,
                -0.0100753105205530f,
                -0.0150736176408234f,
                -0.0154958216565010f,

                -0.0126608778212387f,
                -0.0082714887261119f,
                -0.0039757442382413f,
                -0.0010124148822791f
            };
        )

        void lanczos_resample_8x4(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                // Prepare
                __ASM_EMIT("ldp             q16, q17, [%[kernel], #0x00]")
                __ASM_EMIT("ldp             q18, q19, [%[kernel], #0x20]")
                __ASM_EMIT("ldp             q20, q21, [%[kernel], #0x40]")
                __ASM_EMIT("ldp             q22, q23, [%[kernel], #0x60]")
                __ASM_EMIT("ldp             q24, q25, [%[kernel], #0x80]")
                __ASM_EMIT("ldp             q26, q27, [%[kernel], #0xa0]")
                __ASM_EMIT("ldp             q28, q29, [%[kernel], #0xc0]")
                __ASM_EMIT("ldp             q30, q31, [%[kernel], #0xe0]")
                // 2x blocks
                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("b.lo            2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ld2r            {v0.4s, v1.4s}, [%[src]]") // v0 = s0, v1 = s1

                // Part 1
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v17.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v18.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v19.4s")
                __ASM_EMIT("fmla            v6.4s, v0.4s, v20.4s")
                __ASM_EMIT("fmla            v7.4s, v0.4s, v21.4s")
                __ASM_EMIT("fmla            v4.4s, v1.4s, v16.4s")
                __ASM_EMIT("fmla            v5.4s, v1.4s, v17.4s")
                __ASM_EMIT("fmla            v6.4s, v1.4s, v18.4s")
                __ASM_EMIT("fmla            v7.4s, v1.4s, v19.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x00]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x20]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x40]")

                // Part 2
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0xa0]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v0.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v0.4s, v27.4s")
                __ASM_EMIT("fmla            v2.4s, v1.4s, v20.4s")
                __ASM_EMIT("fmla            v3.4s, v1.4s, v21.4s")
                __ASM_EMIT("fmla            v4.4s, v1.4s, v22.4s")
                __ASM_EMIT("fmla            v5.4s, v1.4s, v23.4s")
                __ASM_EMIT("fmla            v6.4s, v1.4s, v24.4s")
                __ASM_EMIT("fmla            v7.4s, v1.4s, v25.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x60]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x80]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0xa0]")

                // Part 3
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0xc0]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0xe0]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x100]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v28.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v29.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v30.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v31.4s")
                __ASM_EMIT("fmla            v2.4s, v1.4s, v26.4s")
                __ASM_EMIT("fmla            v3.4s, v1.4s, v27.4s")
                __ASM_EMIT("fmla            v4.4s, v1.4s, v28.4s")
                __ASM_EMIT("fmla            v5.4s, v1.4s, v29.4s")
                __ASM_EMIT("fmla            v6.4s, v1.4s, v30.4s")
                __ASM_EMIT("fmla            v7.4s, v1.4s, v31.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0xc0]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0xe0]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x100]")

                __ASM_EMIT("subs            %[count], %[count], #2")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")

                // 1x block
                __ASM_EMIT("adds            %[count], %[count], #1")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ld1r            {v0.4s}, [%[src]]")

                // Part 1
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x00]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x20]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x40]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v16.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v17.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v18.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v19.4s")
                __ASM_EMIT("fmla            v6.4s, v0.4s, v20.4s")
                __ASM_EMIT("fmla            v7.4s, v0.4s, v21.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x00]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x20]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x40]")

                // Part 2
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0x60]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0x80]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0xa0]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v22.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v23.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v24.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v25.4s")
                __ASM_EMIT("fmla            v6.4s, v0.4s, v26.4s")
                __ASM_EMIT("fmla            v7.4s, v0.4s, v27.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0x60]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0x80]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0xa0]")

                // Part 3
                __ASM_EMIT("ldp             q2, q3, [%[dst], #0xc0]")
                __ASM_EMIT("ldp             q4, q5, [%[dst], #0xe0]")
                __ASM_EMIT("ldp             q6, q7, [%[dst], #0x100]")
                __ASM_EMIT("fmla            v2.4s, v0.4s, v28.4s")
                __ASM_EMIT("fmla            v3.4s, v0.4s, v29.4s")
                __ASM_EMIT("fmla            v4.4s, v0.4s, v30.4s")
                __ASM_EMIT("fmla            v5.4s, v0.4s, v31.4s")
                __ASM_EMIT("stp             q2, q3, [%[dst], #0xc0]")
                __ASM_EMIT("stp             q4, q5, [%[dst], #0xe0]")
                __ASM_EMIT("stp             q6, q7, [%[dst], #0x100]")

                __ASM_EMIT("4:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [kernel] "r" (&lanczos_8x4[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23",
                  "v24", "v25", "v26", "v27",
                  "v28", "v29", "v30", "v31"
            );
        }

        void downsample_2x(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("subs            %[count], %[count], #32")
                __ASM_EMIT("b.lo            2f")
                // x32 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")
                __ASM_EMIT("ldp             q16, q17, [%[src], #0x80]")
                __ASM_EMIT("ldp             q18, q19, [%[src], #0xa0]")
                __ASM_EMIT("ldp             q20, q21, [%[src], #0xc0]")
                __ASM_EMIT("ldp             q22, q23, [%[src], #0xe0]")
                __ASM_EMIT("uzp1            v0.4s, v0.4s, v1.4s")
                __ASM_EMIT("uzp1            v2.4s, v2.4s, v3.4s")
                __ASM_EMIT("uzp1            v4.4s, v4.4s, v5.4s")
                __ASM_EMIT("uzp1            v6.4s, v6.4s, v7.4s")
                __ASM_EMIT("uzp1            v16.4s, v16.4s, v17.4s")
                __ASM_EMIT("uzp1            v18.4s, v18.4s, v19.4s")
                __ASM_EMIT("uzp1            v20.4s, v20.4s, v21.4s")
                __ASM_EMIT("uzp1            v22.4s, v22.4s, v23.4s")
                __ASM_EMIT("stp             q0, q2, [%[dst], #0x00]")
                __ASM_EMIT("stp             q4, q6, [%[dst], #0x20]")
                __ASM_EMIT("stp             q16, q18, [%[dst], #0x40]")
                __ASM_EMIT("stp             q20, q22, [%[dst], #0x60]")
                __ASM_EMIT("subs            %[count], %[count], #32")
                __ASM_EMIT("add             %[src], %[src], #0x100")
                __ASM_EMIT("add             %[dst], %[dst], #0x80")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("adds            %[count], %[count], #16")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")
                __ASM_EMIT("uzp1            v0.4s, v0.4s, v1.4s")
                __ASM_EMIT("uzp1            v2.4s, v2.4s, v3.4s")
                __ASM_EMIT("uzp1            v4.4s, v4.4s, v5.4s")
                __ASM_EMIT("uzp1            v6.4s, v6.4s, v7.4s")
                __ASM_EMIT("stp             q0, q2, [%[dst], #0x00]")
                __ASM_EMIT("stp             q4, q6, [%[dst], #0x20]")
                __ASM_EMIT("sub             %[count], %[count], #16")
                __ASM_EMIT("add             %[src], %[src], #0x80")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
                __ASM_EMIT("uzp1            v0.4s, v0.4s, v1.4s")
                __ASM_EMIT("uzp1            v2.4s, v2.4s, v3.4s")
                __ASM_EMIT("stp             q0, q2, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("ld2             {v0.4s, v1.4s}, [%[src]]")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("8:")
                // x1 blocks
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x08")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            9b")
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void downsample_3x(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   s1 , v1  = ?   ?   s2  ?
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")       // v2  = ?   s3  ?   ?  , v3  = s4  ?   ?   s5
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")       // v4  = ?   ?   s6  ?  , v5  = ?   s7  ?   ?
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")       // v6  = s8  ?   ?   s9 , v7  = ?   ?   s10 ?
                __ASM_EMIT("ldp             q8, q9, [%[src], #0x80]")       // v8  = ?   s11 ?   ?  , v9  = s12 ?   ?   s13
                __ASM_EMIT("ldp             q10, q11, [%[src], #0xa0]")     // v10 = ?   ?   s14 ?  , v11 = ?   s15 ?   ?
                __ASM_EMIT("mov             v0.s[1], v0.s[3]")              // v0  = s0  s1  ?   s1
                __ASM_EMIT("mov             v1.s[3], v2.s[1]")              // v1  = ?   ?   s2  s3
                __ASM_EMIT("mov             v3.s[1], v3.s[3]")
                __ASM_EMIT("mov             v4.s[3], v5.s[1]")
                __ASM_EMIT("mov             v6.s[1], v6.s[3]")
                __ASM_EMIT("mov             v7.s[3], v8.s[1]")
                __ASM_EMIT("mov             v9.s[1], v9.s[3]")
                __ASM_EMIT("mov             v10.s[3], v11.s[1]")
                __ASM_EMIT("mov             v0.d[1], v1.d[1]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("mov             v3.d[1], v4.d[1]")
                __ASM_EMIT("mov             v6.d[1], v7.d[1]")
                __ASM_EMIT("mov             v9.d[1], v10.d[1]")
                __ASM_EMIT("stp             q0, q3, [%[dst], #0x00]")
                __ASM_EMIT("stp             q6, q9, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[src], %[src], #0xc0")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   s1 , v1  = ?   ?   s2  ?
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")       // v2  = ?   s3  ?   ?  , v3  = s4  ?   ?   s5
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")       // v4  = ?   ?   s6  ?  , v5  = ?   s7  ?   ?
                __ASM_EMIT("mov             v0.s[1], v0.s[3]")              // v0  = s0  s1  ?   s1
                __ASM_EMIT("mov             v1.s[3], v2.s[1]")              // v1  = ?   ?   s2  s3
                __ASM_EMIT("mov             v3.s[1], v3.s[3]")
                __ASM_EMIT("mov             v4.s[3], v5.s[1]")
                __ASM_EMIT("mov             v0.d[1], v1.d[1]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("mov             v3.d[1], v4.d[1]")
                __ASM_EMIT("stp             q0, q3, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x60")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   s1 , v1  = ?   ?   s2  ?
                __ASM_EMIT("ldr             q2, [%[src], #0x20]")           // v2  = ?   s3  ?   ?
                __ASM_EMIT("mov             v0.s[1], v0.s[3]")              // v0  = s0  s1  ?   s1
                __ASM_EMIT("mov             v1.s[3], v2.s[1]")              // v1  = ?   ?   s2  s3
                __ASM_EMIT("mov             v0.d[1], v1.d[1]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x30")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x0c")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.hs            7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11"
            );
        }

        void downsample_4x(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")
                __ASM_EMIT("ldp             q16, q17, [%[src], #0x80]")
                __ASM_EMIT("ldp             q18, q19, [%[src], #0xa0]")
                __ASM_EMIT("ldp             q20, q21, [%[src], #0xc0]")
                __ASM_EMIT("ldp             q22, q23, [%[src], #0xe0]")
                __ASM_EMIT("mov             v0.s[1], v1.s[0]")
                __ASM_EMIT("mov             v2.s[1], v3.s[0]")
                __ASM_EMIT("mov             v4.s[1], v5.s[0]")
                __ASM_EMIT("mov             v6.s[1], v7.s[0]")
                __ASM_EMIT("mov             v16.s[1], v17.s[0]")
                __ASM_EMIT("mov             v18.s[1], v19.s[0]")
                __ASM_EMIT("mov             v20.s[1], v21.s[0]")
                __ASM_EMIT("mov             v22.s[1], v23.s[0]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("mov             v16.d[1], v18.d[0]")
                __ASM_EMIT("mov             v20.d[1], v22.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("stp             q16, q20, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[src], %[src], #0x100")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x20]")
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x40]")
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x60]")
                __ASM_EMIT("mov             v0.s[1], v1.s[0]")
                __ASM_EMIT("mov             v2.s[1], v3.s[0]")
                __ASM_EMIT("mov             v4.s[1], v5.s[0]")
                __ASM_EMIT("mov             v6.s[1], v7.s[0]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x80")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ld4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[src]]")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x40")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x10")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }

        void downsample_6x(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   ?  , v1  = ?   ?   s1  ?
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x30]")       // v2  = s2  ?   ?   ?  , v3  = ?   ?   s3  ?
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x60]")       // v4  = s4  ?   ?   ?  , v5  = ?   ?   s5  ?
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x90]")       // v6  = s6  ?   ?   ?  , v7  = ?   ?   s7  ?
                __ASM_EMIT("ldp             q8, q9, [%[src], #0xc0]")       // v0  = s8  ?   ?   ?  , v1  = ?   ?   s9  ?
                __ASM_EMIT("ldp             q10, q11, [%[src], #0xf0]")     // v2  = s10 ?   ?   ?  , v3  = ?   ?   s11 ?
                __ASM_EMIT("ldp             q12, q13, [%[src], #0x120]")    // v4  = s12 ?   ?   ?  , v5  = ?   ?   s13 ?
                __ASM_EMIT("ldp             q14, q15, [%[src], #0x150]")    // v6  = s14 ?   ?   ?  , v7  = ?   ?   s15 ?
                __ASM_EMIT("mov             v0.s[1], v1.s[2]")
                __ASM_EMIT("mov             v2.s[1], v3.s[2]")
                __ASM_EMIT("mov             v4.s[1], v5.s[2]")
                __ASM_EMIT("mov             v6.s[1], v7.s[2]")
                __ASM_EMIT("mov             v8.s[1], v9.s[2]")
                __ASM_EMIT("mov             v10.s[1], v11.s[2]")
                __ASM_EMIT("mov             v12.s[1], v13.s[2]")
                __ASM_EMIT("mov             v14.s[1], v15.s[2]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("mov             v8.d[1], v10.d[0]")
                __ASM_EMIT("mov             v12.d[1], v14.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("stp             q8, q12, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[src], %[src], #0x180")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   ?  , v1  = ?   ?   s1  ?
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x30]")       // v2  = s2  ?   ?   ?  , v3  = ?   ?   s3  ?
                __ASM_EMIT("ldp             q4, q5, [%[src], #0x60]")       // v4  = s4  ?   ?   ?  , v5  = ?   ?   s5  ?
                __ASM_EMIT("ldp             q6, q7, [%[src], #0x90]")       // v6  = s6  ?   ?   ?  , v7  = ?   ?   s7  ?
                __ASM_EMIT("mov             v0.s[1], v1.s[2]")
                __ASM_EMIT("mov             v2.s[1], v3.s[2]")
                __ASM_EMIT("mov             v4.s[1], v5.s[2]")
                __ASM_EMIT("mov             v6.s[1], v7.s[2]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0xc0")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldp             q0, q1, [%[src], #0x00]")       // v0  = s0  ?   ?   ?  , v1  = ?   ?   s1  ?
                __ASM_EMIT("ldp             q2, q3, [%[src], #0x30]")       // v2  = s2  ?   ?   ?  , v3  = ?   ?   s3  ?
                __ASM_EMIT("mov             v0.s[1], v1.s[2]")
                __ASM_EMIT("mov             v2.s[1], v3.s[2]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")              // v0  = s0  s1  s2  s3
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x60")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("add             %[src], %[src], #0x18")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.hs            7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11"
            );
        }

        void downsample_8x(float *dst, const float *src, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")
                __ASM_EMIT("ldr             q1, [%[src], #0x20]")
                __ASM_EMIT("ldr             q2, [%[src], #0x40]")
                __ASM_EMIT("ldr             q3, [%[src], #0x60]")
                __ASM_EMIT("ldr             q4, [%[src], #0x80]")
                __ASM_EMIT("ldr             q5, [%[src], #0xa0]")
                __ASM_EMIT("ldr             q6, [%[src], #0xc0]")
                __ASM_EMIT("ldr             q7, [%[src], #0xe0]")
                __ASM_EMIT("ldr             q16, [%[src], #0x100]")
                __ASM_EMIT("ldr             q17, [%[src], #0x120]")
                __ASM_EMIT("ldr             q18, [%[src], #0x140]")
                __ASM_EMIT("ldr             q19, [%[src], #0x160]")
                __ASM_EMIT("ldr             q20, [%[src], #0x180]")
                __ASM_EMIT("ldr             q21, [%[src], #0x1a0]")
                __ASM_EMIT("ldr             q22, [%[src], #0x1c0]")
                __ASM_EMIT("ldr             q23, [%[src], #0x1e0]")
                __ASM_EMIT("mov             v0.s[1], v1.s[0]")
                __ASM_EMIT("mov             v2.s[1], v3.s[0]")
                __ASM_EMIT("mov             v4.s[1], v5.s[0]")
                __ASM_EMIT("mov             v6.s[1], v7.s[0]")
                __ASM_EMIT("mov             v16.s[1], v17.s[0]")
                __ASM_EMIT("mov             v18.s[1], v19.s[0]")
                __ASM_EMIT("mov             v20.s[1], v21.s[0]")
                __ASM_EMIT("mov             v22.s[1], v23.s[0]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("mov             v16.d[1], v18.d[0]")
                __ASM_EMIT("mov             v20.d[1], v22.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("stp             q16, q20, [%[dst], #0x20]")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("add             %[src], %[src], #0x200")
                __ASM_EMIT("add             %[dst], %[dst], #0x40")
                __ASM_EMIT("b.hs            1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt            4f")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")
                __ASM_EMIT("ldr             q1, [%[src], #0x20]")
                __ASM_EMIT("ldr             q2, [%[src], #0x40]")
                __ASM_EMIT("ldr             q3, [%[src], #0x60]")
                __ASM_EMIT("ldr             q4, [%[src], #0x80]")
                __ASM_EMIT("ldr             q5, [%[src], #0xa0]")
                __ASM_EMIT("ldr             q6, [%[src], #0xc0]")
                __ASM_EMIT("ldr             q7, [%[src], #0xe0]")
                __ASM_EMIT("mov             v0.s[1], v1.s[0]")
                __ASM_EMIT("mov             v2.s[1], v3.s[0]")
                __ASM_EMIT("mov             v4.s[1], v5.s[0]")
                __ASM_EMIT("mov             v6.s[1], v7.s[0]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")
                __ASM_EMIT("mov             v4.d[1], v6.d[0]")
                __ASM_EMIT("stp             q0, q4, [%[dst], #0x00]")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("add             %[src], %[src], #0x100")
                __ASM_EMIT("add             %[dst], %[dst], #0x20")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q0, [%[src], #0x00]")
                __ASM_EMIT("ldr             q1, [%[src], #0x20]")
                __ASM_EMIT("ldr             q2, [%[src], #0x40]")
                __ASM_EMIT("ldr             q3, [%[src], #0x60]")
                __ASM_EMIT("mov             v0.s[1], v1.s[0]")
                __ASM_EMIT("mov             v2.s[1], v3.s[0]")
                __ASM_EMIT("mov             v0.d[1], v2.d[0]")
                __ASM_EMIT("str             q0, [%[dst]]")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("add             %[src], %[src], #0x80")
                __ASM_EMIT("add             %[dst], %[dst], #0x10")
                __ASM_EMIT("6:")
                // x1 blocks
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("b.lt            8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1             {v0.s}[0], [%[src]]")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v0.s}[0], [%[dst]]")
                __ASM_EMIT("add             %[src], %[src], #0x20")
                __ASM_EMIT("add             %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge            7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_RESAMPLING_H_ */
