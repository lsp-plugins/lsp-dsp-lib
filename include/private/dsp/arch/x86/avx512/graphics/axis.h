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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_GRAPHICS_AXIS_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_GRAPHICS_AXIS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {

        void axis_apply_lin1(float *x, const float *v, float zero, float norm, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    %[norm], %%zmm0")
                __ASM_EMIT("vbroadcastss    %[zero], %%zmm1")
                __ASM_EMIT("xor             %[off], %[off]")
                /* blocks x96 */
                __ASM_EMIT("sub             $96, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vaddps          0x000(%[v], %[off]), %%zmm1, %%zmm2")
                __ASM_EMIT("vaddps          0x040(%[v], %[off]), %%zmm1, %%zmm3")
                __ASM_EMIT("vaddps          0x080(%[v], %[off]), %%zmm1, %%zmm4")
                __ASM_EMIT("vaddps          0x0c0(%[v], %[off]), %%zmm1, %%zmm5")
                __ASM_EMIT("vaddps          0x100(%[v], %[off]), %%zmm1, %%zmm6")
                __ASM_EMIT("vaddps          0x140(%[v], %[off]), %%zmm1, %%zmm7")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm3, %%zmm3")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm4, %%zmm4")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm5, %%zmm5")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm6, %%zmm6")
                __ASM_EMIT("vmulps          %%zmm0, %%zmm7, %%zmm7")
                __ASM_EMIT("vaddps          0x000(%[x], %[off]), %%zmm2, %%zmm2")
                __ASM_EMIT("vaddps          0x040(%[x], %[off]), %%zmm3, %%zmm3")
                __ASM_EMIT("vaddps          0x080(%[x], %[off]), %%zmm4, %%zmm4")
                __ASM_EMIT("vaddps          0x0c0(%[x], %[off]), %%zmm5, %%zmm5")
                __ASM_EMIT("vaddps          0x100(%[x], %[off]), %%zmm6, %%zmm6")
                __ASM_EMIT("vaddps          0x140(%[x], %[off]), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups         %%zmm2, 0x000(%[x], %[off])")
                __ASM_EMIT("vmovups         %%zmm3, 0x040(%[x], %[off])")
                __ASM_EMIT("vmovups         %%zmm4, 0x080(%[x], %[off])")
                __ASM_EMIT("vmovups         %%zmm5, 0x0c0(%[x], %[off])")
                __ASM_EMIT("vmovups         %%zmm6, 0x100(%[x], %[off])")
                __ASM_EMIT("vmovups         %%zmm7, 0x140(%[x], %[off])")
                __ASM_EMIT("add             $0x180, %[off]")
                __ASM_EMIT("sub             $96, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                /* blocks x32 */
                __ASM_EMIT("add             $64, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("3:")
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
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jge             3b")
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

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_GRAPHICS_AXIS_H_ */
