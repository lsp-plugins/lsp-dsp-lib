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

#ifndef PRIVATE_DSP_ARCH_X86_FEATURES_H_
#define PRIVATE_DSP_ARCH_X86_FEATURES_H_

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/cpuid.h>

#ifdef ARCH_X86

namespace lsp
{
    namespace x86
    {
        enum feature_t
        {
            FEAT_FAST_MOVS,         // Processor implements optimized MOVS instruction
            FEAT_FAST_AVX,          // Fast AVX implementation
            FEAT_FAST_FMA3,         // Fast FMA3 implementation
            FEAT_FAST_AVX512,       // Fast AVX-512 implementation
            FEAT_BELOW_ZEN3         // CPU has AMD architecture and is below Zen3
        };

        /**
         * Check only one feature from feature_t at one time
         * @param f detected CPU features
         * @param ops feature to check
         * @return true if feature is supported
         */
        bool feature_check(const cpuid_t *f, feature_t ops);

        void dsp_init(const cpuid_t *f);

    } /* namespace x86 */
} /* namespace lsp */

#define LSP_DSP_CPU_NAMESPACE           x86

#endif /* ARCH_X86 */

#endif /* PRIVATE_DSP_ARCH_X86_FEATURES_H_ */
