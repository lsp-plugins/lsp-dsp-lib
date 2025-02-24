/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 февр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FFT_BUTTERFLY_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FFT_BUTTERFLY_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        #define FFT_BUTTERFLY_BODY16(add_b, add_a) \
            __IF_32(float *ptr1, *ptr2);\
            \
            ARCH_X86_ASM( \
                /* Prepare angle */ \
                __ASM_EMIT32("mov           %[fft_a], %[ptr2]") \
                __ASM_EMIT32("mov           %[dst_re], %[ptr1]") \
                __ASM_EMIT("vmovaps         0x00(%[" __IF_32_64("ptr2", "fft_a") "]), %%zmm6")        /* zmm6 = x_re */ \
                __ASM_EMIT("vmovaps         0x40(%[" __IF_32_64("ptr2", "fft_a") "]), %%zmm7")        /* zmm7 = x_im */ \
                __ASM_EMIT32("mov           %[dst_im], %[ptr2]") \
                /* Start loop */ \
                __ASM_EMIT("1:") \
                    __ASM_EMIT("vmovups         0x00(%[" __IF_32_64("ptr1", "dst_re") "], %[off1]), %%zmm0")    /* zmm0 = a_re */ \
                    __ASM_EMIT("vmovups         0x00(%[" __IF_32_64("ptr1", "dst_re") "], %[off2]), %%zmm2")    /* zmm2 = b_re */ \
                    __ASM_EMIT("vmovups         0x00(%[" __IF_32_64("ptr2", "dst_im") "], %[off1]), %%zmm1")    /* zmm1 = a_im */ \
                    __ASM_EMIT("vmovups         0x00(%[" __IF_32_64("ptr2", "dst_im") "], %[off2]), %%zmm3")    /* zmm3 = b_im */ \
                    /* Calculate complex multiplication */ \
                    __ASM_EMIT("vmulps          %%zmm7, %%zmm2, %%zmm4")            /* zmm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          %%zmm7, %%zmm3, %%zmm5")            /* zmm5 = x_im * b_im */ \
                    __ASM_EMIT(add_b "          %%zmm6, %%zmm2, %%zmm5")            /* zmm5 = c_re = x_re * b_re +- x_im * b_im */ \
                    __ASM_EMIT(add_a "          %%zmm6, %%zmm3, %%zmm4")            /* zmm4 = c_im = x_re * b_im -+ x_im * b_re */ \
                    /* Perform butterfly */ \
                    __ASM_EMIT("vsubps          %%zmm5, %%zmm0, %%zmm2")            /* zmm2 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%zmm4, %%zmm1, %%zmm3")            /* zmm3 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%zmm5, %%zmm0, %%zmm0")            /* zmm0 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%zmm4, %%zmm1, %%zmm1")            /* zmm1 = a_im + c_im */ \
                    /* Store values */ \
                    __ASM_EMIT("vmovups         %%zmm0, 0x00(%[" __IF_32_64("ptr1", "dst_re") "], %[off1])") \
                    __ASM_EMIT("vmovups         %%zmm2, 0x00(%[" __IF_32_64("ptr1", "dst_re") "], %[off2])") \
                    __ASM_EMIT("vmovups         %%zmm1, 0x00(%[" __IF_32_64("ptr2", "dst_im") "], %[off1])") \
                    __ASM_EMIT("vmovups         %%zmm3, 0x00(%[" __IF_32_64("ptr2", "dst_im") "], %[off2])") \
                    __ASM_EMIT("add             $0x40, %[off1]") \
                    __ASM_EMIT("add             $0x40, %[off2]") \
                    __ASM_EMIT32("subl          $16, %[np]") \
                    __ASM_EMIT64("subq          $16, %[np]") \
                    __ASM_EMIT("jz              2f") \
                    /* Rotate angle */ \
                    __ASM_EMIT32("mov           %[fft_w], %[ptr2]") \
                    __ASM_EMIT("vmovaps         0x00(%[" __IF_32_64("ptr2", "fft_w") "]), %%zmm4")        /* zmm4 = w_re */ \
                    __ASM_EMIT("vmovaps         0x40(%[" __IF_32_64("ptr2", "fft_w") "]), %%zmm5")        /* zmm5 = w_im */ \
                    __ASM_EMIT32("mov           %[dst_im], %[ptr2]") \
                    __ASM_EMIT("vmulps          %%zmm5, %%zmm6, %%zmm2")            /* zmm2 = w_im * x_re */ \
                    __ASM_EMIT("vmulps          %%zmm5, %%zmm7, %%zmm3")            /* zmm3 = w_im * x_im */ \
                    __ASM_EMIT("vfmsub132ps     %%zmm4, %%zmm3, %%zmm6")            /* zmm6 = x_re' = w_re * x_re - w_im * x_im */ \
                    __ASM_EMIT("vfmadd132ps     %%zmm4, %%zmm2, %%zmm7")            /* zmm7 = x_im' = w_re * x_im + w_im * x_re */ \
                    /* Repeat loop */ \
                __ASM_EMIT("jmp             1b") \
                __ASM_EMIT("2:") \
                \
                : __IF_32([ptr1] "=&r" (ptr1), [ptr2] "=&r" (ptr2), ) \
                  [off1] "+r" (off1), [off2] "+r" (off2), \
                  [np] X86_PGREG (np) \
                : [dst_re] X86_GREG (dst_re), [dst_im] X86_GREG (dst_im), \
                  [fft_a] X86_GREG (fft_a), \
                  [fft_w] X86_GREG (fft_w) \
                : "cc", "memory",  \
                "%xmm0", "%xmm1", "%xmm2", "%xmm3", \
                "%xmm4", "%xmm5", "%xmm6", "%xmm7"  \
            );

        static inline void butterfly_direct16p(float *dst_re, float *dst_im, size_t rank, size_t blocks)
        {
            size_t pairs = 1 << rank;
            size_t off1 = 0, shift = sizeof(float) << rank;
            const float *fft_a = &FFT_A[(rank - 2) << 5];
            const float *fft_w = &FFT_DW[(rank - 3) << 5];

            for (size_t b=0; b<blocks; ++b)
            {
                size_t off2  = off1 + shift;
                size_t np    = pairs;

                FFT_BUTTERFLY_BODY16("vfmadd231ps", "vfmsub231ps");

                off1        = off2;
            }
        }

        static inline void butterfly_reverse16p(float *dst_re, float *dst_im, size_t rank, size_t blocks)
        {
            size_t pairs = 1 << rank;
            size_t off1 = 0, shift = sizeof(float) << rank;
            const float *fft_a = &FFT_A[(rank - 2) << 5];
            const float *fft_w = &FFT_DW[(rank - 3) << 5];

            for (size_t b=0; b<blocks; ++b)
            {
                size_t off2  = off1 + shift;
                size_t np    = pairs;

                FFT_BUTTERFLY_BODY16("vfmsub231ps", "vfmadd231ps");

                off1        = off2;
            }
        }

    #undef FFT_BUTTERFLY_BODY16
    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FFT_BUTTERFLY_H_ */
