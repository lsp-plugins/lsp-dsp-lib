/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 апр. 2023 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace generic
    {
        using namespace dsp;
    } /* namespace generic */
} /* namespace lsp */

#define PRIVATE_DSP_ARCH_GENERIC_IMPL
#include <private/dsp/arch/generic/bitmap/helpers.h>
#undef PRIVATE_DSP_ARCH_GENERIC_IMPL

UTEST_BEGIN("dsp.bitmap", helpers)

    void test_correct_clipping1()
    {
        printf("Testing correct clipping (case 1)...\n");

        using namespace lsp;
        generic::bitmap_part_t part;
        dsp::bitmap_t dst, src;

        // Case 1
        dst  = { 60, 40, 60, NULL };
        src  = { 40, 20, 40, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 10, 10);
        UTEST_ASSERT(part.src_x == 0);
        UTEST_ASSERT(part.src_y == 0);
        UTEST_ASSERT(part.dst_x == 10);
        UTEST_ASSERT(part.dst_y == 10);
        UTEST_ASSERT(part.count_x == 40);
        UTEST_ASSERT(part.count_y == 20);

        // Case 2
        dst  = { 60, 40, 60, NULL };
        src  = { 40, 20, 40, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, -10, 10);
        UTEST_ASSERT(part.src_x == 10);
        UTEST_ASSERT(part.src_y == 0);
        UTEST_ASSERT(part.dst_x == 0);
        UTEST_ASSERT(part.dst_y == 10);
        UTEST_ASSERT(part.count_x == 30);
        UTEST_ASSERT(part.count_y == 20);

        // Case 3
        dst  = { 60, 40, 60, NULL };
        src  = { 40, 20, 40, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 30, 10);
        UTEST_ASSERT(part.src_x == 0);
        UTEST_ASSERT(part.src_y == 0);
        UTEST_ASSERT(part.dst_x == 30);
        UTEST_ASSERT(part.dst_y == 10);
        UTEST_ASSERT(part.count_x == 30);
        UTEST_ASSERT(part.count_y == 20);

        // Case 4
        dst  = { 60, 40, 60, NULL };
        src  = { 40, 20, 40, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 10, -10);
        UTEST_ASSERT(part.src_x == 0);
        UTEST_ASSERT(part.src_y == 10);
        UTEST_ASSERT(part.dst_x == 10);
        UTEST_ASSERT(part.dst_y == 0);
        UTEST_ASSERT(part.count_x == 40);
        UTEST_ASSERT(part.count_y == 10);

        // Case 5
        dst  = { 60, 40, 60, NULL };
        src  = { 40, 20, 40, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 10, 30);
        UTEST_ASSERT(part.src_x == 0);
        UTEST_ASSERT(part.src_y == 0);
        UTEST_ASSERT(part.dst_x == 10);
        UTEST_ASSERT(part.dst_y == 30);
        UTEST_ASSERT(part.count_x == 40);
        UTEST_ASSERT(part.count_y == 10);
    }

    void test_correct_clipping2()
    {
        printf("Testing correct clipping (case 2)...\n");

        using namespace lsp;
        generic::bitmap_part_t part;
        dsp::bitmap_t dst, src;

        // Case 1
        dst  = { 40, 40, 40, NULL };
        src  = { 80, 20, 80, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, -20, 10);
        UTEST_ASSERT(part.src_x == 20);
        UTEST_ASSERT(part.src_y == 0);
        UTEST_ASSERT(part.dst_x == 0);
        UTEST_ASSERT(part.dst_y == 10);
        UTEST_ASSERT(part.count_x == 40);
        UTEST_ASSERT(part.count_y == 20);

        // Case 2
        dst  = { 40, 40, 40, NULL };
        src  = { 20, 80, 20, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 10, -20);
        UTEST_ASSERT(part.src_x == 0);
        UTEST_ASSERT(part.src_y == 20);
        UTEST_ASSERT(part.dst_x == 10);
        UTEST_ASSERT(part.dst_y == 0);
        UTEST_ASSERT(part.count_x == 20);
        UTEST_ASSERT(part.count_y == 40);

        // Case 3
        dst  = { 40, 40, 40, NULL };
        src  = { 60, 60, 60, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, -10, -10);
        UTEST_ASSERT(part.src_x == 10);
        UTEST_ASSERT(part.src_y == 10);
        UTEST_ASSERT(part.dst_x == 0);
        UTEST_ASSERT(part.dst_y == 0);
        UTEST_ASSERT(part.count_x == 40);
        UTEST_ASSERT(part.count_y == 40);
    }

    void test_incorrect_clipping()
    {
        printf("Testing incorrect clipping...\n");

        using namespace lsp;
        generic::bitmap_part_t part;
        dsp::bitmap_t dst, src;

        // Case 1
        dst  = { 40, 40, 40, NULL };
        src  = { 10, 10, 10, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, -10, 0);
        UTEST_ASSERT((part.count_x <= 0) || (part.count_y <= 0));

        // Case 2
        dst  = { 40, 40, 40, NULL };
        src  = { 10, 10, 10, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 40, 0);
        UTEST_ASSERT((part.count_x <= 0) || (part.count_y <= 0));

        // Case 3
        dst  = { 40, 40, 40, NULL };
        src  = { 10, 10, 10, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 0, -10);
        UTEST_ASSERT((part.count_x <= 0) || (part.count_y <= 0));

        // Case 4
        dst  = { 40, 40, 40, NULL };
        src  = { 10, 10, 10, NULL };
        part = generic::bitmap_clip_rect(&dst, &src, 0, 40);
        UTEST_ASSERT((part.count_x <= 0) || (part.count_y <= 0));
    }

    UTEST_MAIN
    {
        test_correct_clipping1();
        test_correct_clipping2();
        test_incorrect_clipping();
    }

UTEST_END;

