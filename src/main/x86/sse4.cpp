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

#ifdef ARCH_X86
    #include <private/dsp/exports.h>
    #include <lsp-plug.in/dsp/dsp.h>
    #include <lsp-plug.in/common/bits.h>
    #include <lsp-plug.in/stdlib/math.h>

    // Test framework
    #ifdef LSP_TESTING
        #include <lsp-plug.in/test-fw/test.h>
    #else
        #define TEST_EXPORT(...)
    #endif /* LSP_TESTING */

    // Feature detection
    #define PRIVATE_DSP_ARCH_X86_IMPL
        #include <private/dsp/arch/x86/defs.h>
        #include <private/dsp/arch/x86/features.h>
    #undef PRIVATE_DSP_ARCH_X86_IMPL

    #define PRIVATE_DSP_ARCH_X86_SSE4_IMPL
        #include <private/dsp/arch/x86/sse4/3dmath.h>
    #undef PRIVATE_DSP_ARCH_X86_SSE4_IMPL

    namespace lsp
    {
        namespace sse4
        {
            using namespace x86;

            #define EXPORT1(function) \
            { \
                dsp::function                       = sse4::function; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = sse4::function; \
                TEST_EXPORT(sse4::function); \
            }

            void dsp_init(const cpu_features_t *f)
            {
                if (!(f->features & CPU_OPTION_SSE4_1))
                    return;

                // 3D Math
                EXPORT1(normalize_point);
                EXPORT1(scale_point1);
                EXPORT1(scale_point2);

                EXPORT1(normalize_vector);
                EXPORT1(scale_vector1);
                EXPORT1(scale_vector2);

                EXPORT1(check_point3d_on_triangle_p3p);
                EXPORT1(check_point3d_on_triangle_pvp);
            }

            #undef EXPORT1
        } /* namespace sse4 */
    } /* namespace lsp */

#endif /* ARCH_X86 */


