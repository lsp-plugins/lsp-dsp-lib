/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_GRAPHICS_AXIS_H_
#define PRIVATE_DSP_ARCH_X86_SSE_GRAPHICS_AXIS_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {

        void axis_apply_lin1(float *x, const float *v, float zero, float norm, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("movss       %[zero], %%xmm1")
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jb          2f")
                /* 12x blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[v], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x10(%[v], %[off]), %%xmm6")
                __ASM_EMIT("movups      0x20(%[v], %[off]), %%xmm7")
                __ASM_EMIT("movups      0x00(%[x], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[x], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[x], %[off]), %%xmm4")
                __ASM_EMIT("addps       %%xmm1, %%xmm5")
                __ASM_EMIT("addps       %%xmm1, %%xmm6")
                __ASM_EMIT("addps       %%xmm1, %%xmm7")
                __ASM_EMIT("mulps       %%xmm0, %%xmm5")
                __ASM_EMIT("mulps       %%xmm0, %%xmm6")
                __ASM_EMIT("mulps       %%xmm0, %%xmm7")
                __ASM_EMIT("addps       %%xmm5, %%xmm2")
                __ASM_EMIT("addps       %%xmm6, %%xmm3")
                __ASM_EMIT("addps       %%xmm7, %%xmm4")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[x], %[off])")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[x], %[off])")
                __ASM_EMIT("add         $0x30, %[off]")
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[v], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x10(%[v], %[off]), %%xmm6")
                __ASM_EMIT("movups      0x00(%[x], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[x], %[off]), %%xmm3")
                __ASM_EMIT("addps       %%xmm1, %%xmm5")
                __ASM_EMIT("addps       %%xmm1, %%xmm6")
                __ASM_EMIT("mulps       %%xmm0, %%xmm5")
                __ASM_EMIT("mulps       %%xmm0, %%xmm6")
                __ASM_EMIT("addps       %%xmm5, %%xmm2")
                __ASM_EMIT("addps       %%xmm6, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[x], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[v], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x00(%[x], %[off]), %%xmm2")
                __ASM_EMIT("addps       %%xmm1, %%xmm5")
                __ASM_EMIT("mulps       %%xmm0, %%xmm5")
                __ASM_EMIT("addps       %%xmm5, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("6:")
                /* 1x blocks */
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[v], %[off]), %%xmm5")
                __ASM_EMIT("movss       0x00(%[x], %[off]), %%xmm2")
                __ASM_EMIT("addss       %%xmm1, %%xmm5")
                __ASM_EMIT("mulss       %%xmm0, %%xmm5")
                __ASM_EMIT("addss       %%xmm5, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[x], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [norm] "+Yz"(norm)
                : [x] "r" (x), [v] "r" (v),
                  [zero] "m" (zero)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_GRAPHICS_AXIS_H_ */
