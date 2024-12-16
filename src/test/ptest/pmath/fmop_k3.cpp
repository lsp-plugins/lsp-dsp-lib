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
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        }

        namespace sse2
        {
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
        }

        namespace avx
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);

            void    fmadd_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3_fma3(float *dst, const float *src, float k, size_t count);
        }

        namespace avx2
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);

            void    fmadd_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3_fma3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3_fma3(float *dst, const float *src, float k, size_t count);
        }

        namespace avx512
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    fmadd_k3(float *dst, const float *src, float k, size_t count);
            void    fmsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
            void    fmmul_k3(float *dst, const float *src, float k, size_t count);
            void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
            void    fmmod_k3(float *dst, const float *src, float k, size_t count);
            void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
        }
    )

    typedef void (* fmop_k3_t)(float *dst, const float *src, float k, size_t count);
}

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop_k3, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, fmop_k3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_sign(dst, buf_size*2);
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#method, dst, src, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::fmadd_k3);
            IF_ARCH_X86(CALL(sse::fmadd_k3));
            IF_ARCH_X86(CALL(avx::fmadd_k3));
            IF_ARCH_X86(CALL(avx::fmadd_k3_fma3));
            IF_ARCH_X86(CALL(avx2::fmadd_k3));
            IF_ARCH_X86(CALL(avx2::fmadd_k3_fma3));
            IF_ARCH_X86(CALL(avx512::fmadd_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmadd_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmsub_k3);
            IF_ARCH_X86(CALL(sse::fmsub_k3));
            IF_ARCH_X86(CALL(avx::fmsub_k3));
            IF_ARCH_X86(CALL(avx::fmsub_k3_fma3));
            IF_ARCH_X86(CALL(avx2::fmsub_k3));
            IF_ARCH_X86(CALL(avx2::fmsub_k3_fma3));
            IF_ARCH_X86(CALL(avx512::fmsub_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmsub_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmrsub_k3);
            IF_ARCH_X86(CALL(sse::fmrsub_k3));
            IF_ARCH_X86(CALL(avx::fmrsub_k3));
            IF_ARCH_X86(CALL(avx::fmrsub_k3_fma3));
            IF_ARCH_X86(CALL(avx2::fmrsub_k3));
            IF_ARCH_X86(CALL(avx2::fmrsub_k3_fma3));
            IF_ARCH_X86(CALL(avx512::fmrsub_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmmul_k3);
            IF_ARCH_X86(CALL(sse::fmmul_k3));
            IF_ARCH_X86(CALL(avx::fmmul_k3));
            IF_ARCH_X86(CALL(avx2::fmmul_k3));
            IF_ARCH_X86(CALL(avx512::fmmul_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmmul_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmmul_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmdiv_k3);
            IF_ARCH_X86(CALL(sse::fmdiv_k3));
            IF_ARCH_X86(CALL(avx::fmdiv_k3));
            IF_ARCH_X86(CALL(avx2::fmdiv_k3));
            IF_ARCH_X86(CALL(avx512::fmdiv_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmrdiv_k3);
            IF_ARCH_X86(CALL(sse::fmrdiv_k3));
            IF_ARCH_X86(CALL(avx::fmrdiv_k3));
            IF_ARCH_X86(CALL(avx2::fmrdiv_k3));
            IF_ARCH_X86(CALL(avx512::fmrdiv_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmmod_k3);
            IF_ARCH_X86(CALL(sse2::fmmod_k3));
            IF_ARCH_X86(CALL(avx::fmmod_k3));
            IF_ARCH_X86(CALL(avx::fmmod_k3_fma3));
            IF_ARCH_X86(CALL(avx2::fmmod_k3));
            IF_ARCH_X86(CALL(avx2::fmmod_k3_fma3));
            IF_ARCH_X86(CALL(avx512::fmmod_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmmod_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmmod_k3));
            PTEST_SEPARATOR;

            CALL(generic::fmrmod_k3);
            IF_ARCH_X86(CALL(sse2::fmrmod_k3));
            IF_ARCH_X86(CALL(avx::fmrmod_k3));
            IF_ARCH_X86(CALL(avx::fmrmod_k3_fma3));
            IF_ARCH_X86(CALL(avx2::fmrmod_k3));
            IF_ARCH_X86(CALL(avx2::fmrmod_k3_fma3));
            IF_ARCH_X86(CALL(avx512::fmrmod_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod_k3));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod_k3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END





