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

#include <lsp-plug.in/common/atomic.h>
#include <lsp-plug.in/common/finally.h>
#include <lsp-plug.in/common/singletone.h>
#include <lsp-plug.in/common/types.h>

#include <private/dsp/arch/aarch64/features.h>
#include <private/dsp/arch/arm/features.h>
#include <private/dsp/arch/generic/features.h>
#include <private/dsp/arch/x86/features.h>


#define LSP_DSP_LIB_SYMBOL(ret, name, ...) \
    namespace lsp { \
        namespace dsp { \
            LSP_DSP_LIB_PUBLIC \
            ret (* name)(__VA_ARGS__) = NULL; \
            \
            extern "C" { \
                LSP_DSP_LIB_PUBLIC \
                ret (* LSP_DSP_LIB_MANGLE(name))(__VA_ARGS__) = NULL; \
            } \
        } \
    }

#include <lsp-plug.in/dsp/dsp.h>

#ifndef LSP_DSP_CPU_NAMESPACE
    #define IF_ARCH_SPECIFIC_INIT(...)
#else
    #define IF_ARCH_SPECIFIC_INIT(...)          __VA_ARGS__
#endif /* LSP_DSP_CPU_NAMESPACE */


namespace lsp
{
    namespace dsp
    {
        static lsp::singletone_t library;

        LSP_DSP_LIB_PUBLIC
        void init()
        {
            // Check that library has already been initialized
            if (library.initialized())
                return;

            // Dectect CPU options
            IF_ARCH_SPECIFIC_INIT(
                LSP_DSP_CPU_NAMESPACE::cpu_features_t f;
                LSP_DSP_CPU_NAMESPACE::detect_cpu_features(&f);
            );

            // Write architecture-optimized pointers to functions
            lsp_singletone_init(library) {
                // Initialize native functions
                generic::dsp_init();

                // Initialize architecture-dependent functions that utilize architecture-specific features
                IF_ARCH_SPECIFIC_INIT(LSP_DSP_CPU_NAMESPACE::dsp_init(&f));
            };
        }

        extern "C"
        {
            LSP_DSP_LIB_PUBLIC
            void LSP_DSP_LIB_MANGLE(init)()
            {
                dsp::init();
            }
        }
    }
}


