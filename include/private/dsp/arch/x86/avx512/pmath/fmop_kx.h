/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_KX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_KX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define FMADDSUB_K3_CORE(DST, SRC1, SRC2, OP) \
            __ASM_EMIT("vbroadcastss %[k], %%zmm0") \
            __ASM_EMIT("xor         %[off], %[off]") \
            __ASM_EMIT("vmovaps     %%zmm0, %%zmm1") \
            /* 96x blocks */ \
            __ASM_EMIT("sub         $96, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%zmm3") \
            __ASM_EMIT("vmovups     0x080(%[" SRC1 "], %[off]), %%zmm4") \
            __ASM_EMIT("vmovups     0x0c0(%[" SRC1 "], %[off]), %%zmm5") \
            __ASM_EMIT("vmovups     0x100(%[" SRC1 "], %[off]), %%zmm6") \
            __ASM_EMIT("vmovups     0x140(%[" SRC1 "], %[off]), %%zmm7") \
            __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%zmm0, %%zmm2") \
            __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%zmm1, %%zmm3") \
            __ASM_EMIT(OP "ps       0x080(%[" SRC2 "], %[off]), %%zmm0, %%zmm4") \
            __ASM_EMIT(OP "ps       0x0c0(%[" SRC2 "], %[off]), %%zmm1, %%zmm5") \
            __ASM_EMIT(OP "ps       0x100(%[" SRC2 "], %[off]), %%zmm0, %%zmm6") \
            __ASM_EMIT(OP "ps       0x140(%[" SRC2 "], %[off]), %%zmm1, %%zmm7") \
            __ASM_EMIT("vmovups     %%zmm2, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm4, 0x080(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm5, 0x0c0(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm6, 0x100(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm7, 0x140(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x180, %[off]") \
            __ASM_EMIT("sub         $96, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 32x blocks */ \
            __ASM_EMIT("add         $64, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("3:") \
            __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm5") \
            __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%ymm6") \
            __ASM_EMIT("vmovups     0x060(%[" SRC1 "], %[off]), %%ymm7") \
            __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
            __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm5") \
            __ASM_EMIT(OP "ps       0x040(%[" SRC2 "], %[off]), %%ymm0, %%ymm6") \
            __ASM_EMIT(OP "ps       0x060(%[" SRC2 "], %[off]), %%ymm1, %%ymm7") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm6, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm7, 0x060(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("jge         3b") \
            __ASM_EMIT("4:") \
            /* 16x block */ \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm5") \
            __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
            __ASM_EMIT(OP "ps       0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm5") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm5, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("6:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm4") \
            __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm4") \
            __ASM_EMIT("vmovups     %%ymm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("8:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm4") \
            __ASM_EMIT(OP "ps       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm4") \
            __ASM_EMIT("vmovups     %%xmm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("10:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          12f") \
            __ASM_EMIT("11:") \
            __ASM_EMIT("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm4") \
            __ASM_EMIT(OP "ss       0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm4") \
            __ASM_EMIT("vmovss      %%xmm4, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         11b") \
            __ASM_EMIT("12:")

        void fmadd_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "dst", "src", "vfmadd231")
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmsub_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "dst", "src", "vfnmadd231")
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrsub_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "dst", "src", "vfmsub231")
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "src1", "src2", "vfmadd231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [k] "m" (k)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "src1", "src2", "vfnmadd231")
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_K3_CORE("dst", "src1", "src2", "vfmsub231")
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef FMADDSUB_K3_CORE

    #define FMOP_K4_CORE(DST, SRC1, SRC2, OP, SEL) \
        __ASM_EMIT("vbroadcastss %[k], %%zmm0") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vmovaps     %%zmm0, %%zmm1") \
        /* 48x blocks */ \
        __ASM_EMIT("sub         $48, %[count]") \
        __ASM_EMIT("jbe         2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%zmm5") \
        __ASM_EMIT("vmovups     0x040(%[" SRC1 "], %[off]), %%zmm6") \
        __ASM_EMIT("vmovups     0x080(%[" SRC1 "], %[off]), %%zmm7") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%zmm0, %%zmm2") \
        __ASM_EMIT("vmulps      0x040(%[" SRC2 "], %[off]), %%zmm1, %%zmm3") \
        __ASM_EMIT("vmulps      0x080(%[" SRC2 "], %[off]), %%zmm0, %%zmm4") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm2", "%%zmm5") ", "  SEL("%%zmm5", "%%zmm2") ", %%zmm5") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm3", "%%zmm6") ", "  SEL("%%zmm6", "%%zmm3") ", %%zmm6") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm4", "%%zmm7") ", "  SEL("%%zmm7", "%%zmm4") ", %%zmm7") \
        __ASM_EMIT("vmovups     %%zmm5, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm6, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm7, 0x080(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0xc0, %[off]") \
        __ASM_EMIT("sub         $48, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 16x blocks */ \
        __ASM_EMIT("add         $32, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm7") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT("vmulps      0x020(%[" SRC2 "], %[off]), %%ymm1, %%ymm3") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "%%ymm7") ", "  SEL("%%ymm7", "%%ymm3") ", %%ymm7") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm7, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%ymm0, %%ymm2") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "%%ymm6") ", "  SEL("%%ymm6", "%%ymm2") ", %%ymm6") \
        __ASM_EMIT("vmovups     %%ymm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("6:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulps      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovups     %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("8:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]")          /* 8 - 4 */ \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss      0x000(%[" SRC1 "], %[off]), %%xmm6") \
        __ASM_EMIT("vmulss      0x000(%[" SRC2 "], %[off]), %%xmm0, %%xmm2") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm2", "%%xmm6") ", "  SEL("%%xmm6", "%%xmm2") ", %%xmm6") \
        __ASM_EMIT("vmovss      %%xmm6, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

        void fmmul_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "dst", "src", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmdiv_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "dst", "src", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrdiv_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "dst", "src", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r"(src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "src1", "src2", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "src1", "src2", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_K4_CORE("dst", "src1", "src2", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef FMOP_K4_CORE

    #define FMOD_KX_CORE(DST, SRC1, SRC2, OP_SEL) \
        __ASM_EMIT("vbroadcastss        %[k], %%zmm0") \
        __ASM_EMIT("xor                 %[off], %[off]") \
        __ASM_EMIT("vmovaps             %%zmm0, %%zmm1") \
        /* x32 blocks */ \
        __ASM_EMIT("sub                 $32, %[count]") \
        __ASM_EMIT("jb                  2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%zmm2", "%%zmm6")) \
        __ASM_EMIT("vmovups             0x40(%[" SRC1 "], %[off]), " OP_SEL("%%zmm3", "%%zmm7")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%zmm0, " OP_SEL("%%zmm6", "%%zmm2")) \
        __ASM_EMIT("vmulps              0x40(%[" SRC2 "], %[off]), %%zmm1, " OP_SEL("%%zmm7", "%%zmm3")) \
        __ASM_EMIT("vdivps              %%zmm6, %%zmm2, %%zmm4") \
        __ASM_EMIT("vdivps              %%zmm7, %%zmm3, %%zmm5") \
        __ASM_EMIT("vcvttps2dq          %%zmm4, %%zmm4") \
        __ASM_EMIT("vcvttps2dq          %%zmm5, %%zmm5") \
        __ASM_EMIT("vcvtdq2ps           %%zmm4, %%zmm4") \
        __ASM_EMIT("vcvtdq2ps           %%zmm5, %%zmm5") \
        __ASM_EMIT("vfnmadd231ps        %%zmm6, %%zmm4, %%zmm2") \
        __ASM_EMIT("vfnmadd231ps        %%zmm7, %%zmm5, %%zmm3") \
        __ASM_EMIT("vmovups             %%zmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%zmm3, 0x40(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x80, %[off]") \
        __ASM_EMIT("sub                 $32, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("2:") \
        /* x16 blocks */ \
        __ASM_EMIT("add                 $16, %[count]") \
        __ASM_EMIT("jl                  4f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%ymm2", "%%ymm6")) \
        __ASM_EMIT("vmovups             0x20(%[" SRC1 "], %[off]), " OP_SEL("%%ymm3", "%%ymm7")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%ymm0, " OP_SEL("%%ymm6", "%%ymm2")) \
        __ASM_EMIT("vmulps              0x20(%[" SRC2 "], %[off]), %%ymm1, " OP_SEL("%%ymm7", "%%ymm3")) \
        __ASM_EMIT("vdivps              %%ymm6, %%ymm2, %%ymm4") \
        __ASM_EMIT("vdivps              %%ymm7, %%ymm3, %%ymm5") \
        __ASM_EMIT("vcvttps2dq          %%ymm4, %%ymm4") \
        __ASM_EMIT("vcvttps2dq          %%ymm5, %%ymm5") \
        __ASM_EMIT("vcvtdq2ps           %%ymm4, %%ymm4") \
        __ASM_EMIT("vcvtdq2ps           %%ymm5, %%ymm5") \
        __ASM_EMIT("vfnmadd231ps        %%ymm6, %%ymm4, %%ymm2") \
        __ASM_EMIT("vfnmadd231ps        %%ymm7, %%ymm5, %%ymm3") \
        __ASM_EMIT("vmovups             %%ymm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%ymm3, 0x20(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x40, %[off]") \
        __ASM_EMIT("sub                 $16, %[count]") \
        __ASM_EMIT("jae                 1b") \
        __ASM_EMIT("4:") \
        /* x8 block */ \
        __ASM_EMIT("add                 $8, %[count]") \
        __ASM_EMIT("jl                  6f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmovups             0x10(%[" SRC1 "], %[off]), " OP_SEL("%%xmm3", "%%xmm7")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vmulps              0x10(%[" SRC2 "], %[off]), %%xmm1, " OP_SEL("%%xmm7", "%%xmm3")) \
        __ASM_EMIT("vdivps              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vdivps              %%xmm7, %%xmm3, %%xmm5") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm5, %%xmm5") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm5, %%xmm5") \
        __ASM_EMIT("vfnmadd231ps        %%xmm6, %%xmm4, %%xmm2") \
        __ASM_EMIT("vfnmadd231ps        %%xmm7, %%xmm5, %%xmm3") \
        __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups             %%xmm3, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $8, %[count]") \
        __ASM_EMIT("add                 $0x20, %[off]") \
        __ASM_EMIT("6:") \
        /* x4 block */ \
        __ASM_EMIT("add                 $4, %[count]") \
        __ASM_EMIT("jl                  8f") \
        __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vdivps              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT("vfnmadd231ps        %%xmm6, %%xmm4, %%xmm2") \
        __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("sub                 $4, %[count]") \
        __ASM_EMIT("add                 $0x10, %[off]") \
        __ASM_EMIT("8:") \
        /* x1 blocks */ \
        __ASM_EMIT("add                 $3, %[count]") \
        __ASM_EMIT("jl                  10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT("vmovss              0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
        __ASM_EMIT("vmulss              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
        __ASM_EMIT("vdivss              %%xmm6, %%xmm2, %%xmm4") \
        __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
        __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
        __ASM_EMIT("vfnmadd231ss        %%xmm6, %%xmm4, %%xmm2") \
        __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add                 $0x04, %[off]") \
        __ASM_EMIT("dec                 %[count]") \
        __ASM_EMIT("jge                 9b") \
        __ASM_EMIT("10:")

        void fmmod_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "dst", "src", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r" (src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrmod_k3(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "dst", "src", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src] "r" (src)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "src1", "src2", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_KX_CORE("dst", "src1", "src2", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                           "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef FMOD_KX_CORE

    #undef OP_DSEL
    #undef OP_RSEL

    } /* namespace avx512 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_KX_H_ */
