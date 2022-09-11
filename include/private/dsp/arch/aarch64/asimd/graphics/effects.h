/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 сент. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_EFFECTS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_EFFECTS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const float EFF_HSLA_HUE_XC[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0.0f),
                LSP_DSP_VEC4(1.0f)
            };
        )

    #define EFF_HSLA_HUE_CORE   \
        /* v0   = v[0]      */ \
        /* v1   = v[1]      */ \
        /* v8   = h         */ \
        /* v9   = s         */ \
        /* v10  = l         */ \
        /* v11  = a         */ \
        /* v12  = T         */ \
        /* v13  = KT        */ \
        /* v14  = 0         */ \
        /* v15  = 1         */ \
        __ASM_EMIT("mov             v4.16b, v1.16b")                /* v4   = v[1] */ \
        __ASM_EMIT("fsub            v2.4s, v15.4s, v0.4s")          /* v2   = 1 - v */ \
        __ASM_EMIT("fsub            v6.4s, v15.4s, v4.4s") \
        __ASM_EMIT("fcmge           v3.4s, v14.4s, v0.4s")          /* v3   = 0 > v  */ \
        __ASM_EMIT("fcmge           v7.4s, v14.4s, v4.4s") \
        __ASM_EMIT("fadd            v0.4s, v0.4s, v15.4s")          /* v0   = v + 1 */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, v15.4s") \
        __ASM_EMIT("bif             v0.16b, v2.16b, v3.16b")        /* v0   = V = (v+1)&[0>v] | (1-v)&[0<=v] */ \
        __ASM_EMIT("bif             v4.16b, v6.16b, v7.16b") \
        __ASM_EMIT("fsub            v3.4s, v0.4s, v12.4s")          /* v3   = V - T */ \
        __ASM_EMIT("fsub            v7.4s, v4.4s, v12.4s") \
        __ASM_EMIT("fcmge           v1.4s, v3.4s, v14.4s")          /* v1   = [(V-T)>=0] */ \
        __ASM_EMIT("fcmge           v5.4s, v7.4s, v14.4s") \
        __ASM_EMIT("fmul            v3.4s, v3.4s, v13.4s")          /* v3   = (V-T)*KT */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v13.4s") \
        __ASM_EMIT("bit             v0.16b, v12.16b, v1.16b")       /* v0   = EH = T&[(V-T)>=0] | V&[(V-T)<0] */ \
        __ASM_EMIT("bit             v4.16b, v12.16b, v5.16b") \
        __ASM_EMIT("and             v3.16b, v3.16b, v1.16b")        /* v3   = A = ((V-T)*KT) & [(V-T)>=0] */ \
        __ASM_EMIT("and             v7.16b, v7.16b, v5.16b") \
        __ASM_EMIT("fadd            v0.4s, v0.4s, v8.4s")           /* v0   = H = h + T&[(V-T)>=0] | V&[(V-T)<0] */ \
        __ASM_EMIT("fadd            v4.4s, v4.4s, v8.4s") \
        __ASM_EMIT("mov             v1.16b, v9.16b")                /* v1   = s */ \
        __ASM_EMIT("mov             v2.16b, v10.16b")               /* v2   = l */ \
        __ASM_EMIT("mov             v5.16b, v9.16b") \
        __ASM_EMIT("mov             v6.16b, v10.16b")

        /*
            float t     = 1.0f - eff->thresh;
            float kt    = 1.0f / eff->thresh;

            value   = v[i];
            value   = (0 > value) ? 1.0f + value : 1.0f - value;

            if ((value - t) >= 0)
            {
                hue         = eff->h + t;
                alpha       = ((value - t) * kt);
            }
            else
            {
                hue         = eff->h + value;
                alpha       = 0.0f;
            }

            dst[0]      = (hue < 1.0f) ? hue : hue - 1.0f;
            dst[1]      = eff->s;
            dst[2]      = eff->l;
            dst[3]      = alpha;
        */

        void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld4r            {v8.4s, v9.4s, v10.4s, v11.4s}, [%[eff]]")  /* v8   = h, v9 = s, v10 = l, v11 = a */
                __ASM_EMIT("add             %[eff], %[eff], #0x10")
                __ASM_EMIT("ldp             q14, q15, [%[XC]]")                         /* v14  = 0.0, v15 = 1.0 */
                __ASM_EMIT("ld1r            {v12.4s}, [%[eff]]")                        /* v12  = t */
                __ASM_EMIT("frecpe          v0.4s, v12.4s")                             /* v0   = TD */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      /* v1   = (2 - TD*T) */
                __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       /* v0   = t' = TD * (2 - TD*T) */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v12.4s")                      /* v1   = (2 - TD*t') */
                __ASM_EMIT("fsub            v12.4s, v15.4s, v12.4s")                    /* v12  = T = 1 - t */
                __ASM_EMIT("fmul            v13.4s, v1.4s, v0.4s")                      /* v13  = KT = 1/t = t' * (2 - TD*t') */

                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")          /* v8 = v[0], v9 = v[1] */
                EFF_HSLA_HUE_CORE
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("st4             {v4.4s, v5.4s, v6.4s, v7.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("b.hs            1b")

                //-----------------------------------------------------------------
                // 1x-8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v1.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x08")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v1.s}[2], [%[src]]")
                __ASM_EMIT("8:")
                EFF_HSLA_HUE_CORE
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st4             {v4.2s, v5.2s, v6.2s, v7.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st4             {v4.s, v5.s, v6.s, v7.s}[2], [%[dst]]")
                // End
                __ASM_EMIT("14:")


                : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
                  [eff] "+r" (eff)
                : [XC] "r" (&EFF_HSLA_HUE_XC[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

    #undef EFF_HSLA_HUE_CORE

    #define EFF_HSLA_SAT_CORE   \
        /* v0   = v[0]      */ \
        /* v1   = v[1]      */ \
        /* v8   = h         */ \
        /* v9   = s         */ \
        /* v10  = l         */ \
        /* v11  = a         */ \
        /* v14  = T         */ \
        /* v15  = KT        */ \
        __ASM_EMIT("fabs            v5.4s, v1.4s")              /* v5   = V1 = abs(v1) */ \
        __ASM_EMIT("fabs            v1.4s, v0.4s")              /* v1   = V  = abs(v) */ \
        __ASM_EMIT("fsub            v3.4s, v14.4s, v1.4s")      /* v3   = T - V */ \
        __ASM_EMIT("fsub            v7.4s, v14.4s, v5.4s") \
        __ASM_EMIT("fcmgt           v2.4s, v3.4s, #0.0")        /* v2   = [(T-V) > 0] */ \
        __ASM_EMIT("fcmgt           v6.4s, v7.4s, #0.0") \
        __ASM_EMIT("fmul            v3.4s, v3.4s, v15.4s")      /* v3   = (T-V)*KT */ \
        __ASM_EMIT("fmul            v7.4s, v7.4s, v15.4s") \
        __ASM_EMIT("bit             v1.16b, v14.16b, v2.16b")   /* v1   = ES = V&[(T-V) <= 0] | T&[(T-V) > 0] */ \
        __ASM_EMIT("bit             v5.16b, v14.16b, v6.16b") \
        __ASM_EMIT("and             v3.16b, v3.16b, v2.16b")    /* v3   = A = ((T-V)*KT) & [(T-V) > 0] */ \
        __ASM_EMIT("and             v7.16b, v7.16b, v6.16b") \
        __ASM_EMIT("fmul            v1.4s, v1.4s, v9.4s")       /* v1   = ES*s = S */ \
        __ASM_EMIT("fmul            v5.4s, v5.4s, v9.4s") \
        __ASM_EMIT("mov             v0.16b, v8.16b") \
        __ASM_EMIT("mov             v2.16b, v10.16b") \
        __ASM_EMIT("mov             v4.16b, v8.16b") \
        __ASM_EMIT("mov             v6.16b, v10.16b")

        /*
            kt      = 1.0f / eff->thresh;
            value   = (value >= 0.0f) ? value : -value;

            if ((eff->thresh - value) <= 0)
            {
                dst[1]      = eff->s * value;
                dst[3]      = 0.0f;
            }
            else
            {
                dst[1]      = eff->s * eff->thresh;
                dst[3]      = (eff->thresh - value) * kt;
            }

            dst[0]      = eff->h;
            dst[2]      = eff->l;
         */

        void eff_hsla_sat(float *dst, const float *v, const dsp::hsla_sat_eff_t *eff, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld4r            {v8.4s, v9.4s, v10.4s, v11.4s}, [%[eff]]")  /* v8 = h, v9 = s, v10 = l, v11 = a */
                __ASM_EMIT("add             %[eff], %[eff], #0x10")
                __ASM_EMIT("ld1r            {v14.4s}, [%[eff]]")                        /* v14  = T */
                __ASM_EMIT("frecpe          v0.4s, v14.4s")                             /* v0   = TD */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v14.4s")                      /* v1   = (2 - TD*T) */
                __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       /* v0   = t' = TD * (2 - TD*T) */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v14.4s")                      /* v1   = (2 - TD*t') */
                __ASM_EMIT("fmul            v15.4s, v1.4s, v0.4s")                      /* v15  = KT = 1/t = t' * (2 - TD*t') */

                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")          /* v8 = v[0], v9 = v[1] */
                EFF_HSLA_SAT_CORE
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("st4             {v4.4s, v5.4s, v6.4s, v7.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("b.hs            1b")

                //-----------------------------------------------------------------
                // 1x-8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v1.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x08")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v1.s}[2], [%[src]]")
                __ASM_EMIT("8:")
                EFF_HSLA_SAT_CORE
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st4             {v4.2s, v5.2s, v6.2s, v7.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st4             {v4.s, v5.s, v6.s, v7.s}[2], [%[dst]]")
                // End
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
                  [eff] "+r" (eff)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

        #define EFF_HSLA_LIGHT_CORE   \
            /* v0   = v[0]      */ \
            /* v1   = v[1]      */ \
            /* v8   = h         */ \
            /* v9   = s         */ \
            /* v10  = l         */ \
            /* v11  = a         */ \
            /* v14  = T         */ \
            /* v15  = KT        */ \
            __ASM_EMIT("fabs            v5.4s, v1.4s")                /* v5   = V1 = abs(v1) */ \
            __ASM_EMIT("fabs            v1.4s, v0.4s")                /* v1   = V  = abs(v) */ \
            __ASM_EMIT("fsub            v3.4s, v14.4s, v1.4s")        /* v3   = T - V */ \
            __ASM_EMIT("fsub            v7.4s, v14.4s, v5.4s") \
            __ASM_EMIT("fcmgt           v2.4s, v3.4s, #0.0")          /* v2   = [(T-V) > 0] */ \
            __ASM_EMIT("fcmgt           v6.4s, v7.4s, #0.0") \
            __ASM_EMIT("fmul            v3.4s, v3.4s, v15.4s")        /* v3   = (T-V)*KT */ \
            __ASM_EMIT("fmul            v7.4s, v7.4s, v15.4s") \
            __ASM_EMIT("bit             v1.16b, v14.16b, v2.16b")     /* v1   = EL = V&[(T-V) <= 0] | T&[(T-V) > 0] */ \
            __ASM_EMIT("bit             v5.16b, v14.16b, v6.16b") \
            __ASM_EMIT("and             v3.16b, v3.16b, v2.16b")      /* v3   = A = ((T-V)*KT) & [(T-V) > 0] */ \
            __ASM_EMIT("and             v7.16b, v7.16b, v6.16b") \
            __ASM_EMIT("fmul            v2.4s, v1.4s, v10.4s")        /* v2   = EL*l = L */ \
            __ASM_EMIT("fmul            v6.4s, v5.4s, v10.4s") \
            __ASM_EMIT("mov             v0.16b, v8.16b") \
            __ASM_EMIT("mov             v1.16b, v9.16b") \
            __ASM_EMIT("mov             v4.16b, v8.16b") \
            __ASM_EMIT("mov             v5.16b, v9.16b")

            /*
                kt      = 1.0f / eff->thresh;
                value   = (value >= 0.0f) ? value : -value;

                if ((eff->thresh - value) <= 0)
                {
                    dst[2]      = eff->l * value;
                    dst[3]      = 0.0f;
                }
                else
                {
                    dst[2]      = eff->l * eff->thresh;
                    dst[3]      = (eff->thresh - value) * kt;
                }

                dst[0]      = eff->h;
                dst[1]      = eff->s;
             */

        void eff_hsla_light(float *dst, const float *v, const dsp::hsla_light_eff_t *eff, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld4r            {v8.4s, v9.4s, v10.4s, v11.4s}, [%[eff]]")  /* v8 = h, v9 = s, v10 = l, v11 = a */
                __ASM_EMIT("add             %[eff], %[eff], #0x10")
                __ASM_EMIT("ld1r            {v14.4s}, [%[eff]]")                        /* v14  = T */
                __ASM_EMIT("frecpe          v0.4s, v14.4s")                             /* v0   = TD */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v14.4s")                      /* v1   = (2 - TD*T) */
                __ASM_EMIT("fmul            v0.4s, v1.4s, v0.4s")                       /* v0   = t' = TD * (2 - TD*T) */
                __ASM_EMIT("frecps          v1.4s, v0.4s, v14.4s")                      /* v1   = (2 - TD*t') */
                __ASM_EMIT("fmul            v15.4s, v1.4s, v0.4s")                      /* v15  = KT = 1/t = t' * (2 - TD*t') */

                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")          /* v8 = v[0], v9 = v[1] */
                EFF_HSLA_LIGHT_CORE
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("st4             {v4.4s, v5.4s, v6.4s, v7.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("b.hs            1b")

                //-----------------------------------------------------------------
                // 1x-8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v1.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x08")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v1.s}[2], [%[src]]")
                __ASM_EMIT("8:")
                EFF_HSLA_LIGHT_CORE
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st4             {v4.2s, v5.2s, v6.2s, v7.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st4             {v4.s, v5.s, v6.s, v7.s}[2], [%[dst]]")
                // End
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count),
                  [eff] "+r" (eff)
                :
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

        #undef EFF_HSLA_LIGHT_CORE

        #define EFF_HSLA_ALPHA_CORE   \
            /* v0   = v[0]      */ \
            /* v1   = v[1]      */ \
            /* v8   = h         */ \
            /* v9   = s         */ \
            /* v10  = l         */ \
            /* v11  = a         */ \
            /* v14  = 0         */ \
            /* v15  = 1         */ \
            __ASM_EMIT("fsub            v2.4s, v15.4s, v0.4s")           /* v2   = 1 - v */ \
            __ASM_EMIT("fsub            v6.4s, v15.4s, v1.4s") \
            __ASM_EMIT("fcmgt           v4.4s, v14.4s, v0.4s")           /* v4   = 0 > v  */ \
            __ASM_EMIT("fcmgt           v5.4s, v14.4s, v1.4s") \
            __ASM_EMIT("fadd            v3.4s, v0.4s, v15.4s")           /* v3   = v + 1 */ \
            __ASM_EMIT("fadd            v7.4s, v1.4s, v15.4s") \
            __ASM_EMIT("bif             v3.16b, v2.16b, v4.16b")         /* v0   = V = (v+1)&[0>v] | (1-v)&[0<=v] */ \
            __ASM_EMIT("bif             v7.16b, v6.16b, v5.16b") \
            __ASM_EMIT("mov             v0.16b, v8.16b") \
            __ASM_EMIT("mov             v1.16b, v9.16b") \
            __ASM_EMIT("mov             v2.16b, v10.16b") \
            __ASM_EMIT("mov             v4.16b, v8.16b") \
            __ASM_EMIT("mov             v5.16b, v9.16b") \
            __ASM_EMIT("mov             v6.16b, v10.16b")

            /*
                value   = v[i];
                value   = (0.0f > value) ? 1.0f + value : 1.0f - value;

                dst[0]  = eff->h;
                dst[1]  = eff->s;
                dst[2]  = eff->l;
                dst[3]  = value; // Fill alpha channel
             */

        IF_ARCH_AARCH64
        (
            static const float EFF_HSLA_ALPHA_XC[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0.0f),
                LSP_DSP_VEC4(1.0f)
            };
        )

        void eff_hsla_alpha(float *dst, const float *v, const dsp::hsla_alpha_eff_t *eff, size_t count)
        {
            ARCH_AARCH64_ASM
            (
                __ASM_EMIT("ld4r            {v8.4s, v9.4s, v10.4s, v11.4s}, [%[eff]]")  /* v8   = h, v9 = s, v10 = l, v11 = a */
                __ASM_EMIT("ldp             q14, q15, [%[XC]]")                         /* v14  = 0.0, v15 = 1.0 */

                //-----------------------------------------------------------------
                // 8x blocks
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("b.lo             2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q0, q1, [%[src]]")          /* v8 = v[0], v9 = v[1] */
                EFF_HSLA_ALPHA_CORE
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("add             %[src], %[src], 0x20")
                __ASM_EMIT("st4             {v4.4s, v5.4s, v6.4s, v7.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("b.hs            1b")

                //-----------------------------------------------------------------
                // 1x-8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.ls            14f")
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            4f")
                __ASM_EMIT("ldr             q0, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x10")
                __ASM_EMIT("4:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            6f")
                __ASM_EMIT("ld1             {v1.2s}, [%[src]]")
                __ASM_EMIT("add             %[src], %[src], 0x08")
                __ASM_EMIT("6:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            8f")
                __ASM_EMIT("ld1             {v1.s}[2], [%[src]]")
                __ASM_EMIT("8:")
                EFF_HSLA_ALPHA_CORE
                __ASM_EMIT("tst             %[count], #4")
                __ASM_EMIT("b.eq            10f")
                __ASM_EMIT("st4             {v0.4s, v1.4s, v2.4s, v3.4s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x40")
                __ASM_EMIT("10:")
                __ASM_EMIT("tst             %[count], #2")
                __ASM_EMIT("b.eq            12f")
                __ASM_EMIT("st4             {v4.2s, v5.2s, v6.2s, v7.2s}, [%[dst]]")
                __ASM_EMIT("add             %[dst], %[dst], 0x20")
                __ASM_EMIT("12:")
                __ASM_EMIT("tst             %[count], #1")
                __ASM_EMIT("b.eq            14f")
                __ASM_EMIT("st4             {v4.s, v5.s, v6.s, v7.s}[2], [%[dst]]")
                // End
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (v), [count] "+r" (count)
                : [eff] "r" (eff),
                  [XC] "r" (&EFF_HSLA_ALPHA_XC[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3",
                  "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11",
                  "v12", "v13", "v14", "v15"
            );
        }

        #undef EFF_HSLA_ALPHA_CORE

    } /* namespace asimd */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_EFFECTS_H_ */
