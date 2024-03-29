/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_PMATH_EXP_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_PMATH_EXP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

namespace lsp
{
    namespace avx2
    {
        IF_ARCH_X86(
            static const uint32_t EXP2_CONST[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x7fffffff), // sign
                LSP_DSP_VEC8(0x7f),       // 0x7f
                LSP_DSP_VEC8(0x3f317218), // ln(2)
                LSP_DSP_VEC8(0x40e00000), // C5 = 7!/6! = 7.0000000000000000
                LSP_DSP_VEC8(0x42280000), // C4 = 7!/5! = 42.0000000000000000
                LSP_DSP_VEC8(0x43520000), // C3 = 7!/4! = 210.0000000000000000
                LSP_DSP_VEC8(0x44520000), // C2 = 7!/3! = 840.0000000000000000
                LSP_DSP_VEC8(0x451d8000), // C1 = 7!/2! = 2520.0000000000000000
                LSP_DSP_VEC8(0x459d8000), // C0 = 7!/1! = 5040.0000000000000000
                LSP_DSP_VEC8(0x39500d01), // 1/7! = 0.0001984127011383
                LSP_DSP_VEC8(0x3f800000), // 1.0
            };

            static const float EXP_LOG2E[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(M_LOG2E)
            };
        )

    #define POW2_CORE_X32 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm4, %%ymm6")        \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm8, %%ymm10")       \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm12, %%ymm14")      \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm6, %%ymm5")                        \
        __ASM_EMIT("vcvttps2dq      %%ymm10, %%ymm9")                       \
        __ASM_EMIT("vcvttps2dq      %%ymm14, %%ymm13")                      \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm5, %%ymm7")                        \
        __ASM_EMIT("vcvtdq2ps       %%ymm9, %%ymm11")                       \
        __ASM_EMIT("vcvtdq2ps       %%ymm13, %%ymm15")                      \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm5, %%ymm5")        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm9, %%ymm9")        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm13, %%ymm13")      \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vsubps          %%ymm11, %%ymm10, %%ymm10")             \
        __ASM_EMIT("vsubps          %%ymm15, %%ymm14, %%ymm14")             \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%ymm5, %%ymm5")                   \
        __ASM_EMIT("vpslld          $23, %%ymm9, %%ymm9")                   \
        __ASM_EMIT("vpslld          $23, %%ymm13, %%ymm13")                 \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm6, %%ymm6")        \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm10, %%ymm10")      \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm14, %%ymm14")      \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C5+X) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm11, %%ymm11")      \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm15, %%ymm15")      \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm6")                \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm10")               \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm14")               \
        __ASM_EMIT("vaddps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vaddps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vaddps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm11")              \
        __ASM_EMIT("vmulps          %%ymm13, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        __ASM_EMIT("vdivps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vdivps          %%ymm11, %%ymm10, %%ymm10")             \
        __ASM_EMIT("vdivps          %%ymm15, %%ymm14, %%ymm14")             \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */ \
        __ASM_EMIT("vblendvps       %%ymm4, %%ymm6, %%ymm7, %%ymm4")        \
        __ASM_EMIT("vblendvps       %%ymm8, %%ymm10, %%ymm11, %%ymm8")      \
        __ASM_EMIT("vblendvps       %%ymm12, %%ymm14, %%ymm15, %%ymm12")

    #define POW2_CORE_X16 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm4, %%ymm6")        \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm6, %%ymm5")                        \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm5, %%ymm7")                        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm5, %%ymm5")        \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%ymm5, %%ymm5")                   \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm6, %%ymm6")        \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C5+X) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm7, %%ymm7")        \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm6")                \
        __ASM_EMIT("vaddps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vaddps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        __ASM_EMIT("vdivps          %%ymm7, %%ymm6, %%ymm6")                \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */ \
        __ASM_EMIT("vblendvps       %%ymm4, %%ymm6, %%ymm7, %%ymm4")


    #define POW2_CORE_X8 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C5+X) */ \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%ymm3, %%ymm3")        /* ymm3 = 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vaddps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define POW2_CORE_X4 \
        /* xmm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%xmm0, %%xmm2")        /* xmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%xmm2, %%xmm1")                        /* xmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%xmm1, %%xmm3")                        /* xmm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%xmm1, %%xmm1")        /* xmm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%xmm1, %%xmm1")                   /* xmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%xmm2, %%xmm2")        /* xmm2 = X = ln(2) * (XP - float(R)) */ \
        /* xmm0 = [x<0], xmm1 = 1 << R, xmm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C5+X */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C5+X) */ \
        __ASM_EMIT("vaddps          0x080 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vaddps          0x0a0 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vaddps          0x0e0 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vaddps          0x100 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          0x120 + %[E2C], %%xmm3, %%xmm3")        /* xmm3 = 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%xmm2")                /* xmm2 = 1 */ \
        __ASM_EMIT("vaddps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%xmm1, %%xmm3, %%xmm3")                /* xmm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = 1/p */ \
        /* xmm0 = x, xmm2 = 1/p, xmm3 = p */ \
        __ASM_EMIT("vblendvps       %%xmm0, %%xmm2, %%xmm3, %%xmm0")        /* xmm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define EXP_CORE_X32 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm8, %%ymm8") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm12, %%ymm12") \
        POW2_CORE_X32

    #define EXP_CORE_X16 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm4, %%ymm4") \
        POW2_CORE_X16

    #define EXP_CORE_X8 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        POW2_CORE_X8

    #define EXP_CORE_X4 \
        __ASM_EMIT("vmulps          %[LOG2E], %%xmm0, %%xmm0") \
        POW2_CORE_X4

        void x64_exp1(float *dst, size_t count)
        {
            IF_ARCH_X86_64(float *src);

            ARCH_X86_64_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void exp1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_exp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_64_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void exp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


    #define POW2_CORE_X32_FMA3 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm4, %%ymm6")        \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm8, %%ymm10")       \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm12, %%ymm14")      \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm6, %%ymm5")                        \
        __ASM_EMIT("vcvttps2dq      %%ymm10, %%ymm9")                       \
        __ASM_EMIT("vcvttps2dq      %%ymm14, %%ymm13")                      \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm5, %%ymm7")                        \
        __ASM_EMIT("vcvtdq2ps       %%ymm9, %%ymm11")                       \
        __ASM_EMIT("vcvtdq2ps       %%ymm13, %%ymm15")                      \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm5, %%ymm5")        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm9, %%ymm9")        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm13, %%ymm13")      \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vsubps          %%ymm11, %%ymm10, %%ymm10")             \
        __ASM_EMIT("vsubps          %%ymm15, %%ymm14, %%ymm14")             \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%ymm5, %%ymm5")                   \
        __ASM_EMIT("vpslld          $23, %%ymm9, %%ymm9")                   \
        __ASM_EMIT("vpslld          $23, %%ymm13, %%ymm13")                 \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm6, %%ymm6")        \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm10, %%ymm10")      \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm14, %%ymm14")      \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm10, %%ymm11, %%ymm11")             \
        __ASM_EMIT("vmulps          %%ymm14, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm6")                \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm10")               \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm14")               \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm10, %%ymm11")      \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm14, %%ymm15")      \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm11")              \
        __ASM_EMIT("vmulps          %%ymm13, %%ymm15, %%ymm15")             \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        __ASM_EMIT("vdivps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vdivps          %%ymm11, %%ymm10, %%ymm10")             \
        __ASM_EMIT("vdivps          %%ymm15, %%ymm14, %%ymm14")             \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */ \
        __ASM_EMIT("vblendvps       %%ymm4, %%ymm6, %%ymm7, %%ymm4")        \
        __ASM_EMIT("vblendvps       %%ymm8, %%ymm10, %%ymm11, %%ymm8")      \
        __ASM_EMIT("vblendvps       %%ymm12, %%ymm14, %%ymm15, %%ymm12")


    #define POW2_CORE_X16_FMA3 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm4, %%ymm6")        \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm6, %%ymm5")                        \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm5, %%ymm7")                        \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm5, %%ymm5")        \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%ymm7, %%ymm6, %%ymm6")                \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%ymm5, %%ymm5")                   \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm6, %%ymm6")        \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm6, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm6")                \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm6, %%ymm7")        \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%ymm5, %%ymm7, %%ymm7")                \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        __ASM_EMIT("vdivps          %%ymm7, %%ymm6, %%ymm6")                \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */ \
        __ASM_EMIT("vblendvps       %%ymm4, %%ymm6, %%ymm7, %%ymm4")

    #define POW2_CORE_X8_FMA3 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define POW2_CORE_X4_FMA3 \
        /* xmm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%xmm0, %%xmm2")        /* xmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%xmm2, %%xmm1")                        /* xmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%xmm1, %%xmm3")                        /* xmm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x020 + %[E2C], %%xmm1, %%xmm1")        /* xmm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%xmm1, %%xmm1")                   /* xmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x040 + %[E2C], %%xmm2, %%xmm2")        /* xmm2 = X = ln(2) * (XP - float(R)) */ \
        /* xmm0 = [x<0], xmm1 = 1 << R, xmm2 = X */ \
        __ASM_EMIT("vaddps          0x060 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C5+X */ \
        __ASM_EMIT("vfmadd213ps     0x080 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x0a0 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x0c0 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x0e0 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x140 + %[E2C], %%xmm2")                /* xmm2 = 1 */ \
        __ASM_EMIT("vfmadd132ps     0x120 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%xmm1, %%xmm3, %%xmm3")                /* xmm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = 1/p */ \
        /* xmm0 = x, xmm2 = 1/p, xmm3 = p */ \
        __ASM_EMIT("vblendvps       %%xmm0, %%xmm2, %%xmm3, %%xmm0")        /* xmm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define EXP_CORE_X32_FMA3 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm4, %%ymm4") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm8, %%ymm8") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm12, %%ymm12") \
        POW2_CORE_X32_FMA3

    #define EXP_CORE_X16_FMA3 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm4, %%ymm4") \
        POW2_CORE_X16_FMA3

    #define EXP_CORE_X8_FMA3 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        POW2_CORE_X8_FMA3

    #define EXP_CORE_X4_FMA3 \
        __ASM_EMIT("vmulps          %[LOG2E], %%xmm0, %%xmm0") \
        POW2_CORE_X4_FMA3


        void x64_exp1_fma3(float *dst, size_t count)
        {
            IF_ARCH_X86_64(float *src);

            ARCH_X86_64_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[dst]), %%ymm12")
                EXP_CORE_X32_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                EXP_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                EXP_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void exp1_fma3(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[dst]), %%ymm4")
                EXP_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                EXP_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_exp2_fma3(float *dst, const float *src, size_t count)
        {
            ARCH_X86_64_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                __ASM_EMIT("vmovups         0x40(%[src]), %%ymm8")
                __ASM_EMIT("vmovups         0x60(%[src]), %%ymm12")
                EXP_CORE_X32_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("vmovups         %%ymm8, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%ymm12, 0x60(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                EXP_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                EXP_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("8:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             16f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("12:")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("14:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("16:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void exp2_fma3(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                __ASM_EMIT("vmovups         0x20(%[src]), %%ymm4")
                EXP_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                EXP_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                EXP_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx2 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX2_PMATH_EXP_H_ */
