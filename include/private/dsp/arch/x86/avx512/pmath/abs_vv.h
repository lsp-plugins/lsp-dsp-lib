/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PMATH_ABS_VV_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PMATH_ABS_VV_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t abs_vv_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(0x7fffffff)
            };
        )

    #define OP_DSEL(a, b)   a
    #define OP_RSEL(a, b)   b

    #define ABS_OP_CORE(DST, SRC1, SRC2, OP, SEL) \
        __ASM_EMIT("vmovaps     %[SIGN], %%zmm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vmovaps     %%zmm6, %%zmm7") \
        /* 48x blocks */ \
        __ASM_EMIT("sub         $48, %[count]") \
        __ASM_EMIT("jb          2f")    \
        __ASM_EMIT("1:") \
        __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%zmm0", "")) \
        __ASM_EMIT(SEL("vmovups     0x040(%[" SRC1 "], %[off]), %%zmm1", "")) \
        __ASM_EMIT(SEL("vmovups     0x080(%[" SRC1 "], %[off]), %%zmm2", "")) \
        __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%zmm6, %%zmm3") \
        __ASM_EMIT("vandps      0x040(%[" SRC2 "], %[off]), %%zmm7, %%zmm4") \
        __ASM_EMIT("vandps      0x080(%[" SRC2 "], %[off]), %%zmm6, %%zmm5") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm3", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%zmm0", "%%zmm3") ", %%zmm0") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm4", "0x040(%[" SRC1 "], %[off])") ", "  SEL("%%zmm1", "%%zmm4") ", %%zmm1") \
        __ASM_EMIT(OP "ps       " SEL("%%zmm5", "0x080(%[" SRC1 "], %[off])") ", "  SEL("%%zmm2", "%%zmm5") ", %%zmm2") \
        __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm2, 0x080(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0xc0, %[off]") \
        __ASM_EMIT("sub         $48, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 16x blocks */ \
        __ASM_EMIT("add         $32, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%ymm0", "")) \
        __ASM_EMIT(SEL("vmovups     0x020(%[" SRC1 "], %[off]), %%ymm1", "")) \
        __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%ymm6, %%ymm2") \
        __ASM_EMIT("vandps      0x020(%[" SRC2 "], %[off]), %%ymm7, %%ymm3") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%ymm0", "%%ymm2") ", %%ymm0") \
        __ASM_EMIT(OP "ps       " SEL("%%ymm3", "0x020(%[" SRC1 "], %[off])") ", "  SEL("%%ymm1", "%%ymm3") ", %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm0", "")) \
        __ASM_EMIT(SEL("vmovups     0x010(%[" SRC1 "], %[off]), %%xmm1", "")) \
        __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%xmm6, %%xmm2") \
        __ASM_EMIT("vandps      0x010(%[" SRC2 "], %[off]), %%xmm7, %%xmm3") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%xmm0", "%%xmm2") ", %%xmm0") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm3", "0x010(%[" SRC1 "], %[off])") ", "  SEL("%%xmm1", "%%xmm3") ", %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x010(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("6:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT(SEL("vmovups     0x000(%[" SRC1 "], %[off]), %%xmm0", "")) \
        __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%xmm6, %%xmm2") \
        __ASM_EMIT(OP "ps       " SEL("%%xmm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%xmm0", "%%xmm2") ", %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("8:") \
        /* 1x blocks */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("9:") \
        __ASM_EMIT(SEL("vmovss     0x000(%[" SRC1 "], %[off]), %%xmm0", "")) \
        __ASM_EMIT("vandps      0x000(%[" SRC2 "], %[off]), %%xmm6, %%xmm2") \
        __ASM_EMIT(OP "ss       " SEL("%%xmm2", "0x000(%[" SRC1 "], %[off])") ", "  SEL("%%xmm0", "%%xmm2") ", %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         9b") \
        __ASM_EMIT("10:")

        void abs_add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vadd", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vsub", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vsub", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_max2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vmax", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_min2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "dst", "src", "vmin", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src] "r"(src),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vadd", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vsub", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_rsub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vsub", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vmul", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vdiv", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vdiv", OP_RSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_max3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vmax", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs_min3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_OP_CORE("dst", "src1", "src2", "vmin", OP_DSEL)
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r"(dst), [src1] "r" (src1), [src2] "r" (src2),
                  [SIGN] "m" (abs_vv_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef ABS_OP_CORE

    #define ABS_CORE(DST, SRC)  \
        __ASM_EMIT("vmovaps     %[SIGN], %%zmm6") \
        __ASM_EMIT("xor         %[off], %[off]") \
        __ASM_EMIT("vmovaps     %%zmm6, %%zmm7") \
        /* 96x blocks */ \
        __ASM_EMIT("sub         $96, %[count]") \
        __ASM_EMIT("jb          2f")    \
        __ASM_EMIT("1:") \
        __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%zmm6, %%zmm0") \
        __ASM_EMIT("vandps      0x040(%[" SRC "], %[off]), %%zmm7, %%zmm1") \
        __ASM_EMIT("vandps      0x080(%[" SRC "], %[off]), %%zmm6, %%zmm2") \
        __ASM_EMIT("vandps      0x0c0(%[" SRC "], %[off]), %%zmm7, %%zmm3") \
        __ASM_EMIT("vandps      0x100(%[" SRC "], %[off]), %%zmm6, %%zmm4") \
        __ASM_EMIT("vandps      0x140(%[" SRC "], %[off]), %%zmm7, %%zmm5") \
        __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm2, 0x080(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm3, 0x0c0(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm4, 0x100(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm5, 0x140(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x180, %[off]") \
        __ASM_EMIT("sub         $96, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* 32x blocks */ \
        __ASM_EMIT("add         $64, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%zmm6, %%zmm0") \
        __ASM_EMIT("vandps      0x040(%[" SRC "], %[off]), %%zmm7, %%zmm1") \
        __ASM_EMIT("vmovups     %%zmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%zmm1, 0x040(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x80, %[off]") \
        __ASM_EMIT("sub         $32, %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:") \
        /* 16x block */ \
        __ASM_EMIT("add         $16, %[count]") \
        __ASM_EMIT("jl          6f") \
        __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%ymm6, %%ymm0") \
        __ASM_EMIT("vandps      0x020(%[" SRC "], %[off]), %%ymm7, %%ymm1") \
        __ASM_EMIT("vmovups     %%ymm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%ymm1, 0x020(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $16, %[count]") \
        __ASM_EMIT("add         $0x40, %[off]") \
        __ASM_EMIT("6:") \
        /* 8x block */ \
        __ASM_EMIT("add         $8, %[count]") \
        __ASM_EMIT("jl          8f") \
        __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vandps      0x010(%[" SRC "], %[off]), %%xmm7, %%xmm1") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("vmovups     %%xmm1, 0x010(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $8, %[count]") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("8:") \
        /* 4x block */ \
        __ASM_EMIT("add         $4, %[count]") \
        __ASM_EMIT("jl          10f") \
        __ASM_EMIT("vandps      0x000(%[" SRC "], %[off]), %%xmm6, %%xmm0") \
        __ASM_EMIT("vmovups     %%xmm0, 0x000(%[" DST "], %[off])") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("add         $0x10, %[off]") \
        __ASM_EMIT("10:") \
        /* 1x block */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          12f") \
        __ASM_EMIT("11:") \
        __ASM_EMIT("vmovss      0x00(%[" SRC "], %[off]), %%xmm0") \
        __ASM_EMIT("vandps      %%xmm0, %%xmm6, %%xmm0") \
        __ASM_EMIT("vmovss      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x04, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         11b") \
        __ASM_EMIT("12:")

        void abs1(float *dst, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_CORE("dst", "dst")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [SIGN] "m" (abs_vv_const)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void abs2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);
            ARCH_X86_64_ASM
            (
                ABS_CORE("dst", "src")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [SIGN] "m" (abs_vv_const)
                : "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef ABS_CORE

    #undef OP_DSEL
    #undef OP_RSEL

    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PMATH_ABS_VV_H_ */
