/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LOG_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LOG_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t LOG2_CONST[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x007fffff), // frac
                LSP_DSP_VEC16(0x0000007f), // 127
                LSP_DSP_VEC16(0x3f800000), // 1.0f
                LSP_DSP_VEC16(0x3dba2e8c), // C0 = 1/11 = 0.0909090936183929
                LSP_DSP_VEC16(0x3de38e39), // C1 = 1/9 = 0.1111111119389534
                LSP_DSP_VEC16(0x3e124925), // C2 = 1/7 = 0.1428571492433548
                LSP_DSP_VEC16(0x3e4ccccd), // C3 = 1/5 = 0.2000000029802322
                LSP_DSP_VEC16(0x3eaaaaab), // C4 = 1/3 = 0.3333333432674408
                LSP_DSP_VEC16(0x3f800000)  // C5 = 1/1 = 1.0000000000000000
            };

            static const float LOGB_C[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(2.0f * float(M_LOG2E))
            };

            static const float LOGE_C[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(1.0f / float(M_LOG2E))
            };

            static const float LOGD_C[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(2.0f * float(M_LOG10E)),
                LSP_DSP_VEC16(0.301029995663981f) // 1/log2(10)
            };
        )


        #define LOGN_CORE_X64 \
            __ASM_EMIT("vpsrld          $23, %%zmm0, %%zmm1")               /* zmm1 = ilog2(x) + 127 */ \
            __ASM_EMIT("vpsrld          $23, %%zmm4, %%zmm5")               \
            __ASM_EMIT("vpsrld          $23, %%zmm8, %%zmm9")               \
            __ASM_EMIT("vpsrld          $23, %%zmm12, %%zmm13")             \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = x & F_MASK */ \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm8, %%zmm8")    \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm12, %%zmm12")  \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm1, %%zmm1")    /* zmm1 = r = ilog2(x) */ \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm5, %%zmm5")    \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm9, %%zmm9")    \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm13, %%zmm13")  \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X = (x & F_MASK) | 1.0f */ \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm8, %%zmm8")    \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm12, %%zmm12")  \
            __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm1")                    /* zmm1 = R = float(r) */ \
            __ASM_EMIT("vcvtdq2ps       %%zmm5, %%zmm5")                    \
            __ASM_EMIT("vcvtdq2ps       %%zmm9, %%zmm9")                    \
            __ASM_EMIT("vcvtdq2ps       %%zmm13, %%zmm13")                  \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm0, %%zmm2")    /* zmm2 = X + 1 */ \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm4, %%zmm6")    \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm8, %%zmm10")   \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm12, %%zmm14")  \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X - 1 */ \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm8, %%zmm8")    \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm12, %%zmm12")  \
            __ASM_EMIT("vdivps          %%zmm2, %%zmm0, %%zmm0")            /* zmm0 = y = (X-1)/(X+1) */ \
            __ASM_EMIT("vdivps          %%zmm6, %%zmm4, %%zmm4")            \
            __ASM_EMIT("vdivps          %%zmm10, %%zmm8, %%zmm8")           \
            __ASM_EMIT("vdivps          %%zmm14, %%zmm12, %%zmm12")         \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm3")            /* zmm3 = C0 */ \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm7")            \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm11")           \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm15")           \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm2")            /* zmm2 = Y = y*y */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm4, %%zmm6")            \
            __ASM_EMIT("vmulps          %%zmm8, %%zmm8, %%zmm10")           \
            __ASM_EMIT("vmulps          %%zmm12, %%zmm12, %%zmm14")         \
            /* zmm0 = y, zmm1 = R, zmm2 = Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C1+C0*Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm10, %%zmm11")  \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm14, %%zmm15")  \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C2+Y*(C1+C0*Y) */ \
            __ASM_EMIT("vfmadd213ps     0x140 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x140 + %[L2C], %%zmm10, %%zmm11")  \
            __ASM_EMIT("vfmadd213ps     0x140 + %[L2C], %%zmm14, %%zmm15")  \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm10, %%zmm11")  \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm14, %%zmm15")  \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm10, %%zmm11")  \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm14, %%zmm15")  \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm10, %%zmm11")  \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm14, %%zmm15")  \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm3, %%zmm0")            /* zmm0 = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm7, %%zmm4")            \
            __ASM_EMIT("vmulps          %%zmm8, %%zmm11, %%zmm8")           \
            __ASM_EMIT("vmulps          %%zmm12, %%zmm15, %%zmm12")         \

        #define LOGN_CORE_X32 \
            __ASM_EMIT("vpsrld          $23, %%zmm0, %%zmm1")               /* zmm1 = ilog2(x) + 127 */ \
            __ASM_EMIT("vpsrld          $23, %%zmm4, %%zmm5")               \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = x & F_MASK */ \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm1, %%zmm1")    /* zmm1 = r = ilog2(x) */ \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm5, %%zmm5")    \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X = (x & F_MASK) | 1.0f */ \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm1")                    /* zmm1 = R = float(r) */ \
            __ASM_EMIT("vcvtdq2ps       %%zmm5, %%zmm5")                    \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm0, %%zmm2")    /* zmm2 = X + 1 */ \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm4, %%zmm6")    \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X - 1 */ \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm4, %%zmm4")    \
            __ASM_EMIT("vdivps          %%zmm2, %%zmm0, %%zmm0")            /* zmm0 = y = (X-1)/(X+1) */ \
            __ASM_EMIT("vdivps          %%zmm6, %%zmm4, %%zmm4")            \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm3")            /* zmm3 = C0 */ \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm7")            \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm2")            /* zmm2 = Y = y*y */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm4, %%zmm6")            \
            /* zmm0 = y, zmm1 = R, zmm2 = Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C1+C0*Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C2+Y*(C1+C0*Y) */ \
            __ASM_EMIT("vfmadd213ps     0x140 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm6, %%zmm7")    \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm3, %%zmm0")            /* zmm0 = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
            __ASM_EMIT("vmulps          %%zmm4, %%zmm7, %%zmm4")

        #define LOGN_CORE_X16 \
            __ASM_EMIT("vpsrld          $23, %%zmm0, %%zmm1")               /* zmm1 = ilog2(x) + 127 */ \
            __ASM_EMIT("vpandd          0x000 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = x & F_MASK */ \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%zmm1, %%zmm1")    /* zmm1 = r = ilog2(x) */ \
            __ASM_EMIT("vpord           0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X = (x & F_MASK) | 1.0f */ \
            __ASM_EMIT("vcvtdq2ps       %%zmm1, %%zmm1")                    /* zmm1 = R = float(r) */ \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%zmm0, %%zmm2")    /* zmm2 = X + 1 */ \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%zmm0, %%zmm0")    /* zmm0 = X - 1 */ \
            __ASM_EMIT("vdivps          %%zmm2, %%zmm0, %%zmm0")            /* zmm0 = y = (X-1)/(X+1) */ \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%zmm3")            /* zmm3 = C0 */ \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm2")            /* zmm2 = Y = y*y */ \
            /* zmm0 = y, zmm1 = R, zmm2 = Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C1+C0*Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C2+Y*(C1+C0*Y) */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%zmm2, %%zmm3")    /* zmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
            __ASM_EMIT("vmulps          %%zmm0, %%zmm3, %%zmm0")            /* zmm0 = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \

        #define LOGN_CORE_X8 \
            __ASM_EMIT("vpsrld          $23, %%ymm0, %%ymm1")               /* ymm1 = ilog2(x) + 127 */ \
            __ASM_EMIT("vpand           0x000 + %[L2C], %%ymm0, %%ymm0")    /* ymm0 = x & F_MASK */ \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%ymm1, %%ymm1")    /* ymm1 = r = ilog2(x) */ \
            __ASM_EMIT("vpor            0x080 + %[L2C], %%ymm0, %%ymm0")    /* ymm0 = X = (x & F_MASK) | 1.0f */ \
            __ASM_EMIT("vcvtdq2ps       %%ymm1, %%ymm1")                    /* ymm1 = R = float(r) */ \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%ymm0, %%ymm2")    /* ymm2 = X + 1 */ \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%ymm0, %%ymm0")    /* ymm0 = X - 1 */ \
            __ASM_EMIT("vdivps          %%ymm2, %%ymm0, %%ymm0")            /* ymm0 = y = (X-1)/(X+1) */ \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%ymm3")            /* ymm3 = C0 */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm2")            /* ymm2 = Y = y*y */ \
            /* ymm0 = y, ymm1 = R, ymm2 = Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%ymm2, %%ymm3")    /* ymm3 = C1+C0*Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%ymm2, %%ymm3")    /* ymm3 = C2+Y*(C1+C0*Y) */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%ymm2, %%ymm3")    /* ymm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%ymm2, %%ymm3")    /* ymm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%ymm2, %%ymm3")    /* ymm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
            __ASM_EMIT("vmulps          %%ymm0, %%ymm3, %%ymm0")            /* ymm0 = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \

        #define LOGN_CORE_X4 \
            __ASM_EMIT("vpsrld          $23, %%xmm0, %%xmm1")               /* xmm1 = ilog2(x) + 127 */ \
            __ASM_EMIT("vpand           0x000 + %[L2C], %%xmm0, %%xmm0")    /* xmm0 = x & F_MASK */ \
            __ASM_EMIT("vpsubd          0x040 + %[L2C], %%xmm1, %%xmm1")    /* xmm1 = r = ilog2(x) */ \
            __ASM_EMIT("vpor            0x080 + %[L2C], %%xmm0, %%xmm0")    /* xmm0 = X = (x & F_MASK) | 1.0f */ \
            __ASM_EMIT("vcvtdq2ps       %%xmm1, %%xmm1")                    /* xmm1 = R = float(r) */ \
            __ASM_EMIT("vaddps          0x080 + %[L2C], %%xmm0, %%xmm2")    /* xmm2 = X + 1 */ \
            __ASM_EMIT("vsubps          0x080 + %[L2C], %%xmm0, %%xmm0")    /* xmm0 = X - 1 */ \
            __ASM_EMIT("vdivps          %%xmm2, %%xmm0, %%xmm0")            /* xmm0 = y = (X-1)/(X+1) */ \
            __ASM_EMIT("vmovaps         0x0c0 + %[L2C], %%xmm3")            /* xmm3 = C0 */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm2")            /* xmm2 = Y = y*y */ \
            /* xmm0 = y, xmm1 = R, xmm2 = Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%xmm2, %%xmm3")    /* xmm3 = C1+C0*Y */ \
            __ASM_EMIT("vfmadd213ps     0x100 + %[L2C], %%xmm2, %%xmm3")    /* xmm3 = C2+Y*(C1+C0*Y) */ \
            __ASM_EMIT("vfmadd213ps     0x180 + %[L2C], %%xmm2, %%xmm3")    /* xmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
            __ASM_EMIT("vfmadd213ps     0x1c0 + %[L2C], %%xmm2, %%xmm3")    /* xmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
            __ASM_EMIT("vfmadd213ps     0x200 + %[L2C], %%xmm2, %%xmm3")    /* xmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
            __ASM_EMIT("vmulps          %%xmm0, %%xmm3, %%xmm0")            /* xmm0 = y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \


        #define LOGB_CORE_X64 \
            LOGN_CORE_X64 \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm5, %%zmm4") \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm9, %%zmm8") \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm13, %%zmm12")

        #define LOGB_CORE_X32 \
            LOGN_CORE_X32 \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm5, %%zmm4") \

        #define LOGB_CORE_X16 \
            LOGN_CORE_X16 \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%zmm1, %%zmm0") \

        #define LOGB_CORE_X8 \
            LOGN_CORE_X8 \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%ymm1, %%ymm0")

        #define LOGB_CORE_X4 \
            LOGN_CORE_X4 \
            __ASM_EMIT("vfmadd132ps     0x00 + %[LOGC], %%xmm1, %%xmm0")


        #define LOGE_CORE_X64 \
            LOGN_CORE_X64 \
            __ASM_EMIT("vaddps          %%zmm0, %%zmm0, %%zmm0") \
            __ASM_EMIT("vaddps          %%zmm4, %%zmm4, %%zmm4") \
            __ASM_EMIT("vaddps          %%zmm8, %%zmm8, %%zmm8") \
            __ASM_EMIT("vaddps          %%zmm12, %%zmm12, %%zmm12") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm5, %%zmm4") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm9, %%zmm8") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm13, %%zmm12")

        #define LOGE_CORE_X32 \
            LOGN_CORE_X32 \
            __ASM_EMIT("vaddps          %%zmm0, %%zmm0, %%zmm0") \
            __ASM_EMIT("vaddps          %%zmm4, %%zmm4, %%zmm4") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm5, %%zmm4")

        #define LOGE_CORE_X16 \
            LOGN_CORE_X16 \
            __ASM_EMIT("vaddps          %%zmm0, %%zmm0, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%zmm1, %%zmm0")

        #define LOGE_CORE_X8 \
            LOGN_CORE_X8 \
            __ASM_EMIT("vaddps          %%ymm0, %%ymm0, %%ymm0") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%ymm1, %%ymm0")

        #define LOGE_CORE_X4 \
            LOGN_CORE_X4 \
            __ASM_EMIT("vaddps          %%xmm0, %%xmm0, %%xmm0") \
            __ASM_EMIT("vfmadd231ps     0x00 + %[LOGC], %%xmm1, %%xmm0")


        #define LOGD_CORE_X64 \
            LOGN_CORE_X64 \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm0, %%zmm0") \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm4, %%zmm4") \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm8, %%zmm8") \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm12, %%zmm12") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm5, %%zmm4") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm9, %%zmm8") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm13, %%zmm12")

        #define LOGD_CORE_X32 \
            LOGN_CORE_X32 \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm0, %%zmm0") \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm4, %%zmm4") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm1, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm5, %%zmm4")

        #define LOGD_CORE_X16 \
            LOGN_CORE_X16 \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%zmm0, %%zmm0") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%zmm1, %%zmm0")

        #define LOGD_CORE_X8 \
            LOGN_CORE_X8 \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%ymm0, %%ymm0") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%ymm1, %%ymm0")

        #define LOGD_CORE_X4 \
            LOGN_CORE_X4 \
            __ASM_EMIT("vmulps          0x00 + %[LOGC], %%xmm0, %%xmm0") \
            __ASM_EMIT("vfmadd231ps     0x40 + %[LOGC], %%xmm1, %%xmm0")



        void x64_logb2(float *dst, const float *src, size_t count)
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
                LOGB_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[src]")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGB_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                LOGB_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGB_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGB_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void logb2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGB_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                LOGB_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGB_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGB_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_logb1(float *dst, size_t count)
        {
            IF_ARCH_X86_64(float *src);

            ARCH_X86_64_ASM(
                // x64 blocks
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                __ASM_EMIT("vmovups         0x80(%[dst]), %%zmm8")
                __ASM_EMIT("vmovups         0xc0(%[dst]), %%zmm12")
                LOGB_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGB_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                LOGB_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGB_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGB_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void logb1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGB_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                LOGB_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGB_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGB_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_loge2(float *dst, const float *src, size_t count)
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
                LOGE_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[src]")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGE_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                LOGE_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGE_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGE_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void loge2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGE_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x16 blocks
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                LOGE_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGE_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGE_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_loge1(float *dst, size_t count)
        {
            IF_ARCH_X86_64(float *src);

            ARCH_X86_64_ASM(
                // x64 blocks
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                __ASM_EMIT("vmovups         0x80(%[dst]), %%zmm8")
                __ASM_EMIT("vmovups         0xc0(%[dst]), %%zmm12")
                LOGE_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGE_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                LOGE_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGE_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGE_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void loge1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGE_CORE_X32
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
                LOGE_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGE_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGE_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_logd2(float *dst, const float *src, size_t count)
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
                LOGD_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[src]")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGD_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[src]")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                LOGD_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGD_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGD_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void logd2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[src]), %%zmm4")
                LOGD_CORE_X32
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
                LOGD_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%ymm0")
                LOGD_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[src]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGD_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_logd1(float *dst, size_t count)
        {
            IF_ARCH_X86_64(float *src);

            ARCH_X86_64_ASM(
                // x64 blocks
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                __ASM_EMIT("vmovups         0x80(%[dst]), %%zmm8")
                __ASM_EMIT("vmovups         0xc0(%[dst]), %%zmm12")
                LOGD_CORE_X64
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x80(%[dst])")
                __ASM_EMIT("vmovups         %%zmm12, 0xc0(%[dst])")
                __ASM_EMIT("add             $0x100, %[dst]")
                __ASM_EMIT("sub             $64, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x32 block
                __ASM_EMIT("add             $32, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGD_CORE_X32
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%zmm4, 0x40(%[dst])")
                __ASM_EMIT("add             $0x80, %[dst]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("4:")
                // x16 block
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                LOGD_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("6:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGD_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("8:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGD_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void logd1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x32 blocks
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%zmm0")
                __ASM_EMIT("vmovups         0x40(%[dst]), %%zmm4")
                LOGD_CORE_X32
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
                LOGD_CORE_X16
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("4:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%ymm0")
                LOGD_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("6:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[dst]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
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
                LOGD_CORE_X4
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
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_LOG_H_ */
