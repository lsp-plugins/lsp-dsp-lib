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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void add2(float *dst, const float *src, size_t count);

        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void add2(float *dst, const float *src, size_t count);

            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        }

        namespace avx
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void add2(float *dst, const float *src, size_t count);

            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);

            void direct_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void complex_mul3_fma3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);

            void fastconv_parse_fma3(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply_fma3(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void add2(float *dst, const float *src, size_t count);

            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void add2(float *dst, const float *src, size_t count);

            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        }
    )

    typedef void (* direct_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    typedef void (* reverse_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    typedef void (* complex_mul3_t)(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    typedef void (* add2_t)(float *dst, const float *src, size_t count);

    typedef void (* fastconv_parse_t)(float *dst, const float *src, size_t rank);
    typedef void (* fastconv_parse_apply_t)(float *dst, float *tmp, const float *c, const float *src, size_t rank);
}

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.fft", fastconv, 10, 1000)

    void call(
            const char *label,
            float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank,
            direct_fft_t direct, complex_mul3_t cmul, reverse_fft_t reverse, add2_t add2
        )
    {
        if (!(PTEST_SUPPORTED(direct) && (PTEST_SUPPORTED(cmul)) && (PTEST_SUPPORTED(reverse)) && (PTEST_SUPPORTED(add2))))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d)...\n", buf, int(rank));
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        direct(conv, &conv[bin_size], cv, &cv[bin_size], rank);

        // Run test
        PTEST_LOOP(buf,
            direct(tmp, &tmp[bin_size], in, &in[bin_size], rank);
            cmul(tmp, &tmp[bin_size], tmp, &tmp[bin_size], conv, &conv[bin_size], bin_size);
            reverse(tmp2, &tmp2[bin_size], tmp, &tmp[bin_size], rank);
            add2(out, tmp2, bin_size);
        )
    }

    void call(
            const char *label,
            float *out, float *tmp, float *conv, const float *in, const float *cv, size_t rank,
            fastconv_parse_t parse, fastconv_parse_apply_t apply
            )
    {
        if (!(PTEST_SUPPORTED(parse) && (PTEST_SUPPORTED(apply))))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d)...\n", buf, int(rank));

        // Prepare data
        dsp::fill_zero(out, 1 << rank);
        parse(conv, cv, rank);

        PTEST_LOOP(buf,
            apply(out, tmp, conv, in, rank);
        );
    }

    PTEST_MAIN
    {
        size_t fft_size = 1 << MAX_RANK;
        size_t alloc    = fft_size * 2 * 2 // in + cv size
                        + fft_size * 2 // conv size
                        + fft_size * 2 // tmp size
                        + fft_size * 2 // tmp2 size
                        + fft_size;     // out size

        uint8_t *data   = NULL;
        float *in       = alloc_aligned<float>(data, alloc, 64);
        dsp::fill_zero(in, alloc);

        float *cv       = &in[fft_size * 2];
        float *conv     = &cv[fft_size * 2];
        float *tmp      = &conv[fft_size * 2];
        float *tmp2     = &tmp[fft_size * 2];
        float *out      = &tmp2[fft_size * 2];

        for (size_t i=0; i < (fft_size / 2); ++i)
        {
            in[i]       = randf(0.0f, 1.0f);
            cv[i]       = randf(0.0f, 1.0f);
        }

        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            call("generic::fft", out, tmp, tmp2, conv, in, cv, rank,
                    generic::direct_fft, generic::complex_mul3, generic::reverse_fft, generic::add2);
            call("generic::fastconv_fft", out, tmp, conv, in, cv, rank,
                    generic::fastconv_parse, generic::fastconv_parse_apply);

            IF_ARCH_X86(
                call("sse::fft", out, tmp, tmp2, conv, in, cv, rank,
                    sse::direct_fft, sse::complex_mul3, sse::reverse_fft, sse::add2);
                call("avx::fft", out, tmp, tmp2, conv, in, cv, rank,
                    avx::direct_fft, avx::complex_mul3, avx::reverse_fft, avx::add2);
                call("avx::fft_fma3", out, tmp, tmp2, conv, in, cv, rank,
                    avx::direct_fft_fma3, avx::complex_mul3_fma3, avx::reverse_fft_fma3, avx::add2);

                call("sse::fastconv_fft", out, tmp, conv, in, cv, rank,
                    sse::fastconv_parse, sse::fastconv_parse_apply);
                call("avx::fastconv_fft", out, tmp, conv, in, cv, rank,
                    avx::fastconv_parse, avx::fastconv_parse_apply);
                call("avx::fastconv_fft_fma3", out, tmp, conv, in, cv, rank,
                    avx::fastconv_parse_fma3, avx::fastconv_parse_apply_fma3);
            )

            IF_ARCH_ARM(
                call("neon_d32::fft", out, tmp, tmp2, conv, in, cv, rank,
                    neon_d32::direct_fft, neon_d32::complex_mul3, neon_d32::reverse_fft, neon_d32::add2);
                call("neon_d32::fastconv_fft", out, tmp, conv, in, cv, rank,
                    neon_d32::fastconv_parse, neon_d32::fastconv_parse_apply);
            )

            IF_ARCH_AARCH64(
                call("asimd::fft", out, tmp, tmp2, conv, in, cv, rank,
                    asimd::direct_fft, asimd::complex_mul3, asimd::reverse_fft, asimd::add2);
                call("asimd::fastconv_fft", out, tmp, conv, in, cv, rank,
                    asimd::fastconv_parse, asimd::fastconv_parse_apply);
            )

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


