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

/**
 * Compressor knee is a curve that consists of three parts:
 *   1. Part with constant gain amplification in the range [-inf .. start] dB
 *   2. Soft compression knee in the range (start .. end) dB present by the quadratic function (2nd-order polynom)
 *   3. Gain reduction part in the range [end .. +inf] dB present by the linear function (1st-order polynom)
 *
 * The typical algorithm of computing the compressor's curve:
 *   1. Take absolute value of the sample: x = fabfs(in)
 *   2. If x <= start then return gain*x
 *   3. Compute the natural logarithm of the x: lx = logf(x).
 *   4. If x < end then compute the gain using the 2nd-order polynom: gain = (herm[0]*lx + herm[1])*lx + herm[2]
 *   5. Otherwise compute the gain using the 1st-order polynom: gain = tilt[0]*lx + tilt[1]
 *   6. return expf(gain) * x
 */
typedef struct LSP_DSP_LIB_TYPE(compressor_knee_t)
{
    float       start;          // The start of the knee, in gain units
    float       end;            // The end of the knee, in gain units
    float       gain;           // Pre-amplification gain
    float       herm[3];        // Hermite interpolation of the knee with the 2nd-order polynom
    float       tilt[2];        // Tilt line parameters after the knee
} LSP_DSP_LIB_TYPE(compressor_knee_t);


/**
 * Two-knee compressor.
 * The result gain/curve is a result of multiplication of gain/curve between both knees.
 */
typedef struct LSP_DSP_LIB_TYPE(compressor_x2_t)
{
    LSP_DSP_LIB_TYPE(compressor_knee_t)   k[2];
} LSP_DSP_LIB_TYPE(compressor_x2_t);


/**
 * Gate knee is a curve that consists of three parts:
 *   1. Part with constant gain amplification in the range [-inf .. start] dB
 *   2. Transition zone in the range (start .. end) dB present by the quadratic function (2nd-order polynom)
 *   3. Part with constant gain amplification in the range [end .. +inf] dB
 *
 * The typical algorithm of computing the gate's curve:
 *   1. Take absolute value of the sample: x = fabfs(in)
 *   2. If x <= start then return gain_start * x
 *   3. If x <= end then return gain_end * x
 *   4. Compute the natural logarithm of the x: lx = logf(x).
 *   5. Compute the gain using the 3rd-order polynom: gain = ((herm[0]*lx + herm[1])*lx + herm[2]*lx) + herm[3]
 *   6. return expf(gain) * x
 */
typedef struct LSP_DSP_LIB_TYPE(gate_knee_t)
{
    float       start;          // The start of the knee, in gain units
    float       end;            // The end of the knee, in gain units
    float       gain_start;     // Gain below the start threshold
    float       gain_end;       // Gain above the end threshold
    float       herm[4];        // Hermite interpolation of the knee with the 3rd-order polynom
} LSP_DSP_LIB_TYPE(gate_knee_t);

/**
 * Gate knee is a curve that consists of three parts:
 *   1. Part with constant gain amplification
 *   2. Soft compression knee
 *   3. Gain reduction part
 *
 * The order of these parts can be direct for upward expander and reverse for downward expander.
 *
 * The typical algorithm of computing the expander's curve:
 *   a. for upward expander:
 *     1. Take absolute value of the sample: x = fabfs(in)
 *     2. If x >= threshold then assume x = threshold
 *     3. If x <= start then return x
 *     4. Compute the natural logarithm of the x: lx = logf(x).
 *     5. If x < end then compute the gain using the 2nd-order polynom: gain = (herm[0]*lx + herm[1])*lx + herm[2]
 *     6. Otherwise compute the gain using the 1st-order polynom: gain = tilt[0]*lx + tilt[1]
 *     7. return expf(gain)
 *   b. for downward expander:
 *     1. Take absolute value of the sample: x = fabfs(in)
 *     2. If x < threshold then return 0.
 *     3. If x >= end then return x
 *     4. Compute the natural logarithm of the x: lx = logf(x).
 *     5. If x > start then compute the gain using the 2nd-order polynom: gain = (herm[0]*lx + herm[1])*lx + herm[2]
 *     6. Otherwise compute the gain using the 1st-order polynom: gain = tilt[0]*lx + tilt[1]
 *     7. return expf(gain)
 */
typedef struct LSP_DSP_LIB_TYPE(expander_knee_t)
{
    float       start;          // The start of the knee, in gain units
    float       end;            // The end of the knee, in gain units
    float       threshold;      // The threshold to limit the expander effect and prevent from +Inf/-Inf values
    float       herm[3];        // Hermite interpolation of the knee with the 2nd-order polynom
    float       tilt[2];        // Tilt interpolation
} LSP_DSP_LIB_TYPE(expander_knee_t);

#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

#endif /* LSP_PLUG_IN_DSP_COMMON_DYNAMICS_TYPES_H_ */
