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

        namespace avx
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

PTEST_BEGIN("dsp.filters", pack_x2, 5, 1000)

    void call(const char * label, dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src, biquad_pack_x2_t func)
    {
        printf("Testing %s...\n", label);

        PTEST_LOOP(label,
            dsp::biquad_x2_t *d = dst;
            const dsp::biquad_x1_t *s = src;
            for (size_t i=0; i<NUM_FILTERS; ++i, s += FILTER_TIMES)
                func(&d[i], s);
        );
    }

    PTEST_MAIN
    {
        void *p1 = NULL, *p2 = NULL;
        dsp::biquad_x2_t * const dst = alloc_aligned<dsp::biquad_x2_t>(p1, NUM_FILTERS, LSP_DSP_BIQUAD_ALIGN);
        dsp::biquad_x1_t * const src = alloc_aligned<dsp::biquad_x1_t>(p2, NUM_FILTERS * FILTER_TIMES, LSP_DSP_BIQUAD_ALIGN);

        for (size_t i=0; i<NUM_FILTERS * FILTER_TIMES; ++i)
        {
            src[i].b0       = randf();
            src[i].b1       = randf();
            src[i].b2       = randf();
            src[i].a1       = randf();
            src[i].a2       = randf();
            src[i].p0       = randf();
            src[i].p1       = randf();
            src[i].p1       = randf();
        }

        #define CALL(func) \
            call(#func, dst, src, func)

        CALL(generic::biquad_pack_x2);
        IF_ARCH_X86(CALL(sse::biquad_pack_x2));
        IF_ARCH_X86(CALL(avx::biquad_pack_x2));
        PTEST_SEPARATOR;
    }

PTEST_END
