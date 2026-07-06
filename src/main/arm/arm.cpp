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

#if defined(ARCH_ARM6) || defined(ARCH_ARM7)

#include <private/dsp/exports.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/stdlib/string.h>

// Test framework
#ifdef LSP_TESTING
    #include <lsp-plug.in/test-fw/test.h>
#else
    #define TEST_EXPORT(...)
#endif /* LSP_TESTING */

#define PRIVATE_DSP_ARCH_ARM_IMPL
    #include <private/dsp/arch/arm/fpscr.h>
#undef PRIVATE_DSP_ARCH_ARM_IMPL

namespace lsp
{
    namespace neon_d32
    {
        extern void dsp_init(const arm::cpu_features_t *f);
    }

    namespace arm
    {
        void start(dsp::context_t *ctx)
        {
            dsp_start(ctx);
            uint32_t fpscr          = read_fpscr();
            ctx->data[ctx->top++]   = fpscr;
            write_fpscr(fpscr | FPSCR_FZ | FPSCR_DN);
        }

        void finish(dsp::context_t *ctx)
        {
            write_fpscr(ctx->data[--ctx->top]);
            dsp_finish(ctx);
        }

        #define EXPORT2(function, export) \
        { \
            dsp::function                       = arm::export; \
            dsp::LSP_DSP_LIB_MANGLE(function)   = arm::export; \
            TEST_EXPORT(arm::export); \
        }
        #define EXPORT1(function)                   EXPORT2(function, function)

        void dsp_init(const cpuid_t *f)
        {
            if (f->hwcap[0] & CPU_HWCAP0_VFP)
            {
                // Save previous entry points
                dsp_start                       = dsp::start;
                dsp_finish                      = dsp::finish;

                // Export routines
                EXPORT1(start);
                EXPORT1(finish);
            }

            // Initialize support of NEON functions with D-32 registers
            neon_d32::dsp_init(f);
        }
    } /* namespace arm */
} /* namespace lsp */

#endif /* defined(ARCH_ARM6) || defined(ARCH_ARM7) */


