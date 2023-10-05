/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 5 окт. 2023 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_DYNAMICS_TYPES_H_
#define LSP_PLUG_IN_DSP_COMMON_DYNAMICS_TYPES_H_

#include <lsp-plug.in/dsp/common/types.h>

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)

typedef struct LSP_DSP_LIB_TYPE(compressor_knee_t)
{
    float       start;          // The start of the knee
    float       end;            // the end of the knee
    float       gain;           // Pre-amplification gain
    float       herm[3];        // Hermite interpolation of the knee with the 2nd-order polynom
    float       tilt[2];        // Tilt line parameters after the knee
} LSP_DSP_LIB_TYPE(compressor_knee_t);


/**
 * Two-knee compressor
 */
typedef struct LSP_DSP_LIB_TYPE(compressor_x2_t)
{
    LSP_DSP_LIB_TYPE(compressor_knee_t)   k[2];
} LSP_DSP_LIB_TYPE(compressor_x2_t);

#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

#endif /* LSP_PLUG_IN_DSP_COMMON_DYNAMICS_TYPES_H_ */
