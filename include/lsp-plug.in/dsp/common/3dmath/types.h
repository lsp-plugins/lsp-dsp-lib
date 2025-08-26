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

#ifndef LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_
#define LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_

#include <lsp-plug.in/dsp/common/types.h>

#define DSP_3D_TOLERANCE        1e-5f
#define DSP_3D_SQR_TOLERANCE    0.00316227766017f
#define DSP_3D_MAXVALUE         1e+20f
#define DSP_3D_MAXISECT         8

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)
typedef struct LSP_DSP_LIB_TYPE(point3d_t)
{
    float       x, y, z, w;
} LSP_DSP_LIB_TYPE(point3d_t);

typedef struct LSP_DSP_LIB_TYPE(vector3d_t)
{
    float       dx, dy, dz, dw;
} LSP_DSP_LIB_TYPE(vector3d_t);

typedef struct LSP_DSP_LIB_TYPE(color3d_t)
{
    float       r, g, b, a;
} LSP_DSP_LIB_TYPE(color3d_t);

typedef struct LSP_DSP_LIB_TYPE(bound_box3d_t)
{
    LSP_DSP_LIB_TYPE(point3d_t)     p[8];       // Bounding-box contains 8 points
} LSP_DSP_LIB_TYPE(bound_box3d_t);

typedef struct LSP_DSP_LIB_TYPE(matrix3d_t)
{
    float       m[16];      // Matrix data
} LSP_DSP_LIB_TYPE(matrix3d_t);

typedef struct LSP_DSP_LIB_TYPE(raw_triangle_t)
{
    LSP_DSP_LIB_TYPE(point3d_t)     v[3];
} LSP_DSP_LIB_TYPE(raw_triangle_t);

#pragma pack(pop)

typedef enum LSP_DSP_LIB_TYPE(axis_orientation_t)
{
    AO3D_POS_X_FWD_POS_Y_UP,
    AO3D_POS_X_FWD_POS_Z_UP,
    AO3D_POS_X_FWD_NEG_Y_UP,
    AO3D_POS_X_FWD_NEG_Z_UP,
    AO3D_NEG_X_FWD_POS_Y_UP,
    AO3D_NEG_X_FWD_POS_Z_UP,
    AO3D_NEG_X_FWD_NEG_Y_UP,
    AO3D_NEG_X_FWD_NEG_Z_UP,

    AO3D_POS_Y_FWD_POS_X_UP,
    AO3D_POS_Y_FWD_POS_Z_UP,
    AO3D_POS_Y_FWD_NEG_X_UP,
    AO3D_POS_Y_FWD_NEG_Z_UP,
    AO3D_NEG_Y_FWD_POS_X_UP,
    AO3D_NEG_Y_FWD_POS_Z_UP,
    AO3D_NEG_Y_FWD_NEG_X_UP,
    AO3D_NEG_Y_FWD_NEG_Z_UP,

    AO3D_POS_Z_FWD_POS_X_UP,
    AO3D_POS_Z_FWD_POS_Y_UP,
    AO3D_POS_Z_FWD_NEG_X_UP,
    AO3D_POS_Z_FWD_NEG_Y_UP,
    AO3D_NEG_Z_FWD_POS_X_UP,
    AO3D_NEG_Z_FWD_POS_Y_UP,
    AO3D_NEG_Z_FWD_NEG_X_UP,
    AO3D_NEG_Z_FWD_NEG_Y_UP
} LSP_DSP_LIB_TYPE(axis_orientation_t);

LSP_DSP_LIB_END_NAMESPACE

#endif /* LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_ */
