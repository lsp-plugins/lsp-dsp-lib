/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 февр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        static inline void FFT_SCRAMBLE_SELF_DIRECT_NAME(float *dst_re, float *dst_im, size_t rank)
        {
            // Calculate number of items
            size_t items    = (1 << rank) - 1;

            for (size_t i = 1; i < items; ++i)
            {
                size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
                if (i >= j)
                    continue;

                /* Copy the values from the reversed position */
                ARCH_X86_ASM
                (
                    __ASM_EMIT("vmovss (%[dst_re], %[i], 4), %%xmm0")
                    __ASM_EMIT("vmovss (%[dst_im], %[i], 4), %%xmm1")
                    __ASM_EMIT("vmovss (%[dst_re], %[j], 4), %%xmm2")
                    __ASM_EMIT("vmovss (%[dst_im], %[j], 4), %%xmm3")
                    __ASM_EMIT("vmovss %%xmm2, (%[dst_re], %[i], 4)")
                    __ASM_EMIT("vmovss %%xmm3, (%[dst_im], %[i], 4)")
                    __ASM_EMIT("vmovss %%xmm0, (%[dst_re], %[j], 4)")
                    __ASM_EMIT("vmovss %%xmm1, (%[dst_im], %[j], 4)")
                    :
                    : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                      [i] "r"(i), [j] "r"(j)
                    : "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                );
            }

            // Perform butterfly 8x
            size_t off = 0;
            items = 1 << (rank - 3);

            // Perform 4-element butterflies
            ARCH_X86_ASM
            (
                /* Loop 4x 4-element butterflies */
                __ASM_EMIT("sub             $4, %[items]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                    /* Load data to registers */
                    __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%zmm0")               /* zmm0 = r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 */
                    __ASM_EMIT("vmovups         0x40(%[dst_re], %[off]), %%zmm4")               /* zmm4 = r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 */
                    __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%zmm2")               /* zmm2 = i0 i1 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11 i12 i13 i14 i15 */
                    __ASM_EMIT("vmovups         0x40(%[dst_im], %[off]), %%zmm5")               /* zmm4 = i16 i17 i18 i19 i20 i21 i22 i23 i24 i25 i26 i27 i28 i29 i30 i31 */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vmovaps         %%zmm0, %%zmm1")                                /* zmm1 = zmm0 = r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 */
                    __ASM_EMIT("vmovaps         %%zmm2, %%zmm3")                                /* zmm3 = zmm2 = i0 i1 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11 i12 i13 i14 i15 */
                    __ASM_EMIT("vmovaps         0x00 + %[FFT_I], %%zmm6")                       /* zmm6 = even indices[0] */
                    __ASM_EMIT("vmovaps         0x40 + %[FFT_I], %%zmm7")                       /* zmm7 = odd indices[0] */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm6, %%zmm0")                        /* zmm0 = r0 r2 r4 r6 r8 r10 r12 r14 r16 r18 r20 r22 r24 r26 r28 r30 */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm7, %%zmm1")                        /* zmm1 = r1 r3 r5 r7 r9 r11 r13 r15 r17 r19 r21 r23 r25 r27 r29 r31 */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm6, %%zmm2")                        /* zmm2 = i0 i2 i4 i6 i8 i10 i12 i14 i16 i18 i20 i22 i24 i26 i28 i30 */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm7, %%zmm3")                        /* zmm3 = i1 i3 i5 i7 i9 i11 i13 i15 i17 i19 i21 i23 i25 i27 i29 i31 */
                    __ASM_EMIT("vsubps          %%zmm1, %%zmm0, %%zmm4")                        /* zmm4 = r0-r1 r2-r3 r4-r5 r6-r7 ... = r1' r3' r5' r7' r9' r11' r13' r15' r17' r19' r21' r23' r25' r27' r29' r31' */
                    __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm5")                        /* zmm5 = i0-i1 i2-i3 i4-i5 i6-i7 ... = i1' i3' i5' i7' i9' i11' i13' i15' i17' i19' i21' i23' i25' i27' i29' i31' */
                    __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")                        /* zmm0 = r0+r1 r2+r3 r4+r5 r6+r7 ... = r0' r2' r4' r6' r8' r10' r12' r14' r16' r18' r20' r22' r24' r26' r28' r30' */
                    __ASM_EMIT("vaddps          %%zmm3, %%zmm2, %%zmm2")                        /* zmm2 = i0+i1 i2+i3 i4+i5 i6+i7 ... = i0' i2' i4' i6' i8' i10' i12' i14' i16' i18' i20' i22' i24' i26' i28' i30' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vmovaps         %%zmm0, %%zmm1")                                /* zmm1 = zmm0 = r0' r2' r4' r6' r8' r10' r12' r14' r16' r18' r20' r22' r24' r26' r28' r30' */
                    __ASM_EMIT("vmovaps         %%zmm2, %%zmm3")                                /* zmm3 = zmm2 = i0' i2' i4' i6' i8' i10' i12' i14' i16' i18' i20' i22' i24' i26' i28' i30' */
                    __ASM_EMIT("vmovaps         0x80 + %[FFT_I], %%zmm6")                       /* zmm6 = even indices[1] */
                    __ASM_EMIT("vmovaps         0xc0 + %[FFT_I], %%zmm7")                       /* zmm7 = odd indices[1] */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm6, %%zmm0")                        /* zmm0 = r0' r4' r1' r5' r8' r12' r9' r13' r16' r20' r17' r21' r24' r28' r25' r29' */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm7, %%zmm1")                        /* zmm1 = r2' r6' i3' i7' r10' r14' i11' i15' r18' r22' i19' i23' r26' r30' i27' i31' */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm6, %%zmm2")                        /* zmm2 = i0' i4' i1' i5' i8' i12' i9' i13' i16' i20' i17' i21' i24' i28' i25' i29' */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm7, %%zmm3")                        /* zmm3 = i2' i6' r3' r7' i10' i14' r11' r15' i18' i22' r19' r23' i26' i30' r27' r31' */
                    __ASM_EMIT("vsubps          %%zmm1, %%zmm0, %%zmm4")                        /* zmm4 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' ... = r2" r6" r3" r7" r10" r14" r11" r15" r18" r22" r19" r23" r26" r30" r27" r31" */
                    __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm5")                        /* zmm5 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' ... = i2" i6" i1" i5" i10" i14"  i9" i13" i18" i22" i17" i21" i26" i30" i25" i29" */
                    __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")                        /* zmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' ... = r0" r4" r1" r5"  r8" r12"  r9" r13" r16" r20" r17" r21" r24" r28" r25" r29" */
                    __ASM_EMIT("vaddps          %%zmm3, %%zmm2, %%zmm2")                        /* zmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' ... = i0" i4" i3" i7"  i8" i12" r11" r15" i16" i20" i19" i23" i24" i28" i27" i31" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmovaps         %%zmm0, %%zmm1")                                /* zmm1 = zmm0 = r0" r4" r1" r5"  r8" r12"  r9" r13" r16" r20" r17" r21" r24" r28" r25" r29" */
                    __ASM_EMIT("vmovaps         %%zmm2, %%zmm3")                                /* zmm3 = zmm2 = i0" i4" i3" i7"  i8" i12" r11" r15" i16" i20" i19" i23" i24" i28" i27" i31" */
                    __ASM_EMIT("vmovaps         0x80 + %[FFT_I], %%zmm6")                       /* zmm6 = even indices[1] */
                    __ASM_EMIT("vmovaps         0xc0 + %[FFT_I], %%zmm7")                       /* zmm7 = odd indices[1] */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm6, %%zmm0")                        /* zmm0 = a_re = r0" r1" r2" r3"  r8"  r9"  r10" r11" r16" r17" r18" r19" r24" r25" r26" r27" */
                    __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm7, %%zmm1")                        /* zmm1 = b_re = r4" r5" r6" r7"  r12" r13" r14" r15" r20" r21" r22" r23" r28" r29" r30" r31" */
                    __ASM_EMIT("vmovaps         0x100 + %[FFT_I], %%zmm6")                      /* zmm6 = even indices[2] */
                    __ASM_EMIT("vmovaps         0x140 + %[FFT_I], %%zmm7")                      /* zmm7 = odd indices[2] */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm6, %%zmm2")                        /* zmm2 = a_im = i0" i1" i2" i3"  i8"  i9"  i10" i11" i16" i17" i18" i19" i24" i25" i26" i27" */
                    __ASM_EMIT("vpermt2ps       %%zmm5, %%zmm7, %%zmm3")                        /* zmm3 = b_im = i4" i5" i6" i7"  i12" i13" i14" i15" i20" i21" i22" i23" i28" i29" i30" i31" */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%zmm1, %%zmm4")               /* zmm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%zmm3, %%zmm5")               /* zmm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%zmm1, %%zmm5")               /* zmm5 = c_re = x_re * b_re + x_im * b_im */
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%zmm3, %%zmm4")               /* zmm4 = c_im = x_re * b_im - x_im * b_re */
                    __ASM_EMIT("vsubps          %%zmm5, %%zmm0, %%zmm1")                        /* zmm1 = a_re - c_re = r16` r17` r18` r19` r20` r21` r22` r23` r24` r25` r26` r27` r28` r29` r30` r31` */
                    __ASM_EMIT("vsubps          %%zmm4, %%zmm2, %%zmm3")                        /* zmm3 = a_im - c_im = i16` i17` i18` i19` i20` i21` i22` i23` i24` i25` i26` i27` i28` i29` i30` i31` */
                    __ASM_EMIT("vaddps          %%zmm5, %%zmm0, %%zmm0")                        /* zmm0 = a_re + c_re = r0` r1` r2` r3` r4` r5` r6` r7` r8` r9` r10` r11` r12` r13` r14` r15` */
                    __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        /* zmm2 = a_im + c_im = i0` i1` i2` i3` i4` i5` i6` i7` i8` i9` i10` i11` i12` i13` i14` i15` */
                    /* 4th-order 16x butterfly */
                    __ASM_EMIT("vmovaps         %%zmm0, %%zmm4")                                /* zmm4 = zmm0 = r0` r1` r2` r3` r4` r5` r6` r7` r8` r9` r10` r11` r12` r13` r14` r15` */
                    __ASM_EMIT("vmovaps         %%zmm2, %%zmm5")                                /* zmm5 = zmm2 = i0` i1` i2` i3` i4` i5` i6` i7` i8` i9` i10` i11` i12` i13` i14` i15` */
                    __ASM_EMIT("vmovaps         0x180 + %[FFT_I], %%zmm6")                      /* zmm6 = even indices[3] */
                    __ASM_EMIT("vmovaps         0x1c0 + %[FFT_I], %%zmm7")                      /* zmm7 = odd indices[3] */
                    __ASM_EMIT("vpermt2ps       %%zmm1, %%zmm6, %%zmm0")                        /* zmm0 = a_re` = r0` r1` r2`  r3`  r4`  r5`  r6`  r7`  r16` r17` r18` r19` r20` r21` r22` r23` */
                    __ASM_EMIT("vpermt2ps       %%zmm1, %%zmm7, %%zmm4")                        /* zmm4 = b_re` = r8` r9` r10` r11` r12` r13` r14` r15` r24` r25` r26` r27` r28` r29` r30` r31` */
                    __ASM_EMIT("vpermt2ps       %%zmm3, %%zmm6, %%zmm2")                        /* zmm2 = a_im` = i0` i1` i2`  i3`  i4`  i5`  i6`  i7`  i16` i17` i18` i19` i20` i21` i22` i23` */
                    __ASM_EMIT("vpermt2ps       %%zmm3, %%zmm7, %%zmm5")                        /* zmm5 = b_im` = i8` i9` i10` i11` i12` i13` i14` i15` i24` i25` i26` i27` i28` i29` i30` i31` */
                    __ASM_EMIT("vmulps          0xc0 + %[FFT_A], %%zmm4, %%zmm1")               /* zmm1 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0xc0 + %[FFT_A], %%zmm5, %%zmm3")               /* zmm3 = x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x80 + %[FFT_A], %%zmm4, %%zmm3")               /* zmm3 = c_re = x_re * b_re + x_im * b_im */
                    __ASM_EMIT("vfmsub231ps     0x80 + %[FFT_A], %%zmm5, %%zmm1")               /* zmm1 = c_im = x_re * b_im - x_im * b_re */
                    __ASM_EMIT("vsubps          %%zmm1, %%zmm0, %%zmm4")                        /* zmm4 = a_re - c_re */
                    __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm5")                        /* zmm5 = a_im - c_im */
                    __ASM_EMIT("vaddps          %%zmm1, %%zmm0, %%zmm0")                        /* zmm0 = a_re + c_re */
                    __ASM_EMIT("vaddps          %%zmm3, %%zmm2, %%zmm2")                        /* zmm2 = a_im + c_im */
                    /* Store */
                    __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf64x4   $1, %%zmm0, 0x40(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf64x4   $1, %%zmm4, 0xc0(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst_im], %[off])")
                    __ASM_EMIT("vmovups         %%ymm5, 0x20(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf64x4   $1, %%zmm2, 0x40(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf64x4   $1, %%zmm5, 0xc0(%[dst_im], %[off])")
                    /* Move pointers and repeat*/
                    __ASM_EMIT("add             $0x80, %[off]")
                    __ASM_EMIT("sub             $4, %[items]")
                    __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")

                /* Loop 2x 4-element butterflies */
                __ASM_EMIT("add             $2, %[items]")
                __ASM_EMIT("jl              4f")
                    /* Load data to registers */
                    __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%xmm0")               /* xmm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_re], %[off]), %%xmm4")               /* xmm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vinsertf128     $1, 0x20(%[dst_re], %[off]), %%ymm0, %%ymm0")   /* ymm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vinsertf128     $1, 0x30(%[dst_re], %[off]), %%ymm4, %%ymm4")   /* ymm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%xmm2")               /* xmm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_im], %[off]), %%xmm6")               /* xmm6 = i4 i5 i6 i7 */
                    __ASM_EMIT("vinsertf128     $1, 0x20(%[dst_im], %[off]), %%ymm2, %%ymm2")   /* ymm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vinsertf128     $1, 0x30(%[dst_im], %[off]), %%ymm6, %%ymm6")   /* ymm6 = i4 i5 i6 i7 */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                        /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                        /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                        /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                        /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")                 /* ymm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")                 /* ymm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                        /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                    __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                        /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                        /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                    __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                        /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm3, %%ymm2, %%ymm4")                 /* ymm4 = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm2, %%ymm3, %%ymm5")                 /* ymm5 = i2" i6" i3" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm2")                 /* ymm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm3")                 /* ymm3 = r4" r5" r6" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm6")                 /* ymm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm7")                 /* ymm7 = i4" i5" i6" i7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm3, %%ymm4")               /* ymm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm7, %%ymm5")               /* ymm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%ymm3, %%ymm5")               /* ymm5 = c_re = x_re * b_re + x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%ymm7, %%ymm4")               /* ymm4 = c_im = x_re * b_im - x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                        /* ymm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                        /* ymm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                        /* ymm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                        /* ymm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm2, 0x20(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm0, 0x30(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im], %[off])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm3, 0x20(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm1, 0x30(%[dst_im], %[off])")
                    /* Move pointers and repeat*/
                    __ASM_EMIT("add             $0x40, %[off]")
                    __ASM_EMIT("sub             $2, %[items]")
                __ASM_EMIT("4:")
                /* x4 scramble block */
                __ASM_EMIT("add             $1, %[items]")
                __ASM_EMIT("jl              6f")
                    __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%xmm0")       /* xmm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_re], %[off]), %%xmm4")       /* xmm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%xmm2")       /* xmm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_im], %[off]), %%xmm6")       /* xmm6 = i4 i5 i6 i7 */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%xmm6, %%xmm2, %%xmm3")                /* xmm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%xmm3, %%xmm1, %%xmm4")         /* xmm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%xmm1, %%xmm3, %%xmm5")         /* xmm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                    __ASM_EMIT("vhsubps         %%xmm5, %%xmm2, %%xmm3")                /* xmm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                    __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                    __ASM_EMIT("vhaddps         %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                    __ASM_EMIT("vblendps        $0xcc, %%xmm3, %%xmm2, %%xmm4")         /* xmm4 = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%xmm2, %%xmm3, %%xmm5")         /* xmm5 = i2" i6" i3" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%xmm1, %%xmm0, %%xmm2")         /* xmm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%xmm1, %%xmm0, %%xmm3")         /* xmm3 = r4" r5" r6" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm6")         /* xmm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm7")         /* xmm7 = i4" i5" i6" i7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%xmm3, %%xmm4")       /* xmm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%xmm7, %%xmm5")       /* xmm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps 0x00 + %[FFT_A], %%xmm3, %%xmm5")           /* xmm5 = c_re = x_re * b_re + x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps 0x00 + %[FFT_A], %%xmm7, %%xmm4")           /* xmm4 = c_im = x_re * b_im - x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%xmm5, %%xmm2, %%xmm0")                /* xmm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm1")                /* xmm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%xmm4, %%xmm6, %%xmm3")                /* xmm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im], %[off])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im], %[off])")
                __ASM_EMIT("6:")

                : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im),
                  [off] "+r" (off), [items] "+r"(items)
                : [FFT_A] "o" (FFT_A), [FFT_I] "o" (FFT_SCRAMBLE_DIRECT_INDICES)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        static inline void FFT_SCRAMBLE_SELF_REVERSE_NAME(float *dst_re, float *dst_im, size_t rank)
        {
            // Calculate number of items
            size_t items    = (1 << rank) - 1;

            for (size_t i = 1; i < items; ++i)
            {
                size_t j = reverse_bits(FFT_TYPE(i), rank);    /* Reverse the order of the bits */
                if (i >= j)
                    continue;

                /* Copy the values from the reversed position */
                ARCH_X86_ASM
                (
                    __ASM_EMIT("vmovss (%[dst_re], %[i], 4), %%xmm0")
                    __ASM_EMIT("vmovss (%[dst_im], %[i], 4), %%xmm1")
                    __ASM_EMIT("vmovss (%[dst_re], %[j], 4), %%xmm2")
                    __ASM_EMIT("vmovss (%[dst_im], %[j], 4), %%xmm3")
                    __ASM_EMIT("vmovss %%xmm2, (%[dst_re], %[i], 4)")
                    __ASM_EMIT("vmovss %%xmm3, (%[dst_im], %[i], 4)")
                    __ASM_EMIT("vmovss %%xmm0, (%[dst_re], %[j], 4)")
                    __ASM_EMIT("vmovss %%xmm1, (%[dst_im], %[j], 4)")
                    :
                    : [dst_re] "r"(dst_re), [dst_im] "r"(dst_im),
                      [i] "r"(i), [j] "r"(j)
                    : "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3"
                );
            }

            // Perform butterfly 8x
            size_t off = 0;
            items = 1 << (rank - 3);

            // Perform 4-element butterflies
            ARCH_X86_ASM
            (
                /* Loop 4x 4-element butterflies */
                __ASM_EMIT("sub             $4, %[items]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                    /* Load data to registers */
                    /* TODO */
                    /* Move pointers and repeat*/
                    __ASM_EMIT("add             $0x80, %[off]")
                    __ASM_EMIT("sub             $4, %[items]")
                    __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")

                /* Loop 2x 4-element butterflies */
                __ASM_EMIT("add             $2, %[items]")
                __ASM_EMIT("jl              4f")
                    /* Load data to registers */
                    __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%xmm0")               /* xmm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_re], %[off]), %%xmm4")               /* xmm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vinsertf128     $1, 0x20(%[dst_re], %[off]), %%ymm0, %%ymm0")   /* ymm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vinsertf128     $1, 0x30(%[dst_re], %[off]), %%ymm4, %%ymm4")   /* ymm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%xmm2")               /* xmm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_im], %[off]), %%xmm6")               /* xmm6 = i4 i5 i6 i7 */
                    __ASM_EMIT("vinsertf128     $1, 0x20(%[dst_im], %[off]), %%ymm2, %%ymm2")   /* ymm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vinsertf128     $1, 0x30(%[dst_im], %[off]), %%ymm6, %%ymm6")   /* ymm6 = i4 i5 i6 i7 */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                    __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                    __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm1, %%ymm0, %%ymm4")         /* ymm4 = r0" i4" r1" r5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm0, %%ymm1, %%ymm5")         /* ymm5 = r2" r6" r3" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm3, %%ymm2, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%ymm3, %%ymm5")       /* ymm5 = c_re = x_re * b_re - x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%ymm7, %%ymm4")       /* ymm4 = c_im = x_re * b_im + x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm2, 0x20(%[dst_re], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm0, 0x30(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im], %[off])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm3, 0x20(%[dst_im], %[off])")
                    __ASM_EMIT("vextractf128    $1, %%ymm1, 0x30(%[dst_im], %[off])")
                    /* Move pointers and repeat*/
                    __ASM_EMIT("add             $0x40, %[off]")
                    __ASM_EMIT("sub             $2, %[items]")
                __ASM_EMIT("4:")
                /* x4 scramble block */
                __ASM_EMIT("add             $1, %[items]")
                __ASM_EMIT("jl              6f")
                    __ASM_EMIT("vmovups         0x00(%[dst_re], %[off]), %%xmm0")       /* xmm0 = r0 r1 r2 r3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_re], %[off]), %%xmm4")       /* xmm4 = r4 r5 r6 r7 */
                    __ASM_EMIT("vmovups         0x00(%[dst_im], %[off]), %%xmm2")       /* xmm2 = i0 i1 i2 i3 */
                    __ASM_EMIT("vmovups         0x10(%[dst_im], %[off]), %%xmm6")       /* xmm6 = i4 i5 i6 i7 */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%xmm6, %%xmm2, %%xmm3")                /* xmm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%xmm6, %%xmm2, %%xmm2")                /* xmm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%xmm3, %%xmm1, %%xmm4")         /* xmm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%xmm1, %%xmm3, %%xmm5")         /* xmm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%xmm4, %%xmm0, %%xmm1")                /* xmm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                    __ASM_EMIT("vhsubps         %%xmm5, %%xmm2, %%xmm3")                /* xmm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                    __ASM_EMIT("vhaddps         %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                    __ASM_EMIT("vhaddps         %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%xmm1, %%xmm0, %%xmm4")         /* xmm4 = r0" i4" r1" r5" */
                    __ASM_EMIT("vblendps        $0xcc, %%xmm0, %%xmm1, %%xmm5")         /* xmm5 = r2" r6" r3" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%xmm3, %%xmm2, %%xmm6")         /* xmm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%xmm3, %%xmm2, %%xmm7")         /* xmm7 = i4" i5" i6" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%xmm5, %%xmm4, %%xmm2")         /* xmm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%xmm5, %%xmm4, %%xmm3")         /* xmm3 = r4" r5" r6" r7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%xmm3, %%xmm4")       /* xmm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%xmm7, %%xmm5")       /* xmm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%xmm3, %%xmm5")       /* xmm5 = c_re = x_re * b_re - x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%xmm7, %%xmm4")       /* xmm4 = c_im = x_re * b_im + x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%xmm5, %%xmm2, %%xmm0")                /* xmm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%xmm4, %%xmm6, %%xmm1")                /* xmm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")                /* xmm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%xmm4, %%xmm6, %%xmm3")                /* xmm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re], %[off])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im], %[off])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im], %[off])")
                __ASM_EMIT("6:")

                : [dst_re] "+r"(dst_re), [dst_im] "+r"(dst_im),
                  [off] "+r" (off), [items] "+r"(items)
                : [FFT_A] "o" (FFT_A)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        static inline void FFT_SCRAMBLE_COPY_DIRECT_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t regs     = 1 << rank;

            for (size_t i=0; i<regs; ++i)
            {
                size_t index    = reverse_bits(FFT_TYPE(i), rank);

                ARCH_X86_ASM
                (
                    /* Load scalar values */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  x r2  x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  x i2  x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  x r10 x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  x i10 x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  x r6  x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  x i6  x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 x r14 x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 x i14 x       */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  r1  r2  x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  i1  i2  x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  r9  r10 x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  i9  i10 x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  r5  r6  x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  i5  i6  x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 r13 r14 x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 i13 i14 x     */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  r1  r2  r3    */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  i1  i2  i3    */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  r9  r10 r11   */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  i9  i10 i11   */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  r5  r6  r7    */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  i5  i6  i7    */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 r13 r14 r15   */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 i13 i14 i15   */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm0, %%ymm0")            /* ymm0 = r0 r1 r2 r3 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm3, %%ymm2, %%ymm2")            /* ymm2 = i0 i1 i2 i3 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm4, %%ymm4")            /* ymm4 = r4 r5 r6 r7 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm6, %%ymm6")            /* ymm0 = i4 i5 i6 i7 ...   */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r3" r7" */
                    __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i1" i5" */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r1" r5" */
                    __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i3" i7" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm3, %%ymm2, %%ymm4")         /* ymm4 = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm2, %%ymm3, %%ymm5")         /* ymm5 = i2" i6" i3" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm1, %%ymm0, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm1, %%ymm0, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%ymm3, %%ymm5")       /* ymm5 = c_re = x_re * b_re + x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%ymm7, %%ymm4")       /* ymm4 = c_im = x_re * b_im - x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re])")
                    __ASM_EMIT("vextractf128    $1, %%ymm2, 0x20(%[dst_re])")
                    __ASM_EMIT("vextractf128    $1, %%ymm0, 0x30(%[dst_re])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im])")
                    __ASM_EMIT("vextractf128    $1, %%ymm3, 0x20(%[dst_im])")
                    __ASM_EMIT("vextractf128    $1, %%ymm1, 0x30(%[dst_im])")
                    __ASM_EMIT("add             $0x40, %[dst_re]")
                    __ASM_EMIT("add             $0x40, %[dst_im]")

                    : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
                    : [src_re] "r" (src_re), [src_im] "r"(src_im),
                      [regs] X86_GREG (regs),
                      [FFT_A] "o" (FFT_A)
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                      "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );
            }
        }

        static inline void FFT_SCRAMBLE_COPY_REVERSE_NAME(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            size_t regs     = 1 << rank;

            for (size_t i=0; i<regs; ++i)
            {
                size_t index    = reverse_bits(FFT_TYPE(i), rank);

                ARCH_X86_ASM
                (
                    /* Load scalar values */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x00, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 x x x         */
                    __ASM_EMIT("vinsertps       $0x00, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 x x x         */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  x r2  x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  x i2  x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  x r10 x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  x i10 x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  x r6  x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  x i6  x       */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x20, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 x r14 x       */
                    __ASM_EMIT("vinsertps       $0x20, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 x i14 x       */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  r1  r2  x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  i1  i2  x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  r9  r10 x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  i9  i10 x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  r5  r6  x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  i5  i6  x     */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x10, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 r13 r14 x     */
                    __ASM_EMIT("vinsertps       $0x10, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 i13 i14 x     */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm0, %%xmm0")       /* xmm0 = r0  r1  r2  r3    */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm2, %%xmm2")       /* xmm2 = i0  i1  i2  i3    */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm1, %%xmm1")       /* xmm1 = r8  r9  r10 r11   */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm3, %%xmm3")       /* xmm3 = i8  i9  i10 i11   */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm4, %%xmm4")       /* xmm4 = r4  r5  r6  r7    */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm6, %%xmm6")       /* xmm6 = i4  i5  i6  i7    */
                    __ASM_EMIT("add             %[regs], %[index]")
                    __ASM_EMIT("vinsertps       $0x30, (%[src_re], %[index], 4), %%xmm5, %%xmm5")       /* xmm5 = r12 r13 r14 r15   */
                    __ASM_EMIT("vinsertps       $0x30, (%[src_im], %[index], 4), %%xmm7, %%xmm7")       /* xmm7 = i12 i13 i14 i15   */
                    __ASM_EMIT("add             %[regs], %[index]")

                    __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm0, %%ymm0")            /* ymm0 = r0 r1 r2 r3 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm3, %%ymm2, %%ymm2")            /* ymm2 = i0 i1 i2 i3 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm4, %%ymm4")            /* ymm4 = r4 r5 r6 r7 ...   */
                    __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm6, %%ymm6")            /* ymm0 = i4 i5 i6 i7 ...   */
                    /* 1st-order 4x butterfly */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0-r1 r2-r3 r4-r5 r6-r7 = r1' r3' r5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm6, %%ymm2, %%ymm3")                /* ymm3 = i0-i1 i2-i3 i4-i5 i6-i7 = i1' i3' i5' i7' */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0+r1 r2+r3 r4+r5 r6+r7 = r0' r2' r4' r6' */
                    __ASM_EMIT("vhaddps         %%ymm6, %%ymm2, %%ymm2")                /* ymm2 = i0+i1 i2+i3 i4+i5 i6+i7 = i0' i2' i4' i6' */
                    /* 2nd-order 4x butterfly */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm3, %%ymm1, %%ymm4")         /* ymm4 = r1' i3' r5' i7' */
                    __ASM_EMIT("vblendps        $0xaa, %%ymm1, %%ymm3, %%ymm5")         /* ymm5 = i1' r3' i5' r7' */
                    __ASM_EMIT("vhsubps         %%ymm4, %%ymm0, %%ymm1")                /* ymm1 = r0'-r2' r4'-r6' r1'-i3' r5'-i7' = r2" r6" r1" r5" */
                    __ASM_EMIT("vhsubps         %%ymm5, %%ymm2, %%ymm3")                /* ymm3 = i0'-i2' i4'-i6' i1'-r3' i5'-r7' = i2" i6" i3" i7" */
                    __ASM_EMIT("vhaddps         %%ymm4, %%ymm0, %%ymm0")                /* ymm0 = r0'+r2' r4'+r6' r1'+i3' r5'+i7' = r0" r4" r3" r7" */
                    __ASM_EMIT("vhaddps         %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = i0'+i2' i4'+i6' i1'+r3' i5'+r7' = i0" i4" i1" i5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm1, %%ymm0, %%ymm4")         /* ymm4 = r0" i4" r1" r5" */
                    __ASM_EMIT("vblendps        $0xcc, %%ymm0, %%ymm1, %%ymm5")         /* ymm5 = r2" r6" r3" r7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm3, %%ymm2, %%ymm6")         /* ymm6 = i0" i1" i2" i3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm3, %%ymm2, %%ymm7")         /* ymm7 = i4" i5" i6" i7" */
                    __ASM_EMIT("vshufps         $0x88, %%ymm5, %%ymm4, %%ymm2")         /* ymm2 = r0" r1" r2" r3" */
                    __ASM_EMIT("vshufps         $0xdd, %%ymm5, %%ymm4, %%ymm3")         /* ymm3 = r4" r5" r6" r7" */
                    /* 3rd-order 8x butterfly */
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm3, %%ymm4")       /* ymm4 = x_im * b_re */ \
                    __ASM_EMIT("vmulps          0x40 + %[FFT_A], %%ymm7, %%ymm5")       /* ymm5 = x_im * b_im */ \
                    __ASM_EMIT("vfmsub231ps     0x00 + %[FFT_A], %%ymm3, %%ymm5")       /* ymm5 = c_re = x_re * b_re - x_im * b_im */ \
                    __ASM_EMIT("vfmadd231ps     0x00 + %[FFT_A], %%ymm7, %%ymm4")       /* ymm4 = c_im = x_re * b_im + x_im * b_re */ \
                    __ASM_EMIT("vsubps          %%ymm5, %%ymm2, %%ymm0")                /* ymm0 = a_re - c_re */ \
                    __ASM_EMIT("vsubps          %%ymm4, %%ymm6, %%ymm1")                /* ymm1 = a_im - c_im */ \
                    __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")                /* ymm2 = a_re + c_re */ \
                    __ASM_EMIT("vaddps          %%ymm4, %%ymm6, %%ymm3")                /* ymm3 = a_im + c_im */ \
                    /* Store */
                    __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst_re])")
                    __ASM_EMIT("vmovups         %%xmm0, 0x10(%[dst_re])")
                    __ASM_EMIT("vextractf128    $1, %%ymm2, 0x20(%[dst_re])")
                    __ASM_EMIT("vextractf128    $1, %%ymm0, 0x30(%[dst_re])")
                    __ASM_EMIT("vmovups         %%xmm3, 0x00(%[dst_im])")
                    __ASM_EMIT("vmovups         %%xmm1, 0x10(%[dst_im])")
                    __ASM_EMIT("vextractf128    $1, %%ymm3, 0x20(%[dst_im])")
                    __ASM_EMIT("vextractf128    $1, %%ymm1, 0x30(%[dst_im])")
                    __ASM_EMIT("add             $0x40, %[dst_re]")
                    __ASM_EMIT("add             $0x40, %[dst_im]")

                    : [dst_re] "+r" (dst_re), [dst_im] "+r"(dst_im), [index] "+r"(index)
                    : [src_re] "r"(src_re), [src_im] "r"(src_im),
                      [regs] X86_GREG (regs),
                      [FFT_A] "o" (FFT_A)
                    : "cc", "memory",
                      "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                      "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                );
            }
        }
    } /* namespace avx512 */
} /* namespace lsp */

#undef FFT_SCRAMBLE_SELF_DIRECT_NAME
#undef FFT_SCRAMBLE_SELF_REVERSE_NAME
#undef FFT_SCRAMBLE_COPY_DIRECT_NAME
#undef FFT_SCRAMBLE_COPY_REVERSE_NAME
#undef FFT_TYPE
#undef FFT_FMA
