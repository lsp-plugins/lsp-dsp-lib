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

#include <lsp-plug.in/common/types.h>

#define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
    namespace lsp { \
        namespace dsp { \
            LSP_DSP_LIB_CPPEXPORT ret (* name)(__VA_ARGS__) = NULL; \
            \
            extern "C" { \
                LSP_DSP_LIB_CEXPORT ret (* LSP_DSP_LIB_MANGLE(name))(__VA_ARGS__) = NULL; \
            } \
        } \
    }

#include <lsp-plug.in/dsp/dsp.h>


namespace lsp
{
    // Generic architecture initialization
    namespace generic
    {
        void dsp_init();
    }

    // x86-specific architecture initialization
    IF_ARCH_X86(
        namespace x86
        {
            void dsp_init();
        }
    )

    // ARM-specific 32-bit architecture initialization
    IF_ARCH_ARM(
        namespace arm
        {
            void dsp_init();
        }
    )

    // AArch64-specific 64-bit architecture initialization
    IF_ARCH_AARCH64(
        namespace aarch64
        {
            void dsp_init();
        }
    )

    namespace dsp
    {
        static bool is_initialized = false;

        LSP_DSP_LIB_CPPEXPORT void init()
        {
            // Check if we are already initialized
            if (is_initialized)
                return;

            // Initialize native functions
            generic::dsp_init();

            // Initialize architecture-dependent functions that utilize architecture-specific features
            IF_ARCH_X86(x86::dsp_init());
            IF_ARCH_ARM(arm::dsp_init());
            IF_ARCH_AARCH64(aarch64::dsp_init());

            // Mark that all DSP modules have been initialized
            is_initialized = true;
        }

        extern "C"
        {
            LSP_DSP_LIB_CEXPORT void LSP_DSP_LIB_MANGLE(init)()
            {
                dsp::init();
            }
        }
    }
}


