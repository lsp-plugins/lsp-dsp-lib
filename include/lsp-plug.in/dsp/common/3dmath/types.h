/*
 * types.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_
#define LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_

#include <lsp-plug.in/dsp/common/types.h>

#define DSP_3D_TOLERANCE        1e-5f
#define DSP_3D_SQR_TOLERANCE    0.00316227766017f
#define DSP_3D_MAXVALUE         1e+20f
#define DSP_3D_MAXISECT         8

#ifdef __cplusplus
namespace lsp
{
    namespace dsp
    {
#endif /* __cplusplus */

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

        typedef struct LSP_DSP_LIB_TYPE(ray3d_t)
        {
            LSP_DSP_LIB_TYPE(point3d_t)     z;          // The start point: x, y, z = point coordinates
            LSP_DSP_LIB_TYPE(vector3d_t)    v;          // The spread vector: dx, dy, dz = direction
        } LSP_DSP_LIB_TYPE(ray3d_t);

        typedef struct LSP_DSP_LIB_TYPE(bound_box3d_t)
        {
            LSP_DSP_LIB_TYPE(point3d_t)     p[8];       // Bounding-box contains 8 points
        } LSP_DSP_LIB_TYPE(bound_box3d_t);

        typedef struct LSP_DSP_LIB_TYPE(segment3d_t)
        {
            LSP_DSP_LIB_TYPE(point3d_t)     p[2];
        } LSP_DSP_LIB_TYPE(segment3d_t);

        typedef struct LSP_DSP_LIB_TYPE(triangle3d_t)
        {
            LSP_DSP_LIB_TYPE(point3d_t)     p[3];       // Vertexes: x, y, z = vertex coordinates, w = length of the corresponding edge
            LSP_DSP_LIB_TYPE(vector3d_t)    n;          // Normal: dx, dy, dz = normal coordinates, dw = plane equation coefficient
        } LSP_DSP_LIB_TYPE(triangle3d_t);

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

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

#endif /* LSP_PLUG_IN_DSP_COMMON_3DMATH_TYPES_H_ */
