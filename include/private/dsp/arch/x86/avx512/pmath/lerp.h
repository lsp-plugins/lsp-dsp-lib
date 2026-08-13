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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LERP_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {

        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT32("subl          $64, %[count]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("jb              2f")
                // 64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vmovups         0x80(%[a], %[off]), %%zmm2")
                __ASM_EMIT("vmovups         0xc0(%[a], %[off]), %%zmm3")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm4")    // zmm4     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm1, %%zmm5")
                __ASM_EMIT("vsubps          0x80(%[b], %[off]), %%zmm2, %%zmm6")
                __ASM_EMIT("vsubps          0xc0(%[b], %[off]), %%zmm3, %%zmm7")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm4, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vfnmadd231ps    0x80(%[k], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vfnmadd231ps    0xc0(%[k], %[off]), %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT32("subl          $64, %[count]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl          $32, %[count]")
                __ASM_EMIT64("add           $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm4")    // zmm4     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm1, %%zmm5")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm4, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%zmm5, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT32("subl          $32, %[count]")
                __ASM_EMIT64("sub           $32, %[count]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl          $16, %[count]")
                __ASM_EMIT64("add           $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm4")    // zmm4     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm4, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm4")    // ymm4     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm4, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT32("subl          $8, %[count]")
                __ASM_EMIT64("sub           $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT32("addl          $4, %[count]")
                __ASM_EMIT64("add           $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%xmm4, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT32("subl          $4, %[count]")
                __ASM_EMIT64("sub           $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT32("addl          $3, %[count]")
                __ASM_EMIT64("add           $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm4")    // xmm4     = a-b
                __ASM_EMIT("vfnmadd231ss    0x00(%[k], %[off]), %%xmm4, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT32("decl          %[count]")
                __ASM_EMIT64("dec           %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[k], %%zmm6")                          // zmm6     = k
                __ASM_EMIT("jb              2f")
                // 48x/64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vmovups         0x80(%[a], %[off]), %%zmm2")
                __ASM_EMIT64("vmovups       0xc0(%[a], %[off]), %%zmm8")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm3")    // zmm3     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm1, %%zmm4")
                __ASM_EMIT("vsubps          0x80(%[b], %[off]), %%zmm2, %%zmm5")
                __ASM_EMIT64("vsubps        0xc0(%[b], %[off]), %%zmm8, %%zmm9")
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm4, %%zmm1")
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm5, %%zmm2")
                __ASM_EMIT64("vfnmadd231ps  %%zmm6, %%zmm9, %%zmm8")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%zmm8, 0xc0(%[dst], %[off])")
                __ASM_EMIT32("add           $0xc0, %[off]")
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("add           $0x100, %[off]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $16, %[count]")
                __ASM_EMIT64("add           $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm3")    // zmm3     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm1, %%zmm4")
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm4, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm0, %%zmm3")    // zmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%zmm6, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm0, %%ymm3")    // ymm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%ymm6, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%xmm6, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm0, %%xmm3")    // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ss    %%xmm6, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%zmm6")                          // zmm6     = b
                __ASM_EMIT("jb              2f")
                // 48x/64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vmovups         0x80(%[a], %[off]), %%zmm2")
                __ASM_EMIT64("vmovups       0xc0(%[a], %[off]), %%zmm8")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vsubps          %%zmm6, %%zmm1, %%zmm4")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm2, %%zmm5")
                __ASM_EMIT64("vsubps        %%zmm6, %%zmm8, %%zmm9")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm3, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%zmm4, %%zmm1")
                __ASM_EMIT("vfnmadd231ps    0x80(%[k], %[off]), %%zmm5, %%zmm2")
                __ASM_EMIT64("vfnmadd231ps  0xc0(%[k], %[off]), %%zmm9, %%zmm8")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%zmm8, 0xc0(%[dst], %[off])")
                __ASM_EMIT32("add           $0xc0, %[off]")
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("add           $0x100, %[off]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $16, %[count]")
                __ASM_EMIT64("add           $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vsubps          %%zmm6, %%zmm1, %%zmm4")
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm3, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    0x40(%[k], %[off]), %%zmm4, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%zmm3, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%ymm3, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    0x00(%[k], %[off]), %%xmm3, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ss    0x00(%[k], %[off]), %%xmm3, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[b], %%zmm6")                          // zmm6     = b
                __ASM_EMIT("vbroadcastss    %[k], %%zmm7")                          // zmm7     = k
                __ASM_EMIT("jb              2f")
                // 48x/64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vmovups         0x80(%[a], %[off]), %%zmm2")
                __ASM_EMIT64("vmovups       0xc0(%[a], %[off]), %%zmm8")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vsubps          %%zmm6, %%zmm1, %%zmm4")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm2, %%zmm5")
                __ASM_EMIT64("vsubps        %%zmm6, %%zmm8, %%zmm9")
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm4, %%zmm1")
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm5, %%zmm2")
                __ASM_EMIT64("vfnmadd231ps  %%zmm7, %%zmm9, %%zmm8")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT64("vmovups       %%zmm8, 0xc0(%[dst], %[off])")
                __ASM_EMIT32("add           $0xc0, %[off]")
                __ASM_EMIT32("sub           $48, %[count]")
                __ASM_EMIT64("add           $0x100, %[off]")
                __ASM_EMIT64("sub           $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("add           $16, %[count]")
                __ASM_EMIT64("add           $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm1")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vsubps          %%zmm6, %%zmm1, %%zmm4")
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm4, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm0")            // zmm0     = a
                __ASM_EMIT("vsubps          %%zmm6, %%zmm0, %%zmm3")                // zmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%zmm7, %%zmm3, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm0")            // ymm0     = a
                __ASM_EMIT("vsubps          %%ymm6, %%ymm0, %%ymm3")                // ymm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%ymm7, %%ymm3, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubps          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ps    %%xmm7, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm0")            // xmm0     = a
                __ASM_EMIT("vsubss          %%xmm6, %%xmm0, %%xmm3")                // xmm3     = a-b
                __ASM_EMIT("vfnmadd231ss    %%xmm7, %%xmm3, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%zmm6")                          // zmm6     = a
                __ASM_EMIT("jb              2f")
                // 64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vsubps          0x80(%[b], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vsubps          0xc0(%[b], %[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    0x40(%[k], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfnmadd132ps    0x80(%[k], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vfnmadd132ps    0xc0(%[k], %[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    0x40(%[k], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ss    0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vbroadcastss    %[a], %%zmm6")                          // zmm6     = a
                __ASM_EMIT("vbroadcastss    %[k], %%zmm7")                          // zmm7     = k
                __ASM_EMIT("jb              2f")
                // 64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vsubps          0x80(%[b], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vsubps          0xc0(%[b], %[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm1")
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm2")
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vsubps          0x40(%[b], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%zmm6, %%zmm0")    // zmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    %%zmm7, %%zmm6, %%zmm0")                // zmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%ymm6, %%ymm0")    // ymm0     = a-b
                __ASM_EMIT("vfnmadd132ps    %%ymm7, %%ymm6, %%ymm0")                // ymm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vsubps          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ps    %%xmm7, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vsubss          0x00(%[b], %[off]), %%xmm6, %%xmm0")    // xmm0     = a-b
                __ASM_EMIT("vfnmadd132ss    %%xmm7, %%xmm6, %%xmm0")                // xmm0     = a - (a-b)*k = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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
                __ASM_EMIT("vbroadcastss    %[a], %%zmm6")                          // zmm6     = a
                __ASM_EMIT("vbroadcastss    %[b], %%zmm7")                          // zmm7     = b
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("vsubps          %%zmm6, %%zmm7, %%zmm7")                // zmm7     = b-a
                __ASM_EMIT("jb              2f")
                // 64x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm0")                        // zmm0     = b-a
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm1")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm2")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm3")
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a + (b-a)*k
                __ASM_EMIT("vfmadd132ps     0x40(%[k], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vfmadd132ps     0x80(%[k], %[off]), %%zmm6, %%zmm2")
                __ASM_EMIT("vfmadd132ps     0xc0(%[k], %[off]), %%zmm6, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                // 32x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm0")                        // zmm0     = b-a
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm1")
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a + (b-a)*k
                __ASM_EMIT("vfmadd132ps     0x40(%[k], %[off]), %%zmm6, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                // 16x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         %%zmm7, %%zmm0")                        // zmm0     = b-a
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%zmm6, %%zmm0")    // zmm0     = a + (b-a)*k
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                // 8x block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovaps         %%ymm7, %%ymm0")                        // ymm0     = b-a
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%ymm6, %%ymm0")    // ymm0     = a + (b-a)*k
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                // 4x block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovaps         %%xmm7, %%xmm0")                        // xmm0     = b-a
                __ASM_EMIT("vfmadd132ps     0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a + (b-a)*k
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                // 1x blocks
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovaps         %%xmm7, %%xmm0")                        // xmm0     = b-a
                __ASM_EMIT("vfmadd132ss     0x00(%[k], %[off]), %%xmm6, %%xmm0")    // xmm0     = a + (b-a)*k
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

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

    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LERP_H_ */
