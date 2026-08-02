/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define TOLERANCE       1e-5f
#define FILTER_TIMES    2

namespace lsp
{
    namespace generic
    {
        void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src);
        }
    )

    typedef void (*biquad_pack_x2_t)(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src);
}

UTEST_BEGIN("dsp.filters", pack_x2)

    void call(const char *label, biquad_pack_x2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        FloatBuffer src((sizeof(dsp::biquad_x1_t) / sizeof(float)) * FILTER_TIMES);
        FloatBuffer dst1(sizeof(dsp::biquad_x2_t) / sizeof(float), LSP_DSP_BIQUAD_ALIGN);
        FloatBuffer dst2(sizeof(dsp::biquad_x2_t) / sizeof(float), LSP_DSP_BIQUAD_ALIGN);

        // Fill input buffer
        dsp::biquad_x1_t *f = src.data<dsp::biquad_x1_t>();
        int v = 1;
        for (size_t i=0; i<FILTER_TIMES; ++i)
        {
            f[i].b0 = v++;
            f[i].b1 = v++;
            f[i].b2 = v++;
            f[i].a1 = v++;
            f[i].a2 = v++;
            f[i].p0 = v++;
            f[i].p1 = v++;
            f[i].p2 = v++;
        }

        // call processing
        printf("Testing %s...\n", label);

        generic::biquad_pack_x2(dst1.data<dsp::biquad_x2_t>(), f);
        func(dst2.data<dsp::biquad_x2_t>(), f);

        // Perform validation
        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

        if (!dst1.equals_adaptive(dst2, TOLERANCE))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                    label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
        }
    }

    UTEST_MAIN
    {
        #define CALL(func) \
            call(#func, func)

        IF_ARCH_X86(CALL(sse::biquad_pack_x2));

    }

UTEST_END




