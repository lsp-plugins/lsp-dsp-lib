/*
 * sse4.cpp
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */


#include <lsp-plug.in/common/types.h>

#ifdef ARCH_X86
    #include <lsp-plug.in/dsp/dsp.h>
    #include <lsp-plug.in/common/bits.h>

    #include <math.h>

    // Test framework
    #ifdef LSP_TESTING
        #include <lsp-plug.in/test-fw/test.h>
    #else
        #define TEST_EXPORT(...)
    #endif /* LSP_TESTING */

    // Feature detection
    #define LSP_PLUG_IN_DSP_ARCH_X86_IMPL
        #include <lsp-plug.in/dsp/arch/x86/features.h>
    #undef LSP_PLUG_IN_DSP_ARCH_X86_IMPL

    #define LSP_PLUG_IN_DSP_ARCH_X86_SSE4_IMPL
        #include <lsp-plug.in/dsp/arch/x86/sse4/3dmath.h>
    #undef LSP_PLUG_IN_DSP_ARCH_X86_SSE4_IMPL

    namespace lsp
    {
        namespace sse4
        {
            using namespace x86;

            #define EXPORT1(function)                   dsp::function = sse4::function; TEST_EXPORT(sse4::function);

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
                EXPORT1(check_point3d_on_triangle_tp);
            }

            #undef EXPORT1
        }
    }

#endif /* ARCH_X86 */


