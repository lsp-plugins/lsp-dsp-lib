/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 окт. 2023 г.
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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace generic
    {
        float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }

        namespace avx
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float sidechain_rms(float *dst, float *head, const float *tail, float rms, float k, size_t count);
        }
    )

    static float sidechain_rms_naive(float *dst, float *head, const float *tail, float rms, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float s         = dst[i];
            head[i]         = s;
            float t         = tail[i];
            rms            += s*s - t*t;
            dst[i]          = (rms > 0.0f) ? sqrtf(rms * k) : 0.0f;
        }

        return rms;
    }
}

typedef float (* sidechain_rms_func_t)(float *dst, float *head, const float *tail, float rms, float k, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.dynamics", sidechain_rms)

    void call(const char *label, size_t align, sidechain_rms_func_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), mask);

                FloatBuffer dst(count, align, mask & 1);
                FloatBuffer head(count, align, mask & 2);
                FloatBuffer tail(count, align, mask & 4);

                dst.randomize_sign();
                head.randomize_sign();
                tail.randomize_sign();
                FloatBuffer dst1(dst);
                FloatBuffer dst2(dst);
                FloatBuffer head1(head);
                FloatBuffer head2(head);
                FloatBuffer tail1(tail);
                FloatBuffer tail2(tail);

                float rms_src   = randf(0.0f, 1.0f);

                // Call functions
                float rms1      = sidechain_rms_naive(dst1, head1, tail1, rms_src, 0.001f, count);
                float rms2      = func(dst2, head2, tail2, rms_src, 0.001f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
                UTEST_ASSERT_MSG(head1.valid(), "Head buffer 1 corrupted");
                UTEST_ASSERT_MSG(head2.valid(), "Head buffer 2 corrupted");
                UTEST_ASSERT_MSG(tail1.valid(), "Tail buffer 1 corrupted");
                UTEST_ASSERT_MSG(tail2.valid(), "Tail buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-4))
                {
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");
                    head1.dump("head1");
                    head1.dump("head2");
                    tail1.dump("tail1");
                    tail1.dump("tail2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs on destination buffer ", label);
                }

                if (!head1.equals_absolute(head2, 1e-4))
                {
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");
                    head1.dump("head1");
                    head1.dump("head2");
                    tail1.dump("tail1");
                    tail1.dump("tail2");
                    printf("index=%d, %.6f vs %.6f\n", head1.last_diff(), head1.get_diff(), head2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs on head buffer ", label);
                }

                if (!tail1.equals_absolute(tail2, 1e-4))
                {
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");
                    head1.dump("head1");
                    head1.dump("head2");
                    tail1.dump("tail1");
                    tail1.dump("tail2");
                    printf("index=%d, %.6f vs %.6f\n", tail1.last_diff(), tail1.get_diff(), tail2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs on tail buffer ", label);
                }

                // Compare envelope value
                if (!float_equals_absolute(rms1, rms2, 1e-4f))
                {
                    dst1.dump("dst1 ");
                    dst2.dump("dst2 ");
                    head1.dump("head1");
                    head1.dump("head2");
                    tail1.dump("tail1");
                    tail1.dump("tail2");
                    UTEST_FAIL_MSG("Output value for test '%s' differs: %.6f vs %.6f", label, rms1, rms2);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func);

        CALL(generic::sidechain_rms, 16);
        IF_ARCH_X86(CALL(sse::sidechain_rms, 16));
        IF_ARCH_X86(CALL(avx::sidechain_rms, 32));
//        IF_ARCH_ARM(CALL(neon_d32::compressor_env));
//        IF_ARCH_AARCH64(CALL(asimd::compressor_env));
    }
UTEST_END



