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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_VV_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_VV_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
    #define OP_VV_CORE(DST, SRC1, SRC2, OP) \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* 128x blocks */ \
        __ASM_EMIT("sub         $128, %[count]") \
        __ASM_EMIT("jb          2f")    \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%zmm0") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%zmm1") \
        __ASM_EMIT("vmovups     0x080(%[" SRC1 "], %[off]), %%zmm2") \
        __ASM_EMIT("vmovups     0x0c0(%[" SRC1 "], %[off]), %%zmm3") \
        __ASM_EMIT("vmovups     0x100(%[" SRC1 "], %[off]), %%zmm4") \
        __ASM_EMIT("vmovups     0x140(%[" SRC1 "], %[off]), %%zmm5") \
        __ASM_EMIT("vmovups     0x180(%[" SRC1 "], %[off]), %%zmm6") \
        __ASM_EMIT("vmovups     0x1c0(%[" SRC1 "], %[off]), %%zmm7") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%zmm0, %%zmm0") \
        __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%zmm1, %%zmm1") \
        __ASM_EMIT(OP "ps       0x080(%[" SRC2 "], %[off]), %%zmm2, %%zmm2") \
        __ASM_EMIT(OP "ps       0x0c0(%[" SRC2 "], %[off]), %%zmm3, %%zmm3") \
        __ASM_EMIT(OP "ps       0x100(%[" SRC2 "], %[off]), %%zmm4, %%zmm4") \
        __ASM_EMIT(OP "ps       0x140(%[" SRC2 "], %[off]), %%zmm5, %%zmm5") \
        __ASM_EMIT(OP "ps       0x180(%[" SRC2 "], %[off]), %%zmm6, %%zmm6") \
        __ASM_EMIT(OP "ps       0x1c0(%[" SRC2 "], %[off]), %%zmm7, %%zmm7") \
        __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm2, 0x080(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm3, 0x0c0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm4, 0x100(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm5, 0x140(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm6, 0x180(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm7, 0x1c0(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x200, %[off]") \
        __ASM_EMIT("sub         $128, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 64x blocks */ \
        __ASM_EMIT("add         $64, %[count]") \
        __ASM_EMIT("jl          4f")    \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x060(%[" SRC1 "], %[off]), %%ymm3") \
        __ASM_EMIT("vmovups     0x080(%[" SRC1 "], %[off]), %%ymm4") \
        __ASM_EMIT("vmovups     0x0a0(%[" SRC1 "], %[off]), %%ymm5") \
        __ASM_EMIT("vmovups     0x0c0(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x0e0(%[" SRC1 "], %[off]), %%ymm7") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%ymm2, %%ymm2") \
        __ASM_EMIT(OP "ps       0x060(%[" SRC2 "], %[off]), %%ymm3, %%ymm3") \
        __ASM_EMIT(OP "ps       0x080(%[" SRC2 "], %[off]), %%ymm4, %%ymm4") \
        __ASM_EMIT(OP "ps       0x0a0(%[" SRC2 "], %[off]), %%ymm5, %%ymm5") \
        __ASM_EMIT(OP "ps       0x0c0(%[" SRC2 "], %[off]), %%ymm6, %%ymm6") \
        __ASM_EMIT(OP "ps       0x0e0(%[" SRC2 "], %[off]), %%ymm7, %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm4, 0x080(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm5, 0x0a0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm6, 0x0c0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x0e0(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $64, %[count]") \
        __ASM_EMIT("add         $0x100, %[off]") \
        __ASM_EMIT("4:") \
        /* 32x block */ \
        __ASM_EMIT("add         $32, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm1") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm2") \
        __ASM_EMIT("vmovups     0x060(%[" SRC1 "], %[off]), %%ymm3") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%ymm2, %%ymm2") \
        __ASM_EMIT(OP "ps       0x060(%[" SRC2 "], %[off]), %%ymm3, %%ymm3") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("6:") \
        /* 16x block */ \
        __ASM_EMIT("add         $16, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm0") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm1") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("8:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm0") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm0") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("10:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          12f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm0") \
        __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("12:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          14f") \
        __ASM_EMIT("13:") \
        __ASM_EMIT("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm0") \
        __ASM_EMIT(OP "ss       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         13b") \
        __ASM_EMIT("14:")

        void add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "dst", "src", "vadd")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "dst", "src", "vsub")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src", "dst", "vsub")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "dst", "src", "vmul")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "dst", "src", "vdiv")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src", "dst", "vdiv")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void add3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src1", "src2", "vadd")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void sub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src1", "src2", "vsub")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src1", "src2", "vmul")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void div3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                OP_VV_CORE("dst", "src1", "src2", "vdiv")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef OP_VV_CORE

        #define FMOD_VV_CORE(DST, SRC1, SRC2) \
            __ASM_EMIT("xor                 %[off], %[off]") \
            /* x32 blocks */ \
            __ASM_EMIT("sub                 $32, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups             0x40(%[" SRC1 "], %[off]), %%zmm3") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups             0x40(%[" SRC2 "], %[off]), %%zmm1") \
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
            __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups             0x20(%[" SRC1 "], %[off]), %%ymm3") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups             0x20(%[" SRC2 "], %[off]), %%ymm1") \
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
            __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), %%xmm2") \
            __ASM_EMIT("vmovups             0x10(%[" SRC1 "], %[off]), %%xmm3") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups             0x10(%[" SRC2 "], %[off]), %%xmm1") \
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
            __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), %%xmm2") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%xmm0") \
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
            __ASM_EMIT("vmovss              0x00(%[" SRC1 "], %[off]), %%xmm2") \
            __ASM_EMIT("vmovss              0x00(%[" SRC2 "], %[off]), %%xmm0") \
            __ASM_EMIT("vdivps              %%xmm0, %%xmm2, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd231ps        %%xmm0, %%xmm4, %%xmm2") \
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x04, %[off]") \
            __ASM_EMIT("dec                 %[count]") \
            __ASM_EMIT("jge                 9b") \
            __ASM_EMIT("10:")

        void mod2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void rmod2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "src", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void mod3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "src1", "src2")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #undef FMOD_VV_CORE
    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_OP_VV_H_ */
