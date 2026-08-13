/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 4 авг. 2026 г.
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
#define FILTER_TIMES    4

namespace lsp
{
    namespace generic
    {
        void fcascade_fill_x4(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void fcascade_fill_x4(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }
        namespace avx
        {
            void fcascade_fill_x4(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }
        namespace avx512
        {
            void fcascade_fill_x4(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }
    )

    typedef void (*fcascade_fill_x4_t)(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
}

UTEST_BEGIN("dsp.filters", fill_x4)

    void call(const char *label, fcascade_fill_x4_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        constexpr size_t matrix_size    = FILTER_TIMES * FILTER_TIMES * 2;
        constexpr size_t num_floats     = (sizeof(dsp::f_cascade_t) * matrix_size) / sizeof(float);
        constexpr size_t tail_offset    = (FILTER_TIMES + 1) * FILTER_TIMES;

        FloatBuffer src(num_floats);
        FloatBuffer dst1(num_floats);
        FloatBuffer dst2(num_floats);

        // Fill input buffer
        src.randomize(1.0f, 2.0f);
        dst1.copy(src);
        dst2.copy(src);

        // call processing
        printf("Testing %s...\n", label);

        generic::fcascade_fill_x4(dst1.data<dsp::f_cascade_t>(), dst1.data<dsp::f_cascade_t>() + tail_offset);
        func(dst2.data<dsp::f_cascade_t>(), dst2.data<dsp::f_cascade_t>() + tail_offset);

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

        IF_ARCH_X86(CALL(sse::fcascade_fill_x4));
        IF_ARCH_X86(CALL(avx::fcascade_fill_x4));
        IF_ARCH_X86(CALL(avx512::fcascade_fill_x4));
    }

UTEST_END


