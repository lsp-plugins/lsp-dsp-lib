/*
 * float.h
 *
 *  Created on: 1 апр. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_DSP_ARCH_X86_FLOAT_H_
#define PRIVATE_DSP_ARCH_X86_FLOAT_H_

#ifndef PRIVATE_DSP_ARCH_X86_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_IMPL */

namespace lsp
{
    namespace x86
    {
        static uint32_t fpu_read_cr()
        {
            uint16_t cr = 0;

            ARCH_X86_ASM
            (
                __ASM_EMIT("fstcw   %[cr]")

                : [cr] "+m" (cr)
                :
                : "memory"
            );

            return cr;
        }

        static void fpu_write_cr(uint32_t value)
        {
            uint16_t cr     = value;

            ARCH_X86_ASM
            (
                __ASM_EMIT("fldcw   %[cr]")

                :
                : [cr] "m" (cr)
                :
            );
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_X86_FLOAT_H_ */
