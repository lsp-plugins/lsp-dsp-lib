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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>

namespace lsp
{
    namespace generic
    {
        float ipowf(float x, int deg);
        float irootf(float x, int deg);
    }

    IF_ARCH_X86 (
        namespace sse
        {
            float ipowf(float x, int deg);
            float irootf(float x, int deg);
        }
    )

    typedef float (* ipowrootf_t)(float x, int deg);
}

UTEST_BEGIN("dsp.smath", ipowroot)

    void check_generic()
    {
        for (int i=-16; i <= 16; ++i)
        {
            float x1 = generic::ipowf(M_PI, i);
            float x2 = 1;
            for (size_t j=0, k=(i<0) ? -i: i; j<k; ++j)
                x2 *= M_PI;
            if (i < 0)
                x2 = 1.0f / x2;

            UTEST_ASSERT_MSG(float_equals_relative(x1, x2, 1e-4), "Failed ipowf(PI, %d): x1=%f, x2=%f", int(i), x1, x2);
        }

        for (int i=1; i <= 16; ++i)
        {
            float x1 = generic::ipowf(M_PI, i);
            float y = generic::irootf(x1, i);

            UTEST_ASSERT_MSG(float_equals_relative(M_PI, y, 1e-4), "Failed irootf(%f, %d) = %f, expected %f", x1, int(i), y, M_PI);
        }
    }

    void call(const char *label, int start, int end, ipowrootf_t func1, ipowrootf_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        printf("Testing %s", label);
        for (int i=start; i <= end; ++i)
        {
            float x1 = func1(M_PI, i);
            float x2 = func2(M_PI, i);

            UTEST_ASSERT_MSG(float_equals_relative(x1, x2, 1e-4), "Failed %s(PI, %d): x1=%f, x2=%f", label, int(i), x1, x2);
        }
    }

    UTEST_MAIN
    {
        check_generic();
        IF_ARCH_X86(call("irootf_sse", -16, 16, generic::irootf, sse::irootf));
        IF_ARCH_X86(call("ipowf_sse", 1, 16, generic::ipowf, sse::ipowf));
    }


UTEST_END



