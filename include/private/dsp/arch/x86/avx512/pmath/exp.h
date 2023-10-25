/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 25 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_EXP_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_EXP_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t EXP2_CONST[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff),  // sign
                LSP_DSP_VEC16(0x7f),        // 0x7f
                LSP_DSP_VEC16(0x3f317218),  // ln(2)
                LSP_DSP_VEC16(0x40e00000),  // C5 = 7!/6! = 7.0000000000000000
                LSP_DSP_VEC16(0x42280000),  // C4 = 7!/5! = 42.0000000000000000
                LSP_DSP_VEC16(0x43520000),  // C3 = 7!/4! = 210.0000000000000000
                LSP_DSP_VEC16(0x44520000),  // C2 = 7!/3! = 840.0000000000000000
                LSP_DSP_VEC16(0x451d8000),  // C1 = 7!/2! = 2520.0000000000000000
                LSP_DSP_VEC16(0x459d8000),  // C0 = 7!/1! = 5040.0000000000000000
                LSP_DSP_VEC16(0x39500d01),  // 1/7! = 0.0001984127011383
                LSP_DSP_VEC16(0x3f800000),  // 1.0
            };

            static const float EXP_LOG2E[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(M_LOG2E)
            };
        )

    #define POW2_CORE_X64 \
        /* zmm0 = x */ \
        __ASM_EMIT("vxorps          %%zmm3, %%zmm3, %%zmm3")                /* zmm3 = 0 */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm0, %%zmm2")        /* zmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm4, %%zmm6") \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm8, %%zmm10") \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm12, %%zmm14") \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm0, %%k4")              /* k4   = x < 0 */ \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm4, %%k5") \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm8, %%k6") \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm12, %%k7") \
        __ASM_EMIT("vcvttps2dq      %%zmm2, %%zmm1")                        /* zmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%zmm6, %%zmm5") \
        __ASM_EMIT("vcvttps2dq      %%zmm10, %%zmm9") \
        __ASM_EMIT("vcvttps2dq      %%zmm14, %%zmm13") \
        __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm3")                        /* zmm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%zmm5, %%zmm7") \
        __ASM_EMIT("vcvtdq2ps       %%zmm9, %%zmm11") \
        __ASM_EMIT("vcvtdq2ps       %%zmm13, %%zmm15") \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm1, %%zmm1")        /* zmm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm5, %%zmm5") \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm9, %%zmm9") \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm13, %%zmm13") \
        __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm2")                /* zmm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%zmm7, %%zmm6, %%zmm6") \
        __ASM_EMIT("vsubps          %%zmm11, %%zmm10, %%zmm10") \
        __ASM_EMIT("vsubps          %%zmm15, %%zmm14, %%zmm14") \
        __ASM_EMIT("vpslld          $23, %%zmm1, %%zmm1")                   /* zmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%zmm5, %%zmm5") \
        __ASM_EMIT("vpslld          $23, %%zmm9, %%zmm9") \
        __ASM_EMIT("vpslld          $23, %%zmm13, %%zmm13") \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm2, %%zmm2")        /* zmm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm6, %%zmm6") \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm10, %%zmm10") \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm14, %%zmm14") \
        /* zmm0 = [x<0], zmm1 = 1 << R, zmm2 = X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vmulps          %%zmm2, %%zmm3, %%zmm3")                /* zmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%zmm6, %%zmm7, %%zmm7") \
        __ASM_EMIT("vmulps          %%zmm10, %%zmm11, %%zmm11") \
        __ASM_EMIT("vmulps          %%zmm14, %%zmm15, %%zmm15") \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm2")                /* zmm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm6") \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm10") \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm14") \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm10, %%zmm11") \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm14, %%zmm15") \
        __ASM_EMIT("vmulps          %%zmm1, %%zmm3, %%zmm0")                /* zmm0 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%zmm5, %%zmm7, %%zmm4") \
        __ASM_EMIT("vmulps          %%zmm9, %%zmm11, %%zmm8") \
        __ASM_EMIT("vmulps          %%zmm13, %%zmm15, %%zmm12") \
        __ASM_EMIT("vdivps          %%zmm0, %%zmm2, %%zmm0 %{%%k4%}")       /* zmm0 = [x<0] ? 1/p : p */ \
        __ASM_EMIT("vdivps          %%zmm4, %%zmm6, %%zmm4 %{%%k5%}") \
        __ASM_EMIT("vdivps          %%zmm8, %%zmm10, %%zmm8 %{%%k6%}") \
        __ASM_EMIT("vdivps          %%zmm12, %%zmm14, %%zmm12 %{%%k7%}")

    #define POW2_CORE_X32 \
        /* zmm0 = x */ \
        __ASM_EMIT("vxorps          %%zmm3, %%zmm3, %%zmm3")                /* zmm3 = 0 */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm0, %%zmm2")        /* zmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm4, %%zmm6") \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm0, %%k4")              /* k4   = x < 0 */ \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm4, %%k5") \
        __ASM_EMIT("vcvttps2dq      %%zmm2, %%zmm1")                        /* zmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvttps2dq      %%zmm6, %%zmm5") \
        __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm3")                        /* zmm3 = float(R) */ \
        __ASM_EMIT("vcvtdq2ps       %%zmm5, %%zmm7") \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm1, %%zmm1")        /* zmm1 = R + 127 */ \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm5, %%zmm5") \
        __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm2")                /* zmm2 = XP - float(R) */ \
        __ASM_EMIT("vsubps          %%zmm7, %%zmm6, %%zmm6") \
        __ASM_EMIT("vpslld          $23, %%zmm1, %%zmm1")                   /* zmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vpslld          $23, %%zmm5, %%zmm5") \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm2, %%zmm2")        /* zmm2 = X = ln(2) * (XP - float(R)) */ \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm6, %%zmm6") \
        /* zmm0 = [x<0], zmm1 = 1 << R, zmm2 = X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C5+X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vmulps          %%zmm2, %%zmm3, %%zmm3")                /* zmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%zmm6, %%zmm7, %%zmm7") \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm2")                /* zmm2 = 1 */ \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm6") \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm6, %%zmm7") \
        __ASM_EMIT("vmulps          %%zmm1, %%zmm3, %%zmm0")                /* zmm0 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vmulps          %%zmm5, %%zmm7, %%zmm4") \
        __ASM_EMIT("vdivps          %%zmm0, %%zmm2, %%zmm0 %{%%k4%}")       /* zmm0 = [x<0] ? 1/p : p */ \
        __ASM_EMIT("vdivps          %%zmm4, %%zmm6, %%zmm4 %{%%k5%}")

    #define POW2_CORE_X16 \
        /* zmm0 = x */ \
        __ASM_EMIT("vxorps          %%zmm3, %%zmm3, %%zmm3")                /* zmm3 = 0 */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%zmm0, %%zmm2")        /* zmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcmpps          $1, %%zmm3, %%zmm0, %%k4")              /* k4   = x < 0 */ \
        __ASM_EMIT("vcvttps2dq      %%zmm2, %%zmm1")                        /* zmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm3")                        /* zmm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%zmm1, %%zmm1")        /* zmm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%zmm3, %%zmm2, %%zmm2")                /* zmm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%zmm1, %%zmm1")                   /* zmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%zmm2, %%zmm2")        /* zmm2 = X = ln(2) * (XP - float(R)) */ \
        /* zmm0 = [x<0], zmm1 = 1 << R, zmm2 = X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C5+X */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%zmm2, %%zmm3, %%zmm3")                /* zmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%zmm2")                /* zmm2 = 1 */ \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%zmm2, %%zmm3")        /* zmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%zmm1, %%zmm3, %%zmm0")                /* zmm0 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%zmm0, %%zmm2, %%zmm0 %{%%k4%}")       /* zmm0 = [x<0] ? 1/p : p */

    #define POW2_CORE_X8 \
        /* ymm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%ymm0, %%ymm2")        /* ymm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%ymm2, %%ymm1")                        /* ymm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm3")                        /* ymm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%ymm1, %%ymm1")        /* ymm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%ymm1, %%ymm1")                   /* ymm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%ymm2, %%ymm2")        /* ymm2 = X = ln(2) * (XP - float(R)) */ \
        /* ymm0 = [x<0], ymm1 = 1 << R, ymm2 = X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C5+X */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%ymm2, %%ymm3, %%ymm3")                /* ymm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%ymm2")                /* ymm2 = 1 */ \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%ymm2, %%ymm3")        /* ymm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm3")                /* ymm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = 1/p */ \
        /* ymm0 = x, ymm2 = 1/p, ymm3 = p */ \
        __ASM_EMIT("vblendvps       %%ymm0, %%ymm2, %%ymm3, %%ymm0")        /* ymm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define POW2_CORE_X4 \
        /* xmm0 = x */ \
        __ASM_EMIT("vandps          0x000 + %[E2C], %%xmm0, %%xmm2")        /* xmm2 = XP = fabs(x) */ \
        __ASM_EMIT("vcvttps2dq      %%xmm2, %%xmm1")                        /* xmm1 = R = int(XP) */ \
        __ASM_EMIT("vcvtdq2ps       %%xmm1, %%xmm3")                        /* xmm3 = float(R) */ \
        __ASM_EMIT("vpaddd          0x040 + %[E2C], %%xmm1, %%xmm1")        /* xmm1 = R + 127 */ \
        __ASM_EMIT("vsubps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = XP - float(R) */ \
        __ASM_EMIT("vpslld          $23, %%xmm1, %%xmm1")                   /* xmm1 = 1 << (R+127) */ \
        __ASM_EMIT("vmulps          0x080 + %[E2C], %%xmm2, %%xmm2")        /* xmm2 = X = ln(2) * (XP - float(R)) */ \
        /* xmm0 = [x<0], xmm1 = 1 << R, xmm2 = X */ \
        __ASM_EMIT("vaddps          0x0c0 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C5+X */ \
        __ASM_EMIT("vfmadd213ps     0x100 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C4+X*(C5+X) */ \
        __ASM_EMIT("vfmadd213ps     0x140 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C3+X*(C4+X*(C5+X)) */ \
        __ASM_EMIT("vfmadd213ps     0x180 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C2+X*(C3+X*(C4+X*(C5+X))) */ \
        __ASM_EMIT("vfmadd213ps     0x1c0 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))) */ \
        __ASM_EMIT("vfmadd213ps     0x200 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))) */ \
        __ASM_EMIT("vmulps          %%xmm2, %%xmm3, %%xmm3")                /* xmm3 = X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmovaps         0x280 + %[E2C], %%xmm2")                /* xmm2 = 1 */ \
        __ASM_EMIT("vfmadd132ps     0x240 + %[E2C], %%xmm2, %%xmm3")        /* xmm3 = 1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X)))))) */ \
        __ASM_EMIT("vmulps          %%xmm1, %%xmm3, %%xmm3")                /* xmm3 = p = (1<<R)*(1 + 1/7!*X*(C0+X*(C1+X*(C2+X*(C3+X*(C4+X*(C5+X))))))) */ \
        __ASM_EMIT("vdivps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2 = 1/p */ \
        /* xmm0 = x, xmm2 = 1/p, xmm3 = p */ \
        __ASM_EMIT("vblendvps       %%xmm0, %%xmm2, %%xmm3, %%xmm0")        /* xmm0 = ([x<0]&(1/p)) | ([x>=0]&p) */

    #define EXP_CORE_X64 \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm0, %%zmm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm4, %%zmm4") \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm8, %%zmm8") \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm12, %%zmm12") \
        POW2_CORE_X64

    #define EXP_CORE_X32 \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm0, %%zmm0") \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm4, %%zmm4") \
        POW2_CORE_X32

    #define EXP_CORE_X16 \
        __ASM_EMIT("vmulps          %[LOG2E], %%zmm0, %%zmm0") \
        POW2_CORE_X16

    #define EXP_CORE_X8 \
        __ASM_EMIT("vmulps          %[LOG2E], %%ymm0, %%ymm0") \
        POW2_CORE_X8

    #define EXP_CORE_X4 \
        __ASM_EMIT("vmulps          %[LOG2E], %%xmm0, %%xmm0") \
        POW2_CORE_X4

        void exp1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
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
                  "%k4", "%k5"
            );
        }

    IF_ARCH_X86_64(
        void x64_exp1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);
            ARCH_X86_64_ASM(
                // x64 blocks
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                __ASM_EMIT("vmovups         0x80(%[dst]), %%zmm8")
                __ASM_EMIT("vmovups         0xc0(%[dst]), %%zmm12")
                EXP_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 blocks
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("10:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             18f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("14:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("16:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              18f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("18:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }
    )

        void exp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
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
                  "%k4", "%k5"
            );
        }

    IF_ARCH_X86_64(
        void x64_exp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_64_ASM(
                // x64 blocks
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                __ASM_EMIT("vmovups         0x80(%[src]), %%zmm8")
                __ASM_EMIT("vmovups         0xc0(%[src]), %%zmm12")
                EXP_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[src]")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 blocks
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                EXP_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                EXP_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                EXP_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                EXP_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("10:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             18f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("14:")
                EXP_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              16f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("16:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              18f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("18:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [E2C] "o" (EXP2_CONST),
                  [LOG2E] "m" (EXP_LOG2E)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15",
                  "%k4", "%k5", "%k6", "%k7"
            );
        }
    )

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_EXP_H_ */
