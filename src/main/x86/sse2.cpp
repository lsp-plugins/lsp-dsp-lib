/*
 * sse2.cpp
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


    #define LSP_PLUG_IN_DSP_ARCH_X86_SSE2_IMPL
        #include <lsp-plug.in/dsp/arch/x86/sse2/float.h>

        #include <lsp-plug.in/dsp/arch/x86/sse2/search/iminmax.h>

        #include <lsp-plug.in/dsp/arch/x86/sse2/graphics.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/graphics/effects.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/graphics/axis.h>

        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/op_kx.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/op_vv.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/fmop_kx.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/fmop_vv.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/exp.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/log.h>
        #include <lsp-plug.in/dsp/arch/x86/sse2/pmath/pow.h>
    #undef LSP_PLUG_IN_DSP_ARCH_X86_SSE2_IMPL

    namespace lsp
    {
        namespace sse2
        {
            using namespace x86;

        #define EXPORT2(function, export)           dsp::function = sse2::export; TEST_EXPORT(sse2::export);
        #define EXPORT1(function)                   EXPORT2(function, function);

            void dsp_init(const cpu_features_t *f)
            {
                if (((f->features) & (CPU_OPTION_SSE | CPU_OPTION_SSE2)) != (CPU_OPTION_SSE | CPU_OPTION_SSE2))
                    return;

                EXPORT1(copy_saturated);
                EXPORT1(saturate);
                EXPORT1(limit_saturate1);
                EXPORT1(limit_saturate2);
                EXPORT1(sanitize1);
                EXPORT1(sanitize2);

                EXPORT1(mod_k2);
                EXPORT1(rmod_k2);
                EXPORT1(mod_k3);
                EXPORT1(rmod_k3);

                EXPORT1(fmmod_k3);
                EXPORT1(fmrmod_k3);
                EXPORT1(fmmod_k4);
                EXPORT1(fmrmod_k4);

                EXPORT1(mod2);
                EXPORT1(rmod2);
                EXPORT1(mod3);

                EXPORT1(exp1);
                EXPORT1(exp2);
                EXPORT1(logb1);
                EXPORT1(logb2);
                EXPORT1(loge1);
                EXPORT1(loge2);
                EXPORT1(logd1);
                EXPORT1(logd2);
                EXPORT1(powcv1);
                EXPORT1(powcv2);
                EXPORT1(powvc1);
                EXPORT1(powvc2);
                EXPORT1(powvx1);
                EXPORT1(powvx2);

                EXPORT1(min_index);
                EXPORT1(max_index);
                EXPORT1(minmax_index);

                EXPORT1(abs_min_index);
                EXPORT1(abs_max_index);
                EXPORT1(abs_minmax_index);

                EXPORT1(hsla_to_rgba);
                EXPORT1(rgba_to_hsla);
                EXPORT1(rgba_to_bgra32);

                EXPORT1(eff_hsla_hue);
                EXPORT1(eff_hsla_sat);
                EXPORT1(eff_hsla_light);
                EXPORT1(eff_hsla_alpha);

                EXPORT1(axis_apply_log1);
                EXPORT1(axis_apply_log2);
                EXPORT1(rgba32_to_bgra32);
            }

            #undef EXPORT1
            #undef EXPORT2
        }
    }

#endif /* ARCH_X86 */
