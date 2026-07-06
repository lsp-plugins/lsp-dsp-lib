/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/common/types.h>

#ifdef ARCH_AARCH64
    #include <private/dsp/exports.h>
    #include <lsp-plug.in/dsp/dsp.h>
    #include <lsp-plug.in/stdlib/string.h>
    #include <lsp-plug.in/stdlib/math.h>

    // Test framework
    #ifdef LSP_TESTING
        #include <lsp-plug.in/test-fw/test.h>
    #else
        #define TEST_EXPORT(...)
    #endif /* LSP_TESTING */

    // Include common architectural definitions
    #define PRIVATE_DSP_ARCH_AARCH64_IMPL
        #include <private/dsp/arch/aarch64/fpcr.h>
    #undef PRIVATE_DSP_ARCH_AARCH64_IMPL

namespace lsp
{
    namespace asimd
    {
        extern void dsp_init(const aarch64::cpu_features_t *f);
    }

    namespace aarch64
    {
        #define EXPORT2(function, export) \
        { \
            dsp::function                       = aarch64::export; \
            dsp::LSP_DSP_LIB_MANGLE(function)   = aarch64::export; \
            TEST_EXPORT(aarch64::export); \
        }
        #define EXPORT1(function)                   EXPORT2(function, function)

        void dsp_init(const cpu_features_t *f)
        {
            // Initialize Advanced SIMD support
            asimd::dsp_init(f);
        }
    } /* namespace aarch64 */
} /* namespace lsp */

#endif /* ARCH_AARCH64 */


