/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 июл. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_CONVOLUTION_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_CONVOLUTION_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        static const uint32_t convolve_const[] __lsp_aligned64 = {
            0x10, 0x11, 0x12, 0x13,
            0x00, 0x01, 0x02, 0x03,
            0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b
        };

        void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
        {
            IF_ARCH_X86(
                const float *c;
                float *d;
                size_t clen;
            );

            ARCH_X86_ASM(
                // 4x blocks
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("jb                  200f")
                __ASM_EMIT64("vmovaps           %[CC], %%zmm8")                 // zmm8 = permute table
                __ASM_EMIT("100:")
                    __ASM_EMIT("vxorps              %%zmm7, %%zmm7, %%zmm7")        // zmm7 = 0
                    __ASM_EMIT("mov                 %[length], %[clen]")
                    __ASM_EMIT("mov                 %[dst], %[d]")
                    __ASM_EMIT("mov                 %[conv], %[c]")

                    __ASM_EMIT("vbroadcastss        0x00(%[k]), %%zmm0")            // ymm0 = k0
                    __ASM_EMIT("vbroadcastss        0x04(%[k]), %%zmm1")            // ymm1 = k1
                    __ASM_EMIT("vbroadcastss        0x08(%[k]), %%zmm2")            // ymm2 = k2
                    __ASM_EMIT("vbroadcastss        0x0c(%[k]), %%zmm3")            // ymm3 = k3
                    // 16x convolution
                    __ASM_EMIT("sub                 $16, %[clen]")
                    __ASM_EMIT("jb                  12f")
                    __ASM_EMIT(".align              16")
                    __ASM_EMIT("11:")
                    __ASM_EMIT("vmovups             (%[c]), %%zmm4")                // zmm4 = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15
                    __ASM_EMIT32("vmovaps           %[CC], %%zmm6")                 // zmm6 = permute table
                    __ASM_EMIT("vmovaps             %%zmm4, %%zmm5")                // zmm5 = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15
                    __ASM_EMIT32("vpermt2ps         %%zmm7, %%zmm6, %%zmm4")        // zmm4 = p0 p1 p2 p3 c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11
                    __ASM_EMIT64("vpermt2ps         %%zmm7, %%zmm8, %%zmm4")        // zmm4 = p0 p1 p2 p3 c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11
                    __ASM_EMIT("vmovaps             %%zmm5, %%zmm7")                // zmm7 = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15
                    __ASM_EMIT("vshufps             $0x4e, %%zmm5, %%zmm4, %%zmm5") // zmm5 = p2 p3 c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13
                    __ASM_EMIT("vshufps             $0x99, %%zmm5, %%zmm4, %%zmm4") // zmm4 = p1 p2 p3 c0 c1 c2 c3 c4 p3 c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12
                    __ASM_EMIT("vshufps             $0x99, %%zmm7, %%zmm5, %%zmm6") // zmm6 = p3 c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14
                    __ASM_EMIT("vfmadd213ps         (%[d]), %%zmm3, %%zmm4")        // zmm4 = d0 + k3*p1 ...
                    __ASM_EMIT("vmulps              %%zmm2, %%zmm5, %%zmm5")        // zmm5 = k2*p2 ...
                    __ASM_EMIT("vfmadd231ps         %%zmm0, %%zmm7, %%zmm4")        // zmm4 = d0 + k0*c0 + k3*p1 ...
                    __ASM_EMIT("vfmadd213ps         %%zmm5, %%zmm1, %%zmm6")        // zmm6 = k1*p3 + k2*p2
                    __ASM_EMIT("vextractf32x4       $3, %%zmm7, %%xmm7")            // xmm7 = c12 c13 c14 c15 = p0 p1 p2 p3
                    __ASM_EMIT("vaddps              %%zmm6, %%zmm4, %%zmm4")        // ymm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovups             %%zmm4, (%[d])")
                    __ASM_EMIT("add                 $0x40, %[c]")                   // c += 16
                    __ASM_EMIT("add                 $0x40, %[d]")                   // d += 16
                    __ASM_EMIT("sub                 $16, %[clen]")                  // clen -= 16
                    __ASM_EMIT("jae                 11b")
                    __ASM_EMIT("12:")
                    // 8x convolution
                    __ASM_EMIT("add                 $8, %[clen]")
                    __ASM_EMIT("jl                  14f")
                    __ASM_EMIT("vmovups             (%[c]), %%ymm5")                // ymm5 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vinsertf128         $1, %%xmm5, %%ymm7, %%ymm4")    // ymm4 = p0 p1 p2 p3 c0 c1 c2 c3
                    __ASM_EMIT("vmovaps             %%ymm5, %%ymm7")                // ymm7 = c0 c1 c2 c3 c4 c5 c6 c7
                    __ASM_EMIT("vshufps             $0x4e, %%ymm5, %%ymm4, %%ymm5") // ymm5 = p2 p3 c0 c1 c2 c3 c4 c5
                    __ASM_EMIT("vshufps             $0x99, %%ymm5, %%ymm4, %%ymm4") // ymm4 = p1 p2 p3 c0 c1 c2 c3 c4
                    __ASM_EMIT("vshufps             $0x99, %%ymm7, %%ymm5, %%ymm6") // ymm6 = p3 c0 c1 c2 c3 c4 c5 c6
                    __ASM_EMIT("vfmadd213ps         (%[d]), %%ymm3, %%ymm4")        // ymm4 = d0 + k3*p1 ...
                    __ASM_EMIT("vmulps              %%ymm2, %%ymm5, %%ymm5")        // ymm5 = k2*p2 ...
                    __ASM_EMIT("vfmadd231ps         %%ymm0, %%ymm7, %%ymm4")        // ymm4 = d0 + k0*c0 + k3*p1 ...
                    __ASM_EMIT("vfmadd213ps         %%ymm5, %%ymm1, %%ymm6")        // ymm6 = k1*p3 + k2*p2
                    __ASM_EMIT("vextractf128        $1, %%ymm7, %%xmm7")            // xmm7 = c4 c5 c6 c7
                    __ASM_EMIT("vaddps              %%ymm6, %%ymm4, %%ymm4")        // ymm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovups             %%ymm4, (%[d])")
                    __ASM_EMIT("add                 $0x20, %[c]")                   // c += 8
                    __ASM_EMIT("add                 $0x20, %[d]")                   // d += 8
                    __ASM_EMIT("sub                 $8, %[clen]")                   // clen -= 8
                    __ASM_EMIT("14:")
                    // 4x convolution
                    __ASM_EMIT("add                 $4, %[clen]")
                    __ASM_EMIT("jl                  16f")
                    __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                    __ASM_EMIT("vmovups             (%[c]), %%xmm7")                // xmm7 = c0 c1 c2 c3
                    __ASM_EMIT("vshufps             $0x4e, %%xmm7, %%xmm4, %%xmm5") // xmm5 = p2 p3 c0 c1
                    __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 c0 c1 c2
                    __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 c0
                    __ASM_EMIT("vmulps              %%xmm2, %%xmm5, %%xmm5")        // xmm5 = k2*p2 ...
                    __ASM_EMIT("vfmadd213ps         (%[d]), %%xmm3, %%xmm4")        // xmm4 = d0 + k3*p1 ...
                    __ASM_EMIT("vfmadd213ps         %%xmm5, %%xmm1, %%xmm6")        // xmm6 = k1*p3 + k2*p2
                    __ASM_EMIT("vfmadd231ps         %%xmm0, %%xmm7, %%xmm4")        // xmm4 = d0 + k0*c0 + k3*p1 ...
                    __ASM_EMIT("vaddps              %%xmm6, %%xmm4, %%xmm4")        // xmm4 = d0 + k0*c0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovups             %%xmm4, (%[d])")
                    __ASM_EMIT("sub                 $4, %[clen]")                   // clen -= 4
                    __ASM_EMIT("add                 $0x10, %[c]")                   // c += 4
                    __ASM_EMIT("add                 $0x10, %[d]")                   // d += 4
                    __ASM_EMIT("16:")
                    // 4x tail
                    __ASM_EMIT("vmovaps             %%xmm7, %%xmm4")                // xmm4 = p0 p1 p2 p3
                    __ASM_EMIT("vxorps              %%xmm7, %%xmm7, %%xmm7")        // xmm7 = 0 0 0 0
                    __ASM_EMIT("vmovhlps            %%xmm4, %%xmm7, %%xmm5")        // xmm5 = p2 p3 0 0
                    __ASM_EMIT("vshufps             $0x99, %%xmm7, %%xmm5, %%xmm6") // xmm6 = p3 0 0 0
                    __ASM_EMIT("vmovlps             0x00(%[d]), %%xmm0, %%xmm0")    // xmm0 = d0 d1
                    __ASM_EMIT("vshufps             $0x99, %%xmm5, %%xmm4, %%xmm4") // xmm4 = p1 p2 p3 0
                    __ASM_EMIT("vmovss              0x08(%[d]), %%xmm7")            // xmm7 = d2 0
                    __ASM_EMIT("vmovlhps            %%xmm7, %%xmm0, %%xmm0")        // xmm0 = d0 d1 d2 0
                    __ASM_EMIT("vmulps              %%xmm3, %%xmm4, %%xmm4")        // xmm4 = k3*p1 ...
                    __ASM_EMIT("vfmadd213ps         %%xmm0, %%xmm2, %%xmm5")        // xmm5 = d0 + k2*p2 ...
                    __ASM_EMIT("vfmadd213ps         %%xmm4, %%xmm1, %%xmm6")        // xmm6 = k1*p3 + k3*p1 ...
                    __ASM_EMIT("vaddps              %%xmm5, %%xmm6, %%xmm6")        // xmm6 = d0 + k1*p3 + k2*p2 + k3*p1
                    __ASM_EMIT("vmovhlps            %%xmm6, %%xmm7, %%xmm7")
                    __ASM_EMIT("vmovlps             %%xmm6, 0x00(%[d])")
                    __ASM_EMIT("vmovss              %%xmm7, 0x08(%[d])")
                    // 1x convolution
                    __ASM_EMIT("add                 $3, %[clen]")                   // while (clen >= 0)
                    __ASM_EMIT("jl                  14f")
                    __ASM_EMIT("vmovups             0x00(%[k]), %%xmm1")            // xmm1 = k0 k1 k2 k3
                    __ASM_EMIT("15:")
                        __ASM_EMIT("vbroadcastss        0x00(%[c]), %%xmm0")            // xmm0 = c0 c0 c0 c0
                        __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%xmm1, %%xmm0")    // xmm0 = d0+k0*c0 d1+k1*c0 d2+k2*c0 d3+k3*c0
                        __ASM_EMIT("vmovups             %%xmm0, 0x00(%[d])")
                        __ASM_EMIT("add                 $0x04, %[c]")               // c++
                        __ASM_EMIT("add                 $0x04, %[d]")               // d++
                        __ASM_EMIT("dec                 %[clen]")                   // clen--
                        __ASM_EMIT("jge                 15b")
                    __ASM_EMIT("14:")
                __ASM_EMIT64("add               $0x10, %[dst]")         // dst += 4
                __ASM_EMIT32("addl              $0x10, %[dst]")
                __ASM_EMIT("add                 $0x10, %[k]")           // k += 4
                __ASM_EMIT("sub                 $0x04, %[count]")       // count -= 4
                __ASM_EMIT("jge                 100b")

                // 1x blocks
                __ASM_EMIT("200:")
                __ASM_EMIT("add                 $3, %[count]")
                __ASM_EMIT("jl                  400f")
                __ASM_EMIT("300:")
                    __ASM_EMIT("mov                 %[length], %[clen]")
                    __ASM_EMIT("mov                 %[dst], %[d]")
                    __ASM_EMIT("vbroadcastss        0x00(%[k]), %%zmm0")            // zmm0 = k0
                    __ASM_EMIT("mov                 %[conv], %[c]")
                    __ASM_EMIT("vmovaps             %%zmm0, %%zmm1")                // zmm1 = k0
                    // 64x convolution
                    __ASM_EMIT("sub                 $64, %[clen]")
                    __ASM_EMIT("jb                  22f")
                    __ASM_EMIT(".align              16")
                    __ASM_EMIT("21:")
                        __ASM_EMIT("vmovups             0x00(%[c]), %%zmm2")                // zmm2 = c0 c1 ...
                        __ASM_EMIT("vmovups             0x40(%[c]), %%zmm3")
                        __ASM_EMIT("vmovups             0x80(%[c]), %%zmm4")
                        __ASM_EMIT("vmovups             0xc0(%[c]), %%zmm5")
                        __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%zmm0, %%zmm2")        // zmm2 = d0+k0*c0 d1+k0*c1 ...
                        __ASM_EMIT("vfmadd213ps         0x40(%[d]), %%zmm1, %%zmm3")
                        __ASM_EMIT("vfmadd213ps         0x80(%[d]), %%zmm0, %%zmm4")
                        __ASM_EMIT("vfmadd213ps         0xc0(%[d]), %%zmm1, %%zmm5")
                        __ASM_EMIT("vmovups             %%zmm2, 0x00(%[d])")
                        __ASM_EMIT("vmovups             %%zmm3, 0x40(%[d])")
                        __ASM_EMIT("vmovups             %%zmm4, 0x80(%[d])")
                        __ASM_EMIT("vmovups             %%zmm5, 0xc0(%[d])")
                        __ASM_EMIT("add                 $0x100, %[c]")                      // c += 64
                        __ASM_EMIT("add                 $0x100, %[d]")                      // d += 64
                        __ASM_EMIT("sub                 $64, %[clen]")                      // clen -= 64
                        __ASM_EMIT("jae                 21b")
                    __ASM_EMIT("22:")
                    // 32x convolution
                    __ASM_EMIT("add                 $32, %[clen]")
                    __ASM_EMIT("jl                  24f")
                    __ASM_EMIT("vmovups             0x00(%[c]), %%ymm2")                // ymm2 = c0 c1 ...
                    __ASM_EMIT("vmovups             0x20(%[c]), %%ymm3")
                    __ASM_EMIT("vmovups             0x40(%[c]), %%ymm4")
                    __ASM_EMIT("vmovups             0x60(%[c]), %%ymm5")
                    __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%ymm0, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vfmadd213ps         0x20(%[d]), %%ymm1, %%ymm3")
                    __ASM_EMIT("vfmadd213ps         0x40(%[d]), %%ymm0, %%ymm4")
                    __ASM_EMIT("vfmadd213ps         0x60(%[d]), %%ymm1, %%ymm5")
                    __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                    __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                    __ASM_EMIT("vmovups             %%ymm4, 0x40(%[d])")
                    __ASM_EMIT("vmovups             %%ymm5, 0x60(%[d])")
                    __ASM_EMIT("sub                 $32, %[clen]")                      // clen -= 32
                    __ASM_EMIT("add                 $0x80, %[c]")                       // c += 32
                    __ASM_EMIT("add                 $0x80, %[d]")                       // d += 32
                    __ASM_EMIT("24:")
                    // 16x convolution
                    __ASM_EMIT("add                 $16, %[clen]")
                    __ASM_EMIT("jl                  26f")
                    __ASM_EMIT("vmovups             0x00(%[c]), %%ymm2")                // ymm2 = c0 c1 ...
                    __ASM_EMIT("vmovups             0x20(%[c]), %%ymm3")
                    __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%ymm0, %%ymm2")        // ymm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vfmadd213ps         0x20(%[d]), %%ymm1, %%ymm3")
                    __ASM_EMIT("vmovups             %%ymm2, 0x00(%[d])")
                    __ASM_EMIT("vmovups             %%ymm3, 0x20(%[d])")
                    __ASM_EMIT("sub                 $16, %[clen]")                      // clen -= 16
                    __ASM_EMIT("add                 $0x40, %[c]")                       // c += 16
                    __ASM_EMIT("add                 $0x40, %[d]")                       // d += 16
                    __ASM_EMIT("26:")
                    // 8x convolution
                    __ASM_EMIT("add                 $8, %[clen]")
                    __ASM_EMIT("jl                  28f")
                    __ASM_EMIT("vmovups             0x00(%[c]), %%xmm2")                // xmm2 = c0 c1 ...
                    __ASM_EMIT("vmovups             0x10(%[c]), %%xmm3")
                    __ASM_EMIT("vfmadd213ps         0x00(%[d]), %%xmm0, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vfmadd213ps         0x10(%[d]), %%xmm1, %%xmm3")
                    __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("vmovups             %%xmm3, 0x10(%[d])")
                    __ASM_EMIT("sub                 $8, %[clen]")                       // clen -= 8
                    __ASM_EMIT("add                 $0x20, %[c]")                       // c += 8
                    __ASM_EMIT("add                 $0x20, %[d]")                       // d += 8
                    __ASM_EMIT("28:")
                    // 4x convolution
                    __ASM_EMIT("add                 $4, %[clen]")
                    __ASM_EMIT("jl                  30f")
                    __ASM_EMIT("vmulps              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0 k0*c1 ...
                    __ASM_EMIT("vaddps              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0 d1+k0*c1 ...
                    __ASM_EMIT("vmovups             %%xmm2, 0x00(%[d])")
                    __ASM_EMIT("sub                 $4, %[clen]")                       // clen -= 4
                    __ASM_EMIT("add                 $0x10, %[c]")                       // c += 4
                    __ASM_EMIT("add                 $0x10, %[d]")                       // d += 4
                    __ASM_EMIT("30:")
                    // 1x convolution
                    __ASM_EMIT("add                 $3, %[clen]")
                    __ASM_EMIT("jl                  32f")
                    __ASM_EMIT("31:")
                        __ASM_EMIT("vmulss              0x00(%[c]), %%xmm0, %%xmm2")        // xmm2 = k0*c0
                        __ASM_EMIT("vaddss              0x00(%[d]), %%xmm2, %%xmm2")        // xmm2 = d0+k0*c0
                        __ASM_EMIT("vmovss              %%xmm2, 0x00(%[d])")
                        __ASM_EMIT("add                 $0x04, %[c]")                       // c ++
                        __ASM_EMIT("add                 $0x04, %[d]")                       // d ++
                        __ASM_EMIT("dec                 %[clen]")
                        __ASM_EMIT("jge                 31b")
                    __ASM_EMIT("32:")
                __ASM_EMIT("add         $0x04, %[k]")
                __ASM_EMIT64("add       $0x04, %[dst]")             // dst++
                __ASM_EMIT32("addl      $0x04, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         300b")

                __ASM_EMIT("400:")
                : [dst] __IF_32("+m") __IF_64("+r") (dst),
                  [k] "+r" (src), [count] "+r" (count),
                  [c] "=&r" (c), [d] "=&r" (d), [clen] "=&r" (clen)
                : [conv] "m" (conv), [length] "m" (length),
                  [CC] "m" (convolve_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7" __IF_64(, "%xmm8")
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_CONVOLUTION_H_ */
