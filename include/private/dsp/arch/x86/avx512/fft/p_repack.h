/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 18 мар. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FFT_P_REPACK_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FFT_P_REPACK_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        static inline void packed_fft_repack(float *dst, size_t rank)
        {
            size_t blocks = 1 << rank;

            ARCH_X86_ASM
            (
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k4")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k5")
                // 32x blocks
                __ASM_EMIT("sub             $32, %[blocks]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps       0x000(%[dst]), %%zmm0 %{%%k4%}%{z%}")   /* zmm0 =  r0   0  r1   0  r2   0  r3   0  r4   0  r5   0  r6   0  r7   0 */
                __ASM_EMIT("vexpandps       0x020(%[dst]), %%zmm1 %{%%k4%}%{z%}")   /* zmm1 =  r8   0  r9   0 r10   0 r11   0 r12   0 r13   0 r14   0 r15   0 */
                __ASM_EMIT("vexpandps       0x040(%[dst]), %%zmm0 %{%%k5%}")        /* zmm0 =  r0  i0  r1  i1  r2  i2  r3  i3  r4  i4  r5  i5  r6  i6  r7  i7 */
                __ASM_EMIT("vexpandps       0x060(%[dst]), %%zmm1 %{%%k5%}")        /* zmm1 =  r8  i8  r9  i9 r10 i10 r11 i11 r12 i12 r13 i13 r14 i14 r15 i15 */
                __ASM_EMIT("vexpandps       0x080(%[dst]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps       0x0a0(%[dst]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps       0x0c0(%[dst]), %%zmm2 %{%%k5%}")
                __ASM_EMIT("vmovups         0x0e0(%[dst]), %%ymm4")
                __ASM_EMIT("vexpandps       %%zmm4, %%zmm3 %{%%k5%}")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst])")
                __ASM_EMIT("vmovups         %%zmm2, 0x080(%[dst])")
                __ASM_EMIT("vmovups         %%zmm3, 0x0c0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $32, %[blocks]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[blocks]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vexpandps       0x000(%[dst]), %%zmm0 %{%%k4%}%{z%}")   /* zmm0 =  r0   0  r1   0  r2   0  r3   0  r4   0  r5   0  r6   0  r7   0 */
                __ASM_EMIT("vexpandps       0x020(%[dst]), %%zmm1 %{%%k4%}%{z%}")   /* zmm1 =  r8   0  r9   0 r10   0 r11   0 r12   0 r13   0 r14   0 r15   0 */
                __ASM_EMIT("vexpandps       0x040(%[dst]), %%zmm0 %{%%k5%}")        /* zmm0 =  r0  i0  r1  i1  r2  i2  r3  i3  r4  i4  r5  i5  r6  i6  r7  i7 */
                __ASM_EMIT("vmovups         0x060(%[dst]), %%ymm4")
                __ASM_EMIT("vexpandps       %%zmm4, %%zmm1 %{%%k5%}")               /* zmm1 =  r8  i8  r9  i9 r10 i10 r11 i11 r12 i12 r13 i13 r14 i14 r15 i15 */
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst])")
                __ASM_EMIT("sub             $16, %[blocks]")
                __ASM_EMIT("add             $0x100, %[dst]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[blocks]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")                  /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm1")                  /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
                __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")                /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
                __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")                /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
                __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
                __ASM_EMIT("6:")

                : [dst] "+r"(dst), [blocks] "+r" (blocks)
                : [MASK] "o" (FFT_REPACK_MASKS)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4",
                  "%k4", "%k5"
            );
        }

        static inline void packed_fft_repack_normalize(float *dst, size_t rank)
        {
            size_t blocks = 1 << rank;
            const float norm = 1.0f / float(blocks);

            // Perform 4-element butterflies
            ARCH_X86_ASM
            (
                __ASM_EMIT("vbroadcastss    %[norm], %%zmm6")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k4")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k5")
                __ASM_EMIT("vmovaps         %%zmm6, %%zmm7")
                // 32x blocks
                __ASM_EMIT("sub             $32, %[blocks]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps       0x000(%[dst]), %%zmm0 %{%%k4%}%{z%}")   /* zmm0 =  r0   0  r1   0  r2   0  r3   0  r4   0  r5   0  r6   0  r7   0 */
                __ASM_EMIT("vexpandps       0x020(%[dst]), %%zmm1 %{%%k4%}%{z%}")   /* zmm1 =  r8   0  r9   0 r10   0 r11   0 r12   0 r13   0 r14   0 r15   0 */
                __ASM_EMIT("vexpandps       0x040(%[dst]), %%zmm0 %{%%k5%}")        /* zmm0 =  r0  i0  r1  i1  r2  i2  r3  i3  r4  i4  r5  i5  r6  i6  r7  i7 */
                __ASM_EMIT("vexpandps       0x060(%[dst]), %%zmm1 %{%%k5%}")        /* zmm1 =  r8  i8  r9  i9 r10 i10 r11 i11 r12 i12 r13 i13 r14 i14 r15 i15 */
                __ASM_EMIT("vexpandps       0x080(%[dst]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps       0x0a0(%[dst]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps       0x0c0(%[dst]), %%zmm2 %{%%k5%}")
                __ASM_EMIT("vmovups         0x0e0(%[dst]), %%ymm4")
                __ASM_EMIT("vexpandps       %%zmm4, %%zmm3 %{%%k5%}")
                __ASM_EMIT("vmulps          %%zmm6, %%zmm0, %%zmm0")
                __ASM_EMIT("vmulps          %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vmulps          %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst])")
                __ASM_EMIT("vmovups         %%zmm2, 0x080(%[dst])")
                __ASM_EMIT("vmovups         %%zmm3, 0x0c0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $32, %[blocks]")
                __ASM_EMIT("jae             1b")
                // 16x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[blocks]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vexpandps       0x000(%[dst]), %%zmm0 %{%%k4%}%{z%}")   /* zmm0 =  r0   0  r1   0  r2   0  r3   0  r4   0  r5   0  r6   0  r7   0 */
                __ASM_EMIT("vexpandps       0x020(%[dst]), %%zmm1 %{%%k4%}%{z%}")   /* zmm1 =  r8   0  r9   0 r10   0 r11   0 r12   0 r13   0 r14   0 r15   0 */
                __ASM_EMIT("vexpandps       0x040(%[dst]), %%zmm0 %{%%k5%}")        /* zmm0 =  r0  i0  r1  i1  r2  i2  r3  i3  r4  i4  r5  i5  r6  i6  r7  i7 */
                __ASM_EMIT("vmovups         0x060(%[dst]), %%ymm4")
                __ASM_EMIT("vexpandps       %%zmm4, %%zmm1 %{%%k5%}")               /* zmm1 =  r8  i8  r9  i9 r10 i10 r11 i11 r12 i12 r13 i13 r14 i14 r15 i15 */
                __ASM_EMIT("vmulps          %%zmm6, %%zmm0, %%zmm0")
                __ASM_EMIT("vmulps          %%zmm7, %%zmm1, %%zmm1")
                __ASM_EMIT("vmovups         %%zmm0, 0x000(%[dst])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst])")
                __ASM_EMIT("sub             $16, %[blocks]")
                __ASM_EMIT("add             $0x100, %[dst]")
                // 8x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[blocks]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x00(%[dst]), %%ymm6, %%ymm0")          /* ymm0 = r0  r1  r2  r3  r4  r5  r6  r7  */
                __ASM_EMIT("vmulps          0x20(%[dst]), %%ymm7, %%ymm1")          /* ymm1 = i0  i1  i2  i3  i4  i5  i6  i7  */
                __ASM_EMIT("vunpcklps       %%ymm1, %%ymm0, %%ymm4")                /* ymm4 = r0  i0  r1  i1  r4  i4  r5  i5  */
                __ASM_EMIT("vunpckhps       %%ymm1, %%ymm0, %%ymm5")                /* ymm5 = r2  i2  r3  i3  r6  i6  r7  i7  */
                __ASM_EMIT("vmovups         %%xmm4, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%xmm5, 0x10(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vextractf128    $1, %%ymm5, 0x30(%[dst])")
                __ASM_EMIT("6:")

                : [dst] "+r"(dst), [blocks] "+r" (blocks)
                : [norm] "m" (norm),
                  [MASK] "o" (FFT_REPACK_MASKS)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FFT_P_REPACK_H_ */
