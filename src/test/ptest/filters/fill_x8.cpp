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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/ptest.h>
#include <lsp-plug.in/test-fw/helpers.h>

#define NUM_FILTERS     512
#define FILTER_CYCLES   512
#define FILTER_TIMES    8

namespace lsp
{
    namespace generic
    {
        void fcascade_fill_x8(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void fcascade_fill_x8(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }

        namespace avx
        {
            void fcascade_fill_x8(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }

        namespace avx512
        {
            void fcascade_fill_x8(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
        }
    )

    typedef void (*fcascade_fill_x8_t)(dsp::f_cascade_t *h, dsp::f_cascade_t *t);
}

PTEST_BEGIN("dsp.filters", fill_x8, 5, 1000)

    void call(const char * label, dsp::f_cascade_t *dst, fcascade_fill_x8_t func)
    {
        printf("Testing %s...\n", label);

        constexpr size_t tail_offset = (NUM_FILTERS - FILTER_TIMES + 1) * FILTER_TIMES;

        PTEST_LOOP(label,
            for (size_t i=0; i<FILTER_CYCLES; ++i)
                func(dst, &dst[tail_offset]);
        );
    }

    PTEST_MAIN
    {
        void *p1 = NULL;
        dsp::f_cascade_t * const dst = alloc_aligned<dsp::f_cascade_t>(p1, NUM_FILTERS * FILTER_TIMES * sizeof(dsp::f_cascade_t));
        lsp_finally { free_aligned(p1); };

        for (size_t i=0; i<NUM_FILTERS * FILTER_TIMES; ++i)
        {
            dst[i].t[0]     = randf();
            dst[i].t[1]     = randf();
            dst[i].t[2]     = randf();
            dst[i].t[3]     = randf();
            dst[i].b[0]     = randf();
            dst[i].b[1]     = randf();
            dst[i].b[2]     = randf();
            dst[i].b[3]     = randf();
        }

        #define CALL(func) \
            call(#func, dst, func)

        CALL(generic::fcascade_fill_x8);
        IF_ARCH_X86(CALL(sse::fcascade_fill_x8));
        IF_ARCH_X86(CALL(avx::fcascade_fill_x8));
        IF_ARCH_X86(CALL(avx512::fcascade_fill_x8));
        PTEST_SEPARATOR;
    }

PTEST_END
