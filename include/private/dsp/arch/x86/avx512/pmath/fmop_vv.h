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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_VV_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_VV_H_

namespace lsp
{
    namespace avx512
    {
        #define OP_DSEL(a, b)   a
        #define OP_RSEL(a, b)   b

        #define FMADDSUB_VV_CORE(DST, A, B, C, OP) \
            __ASM_EMIT("xor         %[off], %[off]") \
            /* 64x blocks */ \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x080(%[" A "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups     0x0c0(%[" A "], %[off]), %%zmm3") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%zmm4") \
            __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vmovups     0x080(%[" B "], %[off]), %%zmm6") \
            __ASM_EMIT("vmovups     0x0c0(%[" B "], %[off]), %%zmm7") \
            __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%zmm4, %%zmm0") \
            __ASM_EMIT(OP "ps       0x040(%[" C "], %[off]), %%zmm5, %%zmm1") \
            __ASM_EMIT(OP "ps       0x080(%[" C "], %[off]), %%zmm6, %%zmm2") \
            __ASM_EMIT(OP "ps       0x0c0(%[" C "], %[off]), %%zmm7, %%zmm3") \
            __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm2, 0x080(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0x0c0(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x100, %[off]") \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 32x blocks */ \
            __ASM_EMIT("add         $32, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups     0x060(%[" A "], %[off]), %%ymm3") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%ymm6") \
            __ASM_EMIT("vmovups     0x060(%[" B "], %[off]), %%ymm7") \
            __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
            __ASM_EMIT(OP "ps       0x020(%[" C "], %[off]), %%ymm5, %%ymm1") \
            __ASM_EMIT(OP "ps       0x040(%[" C "], %[off]), %%ymm6, %%ymm2") \
            __ASM_EMIT(OP "ps       0x060(%[" C "], %[off]), %%ymm7, %%ymm3") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("4:") \
            /* 16x block */ \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
            __ASM_EMIT(OP "ps       0x020(%[" C "], %[off]), %%ymm5, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("6:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%ymm4, %%ymm0") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("8:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%xmm4") \
            __ASM_EMIT(OP "ps       0x000(%[" C "], %[off]), %%xmm4, %%xmm0") \
            __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("10:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          12f") \
            __ASM_EMIT("11:") \
            __ASM_EMIT("vmovss      0x000(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovss      0x000(%[" B "], %[off]), %%xmm4") \
            __ASM_EMIT(OP "ss       0x000(%[" C "], %[off]), %%xmm4, %%xmm0") \
            __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         11b") \
            __ASM_EMIT("12:")

        void fmadd3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfmadd231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmsub3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfnmadd231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrsub3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "dst", "a", "b", "vfmsub231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfmadd231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfnmadd231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMADDSUB_VV_CORE("dst", "a", "b", "c", "vfmsub231")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef FMADDSUB_VV_CORE

        #define FMOP_VV_CORE(DST, A, B, C, OP, SEL) \
            __ASM_EMIT("xor         %[off], %[off]") \
            /* 64x blocks */ \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jb          2f")    \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%zmm4") \
            __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vmovups     0x080(%[" B "], %[off]), %%zmm6") \
            __ASM_EMIT("vmovups     0x0c0(%[" B "], %[off]), %%zmm7") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups     0x080(%[" A "], %[off]), %%zmm2") \
            __ASM_EMIT("vmovups     0x0c0(%[" A "], %[off]), %%zmm3") \
            __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%zmm4, %%zmm4") \
            __ASM_EMIT("vmulps      0x040(%[" C "], %[off]), %%zmm5, %%zmm5") \
            __ASM_EMIT("vmulps      0x080(%[" C "], %[off]), %%zmm6, %%zmm6") \
            __ASM_EMIT("vmulps      0x0c0(%[" C "], %[off]), %%zmm7, %%zmm7") \
            __ASM_EMIT(OP "ps       " SEL("%%zmm4", "%%zmm0") ", "  SEL("%%zmm0", "%%zmm4") ", %%zmm0") \
            __ASM_EMIT(OP "ps       " SEL("%%zmm5", "%%zmm1") ", "  SEL("%%zmm1", "%%zmm5") ", %%zmm1") \
            __ASM_EMIT(OP "ps       " SEL("%%zmm6", "%%zmm2") ", "  SEL("%%zmm2", "%%zmm6") ", %%zmm2") \
            __ASM_EMIT(OP "ps       " SEL("%%zmm7", "%%zmm3") ", "  SEL("%%zmm3", "%%zmm7") ", %%zmm3") \
            __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm2, 0x080(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%zmm3, 0x0c0(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x100, %[off]") \
            __ASM_EMIT("sub         $64, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* 32x blocks */ \
            __ASM_EMIT("add         $32, %[count]") \
            __ASM_EMIT("jl          4f")    \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vmovups     0x040(%[" B "], %[off]), %%ymm6") \
            __ASM_EMIT("vmovups     0x060(%[" B "], %[off]), %%ymm7") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x040(%[" A "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups     0x060(%[" A "], %[off]), %%ymm3") \
            __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
            __ASM_EMIT("vmulps      0x020(%[" C "], %[off]), %%ymm5, %%ymm5") \
            __ASM_EMIT("vmulps      0x040(%[" C "], %[off]), %%ymm6, %%ymm6") \
            __ASM_EMIT("vmulps      0x060(%[" C "], %[off]), %%ymm7, %%ymm7") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm5", "%%ymm1") ", "  SEL("%%ymm1", "%%ymm5") ", %%ymm1") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm6", "%%ymm2") ", "  SEL("%%ymm2", "%%ymm6") ", %%ymm2") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm7", "%%ymm3") ", "  SEL("%%ymm3", "%%ymm7") ", %%ymm3") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm2, 0x040(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm3, 0x060(%[" DST "], %[off])") \
            __ASM_EMIT("sub         $32, %[count]") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("4:") \
            /* 16x block */ \
            __ASM_EMIT("add         $16, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x020(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups     0x020(%[" A "], %[off]), %%ymm1") \
            __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
            __ASM_EMIT("vmulps      0x020(%[" C "], %[off]), %%ymm5, %%ymm5") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm5", "%%ymm1") ", "  SEL("%%ymm1", "%%ymm5") ", %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("6:") \
            /* 8x block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%ymm0") \
            __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%ymm4, %%ymm4") \
            __ASM_EMIT(OP "ps       " SEL("%%ymm4", "%%ymm0") ", "  SEL("%%ymm0", "%%ymm4") ", %%ymm0") \
            __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("8:") \
            /* 4x block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT("vmovups     0x000(%[" B "], %[off]), %%xmm4") \
            __ASM_EMIT("vmovups     0x000(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmulps      0x000(%[" C "], %[off]), %%xmm4, %%xmm4") \
            __ASM_EMIT(OP "ps       " SEL("%%xmm4", "%%xmm0") ", "  SEL("%%xmm0", "%%xmm4") ", %%xmm0") \
            __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("10:") \
            /* 1x blocks */ \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          12f") \
            __ASM_EMIT("11:") \
            __ASM_EMIT("vmovss      0x000(%[" B "], %[off]), %%xmm4") \
            __ASM_EMIT("vmovss      0x000(%[" A "], %[off]), %%xmm0") \
            __ASM_EMIT("vmulss      0x000(%[" C "], %[off]), %%xmm4, %%xmm4") \
            __ASM_EMIT(OP "ss       " SEL("%%xmm4", "%%xmm0") ", "  SEL("%%xmm0", "%%xmm4") ", %%xmm0") \
            __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x04, %[off]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         11b") \
            __ASM_EMIT("12:")

        void fmmul3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "dst", "a", "b", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmdiv3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "dst", "a", "b", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "dst", "a", "b", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "a", "b", "c", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "a", "b", "c", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOP_VV_CORE("dst", "a", "b", "c", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef FMOP_VV_CORE

        #define FMA_OFF(a, b)   a
        #define FMA_ON(a, b)    b

        #define FMOD_VV_CORE(DST, SRC1, SRC2, SRC3, OP_SEL) \
            __ASM_EMIT("xor                 %[off], %[off]") \
            /* x32 blocks */ \
            __ASM_EMIT32("subl              $32, %[count]") \
            __ASM_EMIT64("sub               $32, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%zmm0") \
            __ASM_EMIT("vmovups             0x40(%[" SRC3 "], %[off]), %%zmm1") \
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
            __ASM_EMIT32("subl              $32, %[count]") \
            __ASM_EMIT64("sub               $32, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("2:") \
            /* x16 blocks */ \
            __ASM_EMIT32("addl              $16, %[count]") \
            __ASM_EMIT64("add               $16, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%ymm0") \
            __ASM_EMIT("vmovups             0x20(%[" SRC3 "], %[off]), %%ymm1") \
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
            __ASM_EMIT32("subl              $16, %[count]") \
            __ASM_EMIT64("sub               $16, %[count]") \
            __ASM_EMIT("add                 $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /* x8 block */ \
            __ASM_EMIT32("addl              $8, %[count]") \
            __ASM_EMIT64("add               $8, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups             0x10(%[" SRC3 "], %[off]), %%xmm1") \
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
            __ASM_EMIT32("subl              $8, %[count]") \
            __ASM_EMIT64("sub               $8, %[count]") \
            __ASM_EMIT("add                 $0x20, %[off]") \
            __ASM_EMIT("6:") \
            /* x4 block */ \
            __ASM_EMIT32("addl              $4, %[count]") \
            __ASM_EMIT64("add               $4, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC3 "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovups             0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
            __ASM_EMIT("vmulps              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
            __ASM_EMIT("vdivps              %%xmm6, %%xmm2, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd231ps        %%xmm6, %%xmm4, %%xmm2") \
            __ASM_EMIT("vmovups             %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT32("subl              $4, %[count]") \
            __ASM_EMIT64("sub               $4, %[count]") \
            __ASM_EMIT("add                 $0x10, %[off]") \
            __ASM_EMIT("8:") \
            /* x1 blocks */ \
            __ASM_EMIT32("addl              $3, %[count]") \
            __ASM_EMIT64("add               $3, %[count]") \
            __ASM_EMIT("jl                  10f") \
            __ASM_EMIT("9:") \
            __ASM_EMIT("vmovss              0x00(%[" SRC3 "], %[off]), %%xmm0") \
            __ASM_EMIT("vmovss              0x00(%[" SRC1 "], %[off]), " OP_SEL("%%xmm2", "%%xmm6")) \
            __ASM_EMIT("vmulss              0x00(%[" SRC2 "], %[off]), %%xmm0, " OP_SEL("%%xmm6", "%%xmm2")) \
            __ASM_EMIT("vdivss              %%xmm6, %%xmm2, %%xmm4") \
            __ASM_EMIT("vcvttps2dq          %%xmm4, %%xmm4") \
            __ASM_EMIT("vcvtdq2ps           %%xmm4, %%xmm4") \
            __ASM_EMIT("vfnmadd231ss        %%xmm6, %%xmm4, %%xmm2") \
            __ASM_EMIT("vmovss              %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x04, %[off]") \
            __ASM_EMIT32("decl              %[count]") \
            __ASM_EMIT64("dec               %[count]") \
            __ASM_EMIT("jge                 9b") \
            __ASM_EMIT("10:")

        void fmmod3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "dst", "a", "b", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrmod3(float *dst, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "dst", "a", "b", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "a", "b", "c", OP_DSEL)
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                FMOD_VV_CORE("dst", "a", "b", "c", OP_RSEL)
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r"(dst), [a] "r" (a), [b] "r" (b), [c] "r" (c)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


        #undef FMOD_VV_CORE

        #undef FMA_OFF
        #undef FMA_ON

        #undef OP_DSEL
        #undef OP_RSEL
    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_FMOP_VV_H_ */
