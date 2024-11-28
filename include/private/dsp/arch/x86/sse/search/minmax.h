/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_SEARCH_MINMAX_H_
#define PRIVATE_DSP_ARCH_X86_SSE_SEARCH_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
    #define SSE_MINMAX_CORE(op)    \
            float result; \
            \
            ARCH_X86_ASM \
            ( \
                /* Prepare */ \
                __ASM_EMIT("xorps       %%xmm0, %%xmm0") \
                __ASM_EMIT("test        %[count], %[count]") \
                __ASM_EMIT("jz          2000f") \
                \
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0") \
                \
                /* Loop 1: align source to 4x boundary */ \
                __ASM_EMIT("1:") \
                __ASM_EMIT("test        $0x0f, %[src]") \
                __ASM_EMIT("jz          2f") \
                __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
                __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
                __ASM_EMIT("add         $0x4, %[src]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         1b") \
                __ASM_EMIT("jmp         2000f") \
                \
                /* Prepare vectorization */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
                __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
                __ASM_EMIT("sub         $0x18, %[count]") \
                __ASM_EMIT("jb          4f") \
                /* x24 Blocks */ \
                __ASM_EMIT("3:") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4") \
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5") \
                __ASM_EMIT("movaps      0x40(%[src]), %%xmm6") \
                __ASM_EMIT("movaps      0x50(%[src]), %%xmm7") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm3, %%xmm1") \
                __ASM_EMIT(op "ps       %%xmm4, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm5, %%xmm1") \
                __ASM_EMIT(op "ps       %%xmm6, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm7, %%xmm1") \
                __ASM_EMIT("add         $0x60, %[src]") \
                __ASM_EMIT("sub         $0x18, %[count]") \
                __ASM_EMIT("jae         3b") \
                /* x16 block */ \
                __ASM_EMIT("4:") \
                __ASM_EMIT("add         $0x18, %[count]") \
                __ASM_EMIT("test        $0x10, %[count]") \
                __ASM_EMIT("jz          5f") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4") \
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm3, %%xmm1") \
                __ASM_EMIT(op "ps       %%xmm4, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm5, %%xmm1") \
                __ASM_EMIT("add         $0x40, %[src]") \
                /* x8 block */ \
                __ASM_EMIT("5:") \
                __ASM_EMIT("test        $0x08, %[count]") \
                __ASM_EMIT("jz          6f") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT(op "ps       %%xmm3, %%xmm1") \
                __ASM_EMIT("add         $0x20, %[src]") \
                /* x4 block */ \
                __ASM_EMIT("6:") \
                __ASM_EMIT("test        $0x04, %[count]") \
                __ASM_EMIT("jz          7f") \
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
                __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
                __ASM_EMIT("add         $0x10, %[src]") \
                /* x4 block */ \
                __ASM_EMIT("7:") \
                __ASM_EMIT(op "ps       %%xmm1, %%xmm0") \
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1") \
                __ASM_EMIT(op "ps       %%xmm1, %%xmm0") \
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0") \
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1") \
                __ASM_EMIT("and         $0x03, %[count]") \
                __ASM_EMIT(op "ss       %%xmm1, %%xmm0") \
                __ASM_EMIT("jz          2000f") \
                __ASM_EMIT("8:") \
                __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
                __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
                __ASM_EMIT("add         $0x4, %[src]") \
                __ASM_EMIT("dec         %[count]") \
                __ASM_EMIT("jnz         8b") \
                /* End */ \
                __ASM_EMIT("2000:") \
                \
                : [src] "+r" (src), [count] "+r" (count), "=Yz" (result) \
                : \
                : "cc", \
                    "%xmm1", "%xmm2", "%xmm3", \
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
            ); \
            \
            return result;

        float min(const float *src, size_t count)
        {
            SSE_MINMAX_CORE("min");
        }

        float max(const float *src, size_t count)
        {
            SSE_MINMAX_CORE("max");
        }

        #undef SSE_MINMAX_CORE

        void minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                /* Align */
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm2")
                __ASM_EMIT("1:")
                __ASM_EMIT("test        $0x0f, %[src]")
                __ASM_EMIT("jz          2f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm4")
                __ASM_EMIT("minss       %%xmm4, %%xmm0")
                __ASM_EMIT("maxss       %%xmm4, %%xmm2")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         1b")
                __ASM_EMIT("jmp         2000f")

                /* x16 Blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
                __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm5")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm6")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm7")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("minps       %%xmm6, %%xmm0")
                __ASM_EMIT("minps       %%xmm7, %%xmm1")
                __ASM_EMIT("maxps       %%xmm4, %%xmm2")
                __ASM_EMIT("maxps       %%xmm5, %%xmm3")
                __ASM_EMIT("maxps       %%xmm6, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("add         $0x40, %[src]")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jae         3b")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("test        $0x08, %[count]")
                __ASM_EMIT("jz          5f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm5")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("maxps       %%xmm4, %%xmm2")
                __ASM_EMIT("maxps       %%xmm5, %%xmm3")
                __ASM_EMIT("add         $0x20, %[src]")
                /* x4 block */
                __ASM_EMIT("5:")
                __ASM_EMIT("test        $0x04, %[count]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm4")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm4, %%xmm2")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("6:")
                /* x1 blocks */
                __ASM_EMIT("minps       %%xmm1, %%xmm0")
                __ASM_EMIT("maxps       %%xmm3, %%xmm2")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm2, %%xmm3")
                __ASM_EMIT("minps       %%xmm1, %%xmm0")
                __ASM_EMIT("maxps       %%xmm3, %%xmm2")
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm2, %%xmm3")
                __ASM_EMIT("minss       %%xmm1, %%xmm0")
                __ASM_EMIT("maxss       %%xmm3, %%xmm2")
                __ASM_EMIT("and         $0x03, %[count]")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm4")
                __ASM_EMIT("minss       %%xmm4, %%xmm0")
                __ASM_EMIT("maxss       %%xmm4, %%xmm2")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         7b")

                __ASM_EMIT("2000:")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[min])")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[max])")

                : [src] "+r" (src), [count] "+r" (count)
                : [min] "r" (min), [max] "r" (max)
                : "cc",
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #define SSE_MINMAX_ABS_CORE(op)    \
        float result; \
        \
        ARCH_X86_ASM \
        ( \
            /* Prepare */ \
            __ASM_EMIT("xorps       %%xmm0, %%xmm0") \
            __ASM_EMIT("test        %[count], %[count]") \
            __ASM_EMIT("jz          2000f") \
            \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm0") \
            __ASM_EMIT("movaps      %[mask], %%xmm7") \
            __ASM_EMIT("andps       %%xmm7, %%xmm0") \
            \
            /* Loop 1: align source to 4x boundary */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("test        $0x0f, %[src]") \
            __ASM_EMIT("jz          2f") \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
            __ASM_EMIT("andps       %%xmm7, %%xmm2") \
            __ASM_EMIT("add         $0x4, %[src]") \
            __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         1b") \
            __ASM_EMIT("jmp         2000f") \
            \
            /* Prepare vectorization */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0") \
            __ASM_EMIT("movaps      %%xmm7, %%xmm6") \
            __ASM_EMIT("movaps      %%xmm0, %%xmm1") \
            __ASM_EMIT("sub         $0x10, %[count]") \
            __ASM_EMIT("jb          4f") \
            /* x16 Blocks */ \
            __ASM_EMIT("3:") \
            __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
            __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
            __ASM_EMIT("movaps      0x20(%[src]), %%xmm4") \
            __ASM_EMIT("movaps      0x30(%[src]), %%xmm5") \
            __ASM_EMIT("andps       %%xmm6, %%xmm2") \
            __ASM_EMIT("andps       %%xmm7, %%xmm3") \
            __ASM_EMIT("andps       %%xmm6, %%xmm4") \
            __ASM_EMIT("andps       %%xmm7, %%xmm5") \
            __ASM_EMIT(op "ps       %%xmm3, %%xmm2") \
            __ASM_EMIT(op "ps       %%xmm5, %%xmm4") \
            __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
            __ASM_EMIT(op "ps       %%xmm4, %%xmm1") \
            __ASM_EMIT("add         $0x40, %[src]") \
            __ASM_EMIT("sub         $0x10, %[count]") \
            __ASM_EMIT("jae         3b") \
            /* x8 block */ \
            __ASM_EMIT("4:") \
            __ASM_EMIT("add         $0x10, %[count]") \
            __ASM_EMIT("test        $0x08, %[count]") \
            __ASM_EMIT("jz          5f") \
            __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
            __ASM_EMIT("movaps      0x10(%[src]), %%xmm3") \
            __ASM_EMIT("andps       %%xmm6, %%xmm2") \
            __ASM_EMIT("andps       %%xmm7, %%xmm3") \
            __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
            __ASM_EMIT(op "ps       %%xmm3, %%xmm1") \
            __ASM_EMIT("add         $0x20, %[src]") \
            /* x4 block */ \
            __ASM_EMIT("5:") \
            __ASM_EMIT("test        $0x04, %[count]") \
            __ASM_EMIT("jz          6f") \
            __ASM_EMIT("movaps      0x00(%[src]), %%xmm2") \
            __ASM_EMIT("andps       %%xmm6, %%xmm2") \
            __ASM_EMIT(op "ps       %%xmm2, %%xmm0") \
            __ASM_EMIT("add         $0x10, %[src]") \
            /* x4 block */ \
            __ASM_EMIT("6:") \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0") \
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1") \
            __ASM_EMIT(op "ps       %%xmm1, %%xmm0") \
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm0") \
            __ASM_EMIT("movhlps     %%xmm0, %%xmm1") \
            __ASM_EMIT("and         $0x03, %[count]") \
            __ASM_EMIT(op "ss       %%xmm1, %%xmm0") \
            __ASM_EMIT("jz          2000f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
            __ASM_EMIT("andps       %%xmm7, %%xmm2") \
            __ASM_EMIT("add         $0x4, %[src]") \
            __ASM_EMIT(op "ss       %%xmm2, %%xmm0") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jnz         7b") \
            /* End */ \
            __ASM_EMIT("2000:") \
            \
            : [src] "+r" (src), [count] "+r" (count), "=Yz" (result) \
            : [mask] "m" (minmax_const) \
            : "cc", \
                "%xmm1", "%xmm2", "%xmm3", \
                "%xmm4", "%xmm5", "%xmm6", "%xmm7" \
        ); \
        \
        return result;

    IF_ARCH_X86(
        static const uint32_t minmax_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(0x7fffffff)
        };
    );

        float abs_min(const float *src, size_t count)
        {
            SSE_MINMAX_ABS_CORE("min");
        }

        float abs_max(const float *src, size_t count)
        {
            SSE_MINMAX_ABS_CORE("max");
        }

        #undef SSE_MINMAX_ABS_CORE

        void abs_minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                __ASM_EMIT("movaps      %[mask], %%xmm7")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("andps       %%xmm7, %%xmm0")
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")

                /* Align */
                __ASM_EMIT("1:")
                __ASM_EMIT("test        $0x0f, %[src]")
                __ASM_EMIT("jz          2f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm2")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("minss       %%xmm2, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("maxss       %%xmm2, %%xmm1")
                __ASM_EMIT("jnz         1b")
                __ASM_EMIT("jmp         2000f")

                /* x16 Blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm7, %%xmm6")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("movaps      0x20(%[src]), %%xmm4")
                __ASM_EMIT("movaps      0x30(%[src]), %%xmm5")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm3")
                __ASM_EMIT("andps       %%xmm6, %%xmm4")
                __ASM_EMIT("andps       %%xmm7, %%xmm5")
                __ASM_EMIT("minps       %%xmm2, %%xmm0")
                __ASM_EMIT("maxps       %%xmm3, %%xmm1")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("minps       %%xmm3, %%xmm0")
                __ASM_EMIT("maxps       %%xmm2, %%xmm1")
                __ASM_EMIT("minps       %%xmm5, %%xmm0")
                __ASM_EMIT("maxps       %%xmm4, %%xmm1")
                __ASM_EMIT("add         $0x40, %[src]")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("jae         3b")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("test        $0x08, %[count]")
                __ASM_EMIT("jz          5f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("movaps      0x10(%[src]), %%xmm3")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm3")
                __ASM_EMIT("minps       %%xmm2, %%xmm0")
                __ASM_EMIT("maxps       %%xmm3, %%xmm1")
                __ASM_EMIT("minps       %%xmm3, %%xmm0")
                __ASM_EMIT("maxps       %%xmm2, %%xmm1")
                __ASM_EMIT("add         $0x20, %[src]")
                /* x4 block */
                __ASM_EMIT("5:")
                __ASM_EMIT("test        $0x04, %[count]")
                __ASM_EMIT("jz          6f")
                __ASM_EMIT("movaps      0x00(%[src]), %%xmm2")
                __ASM_EMIT("andps       %%xmm6, %%xmm2")
                __ASM_EMIT("minps       %%xmm2, %%xmm0")
                __ASM_EMIT("maxps       %%xmm2, %%xmm1")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("6:")
                /* x1 blocks */
                __ASM_EMIT("movhlps     %%xmm0, %%xmm2")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm3")
                __ASM_EMIT("minps       %%xmm2, %%xmm0")
                __ASM_EMIT("maxps       %%xmm3, %%xmm1")
                __ASM_EMIT("unpcklps    %%xmm2, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm0, %%xmm2")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm3")
                __ASM_EMIT("minss       %%xmm2, %%xmm0")
                __ASM_EMIT("maxss       %%xmm3, %%xmm1")
                __ASM_EMIT("and         $0x03, %[count]")
                __ASM_EMIT("jz          2000f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm2")
                __ASM_EMIT("andps       %%xmm7, %%xmm2")
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("minss       %%xmm2, %%xmm0")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("maxss       %%xmm2, %%xmm1")
                __ASM_EMIT("jnz         7b")

                __ASM_EMIT("2000:")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[min])")
                __ASM_EMIT("movss       %%xmm1, 0x00(%[max])")

                : [src] "+r" (src), [count] "+r" (count)
                : [min] "r" (min), [max] "r" (max), [mask] "m" (minmax_const)
                : "cc",
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


    #define SSE_MINMAX_SIGN_CORE(op)    \
        float result; \
        \
        ARCH_X86_ASM \
        ( \
            /* Prepare */ \
            __ASM_EMIT("xorps       %%xmm0, %%xmm0") \
            __ASM_EMIT("test        %[count], %[count]") \
            __ASM_EMIT("jz          4f") \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm0")          /* xmm0 = result */ \
            __ASM_EMIT("movaps      %[mask], %%xmm7")               /* xmm7 = mask   */ \
            __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = result */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm1")                /* xmm1 = result */ \
            __ASM_EMIT("andps       %%xmm7, %%xmm1")                /* xmm1 = abs(result) */ \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("jb          2f") \
            /* x4 block */ \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movups      0x00(%[src]), %%xmm2")          /* xmm2 = x */ \
            __ASM_EMIT("movaps      %%xmm2, %%xmm4")                /* xmm4 = x */ \
            __ASM_EMIT("andps       %%xmm7, %%xmm4")                /* xmm4 = abs(x) */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm3")                /* xmm3 = abs(x) */ \
            __ASM_EMIT("cmpps  " op ", %%xmm1, %%xmm4")             /* xmm4 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm0")                /* xmm2 = [abs(x) <=> abs (result)] & result */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")                /* xmm2 = [abs(x) <=> abs (result)] & abs(result) */ \
            __ASM_EMIT("andnps      %%xmm2, %%xmm4")                /* xmm4 = ![abs(x) <=> abs (result)] & x */ \
            __ASM_EMIT("andnps      %%xmm3, %%xmm5")                /* xmm5 = ![abs(x) <=> abs (result)] & abs(x) */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm0")                /* xmm0 = [abs(x) <=> abs (result)] ? result : x */ \
            __ASM_EMIT("orps        %%xmm5, %%xmm1")                /* xmm1 = [abs(x) <=> abs (result)] ? abs(result) : abs(x) */ \
            __ASM_EMIT("add         $0x10, %[src]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("jae         1b") \
            /* deduce to 1, step 1 */ \
            __ASM_EMIT("movhlps     %%xmm1, %%xmm3")                /* xmm3 = abs(x) */ \
            __ASM_EMIT("movhlps     %%xmm0, %%xmm2")                /* xmm2 = x */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm4")                /* xmm4 = abs(x) */ \
            __ASM_EMIT("cmpps  " op ", %%xmm1, %%xmm4")             /* xmm4 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm0")                /* xmm2 = [abs(x) <=> abs (result)] & result */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")                /* xmm2 = [abs(x) <=> abs (result)] & abs(result) */ \
            __ASM_EMIT("andnps      %%xmm2, %%xmm4")                /* xmm4 = ![abs(x) <=> abs (result)] & x */ \
            __ASM_EMIT("andnps      %%xmm3, %%xmm5")                /* xmm5 = ![abs(x) <=> abs (result)] & abs(x) */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm0")                /* xmm0 = [abs(x) <=> abs (result)] ? result : x */ \
            __ASM_EMIT("orps        %%xmm5, %%xmm1")                /* xmm1 = [abs(x) <=> abs (result)] ? abs(result) : abs(x) */ \
            /* deduce to 1, step 2 */ \
            __ASM_EMIT("unpcklps    %%xmm1, %%xmm1") \
            __ASM_EMIT("unpcklps    %%xmm0, %%xmm0") \
            __ASM_EMIT("movhlps     %%xmm1, %%xmm3")                /* xmm3 = abs(x) */ \
            __ASM_EMIT("movhlps     %%xmm0, %%xmm2")                /* xmm2 = x */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm4")                /* xmm4 = abs(x) */ \
            __ASM_EMIT("cmpps  " op ", %%xmm1, %%xmm4")             /* xmm4 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm0")                /* xmm2 = [abs(x) <=> abs (result)] & result */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")                /* xmm2 = [abs(x) <=> abs (result)] & abs(result) */ \
            __ASM_EMIT("andnps      %%xmm2, %%xmm4")                /* xmm4 = ![abs(x) <=> abs (result)] & x */ \
            __ASM_EMIT("andnps      %%xmm3, %%xmm5")                /* xmm5 = ![abs(x) <=> abs (result)] & abs(x) */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm0")                /* xmm0 = [abs(x) <=> abs (result)] ? result : x */ \
            __ASM_EMIT("orps        %%xmm5, %%xmm1")                /* xmm1 = [abs(x) <=> abs (result)] ? abs(result) : abs(x) */ \
            /* x1 block */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add         $3, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("3:") \
            __ASM_EMIT("movss       0x00(%[src]), %%xmm2") \
            __ASM_EMIT("movaps      %%xmm2, %%xmm4")                /* xmm4 = x */ \
            __ASM_EMIT("andps       %%xmm7, %%xmm4")                /* xmm4 = abs(x) */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm3")                /* xmm3 = abs(x) */ \
            __ASM_EMIT("cmpps  " op ", %%xmm1, %%xmm4")             /* xmm4 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm0")                /* xmm2 = [abs(x) <=> abs (result)] & result */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = [abs(x) <=> abs (result)] */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm1")                /* xmm2 = [abs(x) <=> abs (result)] & abs(result) */ \
            __ASM_EMIT("andnps      %%xmm2, %%xmm4")                /* xmm4 = ![abs(x) <=> abs (result)] & x */ \
            __ASM_EMIT("andnps      %%xmm3, %%xmm5")                /* xmm5 = ![abs(x) <=> abs (result)] & abs(x) */ \
            __ASM_EMIT("orps        %%xmm4, %%xmm0")                /* xmm0 = [abs(x) <=> abs (result)] ? result : x */ \
            __ASM_EMIT("orps        %%xmm5, %%xmm1")                /* xmm1 = [abs(x) <=> abs (result)] ? abs(result) : abs(x) */ \
            __ASM_EMIT("add         $0x4, %[src]") \
            __ASM_EMIT("dec         %[count]") \
            __ASM_EMIT("jge         3b") \
            /* End */ \
            __ASM_EMIT("4:") \
            \
            : [src] "+r" (src), [count] "+r" (count), "=Yz" (result) \
            : [mask] "m" (minmax_const) \
            : "cc", \
                "%xmm1", "%xmm2", "%xmm3", \
                "%xmm4", "%xmm5", "%xmm7" \
        ); \
        \
        return result;

        float sign_min(const float *src, size_t count)
        {
            SSE_MINMAX_SIGN_CORE("$6");
        }

        float sign_max(const float *src, size_t count)
        {
            SSE_MINMAX_SIGN_CORE("$1");
        }

        #undef SSE_MINMAX_ABS_CORE

        void sign_minmax(const float *src, size_t count, float *min, float *max)
        {
            ARCH_X86_ASM
            (
                /* Prepare */ \
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          4f")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")          /* xmm0 = min ? ? ? */
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = min */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                /* xmm2 = max */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = max */
                __ASM_EMIT("andps       %[mask], %%xmm1")               /* xmm1 = abs(min) */
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")                /* xmm3 = abs(max) */
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jb          2f")
                /* x4 Blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm4")          /* xmm4 = x */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = x */ \
                __ASM_EMIT("andps       %[mask], %%xmm5")               /* xmm5 = abs(x) */ \
                __ASM_EMIT("movaps      %%xmm1, %%xmm6")                /* xmm6 = abs(min) */ \
                __ASM_EMIT("cmpps       $2, %%xmm5, %%xmm6")            /* xmm6 = [abs(min) <= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] & min */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(min) <= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] & abs(min) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(min) <= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(min) <= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] ? min : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] ? abs(min) : abs(x) */
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")                /* xmm6 = abs(max) */ \
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm6")            /* xmm6 = [abs(max) >= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] & max */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(max) >= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] & abs(max) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(max) >= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(max) >= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] ? max : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] ? abs(max) : abs(x) */
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         1b")
                /* Reduce to 1, step 1 */
                __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                /* xmm5 = abs(x) */
                __ASM_EMIT("movhlps     %%xmm0, %%xmm4")                /* xmm4 = x */
                __ASM_EMIT("movaps      %%xmm1, %%xmm6")                /* xmm6 = abs(min) */ \
                __ASM_EMIT("cmpps       $2, %%xmm5, %%xmm6")            /* xmm6 = [abs(min) <= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] & min */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(min) <= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] & abs(min) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(min) <= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(min) <= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] ? min : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] ? abs(min) : abs(x) */
                __ASM_EMIT("movhlps     %%xmm3, %%xmm5")                /* xmm5 = abs(x) */
                __ASM_EMIT("movhlps     %%xmm2, %%xmm4")                /* xmm4 = x */
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")                /* xmm6 = abs(max) */ \
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm6")            /* xmm6 = [abs(max) >= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] & max */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(max) >= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] & abs(max) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(max) >= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(max) >= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] ? max : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] ? abs(max) : abs(x) */

                __ASM_EMIT("unpcklps    %%xmm0, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm1")
                __ASM_EMIT("unpcklps    %%xmm2, %%xmm2")
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm3")

                __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                /* xmm5 = abs(x) */
                __ASM_EMIT("movhlps     %%xmm0, %%xmm4")                /* xmm4 = x */
                __ASM_EMIT("movaps      %%xmm1, %%xmm6")                /* xmm6 = abs(min) */ \
                __ASM_EMIT("cmpps       $2, %%xmm5, %%xmm6")            /* xmm6 = [abs(min) <= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] & min */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(min) <= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] & abs(min) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(min) <= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(min) <= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] ? min : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] ? abs(min) : abs(x) */
                __ASM_EMIT("movhlps     %%xmm3, %%xmm5")                /* xmm5 = abs(x) */
                __ASM_EMIT("movhlps     %%xmm2, %%xmm4")                /* xmm4 = x */
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")                /* xmm6 = abs(max) */ \
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm6")            /* xmm6 = [abs(max) >= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] & max */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(max) >= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] & abs(max) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(max) >= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(max) >= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] ? max : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] ? abs(max) : abs(x) */
                /* x1 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm4")          /* xmm4 = x */ \
                __ASM_EMIT("movaps      %%xmm4, %%xmm5")                /* xmm5 = x */ \
                __ASM_EMIT("andps       %[mask], %%xmm5")               /* xmm5 = abs(x) */ \
                __ASM_EMIT("movaps      %%xmm1, %%xmm6")                /* xmm6 = abs(min) */ \
                __ASM_EMIT("cmpps       $2, %%xmm5, %%xmm6")            /* xmm6 = [abs(min) <= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] & min */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(min) <= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] & abs(min) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(min) <= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(min) <= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm0")                /* xmm0 = [abs(min) <= abs(x)] ? min : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm1")                /* xmm1 = [abs(min) <= abs(x)] ? abs(min) : abs(x) */
                __ASM_EMIT("movaps      %%xmm3, %%xmm6")                /* xmm6 = abs(max) */ \
                __ASM_EMIT("cmpps       $5, %%xmm5, %%xmm6")            /* xmm6 = [abs(max) >= abs(x)] */ \
                __ASM_EMIT("andps       %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] & max */
                __ASM_EMIT("movaps      %%xmm6, %%xmm7")                /* xmm7 = [abs(max) >= abs(x)] */
                __ASM_EMIT("andps       %%xmm6, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] & abs(max) */
                __ASM_EMIT("andnps      %%xmm4, %%xmm6")                /* xmm6 = ![abs(max) >= abs(x)] & x */
                __ASM_EMIT("andnps      %%xmm5, %%xmm7")                /* xmm7 = ![abs(max) >= abs(x)] & abs(x) */
                __ASM_EMIT("orps        %%xmm6, %%xmm2")                /* xmm2 = [abs(max) >= abs(x)] ? max : x */
                __ASM_EMIT("orps        %%xmm7, %%xmm3")                /* xmm3 = [abs(max) >= abs(x)] ? abs(max) : abs(x) */
                __ASM_EMIT("add         $0x04, %[src]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         3b")
                /* end */
                __ASM_EMIT("4:")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[min])")
                __ASM_EMIT("movss       %%xmm1, 0x00(%[max])")

                : [src] "+r" (src), [count] "+r" (count)
                : [min] "r" (min), [max] "r" (max), [mask] "m" (minmax_const)
                : "cc",
                    "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                    "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE_SEARCH_MINMAX_H_ */
