/*
 * const.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t fastconv_swp_mask[8] =
            {
                0, 0xffffffff, 0, 0xffffffff,
                0, 0xffffffff, 0, 0xffffffff
            };
        )
    }
}

#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_FASTCONV_CONST_H_ */
