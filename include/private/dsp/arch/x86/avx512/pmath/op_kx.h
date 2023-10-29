/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_KX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_KX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define OP_K4_CORE(DST, SRC, OP, SEL) \
            __ASM_EMIT("vbroadcastss %[k], %%zmm0") \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("vmovaps     %%zmm0, %%zmm1") \
            /* 64x blocks */ \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%zmm4")) \
            __ASM_EMIT(SEL("", "vmovups     0x040(%[" SRC "], %[off]), %%zmm5")) \
            __ASM_EMIT(SEL("", "vmovups     0x080(%[" SRC "], %[off]), %%zmm6")) \
            __ASM_EMIT(SEL("", "vmovups     0x0c0(%[" SRC "], %[off]), %%zmm7")) \
            __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%zmm0") ", " SEL("%%zmm0", "%%zmm4") ", %%zmm4") \
            __ASM_EMIT(OP "ps       " SEL("0x040(%[" SRC "], %[off])", "%%zmm1") ", " SEL("%%zmm1", "%%zmm5") ", %%zmm5") \
            __ASM_EMIT(OP "ps       " SEL("0x080(%[" SRC "], %[off])", "%%zmm0") ", " SEL("%%zmm0", "%%zmm6") ", %%zmm6") \
            __ASM_EMIT(OP "ps       " SEL("0x0c0(%[" SRC "], %[off])", "%%zmm1") ", " SEL("%%zmm1", "%%zmm7") ", %%zmm7") \
            __ASM_EMIT("vmovups     %%zmm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm5, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm6, 0x080(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm7, 0x0c0(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x100, %[off]") \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 32x blocks */ \
            __ASM_EMIT("add         $32, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
            __ASM_EMIT(SEL("", "vmovups     0x020(%[" SRC "], %[off]), %%ymm5")) \
            __ASM_EMIT(SEL("", "vmovups     0x040(%[" SRC "], %[off]), %%ymm6")) \
            __ASM_EMIT(SEL("", "vmovups     0x060(%[" SRC "], %[off]), %%ymm7")) \
            __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
            __ASM_EMIT(OP "ps       " SEL("0x020(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm5") ", %%ymm5") \
            __ASM_EMIT(OP "ps       " SEL("0x040(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm6") ", %%ymm6") \
            __ASM_EMIT(OP "ps       " SEL("0x060(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm7") ", %%ymm7") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm6, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm7, 0x060(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("4:") \
            /* 16x block */ \
            __ASM_EMIT("add         $16, %[count]")          /* 32 - 16 */ \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
            __ASM_EMIT(SEL("", "vmovups     0x020(%[" SRC "], %[off]), %%ymm5")) \
            __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
            __ASM_EMIT(OP "ps       " SEL("0x020(%[" SRC "], %[off])", "%%ymm1") ", " SEL("%%ymm1", "%%ymm5") ", %%ymm5") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("6:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]")          /* 16 - 8 */ \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%ymm4")) \
            __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%ymm0") ", " SEL("%%ymm0", "%%ymm4") ", %%ymm4") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("8:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT(SEL("", "vmovups     0x000(%[" SRC "], %[off]), %%xmm4")) \
            __ASM_EMIT(OP "ps       " SEL("0x000(%[" SRC "], %[off])", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4") ", %%xmm4") \
            __ASM_EMIT("vmovups     %%xmm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("10:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
            __ASM_EMIT("jl          12f") \
            __ASM_EMIT("11:") \
            __ASM_EMIT(SEL("", "vmovss      0x000(%[" SRC "], %[off]), %%xmm4")) \
            __ASM_EMIT(OP "ss       " SEL("0x000(%[" SRC "], %[off])", "%%xmm0") ", " SEL("%%xmm0", "%%xmm4") ", %%xmm4") \
            __ASM_EMIT("vmovss      %%xmm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         11b") \
            __ASM_EMIT("12:")

        void add_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "dst", "vadd", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "dst", "vsub", OP_RSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void rsub_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "dst", "vsub", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void mul_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "dst", "vmul", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void div_k2(float *dst, float k, size_t count)
        {
            mul_k2(dst, 1.0f/k, count);
        }

        void rdiv_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "dst", "vdiv", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void add_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "src", "vadd", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "src", "vsub", OP_RSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void rsub_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "src", "vsub", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void mul_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "src", "vmul", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void div_k3(float *dst, const float *src, float k, size_t count)
        {
            mul_k3(dst, src, 1.0f/k, count);
        }

        void rdiv_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_K4_CORE("dst", "src", "vdiv", OP_DSEL)
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef OP_K4_CORE
        #undef OP_DSEL
        #undef OP_RSEL

        #define FMOD_KX_CORE(DST, SRC) \
            __ASM_EMIT("vbroadcastss        %[k], %%zmm0") \
            __ASM_EMIT("xor                 %[off], %[off]") \
            __ASM_EMIT("vmovaps             %%zmm0, %%zmm1") \
            /* x32 blocks */ \
            __ASM_EMIT("sub                 $32, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups             0x40(%[" SRC "], %[off]), %%zmm3") \
            __ASM_EMIT("vdivps              %%zmm0, %%zmm2, %%zmm4") \
            __ASM_EMIT("vdivps              %%zmm1, %%zmm3, %%zmm5") \
            __ASM_EMIT("vcvttps2dq          %%zmm4, %%zmm4") \
            __ASM_EMIT("vcvttps2dq          %%zmm5, %%zmm5") \
            __ASM_EMIT("vcvtdq2ps           %%zmm4, %%zmm4") \
            __ASM_EMIT("vcvtdq2ps           %%zmm5, %%zmm5") \
            __ASM_EMIT("vfnmadd231ps        %%zmm0, %%zmm4, %%zmm2") \
            __ASM_EMIT("vfnmadd231ps        %%zmm1, %%zmm5, %%zmm3") \
            __ASM_EMIT("vmovups             %%zmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%zmm3, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x80, %[off]") \
            __ASM_EMIT("sub                 $32, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("2:") \
            /* x16 blocks */ \
            __ASM_EMIT("add                 $16, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups             0x20(%[" SRC "], %[off]), %%ymm3") \
            __ASM_EMIT("vdivps              %%ymm0, %%ymm2, %%ymm4") \
            __ASM_EMIT("vdivps              %%ymm1, %%ymm3, %%ymm5") \
            __ASM_EMIT("vcvttps2dq          %%ymm4, %%ymm4") \
            __ASM_EMIT("vcvttps2dq          %%ymm5, %%ymm5") \
            __ASM_EMIT("vcvtdq2ps           %%ymm4, %%ymm4") \
            __ASM_EMIT("vcvtdq2ps           %%ymm5, %%ymm5") \
            __ASM_EMIT("vfnmadd231ps        %%ymm0, %%ymm4, %%ymm2") \
            __ASM_EMIT("vfnmadd231ps        %%ymm1, %%ymm5, %%ymm3") \
            __ASM_EMIT("vmovups             %%ymm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%ymm3, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("add                 $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /* x8 block */ \
            __ASM_EMIT("add                 $8, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vmovups             0x10(%[" SRC "], %[off]), %%xmm3") \
            __ASM_EMIT("vdivps              %%xmm0, %%xmm2, %%xmm4") \
            __ASM_EMIT("vdivps              %%xmm1, %%xmm3, %%xmm5") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm5, %%xmm5") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm5, %%xmm5") \
            __ASM_EMIT("vfnmadd231ps        %%xmm0, %%xmm4, %%xmm2") \
            __ASM_EMIT("vfnmadd231ps        %%xmm1, %%xmm5, %%xmm3") \
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $8, %[count]") \
            __ASM_EMIT("add                 $0x20, %[off]") \
            __ASM_EMIT("6:") \
            /* x4 block */ \
            __ASM_EMIT("add                 $4, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vdivps              %%xmm0, %%xmm2, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd231ps        %%xmm0, %%xmm4, %%xmm2") \
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $4, %[count]") \
            __ASM_EMIT("add                 $0x10, %[off]") \
            __ASM_EMIT("8:") \
            /* x1 blocks */ \
            __ASM_EMIT("add                 $3, %[count]") \
            __ASM_EMIT("jl                  10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss              0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vdivps              %%xmm0, %%xmm2, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd231ps        %%xmm0, %%xmm4, %%xmm2") \
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x04, %[off]") \
            __ASM_EMIT("dec                 %[count]") \
            __ASM_EMIT("jge                 9b") \
            __ASM_EMIT("10:")

        void mod_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "dst")
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void mod_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "src")
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #undef FMOD_KX_CORE

        #define FRMOD_KX_CORE(DST, SRC) \
            __ASM_EMIT("vbroadcastss        %[k], %%zmm0") \
            __ASM_EMIT("xor                 %[off], %[off]") \
            __ASM_EMIT("vmovaps             %%zmm0, %%zmm1") \
            /* x32 blocks */ \
            __ASM_EMIT("sub                 $32, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups             0x40(%[" SRC "], %[off]), %%zmm3") \
            __ASM_EMIT("vdivps              %%zmm2, %%zmm0, %%zmm4") \
            __ASM_EMIT("vdivps              %%zmm3, %%zmm1, %%zmm5") \
            __ASM_EMIT("vcvttps2dq          %%zmm4, %%zmm4") \
            __ASM_EMIT("vcvttps2dq          %%zmm5, %%zmm5") \
            __ASM_EMIT("vcvtdq2ps           %%zmm4, %%zmm4") \
            __ASM_EMIT("vcvtdq2ps           %%zmm5, %%zmm5") \
            __ASM_EMIT("vfnmadd132ps        %%zmm4, %%zmm0, %%zmm2") \
            __ASM_EMIT("vfnmadd132ps        %%zmm5, %%zmm1, %%zmm3") \
            __ASM_EMIT("vmovups             %%zmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%zmm3, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x80, %[off]") \
            __ASM_EMIT("sub                 $32, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("2:") \
            /* x16 blocks */ \
            __ASM_EMIT("add                 $16, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups             0x20(%[" SRC "], %[off]), %%ymm3") \
            __ASM_EMIT("vdivps              %%ymm2, %%ymm0, %%ymm4") \
            __ASM_EMIT("vdivps              %%ymm3, %%ymm1, %%ymm5") \
            __ASM_EMIT("vcvttps2dq          %%ymm4, %%ymm4") \
            __ASM_EMIT("vcvttps2dq          %%ymm5, %%ymm5") \
            __ASM_EMIT("vcvtdq2ps           %%ymm4, %%ymm4") \
            __ASM_EMIT("vcvtdq2ps           %%ymm5, %%ymm5") \
            __ASM_EMIT("vfnmadd132ps        %%ymm4, %%ymm0, %%ymm2") \
            __ASM_EMIT("vfnmadd132ps        %%ymm5, %%ymm1, %%ymm3") \
            __ASM_EMIT("vmovups             %%ymm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%ymm3, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("add                 $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /* x8 block */ \
            __ASM_EMIT("add                 $8, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vmovups             0x10(%[" SRC "], %[off]), %%xmm3") \
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm4") \
            __ASM_EMIT("vdivps              %%xmm3, %%xmm1, %%xmm5") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm5, %%xmm5") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm5, %%xmm5") \
            __ASM_EMIT("vfnmadd132ps        %%xmm4, %%xmm0, %%xmm2") \
            __ASM_EMIT("vfnmadd132ps        %%xmm5, %%xmm1, %%xmm3") \
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%xmm3, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $8, %[count]") \
            __ASM_EMIT("add                 $0x20, %[off]") \
            __ASM_EMIT("6:") \
            /* x4 block */ \
            __ASM_EMIT("add                 $4, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd132ps        %%xmm4, %%xmm0, %%xmm2") \
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $4, %[count]") \
            __ASM_EMIT("add                 $0x10, %[off]") \
            __ASM_EMIT("8:") \
            /* x1 blocks */ \
            __ASM_EMIT("add                 $3, %[count]") \
            __ASM_EMIT("jl                  10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss              0x00(%[" SRC "], %[off]), %%xmm2") \
            __ASM_EMIT("vdivps              %%xmm2, %%xmm0, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd132ps        %%xmm4, %%xmm0, %%xmm2") \
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x04, %[off]") \
            __ASM_EMIT("dec                 %[count]") \
            __ASM_EMIT("jge                 9b") \
            __ASM_EMIT("10:")

        void rmod_k2(float *dst, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FRMOD_KX_CORE("dst", "dst")
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void rmod_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FRMOD_KX_CORE("dst", "src")
                : [count] "+r" (count), [off] "=&r" (off),
                  [k] "+Yz" (k)
                : [src] "r" (src), [dst] "r" (dst)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #undef FRMOD_KX_CORE

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_KX_H_ */
