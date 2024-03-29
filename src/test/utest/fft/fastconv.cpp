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
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define MIN_RANK    6
#define MAX_RANK    12
#define TOLERANCE   5e-2

namespace lsp
{
    namespace generic
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        void fastconv_restore(float *dst, float *src, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
            void fastconv_restore(float *dst, float *src, size_t rank);
            void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        }

        namespace avx
        {
            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
            void fastconv_restore(float *dst, float *src, size_t rank);
            void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);

            void fastconv_parse_fma3(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply_fma3(float *dst, float *tmp, const float *c, const float *src, size_t rank);
            void fastconv_restore_fma3(float *dst, float *src, size_t rank);
            void fastconv_apply_fma3(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
            void fastconv_restore(float *dst, float *src, size_t rank);
            void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void fastconv_parse(float *dst, const float *src, size_t rank);
            void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
            void fastconv_restore(float *dst, float *src, size_t rank);
            void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        }
    )
}

typedef void (* fastconv_parse_t)(float *dst, const float *src, size_t rank);

typedef void (* fastconv_parse_apply_t)(float *dst, float *tmp, const float *c, const float *src, size_t rank);

typedef void (* fastconv_restore_t)(float *dst, float *src, size_t rank);

typedef void (* fastconv_apply_t)(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);

