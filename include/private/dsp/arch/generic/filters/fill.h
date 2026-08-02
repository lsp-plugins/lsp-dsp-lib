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

#ifndef PRIVATE_DSP_ARCH_GENERIC_FILTERS_FILL_H_
#define PRIVATE_DSP_ARCH_GENERIC_FILTERS_FILL_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <private/dsp/arch/generic/filters/common.h>

namespace lsp
{
    namespace generic
    {
        static const f_cascade_t fcascade_normal __lsp_aligned16 =
        {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f, 0.0f }
        };

        void fcascade_fill_x2(f_cascade_t *h, f_cascade_t *t)
        {
            h[1]    = fcascade_normal;
            t[0]    = fcascade_normal;
        }

        void fcascade_fill_x4(f_cascade_t *h, f_cascade_t *t)
        {
            h[1] = fcascade_normal; h[2] = fcascade_normal; h[3] = fcascade_normal; // row 0
            h[6] = fcascade_normal; h[7] = fcascade_normal; // row 1
            h[11] = fcascade_normal; // row 2

            t[0] = fcascade_normal; // row -3
            t[4] = fcascade_normal; t[5] = fcascade_normal; // row -2
            t[8] = fcascade_normal; t[9] = fcascade_normal; t[10] = fcascade_normal; // row -1
        }

        void fcascade_fill_x8(f_cascade_t *h, f_cascade_t *t)
        {
            // row 0
            h[1] = fcascade_normal; h[2] = fcascade_normal; h[3] = fcascade_normal; h[4] = fcascade_normal;
            h[5] = fcascade_normal; h[6] = fcascade_normal; h[7] = fcascade_normal;
            // row 1
            h[10] = fcascade_normal; h[11] = fcascade_normal; h[12] = fcascade_normal;
            h[13] = fcascade_normal; h[14] = fcascade_normal; h[15] = fcascade_normal;
            // row 2
            h[19] = fcascade_normal; h[20] = fcascade_normal; h[21] = fcascade_normal;
            h[22] = fcascade_normal; h[23] = fcascade_normal;
            // row 3
            h[28] = fcascade_normal; h[29] = fcascade_normal;
            h[30] = fcascade_normal; h[31] = fcascade_normal;
            // row 4
            h[37] = fcascade_normal; h[38] = fcascade_normal;
            h[39] = fcascade_normal;
            // row 5
            h[46] = fcascade_normal; h[47] = fcascade_normal;
            // Row 6
            h[55] = fcascade_normal; // row 6

            // row -7
            t[0] = fcascade_normal;
            // row -6
            t[8] = fcascade_normal; t[9] = fcascade_normal;
            // row -5
            t[16] = fcascade_normal; t[17] = fcascade_normal; t[18] = fcascade_normal;
            // row -4
            t[24] = fcascade_normal; t[25] = fcascade_normal;
            t[26] = fcascade_normal; t[27] = fcascade_normal;
            // row -3
            t[32] = fcascade_normal; t[33] = fcascade_normal; t[34] = fcascade_normal;
            t[35] = fcascade_normal; t[36] = fcascade_normal;
            // row -2
            t[40] = fcascade_normal; t[41] = fcascade_normal; t[42] = fcascade_normal;
            t[43] = fcascade_normal; t[44] = fcascade_normal; t[45] = fcascade_normal;
            // row -1
            t[48] = fcascade_normal; t[49] = fcascade_normal; t[50] = fcascade_normal; t[51] = fcascade_normal;
            t[52] = fcascade_normal; t[53] = fcascade_normal; t[54] = fcascade_normal;
        }

