/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 мар. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {

        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")    /* xv = 0 */
                __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")    /* xa = 0 */
                __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")    /* xb = 0 */
                /* 16x blocks */
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")    /* xv = 0 */
                __ASM_EMIT("eor         v4.16b, v4.16b, v4.16b")    /* xa = 0 */
                __ASM_EMIT("eor         v5.16b, v5.16b, v5.16b")    /* xb = 0 */
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q8, q9, [%[a], 0x00]")      /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldp         q10, q11, [%[a], 0x20]")    /* v10 = a2, v11 = a3 */
                __ASM_EMIT("ldp         q12, q13, [%[b], 0x00]")    /* v12 = b0, v13 = b1 */
                __ASM_EMIT("ldp         q14, q15, [%[b], 0x20]")    /* v14 = b2, v15 = b3 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v3.4s, v9.4s, v13.4s")      /* v3 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v4.4s, v9.4s, v9.4s")       /* v4 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v5.4s, v13.4s, v13.4s")     /* v5 = xb + b1*b1 */
                __ASM_EMIT("fmla        v0.4s, v10.4s, v14.4s")     /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v3.4s, v11.4s, v15.4s")     /* v3 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v10.4s, v10.4s")     /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v4.4s, v11.4s, v11.4s")     /* v4 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v14.4s, v14.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v5.4s, v15.4s, v15.4s")     /* v5 = xb + b1*b1 */
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("add         %[a], %[a], #0x40")
                __ASM_EMIT("add         %[b], %[b], #0x40")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q8, q9, [%[a], 0x00]")      /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldp         q12, q13, [%[b], 0x00]")    /* v12 = b0, v13 = b1 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v0.4s, v9.4s, v13.4s")      /* v0 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v9.4s, v9.4s")       /* v1 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v13.4s, v13.4s")     /* v2 = xb + b1*b1 */
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[a], %[a], #0x20")
                __ASM_EMIT("add         %[b], %[b], #0x20")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldr         q8, [%[a], 0x00]")          /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldr         q12, [%[b], 0x00]")         /* v12 = b0, v13 = b1 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[a], %[a], #0x10")
                __ASM_EMIT("add         %[b], %[b], #0x10")
                __ASM_EMIT("6:")
                __ASM_EMIT("eor         v6.16b, v6.16b, v6.16b")    /* v6 = 0 */
                __ASM_EMIT("ext         v3.16b, v0.16b, v6.16b, #8")/* v3 = xv2 xv3 0 0 */
                __ASM_EMIT("ext         v4.16b, v1.16b, v6.16b, #8")/* v4 = xa2 xb3 0 0 */
                __ASM_EMIT("ext         v5.16b, v2.16b, v6.16b, #8")/* v5 = xa2 xb3 0 0 */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")       /* v0 = xv0+xv2 xv1+xv3 xv2 xv3 */
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")       /* v1 = xa0+xa2 xa1+xa3 xa2 xa3 */
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")       /* v2 = xb0+xb2 xb1+xb3 xb2 xb3 */
                __ASM_EMIT("ext         v3.16b, v0.16b, v6.16b, #4")/* v3 = xv1+xv3 xv2 xv3 0 */
                __ASM_EMIT("ext         v4.16b, v1.16b, v6.16b, #4")/* v4 = xa1+xa3 xa2 xa3 0 */
                __ASM_EMIT("ext         v5.16b, v2.16b, v6.16b, #4")/* v5 = xb1+xb3 xb2 xb3 0 */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")       /* v0 = xv0+xv1+xv2+xv3 xv1+xv2+xv3 xv2+xv3 xv3 */
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")       /* v1 = xa0+xa1+xa2+xa3 xa1+xa2+xa3 xa2+xa3 xa3 */
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")       /* v2 = xb0+xb1+xb2+xb3 xb1+xb2+xb3 xb2+xb3 xb3 */
                /* 1x blocks */
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r        {v8.4s}, [%[a]]")           /* v8 = a0 */
                __ASM_EMIT("ld1r        {v12.4s}, [%[b]]")          /* q12 = b0 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[a], %[a], #0x04")
                __ASM_EMIT("add         %[b], %[b], #0x04")
                __ASM_EMIT("b.ge        7b")
                __ASM_EMIT("8:")
                /* Store result */
                __ASM_EMIT("ld3r        {v3.4s, v4.4s, v5.4s}, [%[corr]]")   /* v3 = v, v4 = a, v5 = b */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")
                __ASM_EMIT("st3         {v0.s, v1.s, v2.s}[0], [%[corr]]")

                : [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
                : [corr] "r" (corr)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_ */