UTEST_BEGIN("dsp.fft", fastconv)

    // This is long-time test, raise time limit for it to one second
    UTEST_TIMELIMIT(60)

    void call_pr(const char *label, size_t align,
            fastconv_parse_t parse,
            fastconv_restore_t restore
        )
    {
        if (!UTEST_SUPPORTED(parse))
            return;
        if (!UTEST_SUPPORTED(restore))
            return;

        for (size_t rank=MIN_RANK; rank<=MAX_RANK; rank ++)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing '%s' for FFT rank=%d, mask=0x%x\n", label, rank, mask);

                FloatBuffer src(1 << (rank-1), align, mask & 0x01);
                FloatBuffer fc1(1 << (rank+1), align, mask & 0x02);
                FloatBuffer fc2(1 << (rank+1), align, mask & 0x02);
                FloatBuffer dst1(1 << rank, align, mask & 0x04);
                FloatBuffer dst2(1 << rank, align, mask & 0x04);

                dsp::fill_one(dst1, dst1.size());
                dsp::fill_one(dst2, dst2.size());

                generic::fastconv_parse(fc1, src, rank);
                UTEST_ASSERT_MSG(fc1.valid(), "Buffer FC1 corrupted");
                parse(fc2, src, rank);
                UTEST_ASSERT_MSG(fc2.valid(), "Buffer FC2 corrupted");
                UTEST_ASSERT_MSG(src.valid(), "Buffer SRC corrupted");

                generic::fastconv_restore(dst1, fc1, rank);
                UTEST_ASSERT_MSG(dst1.valid(), "Buffer DST1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Buffer DST2 corrupted");
                restore(dst2, fc2, rank);
                UTEST_ASSERT_MSG(dst2.valid(), "Buffer DST2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Buffer DST1 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, TOLERANCE))
                {
                    src.dump("src");
                    fc1.dump("fc1");
                    fc2.dump("fc2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");

                    ssize_t diff = dst2.last_diff();
                    UTEST_FAIL_MSG("DST1 differs DST2 for test '%s' at sample %d (%.5f vs %.5f), rank=%d",
                            label, int(diff), dst1.get(diff), dst2.get(diff), int(rank));
                }
            }
        }
    }

    void call_pa(const char *label, size_t align,
            fastconv_parse_t parse,
            fastconv_apply_t apply
        )
    {
        if (!UTEST_SUPPORTED(parse))
            return;
        if (!UTEST_SUPPORTED(apply))
            return;

        for (size_t rank=MIN_RANK; rank<=MAX_RANK; rank ++)
        {
            for (size_t mask=0; mask <= 0x3f; ++mask)
            {
                printf("Testing '%s' for FFT rank=%d, mask=0x%x\n", label, rank, mask);

                FloatBuffer src1(1 << (rank-1), align, mask & 0x01);
                FloatBuffer src2(1 << (rank-1), align, mask & 0x02);
                FloatBuffer fa1(1 << (rank+1), align, mask & 0x04);
                FloatBuffer fa2(1 << (rank+1), align, mask & 0x04);
                FloatBuffer fb1(1 << (rank+1), align, mask & 0x08);
                FloatBuffer fb2(1 << (rank+1), align, mask & 0x08);

                FloatBuffer dst1(1 << rank, align, mask & 0x10);
                FloatBuffer dst2(1 << rank, align, mask & 0x10);

                FloatBuffer tmp1(1 << (rank+1), align, mask & 0x20);
                FloatBuffer tmp2(1 << (rank+1), align, mask & 0x20);

                dsp::fill_one(dst1, dst1.size());
                dsp::fill_one(dst2, dst2.size());

                generic::fastconv_parse(fa1, src1, rank);
                UTEST_ASSERT_MSG(fa1.valid(), "Buffer FA1 corrupted");
                parse(fa2, src1, rank);
                UTEST_ASSERT_MSG(fa2.valid(), "Buffer FA2 corrupted");
                UTEST_ASSERT_MSG(src1.valid(), "Buffer SRC1 corrupted");

                generic::fastconv_parse(fb1, src2, rank);
                UTEST_ASSERT_MSG(fb1.valid(), "Buffer FB1 corrupted");
                parse(fb2, src2, rank);
                UTEST_ASSERT_MSG(fb2.valid(), "Buffer FB2 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Buffer SRC2 corrupted");

                generic::fastconv_apply(dst1, tmp1, fa1, fb1, rank);
                UTEST_ASSERT_MSG(dst1.valid(), "Buffer DST1 corrupted");
                UTEST_ASSERT_MSG(tmp1.valid(), "Buffer TMP1 corrupted");
                UTEST_ASSERT_MSG(fa1.valid(), "Buffer FA1 corrupted");
                UTEST_ASSERT_MSG(fb1.valid(), "Buffer FB1 corrupted");
                apply(dst2, tmp2, fa2, fb2, rank);
                UTEST_ASSERT_MSG(dst2.valid(), "Buffer DST2 corrupted");
                UTEST_ASSERT_MSG(tmp2.valid(), "Buffer TMP2 corrupted");
                UTEST_ASSERT_MSG(fa2.valid(), "Buffer FA2 corrupted");
                UTEST_ASSERT_MSG(fb2.valid(), "Buffer FB2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, TOLERANCE))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    fa1.dump("fa1");
                    fa2.dump("fa2");
                    fb1.dump("fb1");
                    fb2.dump("fb2");
                    tmp1.dump("tmp1");
                    tmp2.dump("tmp2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");

                    ssize_t diff = dst2.last_diff();
                    UTEST_FAIL_MSG("DST1 differs DST2 for test '%s' at sample %d (%.5f vs %.5f), rank=%d",
                            label, int(diff), dst1.get(diff), dst2.get(diff), int(rank));
                }
            }
        }
    }

    void call_pap(const char *label, size_t align,
            fastconv_parse_t parse,
            fastconv_parse_apply_t papply
        )
    {
        if (!UTEST_SUPPORTED(parse))
            return;
        if (!UTEST_SUPPORTED(papply))
            return;

        for (size_t rank=MIN_RANK; rank<=MAX_RANK; rank ++)
        {
            for (size_t mask=0; mask <= 0x1f; ++mask)
            {
                printf("Testing '%s' for FFT rank=%d, mask=0x%x\n", label, rank, mask);

                FloatBuffer src1(1 << (rank-1), align, mask & 0x01);
                FloatBuffer src2(1 << (rank-1), align, mask & 0x02);
                FloatBuffer fc1(1 << (rank+1), align, mask & 0x04);
                FloatBuffer fc2(1 << (rank+1), align, mask & 0x04);
                FloatBuffer dst1(1 << rank, align, mask & 0x08);
                FloatBuffer dst2(1 << rank, align, mask & 0x08);

                FloatBuffer tmp1(1 << (rank+1), align, mask & 0x10);
                FloatBuffer tmp2(1 << (rank+1), align, mask & 0x10);

                dsp::fill_one(dst1, dst1.size());
                dsp::fill_one(dst2, dst2.size());

                generic::fastconv_parse(fc1, src1, rank);
                UTEST_ASSERT_MSG(fc1.valid(), "Buffer FC1 corrupted");
                parse(fc2, src1, rank);
                UTEST_ASSERT_MSG(fc2.valid(), "Buffer FC2 corrupted");
                UTEST_ASSERT_MSG(src1.valid(), "Buffer SRC1 corrupted");

                generic::fastconv_parse_apply(dst1, tmp1, fc1, src2, rank);
                UTEST_ASSERT_MSG(dst1.valid(), "Buffer DST1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Buffer DST2 corrupted");
                UTEST_ASSERT_MSG(tmp1.valid(), "Buffer TMP1 corrupted");
                UTEST_ASSERT_MSG(tmp2.valid(), "Buffer TMP2 corrupted");
                UTEST_ASSERT_MSG(fc1.valid(), "Buffer FC1 corrupted");
                UTEST_ASSERT_MSG(fc2.valid(), "Buffer FC2 corrupted");
                papply(dst2, tmp2, fc2, src2, rank);
                UTEST_ASSERT_MSG(dst2.valid(), "Buffer DST2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Buffer DST1 corrupted");
                UTEST_ASSERT_MSG(tmp2.valid(), "Buffer TMP2 corrupted");
                UTEST_ASSERT_MSG(tmp1.valid(), "Buffer TMP1 corrupted");
                UTEST_ASSERT_MSG(fc2.valid(), "Buffer FC2 corrupted");
                UTEST_ASSERT_MSG(fc1.valid(), "Buffer FC1 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, TOLERANCE))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    fc1.dump("fc1");
                    fc2.dump("fc2");
                    tmp1.dump("tmp1");
                    tmp2.dump("tmp2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");

                    ssize_t diff = dst2.last_diff();
                    UTEST_FAIL_MSG("DST1 differs DST2 for test '%s' at sample %d (%.5f vs %.5f), rank=%d",
                            label, int(diff), dst1.get(diff), dst2.get(diff), int(rank));
                }
            }
        }
    }

    UTEST_MAIN
    {
        // Do tests
        IF_ARCH_X86(call_pr("sse::fastconv_parse + sse::fastconv_restore", 16, sse::fastconv_parse, sse::fastconv_restore));
        IF_ARCH_X86(call_pr("avx::fastconv_parse + avx::fastconv_restore", 32, avx::fastconv_parse, avx::fastconv_restore));
        IF_ARCH_X86(call_pr("avx::fastconv_parse_fma3 + avx::fastconv_restore_fma3", 32, avx::fastconv_parse_fma3, avx::fastconv_restore_fma3));
        IF_ARCH_ARM(call_pr("neon_d32::fastconv_parse + neon_d32::fastconv_restore", 16, neon_d32::fastconv_parse, neon_d32::fastconv_restore));
        IF_ARCH_AARCH64(call_pr("asimd::fastconv_parse + asimd::fastconv_restore", 16, asimd::fastconv_parse, asimd::fastconv_restore));

        IF_ARCH_X86(call_pa("sse::fastconv_parse + sse::fastconv_apply", 16, sse::fastconv_parse, sse::fastconv_apply));
        IF_ARCH_X86(call_pa("avx::fastconv_parse + avx::fastconv_apply", 32, avx::fastconv_parse, avx::fastconv_apply));
        IF_ARCH_X86(call_pa("avx::fastconv_parse_fma3 + avx::fastconv_apply_fma3", 32, avx::fastconv_parse_fma3, avx::fastconv_apply_fma3));
        IF_ARCH_ARM(call_pa("neon_d32::fastconv_parse + neon_d32::fastconv_apply", 16, neon_d32::fastconv_parse, neon_d32::fastconv_apply));
        IF_ARCH_AARCH64(call_pa("asimd::fastconv_parse + asimd::fastconv_apply", 16, asimd::fastconv_parse, asimd::fastconv_apply));

        IF_ARCH_X86(call_pap("sse::fastconv_parse + sse::fastconv_parse_apply", 16, sse::fastconv_parse, sse::fastconv_parse_apply));
        IF_ARCH_X86(call_pap("avx::fastconv_parse + avx::fastconv_parse_apply", 32, avx::fastconv_parse, avx::fastconv_parse_apply));
        IF_ARCH_X86(call_pap("avx::fastconv_parse_fma3 + avx::fastconv_parse_apply_fma3", 32, avx::fastconv_parse_fma3, avx::fastconv_parse_apply_fma3));
        IF_ARCH_ARM(call_pap("neon_d32::fastconv_parse + neon_d32::fastconv_parse_apply", 16, neon_d32::fastconv_parse, neon_d32::fastconv_parse_apply));
        IF_ARCH_AARCH64(call_pap("asimd::fastconv_parse + asimd::fastconv_parse_apply", 16, asimd::fastconv_parse, asimd::fastconv_parse_apply));
    }
UTEST_END;


