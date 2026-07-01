/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_PMATH_LERP_H_
#define PRIVATE_DSP_ARCH_X86_AVX_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("subl          $32, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT("vmovups         0x60(%[a], %[off]), %%ymm3")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm2, %%ymm6")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm3, %%ymm7")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm4, %%ymm4")    // ymm4     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vmulps          0x40(%[k], %[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vmulps          0x60(%[k], %[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vsubps          %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT32("subl          $32, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl          $16, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm4, %%ymm4")    // ymm4     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm4, %%ymm4")    // ymm4     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT32("subl          $8, %[count]")
                __ASM_EMIT64("sub           $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT32("addl          $4, %[count]")
                __ASM_EMIT64("add           $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%xmm4, %%xmm4")    // xmm4     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm4, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT32("subl          $4, %[count]")
                __ASM_EMIT64("sub           $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT32("addl          $3, %[count]")
                __ASM_EMIT64("add           $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vmulss          0x00(%[k], %[off]), %%xmm4, %%xmm4")    // xmm4     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm4, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT32("decl          %[count]")
                __ASM_EMIT64("dec           %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_vvv_fma3(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("subl          $32, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT("vmovups         0x60(%[a], %[off]), %%ymm3")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm2, %%ymm6")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm3, %%ymm7")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm4, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x20(%[k], %[off]), %%ymm5, %%ymm1")
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    0x60(%[k], %[off]), %%ymm7, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT32("subl          $32, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl          $16, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm4, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x20(%[k], %[off]), %%ymm5, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm4, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT32("subl          $8, %[count]")
                __ASM_EMIT64("sub           $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT32("addl          $4, %[count]")
                __ASM_EMIT64("add           $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%xmm4, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT32("subl          $4, %[count]")
                __ASM_EMIT64("sub           $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT32("addl          $3, %[count]")
                __ASM_EMIT64("add           $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vfnmadd231ss    0x00(%[k], %[off]), %%xmm4, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT32("decl          %[count]")
                __ASM_EMIT64("dec           %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_vvk(float *dst, const float *a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[k], %%ymm6")                          // ymm6     = k
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        0x60(%[b], %[off]), %%ymm8, %%ymm9")
                __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm6, %%ymm5, %%ymm5")
                __ASM_EMIT64("vmulps        %%ymm6, %%ymm9, %%ymm9")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm2")
                __ASM_EMIT64("vsubps        %%ymm9, %%ymm8, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm6, %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vmulps          %%ymm6, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vmulps          %%xmm6, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vmulss          %%xmm6, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_vvk_fma3(float *dst, const float *a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[k], %%ymm6")                          // ymm6     = k
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        0x60(%[b], %[off]), %%ymm8, %%ymm9")
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm4, %%ymm1")
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm5, %%ymm2")
                __ASM_EMIT64("vfnmadd231ps  %%ymm6, %%ymm9, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm4, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%xmm6, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ss    %%xmm6, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_vkv(float *dst, const float *a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%ymm6")                          // ymm6     = b
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        %%ymm6, %%ymm8, %%ymm9")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm3, %%ymm3")    // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          0x40(%[k], %[off]), %%ymm5, %%ymm5")
                __ASM_EMIT64("vmulps        0x60(%[k], %[off]), %%ymm9, %%ymm9")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm2")
                __ASM_EMIT64("vsubps        %%ymm9, %%ymm8, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm3, %%ymm3")    // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm3, %%ymm3")    // ymm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%xmm3, %%xmm3")    // xmm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulss          0x00(%[k], %[off]), %%xmm3, %%xmm3")    // xmm3     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_vkv_fma3(float *dst, const float *a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%ymm6")                          // ymm6     = b
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        %%ymm6, %%ymm8, %%ymm9")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm3, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x20(%[k], %[off]), %%ymm4, %%ymm1")
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%ymm5, %%ymm2")
                __ASM_EMIT64("vfnmadd231ps  0x60(%[k], %[off]), %%ymm9, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm3, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x20(%[k], %[off]), %%ymm4, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm3, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%xmm3, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ss    0x00(%[k], %[off]), %%xmm3, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_vkk(float *dst, const float *a, float b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%ymm6")                          // ymm6     = b
                __ASM_EMIT("vbroadcastss    %[k], %%ymm7")                          // ymm7     = k
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        %%ymm6, %%ymm8, %%ymm9")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm7, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm5, %%ymm5")
                __ASM_EMIT64("vmulps        %%ymm7, %%ymm9, %%ymm9")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm2")
                __ASM_EMIT64("vsubps        %%ymm9, %%ymm8, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm7, %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulps          %%xmm7, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulss          %%xmm7, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_vkk_fma3(float *dst, const float *a, float b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%ymm6")                          // ymm6     = b
                __ASM_EMIT("vbroadcastss    %[k], %%ymm7")                          // ymm7     = k
                __ASM_EMIT("jb              2f")
                // 24x/32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%ymm2")
                __ASM_EMIT64("vmovups       0x60(%[a], %[off]), %%ymm8")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm2, %%ymm5")
                __ASM_EMIT64("vsubps        %%ymm6, %%ymm8, %%ymm9")
                __ASM_EMIT("vfnmadd231ps    %%ymm7, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%ymm7, %%ymm4, %%ymm1")
                __ASM_EMIT("vfnmadd231ps    %%ymm7, %%ymm5, %%ymm2")
                __ASM_EMIT64("vfnmadd231ps  %%ymm7, %%ymm9, %%ymm8")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%ymm8, 0x60(%[dst], %[off])")
                __ASM_EMIT32("add           $0x60, %[off]")
                __ASM_EMIT32("sub           $24, %[count]")
                __ASM_EMIT64("add           $0x80, %[off]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $8, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm1")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vsubps          %%ymm6, %%ymm1, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm7, %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm4, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")                // ymm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm3, %%ymm0, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulps          %%xmm7, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vmulss          %%xmm7, %%xmm3, %%xmm3")                // xmm3     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm3, %%xmm0, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  __IF_64("xmm8", "xmm9", )
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm0, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps          0x40(%[k], %[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          0x60(%[k], %[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm2, %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm0, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm0, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%xmm0, %%xmm0")    // xmm0     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vmulss          0x00(%[k], %[off]), %%xmm0, %%xmm0")    // xmm0     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6"
            );
        }

        void lerp_kvv_fma3(float *dst, float a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    0x20(%[k], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfnmadd132ps    0x40(%[k], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vfnmadd132ps    0x60(%[k], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    0x20(%[k], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ss    0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6"
            );
        }

        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[k], %%ymm7")                          // ymm7     = k
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm0, %%ymm0")                // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm2, %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          %%ymm3, %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vmulps          %%ymm7, %%ymm0, %%ymm0")                // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vsubps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vmulps          %%ymm7, %%ymm0, %%ymm0")                // ymm0     = (a-b)*k
                __ASM_EMIT("vsubps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vmulps          %%xmm7, %%xmm0, %%xmm0")                // xmm0     = (a-b)*k
                __ASM_EMIT("vsubps          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vmulss          %%xmm7, %%xmm0, %%xmm0")                // xmm0     = (a-b)*k
                __ASM_EMIT("vsubss          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

        void lerp_kvk_fma3(float *dst, float a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[k], %%ymm7")                          // ymm7     = k
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vsubps          0x60(%[b], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm1")
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm2")
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vsubps          0x20(%[b], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    %%xmm7, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ss    %%xmm7, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

        void lerp_kkv(float *dst, float a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[b], %%ymm7")                          // ymm7     = b
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm7, %%ymm7")                // ymm7     = b-a
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm7, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vmulps          0x40(%[k], %[off]), %%ymm7, %%ymm2")
                __ASM_EMIT("vmulps          0x60(%[k], %[off]), %%ymm7, %%ymm3")
                __ASM_EMIT("vaddps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a + (b-a)*k
                __ASM_EMIT("vaddps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vaddps          %%ymm2, %%ymm6, %%ymm2")
                __ASM_EMIT("vaddps          %%ymm3, %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm7, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vmulps          0x20(%[k], %[off]), %%ymm7, %%ymm1")
                __ASM_EMIT("vaddps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a + (b-a)*k
                __ASM_EMIT("vaddps          %%ymm1, %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%ymm7, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vaddps          %%ymm0, %%ymm6, %%ymm0")                // ymm0     = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x00(%[k], %[off]), %%xmm7, %%xmm0")    // xmm0     = (a-b)*k
                __ASM_EMIT("vaddps          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmulss          0x00(%[k], %[off]), %%xmm7, %%xmm0")    // xmm0     = (b-a)*k
                __ASM_EMIT("vaddss          %%xmm0, %%xmm6, %%xmm0")                // xmm0     = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

        void lerp_kkv_fma3(float *dst, float a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("vbroadcastss    %[a], %%ymm6")                          // ymm6     = a
                __ASM_EMIT("vbroadcastss    %[b], %%ymm7")                          // ymm7     = b
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vsubps          %%ymm6, %%ymm7, %%ymm7")                // ymm7     = b-a
                __ASM_EMIT("jb              2f")
                // 32x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm0")                        // ymm0     = b-a
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm1")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm2")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm3")
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vfmadd132ps     0x20(%[k], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vfmadd132ps     0x40(%[k], %[off]), %%ymm6, %%ymm2")
                __ASM_EMIT("vfmadd132ps     0x60(%[k], %[off]), %%ymm6, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm0")                        // ymm0     = b-a
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm1")
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vfmadd132ps     0x20(%[k], %[off]), %%ymm6, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm0")                        // ymm0     = b-a
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = (a-b)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovaps         %%xmm7, %%xmm0")                        // xmm0     = b-a
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = (a-b)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovaps         %%xmm7, %%xmm0")                        // xmm0     = b-a
                __ASM_EMIT("vfmadd132ss     0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = (a-b)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX_PMATH_LERP_H_ */