        void fcascade_fill_x16(f_cascade_t *h, f_cascade_t *t)
        {
            // row 0
            h[0x01] = fcascade_normal; h[0x02] = fcascade_normal; h[0x03] = fcascade_normal; h[0x04] = fcascade_normal;
            h[0x05] = fcascade_normal; h[0x06] = fcascade_normal; h[0x07] = fcascade_normal; h[0x08] = fcascade_normal;
            h[0x09] = fcascade_normal; h[0x0a] = fcascade_normal; h[0x0b] = fcascade_normal; h[0x0c] = fcascade_normal;
            h[0x0d] = fcascade_normal; h[0x0e] = fcascade_normal; h[0x0f] = fcascade_normal;
            // row 1
            h[0x12] = fcascade_normal; h[0x13] = fcascade_normal; h[0x14] = fcascade_normal;
            h[0x15] = fcascade_normal; h[0x16] = fcascade_normal; h[0x17] = fcascade_normal; h[0x18] = fcascade_normal;
            h[0x19] = fcascade_normal; h[0x1a] = fcascade_normal; h[0x1b] = fcascade_normal; h[0x1c] = fcascade_normal;
            h[0x1d] = fcascade_normal; h[0x1e] = fcascade_normal; h[0x1f] = fcascade_normal;
            // row 2
            h[0x23] = fcascade_normal; h[0x24] = fcascade_normal;
            h[0x25] = fcascade_normal; h[0x26] = fcascade_normal; h[0x27] = fcascade_normal; h[0x28] = fcascade_normal;
            h[0x29] = fcascade_normal; h[0x2a] = fcascade_normal; h[0x2b] = fcascade_normal; h[0x2c] = fcascade_normal;
            h[0x2d] = fcascade_normal; h[0x2e] = fcascade_normal; h[0x2f] = fcascade_normal;
            // row 3
            h[0x34] = fcascade_normal;
            h[0x35] = fcascade_normal; h[0x36] = fcascade_normal; h[0x37] = fcascade_normal; h[0x38] = fcascade_normal;
            h[0x39] = fcascade_normal; h[0x3a] = fcascade_normal; h[0x3b] = fcascade_normal; h[0x3c] = fcascade_normal;
            h[0x3d] = fcascade_normal; h[0x3e] = fcascade_normal; h[0x3f] = fcascade_normal;
            // row 4
            h[0x45] = fcascade_normal; h[0x46] = fcascade_normal; h[0x47] = fcascade_normal; h[0x48] = fcascade_normal;
            h[0x49] = fcascade_normal; h[0x4a] = fcascade_normal; h[0x4b] = fcascade_normal; h[0x4c] = fcascade_normal;
            h[0x4d] = fcascade_normal; h[0x4e] = fcascade_normal; h[0x4f] = fcascade_normal;
            // row 5
            h[0x56] = fcascade_normal; h[0x57] = fcascade_normal; h[0x58] = fcascade_normal;
            h[0x59] = fcascade_normal; h[0x5a] = fcascade_normal; h[0x5b] = fcascade_normal; h[0x5c] = fcascade_normal;
            h[0x5d] = fcascade_normal; h[0x5e] = fcascade_normal; h[0x5f] = fcascade_normal;
            // row 6
            h[0x67] = fcascade_normal; h[0x68] = fcascade_normal;
            h[0x69] = fcascade_normal; h[0x6a] = fcascade_normal; h[0x6b] = fcascade_normal; h[0x6c] = fcascade_normal;
            h[0x6d] = fcascade_normal; h[0x6e] = fcascade_normal; h[0x6f] = fcascade_normal;
            // row 7
            h[0x78] = fcascade_normal;
            h[0x79] = fcascade_normal; h[0x7a] = fcascade_normal; h[0x7b] = fcascade_normal; h[0x7c] = fcascade_normal;
            h[0x7d] = fcascade_normal; h[0x7e] = fcascade_normal; h[0x7f] = fcascade_normal;
            // row 8
            h[0x89] = fcascade_normal; h[0x8a] = fcascade_normal; h[0x8b] = fcascade_normal; h[0x8c] = fcascade_normal;
            h[0x8d] = fcascade_normal; h[0x8e] = fcascade_normal; h[0x8f] = fcascade_normal;
            // row 9
            h[0x9a] = fcascade_normal; h[0x9b] = fcascade_normal; h[0x9c] = fcascade_normal;
            h[0x9d] = fcascade_normal; h[0x9e] = fcascade_normal; h[0x9f] = fcascade_normal;
            // row 10
            h[0xab] = fcascade_normal; h[0xac] = fcascade_normal;
            h[0xad] = fcascade_normal; h[0xae] = fcascade_normal; h[0xaf] = fcascade_normal;
            // row 11
            h[0xbc] = fcascade_normal;
            h[0xbd] = fcascade_normal; h[0xbe] = fcascade_normal; h[0xbf] = fcascade_normal;
            // row 12
            h[0xcd] = fcascade_normal; h[0xce] = fcascade_normal; h[0xcf] = fcascade_normal;
            // row 13
            h[0xde] = fcascade_normal; h[0xdf] = fcascade_normal;
            // row 14
            h[0xef] = fcascade_normal;

            // row -15
            t[0x00] = fcascade_normal;
            // row -14
            t[0x10] = fcascade_normal; t[0x11] = fcascade_normal;
            // row -13
            t[0x20] = fcascade_normal; t[0x21] = fcascade_normal; t[0x22] = fcascade_normal;
            // row -12
            t[0x30] = fcascade_normal; t[0x31] = fcascade_normal; t[0x32] = fcascade_normal; t[0x33] = fcascade_normal;
            // row -11
            t[0x40] = fcascade_normal; t[0x41] = fcascade_normal; t[0x42] = fcascade_normal; t[0x43] = fcascade_normal;
            t[0x44] = fcascade_normal;
            // row -10
            t[0x50] = fcascade_normal; t[0x51] = fcascade_normal; t[0x52] = fcascade_normal; t[0x53] = fcascade_normal;
            t[0x54] = fcascade_normal; t[0x55] = fcascade_normal;
            // row -9
            t[0x60] = fcascade_normal; t[0x61] = fcascade_normal; t[0x62] = fcascade_normal; t[0x63] = fcascade_normal;
            t[0x64] = fcascade_normal; t[0x65] = fcascade_normal; t[0x66] = fcascade_normal;
            // row -8
            t[0x70] = fcascade_normal; t[0x71] = fcascade_normal; t[0x72] = fcascade_normal; t[0x73] = fcascade_normal;
            t[0x74] = fcascade_normal; t[0x75] = fcascade_normal; t[0x76] = fcascade_normal; t[0x77] = fcascade_normal;
            // row -7
            t[0x80] = fcascade_normal; t[0x81] = fcascade_normal; t[0x82] = fcascade_normal; t[0x83] = fcascade_normal;
            t[0x84] = fcascade_normal; t[0x85] = fcascade_normal; t[0x86] = fcascade_normal; t[0x87] = fcascade_normal;
            t[0x88] = fcascade_normal;
            // row -6
            t[0x90] = fcascade_normal; t[0x91] = fcascade_normal; t[0x92] = fcascade_normal; t[0x93] = fcascade_normal;
            t[0x94] = fcascade_normal; t[0x95] = fcascade_normal; t[0x96] = fcascade_normal; t[0x97] = fcascade_normal;
            t[0x98] = fcascade_normal; t[0x99] = fcascade_normal;
            // row -5
            t[0xa0] = fcascade_normal; t[0xa1] = fcascade_normal; t[0xa2] = fcascade_normal; t[0xa3] = fcascade_normal;
            t[0xa4] = fcascade_normal; t[0xa5] = fcascade_normal; t[0xa6] = fcascade_normal; t[0xa7] = fcascade_normal;
            t[0xa8] = fcascade_normal; t[0xa9] = fcascade_normal; t[0xaa] = fcascade_normal;
            // row -4
            t[0xb0] = fcascade_normal; t[0xb1] = fcascade_normal; t[0xb2] = fcascade_normal; t[0xb3] = fcascade_normal;
            t[0xb4] = fcascade_normal; t[0xb5] = fcascade_normal; t[0xb6] = fcascade_normal; t[0xb7] = fcascade_normal;
            t[0xb8] = fcascade_normal; t[0xb9] = fcascade_normal; t[0xba] = fcascade_normal; t[0xbb] = fcascade_normal;
            // row -3
            t[0xc0] = fcascade_normal; t[0xc1] = fcascade_normal; t[0xc2] = fcascade_normal; t[0xc3] = fcascade_normal;
            t[0xc4] = fcascade_normal; t[0xc5] = fcascade_normal; t[0xc6] = fcascade_normal; t[0xc7] = fcascade_normal;
            t[0xc8] = fcascade_normal; t[0xc9] = fcascade_normal; t[0xca] = fcascade_normal; t[0xcb] = fcascade_normal;
            t[0xcc] = fcascade_normal;
            // row -2
            t[0xd0] = fcascade_normal; t[0xd1] = fcascade_normal; t[0xd2] = fcascade_normal; t[0xd3] = fcascade_normal;
            t[0xd4] = fcascade_normal; t[0xd5] = fcascade_normal; t[0xd6] = fcascade_normal; t[0xd7] = fcascade_normal;
            t[0xd8] = fcascade_normal; t[0xd9] = fcascade_normal; t[0xda] = fcascade_normal; t[0xdb] = fcascade_normal;
            t[0xdc] = fcascade_normal; t[0xdd] = fcascade_normal;
            // row -1
            t[0xe0] = fcascade_normal; t[0xe1] = fcascade_normal; t[0xe2] = fcascade_normal; t[0xe3] = fcascade_normal;
            t[0xe4] = fcascade_normal; t[0xe5] = fcascade_normal; t[0xe6] = fcascade_normal; t[0xe7] = fcascade_normal;
            t[0xe8] = fcascade_normal; t[0xe9] = fcascade_normal; t[0xea] = fcascade_normal; t[0xeb] = fcascade_normal;
            t[0xec] = fcascade_normal; t[0xed] = fcascade_normal; t[0xee] = fcascade_normal;
        }

    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_FILTERS_FILL_H_ */
