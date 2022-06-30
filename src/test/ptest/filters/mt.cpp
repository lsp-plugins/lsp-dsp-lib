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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define PERF_BUF_SIZE   0x200
#define KF              100.0f
#define TD              (2*M_PI/48000.0)

namespace lsp
{
    namespace generic
    {
        void matched_transform_x1(dsp::biquad_x1_t *bf, dsp::f_cascade_t *bc, float kf, float td, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void matched_transform_x1(dsp::biquad_x1_t *bf, dsp::f_cascade_t *bc, float kf, float td, size_t count);
        }
    )

    typedef void (* matched_transform_x1_t)(dsp::biquad_x1_t *bf, dsp::f_cascade_t *bc, float kf, float td, size_t count);

    static const dsp::f_cascade_t test_c =
    {
        { 1, 2, 1, 0  },
        { 1, -2, 1, 0 }
    };
}

//-----------------------------------------------------------------------------
// Performance test for bilinear transform
PTEST_BEGIN("dsp.filters", mt, 10, 10000)

    void call_x1(const char * label, size_t count, matched_transform_x1_t func)
    {
        printf("Testing %s matched transform on buffer size %d ...\n", label, int(count));

        void *p1 = NULL, *p2 = NULL;
        dsp::biquad_x1_t *dst = alloc_aligned<dsp::biquad_x1_t>(p1, count, 32);
        dsp::f_cascade_t *src = alloc_aligned<dsp::f_cascade_t>(p2, count, 32);
        dsp::f_cascade_t *tmp = alloc_aligned<dsp::f_cascade_t>(p2, count, 32);

        for (size_t i=0; i<count; ++i)
            src[i]  = test_c;

        size_t to_copy = count * (sizeof(dsp::f_cascade_t) / sizeof(float));

        PTEST_LOOP(label,
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, to_copy);
                generic::matched_transform_x1(dst, tmp, KF, TD, count);
        );

        free_aligned(p1);
        free_aligned(p2);
    }

    PTEST_MAIN
    {
        call_x1("8 matched x1 generic", PERF_BUF_SIZE, generic::matched_transform_x1);
        IF_ARCH_X86(call_x1("8 matched x1 sse", PERF_BUF_SIZE, sse::matched_transform_x1));
        PTEST_SEPARATOR;
    }

PTEST_END
