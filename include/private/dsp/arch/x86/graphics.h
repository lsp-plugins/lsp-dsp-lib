/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_X86_GRAPHICS_H_
#define PRIVATE_DSP_ARCH_X86_GRAPHICS_H_

#ifndef PRIVATE_DSP_ARCH_X86_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_IMPL */

namespace lsp
{
    namespace x86
    {
    #ifdef ARCH_I386
        // Limited number of registers
        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            uint32_t t1, t2;

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("test    %[count], %[count]")
                __ASM_EMIT("jz      2f")

                // Loop multiple of 4
                __ASM_EMIT("1:")
                __ASM_EMIT("mov     (%[src]), %[t1]")       // t1 = *src = RGBA
                __ASM_EMIT("mov     %[t1], %[t2]")          // t1 = *src = RGBA
                __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
                __ASM_EMIT("add     $0x4, %[src]")          // src ++
                __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
                __ASM_EMIT("mov     %[t1], (%[dst])")       // *dst = BGRA
                __ASM_EMIT("add     $0x4, %[dst]")          // dst ++
                __ASM_EMIT("dec     %[count]")              // count --
                __ASM_EMIT("jnz     1b")                    // count > 0 ?

                // Loop not multiple of 4
                __ASM_EMIT("2:")

                : [count] "+r" (count),
                  [dst] "+r"(dst),
                  [src] "+r"(src),
                  [t1] "=&r" (t1),
                  [t2] "=&r" (t2)
                :
                : "cc", "memory"
            );
        }
    #endif /* ARCH_I386 */

    #ifdef ARCH_X86_64
        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            uint32_t t1, t2, t3;

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("sub     $4, %[count]")
                __ASM_EMIT("jb      2f")

                // Loop multiple of 4
                __ASM_EMIT("1:")
                __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = RGBA
                __ASM_EMIT("mov     %[t1], %[t2]")          // t1 = src[0] = RGBA
                __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
                __ASM_EMIT("mov     0x04(%[src]), %[t3]")   // t3 = src[1] = RGBA
                __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
                __ASM_EMIT("mov     %[t1], 0x00(%[dst])")   // dst[0] = BGRA

                __ASM_EMIT("mov     %[t3], %[t2]")          // t2 = RGBA
                __ASM_EMIT("and     $0x00ff00ff, %[t3]")    // t3 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t3]")            // t3 = B0R0
                __ASM_EMIT("mov     0x08(%[src]), %[t1]")   // t1 = src[2] = RGBA
                __ASM_EMIT("or      %[t2], %[t3]")          // t3 = BGRA
                __ASM_EMIT("mov     %[t3], 0x04(%[dst])")   // dst[1] = BGRA

                __ASM_EMIT("mov     %[t1], %[t2]")          // t2 = RGBA
                __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
                __ASM_EMIT("mov     0x0c(%[src]), %[t3]")   // t3 = src[3] = RGBA
                __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
                __ASM_EMIT("mov     %[t1], 0x08(%[dst])")   // dst[2] = BGRA

                __ASM_EMIT("mov     %[t3], %[t2]")          // t2 = RGBA
                __ASM_EMIT("and     $0x00ff00ff, %[t3]")    // t3 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t3]")            // t3 = B0R0
                __ASM_EMIT("add     $0x10, %[src]")         // src += 16
                __ASM_EMIT("or      %[t2], %[t3]")          // t3 = BGRA
                __ASM_EMIT("mov     %[t3], 0x0c(%[dst])")   // dst[3] = BGRA

                __ASM_EMIT("add     $0x10, %[dst]")         // dst += 16
                __ASM_EMIT("sub     $4, %[count]")          // count -= 4
                __ASM_EMIT("jae     1b")

                // Loop not multiple of 4
                __ASM_EMIT("2:")
                __ASM_EMIT("add     $3, %[count]")
                __ASM_EMIT("jl      4f")

                // Complete tail
                __ASM_EMIT("3:")
                __ASM_EMIT("mov     (%[src]), %[t1]")       // t1 = *src = RGBA
                __ASM_EMIT("mov     %[t1], %[t2]")          // t2 = t1 RGBA
                __ASM_EMIT("add     $4, %[src]")            // src++
                __ASM_EMIT("and     $0x00ff00ff, %[t1]")    // t1 = R0B0
                __ASM_EMIT("and     $0xff00ff00, %[t2]")    // t2 = 0G0A
                __ASM_EMIT("ror     $16, %[t1]")            // t1 = B0R0
                __ASM_EMIT("or      %[t2], %[t1]")          // t1 = BGRA
                __ASM_EMIT("mov     %[t1], (%[dst])")       // *dst = BGRA
                __ASM_EMIT("add     $4, %[dst]")            // dst++
                __ASM_EMIT("dec     %[count]")              // count--
                __ASM_EMIT("jge     3b")

                __ASM_EMIT("4:")

                : [count] "+r" (count),
                  [dst] "+r"(dst),
                  [src] "+r"(src),
                  [t1] "=&r" (t1),
                  [t2] "=&r" (t2),
                  [t3] "=&r" (t3)
                :
                : "cc", "memory"
            );
        }
    #endif /* ARCH_X86_64 */

    #ifdef ARCH_64BIT
        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            uint32_t a          = uint32_t(alpha) << 24;
            uint64_t value      = (uint64_t(a) << 32) | a;
            uint64_t mask       = 0x00ffffff00ffffffULL;
            uint64_t t1, t2, t3, t4;

            ARCH_X86_64_ASM
            (
                // Check count
                __ASM_EMIT("sub     $8, %[count]")
                __ASM_EMIT("jb      2f")

                // Loop x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = ABCL
                __ASM_EMIT("mov     0x08(%[src]), %[t2]")
                __ASM_EMIT("mov     0x10(%[src]), %[t3]")
                __ASM_EMIT("and     %[mask], %[t1]")        // t1 = ABC0
                __ASM_EMIT("mov     0x18(%[src]), %[t4]")
                __ASM_EMIT("and     %[mask], %[t2]")
                __ASM_EMIT("or      %[value], %[t1]")       // t1 = ABCv
                __ASM_EMIT("and     %[mask], %[t3]")
                __ASM_EMIT("or      %[value], %[t2]")
                __ASM_EMIT("and     %[mask], %[t4]")
                __ASM_EMIT("mov     %[t1], 0x00(%[dst])")
                __ASM_EMIT("or      %[value], %[t3]")
                __ASM_EMIT("mov     %[t2], 0x08(%[dst])")
                __ASM_EMIT("or      %[value], %[t4]")
                __ASM_EMIT("mov     %[t3], 0x10(%[dst])")
                __ASM_EMIT("mov     %[t4], 0x18(%[dst])")
                __ASM_EMIT("add     $0x20, %[src]")         // src += 8
                __ASM_EMIT("add     $0x20, %[dst]")         // dst += 8
                __ASM_EMIT("sub     $8, %[count]")          // count -= 8
                __ASM_EMIT("jae     1b")

                // Loop x4 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add     $4, %[count]")
                __ASM_EMIT("jl      4f")
                __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = ABCL
                __ASM_EMIT("mov     0x08(%[src]), %[t2]")
                __ASM_EMIT("and     %[mask], %[t1]")        // t1 = ABC0
                __ASM_EMIT("and     %[mask], %[t2]")
                __ASM_EMIT("or      %[value], %[t1]")       // t1 = ABCv
                __ASM_EMIT("or      %[value], %[t2]")
                __ASM_EMIT("mov     %[t1], 0x00(%[dst])")
                __ASM_EMIT("mov     %[t2], 0x08(%[dst])")
                __ASM_EMIT("add     $0x10, %[src]")         // src += 4
                __ASM_EMIT("add     $0x10, %[dst]")         // dst += 4
                __ASM_EMIT("sub     $4, %[count]")          // count -= 4
                __ASM_EMIT("jae     1b")

                // Loop x1 blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add     $3, %[count]")
                __ASM_EMIT("jl      6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("mov     0x00(%[src]), %k[t1]")  // t1 = src[0] = ABCL
                __ASM_EMIT("and     %k[mask], %k[t1]")      // t1 = ABC0
                __ASM_EMIT("or      %k[value], %k[t1]")     // t1 = ABCv
                __ASM_EMIT("mov     %k[t1], 0x00(%[dst])")  // dst[0] = ABCv
                __ASM_EMIT("add     $4, %[src]")            // src++
                __ASM_EMIT("add     $4, %[dst]")            // dst++
                __ASM_EMIT("dec     %[count]")              // count--
                __ASM_EMIT("jge     5b")

                __ASM_EMIT("6:")

                : [count] "+r" (count),
                  [dst] "+r"(dst),
                  [src] "+r"(src),
                  [t1] "=&r" (t1),
                  [t2] "=&r" (t2),
                  [t3] "=&r" (t3),
                  [t4] "=&r" (t4)
                : [value] "r" (value),
                  [mask] "r" (mask)
                : "cc", "memory"
            );
        }
    #else
        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            uint32_t value      = uint32_t(alpha) << 24;
            uint32_t t1, t2;

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("sub     $4, %[count]")
                __ASM_EMIT("jb      2f")

                // Loop x4 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = ABCL
                __ASM_EMIT("mov     0x04(%[src]), %[t2]")
                __ASM_EMIT("and     $0x00ffffff, %[t1]")    // t1 = ABC0
                __ASM_EMIT("and     $0x00ffffff, %[t2]")
                __ASM_EMIT("or      %[value], %[t1]")       // t1 = ABCv
                __ASM_EMIT("or      %[value], %[t2]")
                __ASM_EMIT("mov     %[t1], 0x00(%[dst])")   // dst[0] = ABCv
                __ASM_EMIT("mov     %[t2], 0x04(%[dst])")

                __ASM_EMIT("mov     0x08(%[src]), %[t1]")   // t1 = src[0] = ABCL
                __ASM_EMIT("mov     0x0c(%[src]), %[t2]")
                __ASM_EMIT("and     $0x00ffffff, %[t1]")    // t1 = ABC0
                __ASM_EMIT("and     $0x00ffffff, %[t2]")
                __ASM_EMIT("or      %[value], %[t1]")       // t1 = ABCv
                __ASM_EMIT("or      %[value], %[t2]")
                __ASM_EMIT("mov     %[t1], 0x08(%[dst])")   // dst[0] = ABCv
                __ASM_EMIT("mov     %[t2], 0x0c(%[dst])")

                __ASM_EMIT("add     $0x10, %[src]")         // src += 4
                __ASM_EMIT("add     $0x10, %[dst]")         // dst += 4
                __ASM_EMIT("sub     $4, %[count]")          // count -= 4
                __ASM_EMIT("jae     1b")

                // Loop x1 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add     $3, %[count]")
                __ASM_EMIT("jl      4f")

                // Complete tail
                __ASM_EMIT("3:")
                __ASM_EMIT("mov     0x00(%[src]), %[t1]")   // t1 = src[0] = ABCL
                __ASM_EMIT("and     $0x00ffffff, %[t1]")    // t1 = ABC0
                __ASM_EMIT("or      %[value], %[t1]")       // t1 = ABCv
                __ASM_EMIT("mov     %[t1], 0x00(%[dst])")   // dst[0] = ABCv
                __ASM_EMIT("add     $4, %[src]")            // src++
                __ASM_EMIT("add     $4, %[dst]")            // dst++
                __ASM_EMIT("dec     %[count]")              // count--
                __ASM_EMIT("jge     3b")

                __ASM_EMIT("4:")

                : [count] "+r" (count),
                  [dst] "+r"(dst),
                  [src] "+r"(src),
                  [t1] "=&r" (t1),
                  [t2] "=&r" (t2)
                : [value] X86_GREG (value)
                : "cc", "memory"
            );
        }
    #endif /* ARCH_64BIT */

        // Limited number of registers
        void abgr32_to_bgra32(void *dst, const void *src, size_t count)
        {
            uint32_t tmp;

            ARCH_X86_ASM
            (
                // Check count
                __ASM_EMIT("test    %[count], %[count]")
                __ASM_EMIT("jz      2f")

                // Loop multiple of 4
                __ASM_EMIT("1:")
                __ASM_EMIT("lodsl") // eax = *src = RGBA
                __ASM_EMIT("ror     $8, %%eax")             // eax = ARGB
                __ASM_EMIT("stosl") // eax = *src = RGBA
                __ASM_EMIT("loop    1b")                    // count > 0 ?

                // Loop not multiple of 4
                __ASM_EMIT("2:")

                : [count] "+c" (count),
                  [dst] "+D"(dst),
                  [src] "+S"(src),
                  [tmp] "=a"(tmp)
                :
                : "cc", "memory"
            );
        }
    } /* namespace x86 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_GRAPHICS_H_ */
