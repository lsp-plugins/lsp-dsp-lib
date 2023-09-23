/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 сент. 2023 г.
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

#ifndef INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_AXIS_H_
#define INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_AXIS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        void axis_apply_lin1(float *x, const float *v, float zero, float norm, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    %[norm], %%ymm0")
                __ASM_EMIT("vbroadcastss    %[zero], %%ymm1")
                __ASM_EMIT("xor             %[off], %[off]")
                /* blocks x48 */
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[v], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[v], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vaddps          0x80(%[v], %[off]), %%ymm1, %%ymm6")
                __ASM_EMIT("vaddps          0xa0(%[v], %[off]), %%ymm1, %%ymm7")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm5, %%ymm5")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm6, %%ymm6")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm7, %%ymm7")
                __ASM_EMIT("vaddps          0x00(%[x], %[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[x], %[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[x], %[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[x], %[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vaddps          0x80(%[x], %[off]), %%ymm6, %%ymm6")
                __ASM_EMIT("vaddps          0xa0(%[x], %[off]), %%ymm7, %%ymm7")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm6, 0x80(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm7, 0xa0(%[x], %[off])")
                __ASM_EMIT("add             $0xc0, %[off]")
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                /* block x32 */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[v], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[v], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm5, %%ymm5")
                __ASM_EMIT("vaddps          0x00(%[x], %[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[x], %[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[x], %[off]), %%ymm4, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[x], %[off]), %%ymm5, %%ymm5")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[x], %[off])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("4:")
                /* block x16 */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vaddps          0x00(%[x], %[off]), %%ymm2, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[x], %[off]), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("6:")
                /* block x8 */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vaddps          0x10(%[v], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm3, %%xmm3")
                __ASM_EMIT("vaddps          0x00(%[x], %[off]), %%xmm2, %%xmm2")
                __ASM_EMIT("vaddps          0x10(%[x], %[off]), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%xmm3, 0x10(%[x], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("8:")
                /* block x4 */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm2, %%xmm2")
                __ASM_EMIT("vaddps          0x00(%[x], %[off]), %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("10:")
                /* block x1 */
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vaddss          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vmulss          %%xmm0, %%xmm2, %%xmm2")
                __ASM_EMIT("vaddss          0x00(%[x], %[off]), %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("add             $4, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [x] "r" (x), [v] "r" (v),
                  [norm] "m" (norm),
                  [zero] "m" (zero)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void axis_apply_lin1_fma3(float *x, const float *v, float zero, float norm, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    %[norm], %%ymm0")
                __ASM_EMIT("vbroadcastss    %[zero], %%ymm1")
                __ASM_EMIT("xor             %[off], %[off]")
                /* blocks x48 */
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[v], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[v], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vaddps          0x80(%[v], %[off]), %%ymm1, %%ymm6")
                __ASM_EMIT("vaddps          0xa0(%[v], %[off]), %%ymm1, %%ymm7")
                __ASM_EMIT("vfmadd213ps     0x00(%[x], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vfmadd213ps     0x20(%[x], %[off]), %%ymm0, %%ymm3")
                __ASM_EMIT("vfmadd213ps     0x40(%[x], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT("vfmadd213ps     0x60(%[x], %[off]), %%ymm0, %%ymm5")
                __ASM_EMIT("vfmadd213ps     0x80(%[x], %[off]), %%ymm0, %%ymm6")
                __ASM_EMIT("vfmadd213ps     0xa0(%[x], %[off]), %%ymm0, %%ymm7")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm6, 0x80(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm7, 0xa0(%[x], %[off])")
                __ASM_EMIT("add             $0xc0, %[off]")
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                /* block x32 */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vaddps          0x40(%[v], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vaddps          0x60(%[v], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vfmadd213ps     0x00(%[x], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vfmadd213ps     0x20(%[x], %[off]), %%ymm0, %%ymm3")
                __ASM_EMIT("vfmadd213ps     0x40(%[x], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT("vfmadd213ps     0x60(%[x], %[off]), %%ymm0, %%ymm5")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[x], %[off])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("4:")
                /* block x16 */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%ymm1, %%ymm2")
                __ASM_EMIT("vaddps          0x20(%[v], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vfmadd213ps     0x00(%[x], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vfmadd213ps     0x20(%[x], %[off]), %%ymm0, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[x], %[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("6:")
                /* block x8 */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vaddps          0x10(%[v], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vfmadd213ps     0x00(%[x], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vfmadd213ps     0x10(%[x], %[off]), %%xmm0, %%xmm3")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("vmovups         %%xmm3, 0x10(%[x], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("8:")
                /* block x4 */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vaddps          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vfmadd213ps     0x00(%[x], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("10:")
                /* block x1 */
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vaddss          0x00(%[v], %[off]), %%xmm1, %%xmm2")
                __ASM_EMIT("vfmadd213ss     0x00(%[x], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("add             $4, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")
                __ASM_EMIT("12:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [x] "r" (x), [v] "r" (v),
                  [norm] "m" (norm),
                  [zero] "m" (zero)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_AXIS_H_ */
