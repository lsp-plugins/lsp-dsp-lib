/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_DSP_COMMON_3DMATH_H_
#define LSP_PLUG_IN_DSP_COMMON_3DMATH_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/3dmath/types.h>

/** Init point using coordinates
 *
 * @param p point to initialize
 * @param x X coordinate
 * @param y Y coordinate
 * @param z Z coordinate
 */
LSP_DSP_LIB_SYMBOL(void, init_point_xyz, LSP_DSP_LIB_TYPE(point3d_t) *p, float x, float y, float z);

/** Init point using another point
 *
 * @param p point to initialize
 * @param s source point
 */
LSP_DSP_LIB_SYMBOL(void, init_point, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(point3d_t) *s);

/** Normalize point coordinates
 *
 * @param p point to normalize
 */
LSP_DSP_LIB_SYMBOL(void, normalize_point, LSP_DSP_LIB_TYPE(point3d_t) *p);

/** Normalize point coordinates to specified radius-vector length
 *
 * @param p point to scale
 * @param r radius-vector length
 */
LSP_DSP_LIB_SYMBOL(void, scale_point1, LSP_DSP_LIB_TYPE(point3d_t) *p, float r);

/** Scale point coordinates to match specified radius-vector length
 *
 * @param p point to store result
 * @param s point to scale
 * @param r radius-vector length
 */
LSP_DSP_LIB_SYMBOL(void, scale_point2, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(point3d_t) *s, float r);

/** Initialize vector
 *
 * @param v vector to initialize
 * @param dx delta coordinate X
 * @param dy delta coordinate Y
 * @param dz delta coordinate Y
 */
LSP_DSP_LIB_SYMBOL(void, init_vector_dxyz, LSP_DSP_LIB_TYPE(vector3d_t) *v, float dx, float dy, float dz);

/** Initialize vector
 *
 * @param v destination vector
 * @param s source vector
 */
LSP_DSP_LIB_SYMBOL(void, init_vector, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *s);

/** Initialize vector
 *
 * @param v destination vector
 * @param p1 start point of vector
 * @param p2 end point of vector
 */
LSP_DSP_LIB_SYMBOL(void, init_vector_p2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/** Initialize vector
 *
 * @param v destination vector
 * @param pv array of two points
 */
LSP_DSP_LIB_SYMBOL(void, init_vector_pv, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/** Initialize normal vector
 *
 * @param v vector to initialize
 * @param x1 point 1 x
 * @param y1 point 1 y
 * @param z1 point 1 z
 * @param x2 point 2 x
 * @param y2 point 2 y
 * @param z2 point 2 z
 */
LSP_DSP_LIB_SYMBOL(void, init_normal3d_xyz, LSP_DSP_LIB_TYPE(vector3d_t) *v, float x1, float y1, float z1, float x2, float y2, float z2);

/** Initialize normal vector
 *
 * @param v vector to initialize
 * @param dx delta coordinate X
 * @param dy delta coordinate Y
 * @param dz delta coordinate Y
 */
LSP_DSP_LIB_SYMBOL(void, init_normal3d_dxyz, LSP_DSP_LIB_TYPE(vector3d_t) *v, float dx, float dy, float dz);

/** Initialize normal vector
 *
 * @param p source vector
 * @param s destination vector
 */
LSP_DSP_LIB_SYMBOL(void, init_normal3d, LSP_DSP_LIB_TYPE(vector3d_t) *p, const LSP_DSP_LIB_TYPE(vector3d_t) *s);

/** Normalize vector
 *
 * @param v vector to normalize
 */
LSP_DSP_LIB_SYMBOL(void, normalize_vector, LSP_DSP_LIB_TYPE(vector3d_t) *m);

/** Normalize vector
 *
 * @param v vector to store normalized value
 * @param src source vector to normalize
 */
LSP_DSP_LIB_SYMBOL(void, normalize_vector2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *src);

/**
 * Flip vector coordinates
 * @param v vector to flip
 */
LSP_DSP_LIB_SYMBOL(void, flip_vector_v1, LSP_DSP_LIB_TYPE(vector3d_t) *v);

/**
 * Flip vector coordinates
 * @param v vector to store result
 * @param sv source vector
 */
LSP_DSP_LIB_SYMBOL(void, flip_vector_v2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *sv);

/** Scale vector coordinates to match specified radius-vector length
 *
 * @param v vector to scale
 * @param r radius-vector length
 */
LSP_DSP_LIB_SYMBOL(void, scale_vector1, LSP_DSP_LIB_TYPE(vector3d_t) *v, float r);

/** Scale vector coordinates to match specified radius-vector length
 *
 * @param v vector to store result
 * @param s vector to scale
 * @param r radius-vector length
 */
LSP_DSP_LIB_SYMBOL(void, scale_vector2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *s, float r);

/** Calculate vector multiplications
 *
 * @param r vector to store result
 * @param v1 vector 1
 * @param v2 vector 2
 */
LSP_DSP_LIB_SYMBOL(void, vector_mul_v2, LSP_DSP_LIB_TYPE(vector3d_t) *r, const LSP_DSP_LIB_TYPE(vector3d_t) *v1, const LSP_DSP_LIB_TYPE(vector3d_t) *v2);

/** Calculate vector multiplications
 *
 * @param r vector to store result
 * @param vv array of two vectors to multiply
 */
LSP_DSP_LIB_SYMBOL(void, vector_mul_vv, LSP_DSP_LIB_TYPE(vector3d_t) *r, const LSP_DSP_LIB_TYPE(vector3d_t) *vv);

/** Initialize matrix (make copy)
 *
 * @param dst destination matrix
 * @param src source matrix
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d, LSP_DSP_LIB_TYPE(matrix3d_t) *dst, const LSP_DSP_LIB_TYPE(matrix3d_t) *src);

/** Zero matrix
 *
 * @param m target matrix
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_zero, LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Fill matrix with ones
 *
 * @param m target matrix
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_one, LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Fill matrix with identity values
 *
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_identity, LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Init matrix translation
 *
 * @param m matrix
 * @param dx translation X
 * @param dy translation Y
 * @param dz translation Z
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_translate, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float dx, float dy, float dz);

/** Init matrix translation
 *
 * @param m matrix
 * @param p point that defines translation
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_translate_p1, LSP_DSP_LIB_TYPE(matrix3d_t) *m, const LSP_DSP_LIB_TYPE(point3d_t) *p);

/**
 * Init matrix translation
 * @param m matrix
 * @param v vector that defines translation
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_translate_v1, LSP_DSP_LIB_TYPE(matrix3d_t) *m, const LSP_DSP_LIB_TYPE(vector3d_t) *v);

/** Init matrix scale
 *
 * @param m matrix
 * @param sx size X
 * @param sy size Y
 * @param sz size Z
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_scale, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float sx, float sy, float sz);

/** Initialize rotation matrix around X axis
 *
 * @param m matrix
 * @param angle angle
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_rotate_x, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float angle);

/** Initialize rotation matrix around Y axis
 *
 * @param m matrix
 * @param angle angle
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_rotate_y, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float angle);

/** Initialize rotation matrix around Z axis
 *
 * @param m matrix
 * @param angle angle
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_rotate_z, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float angle);

/** Initialize rotation matrix around vector
 *
 * @param m matrix
 * @param x vector X
 * @param y vector Y
 * @param z vector Z
 * @param angle angle
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_rotate_xyz, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float x, float y, float z, float angle);

/**
 * Initialize projection matrix according to the glFrustum() specification
 * @param m target matrix to store values
 * @param left coordinates for the left vertical clipping plane
 * @param right coordinates for the right vertical clipping plane
 * @param bottom coordinates for the bottom clipping plane
 * @param top coordinates for the top clipping plane
 * @param near distance to the near clipping plane
 * @param far distance to the far clipping plane
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_frustum, LSP_DSP_LIB_TYPE(matrix3d_t) *m, float left, float right, float bottom, float top, float near, float far);

/**
 * Initialize matrix similar to gluPerspective()
 * @param m target matrix to store values
 * @param pov point-of view coordinates
 * @param fwd direction of view (vector)
 * @param up the up vector
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_lookat_p1v2, LSP_DSP_LIB_TYPE(matrix3d_t) *m, const LSP_DSP_LIB_TYPE(point3d_t) *pov, const LSP_DSP_LIB_TYPE(vector3d_t) *fwd, const LSP_DSP_LIB_TYPE(vector3d_t) *up);

/**
 * Initialize matrix similar to gluPerspective()
 * @param m target matrix to store values
 * @param pov point-of view coordinates
 * @param pod point-of-destination coordinates
 * @param up the up vector
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_lookat_p2v1, LSP_DSP_LIB_TYPE(matrix3d_t) *m, const LSP_DSP_LIB_TYPE(point3d_t) *pov, const LSP_DSP_LIB_TYPE(point3d_t) *pod, const LSP_DSP_LIB_TYPE(vector3d_t) *up);

/**
 * Initialize matrix that changes ortogonal orientation
 * @param m matrix to initialize
 * @param orientation axis orientation
 */
LSP_DSP_LIB_SYMBOL(void, init_matrix3d_orientation, LSP_DSP_LIB_TYPE(matrix3d_t) *m, LSP_DSP_LIB_TYPE(axis_orientation_t) orientation);

/** Apply matrix to vector
 *
 * @param r target vector
 * @param v source vector
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mv2, LSP_DSP_LIB_TYPE(vector3d_t) *r, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Apply matrix to vector
 *
 * @param r target vector
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mv1, LSP_DSP_LIB_TYPE(vector3d_t) *r, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Apply matrix to point
 *
 * @param r target point
 * @param v source point
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mp2, LSP_DSP_LIB_TYPE(point3d_t) *r, const LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Apply matrix to point
 *
 * @param r target point
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mp1, LSP_DSP_LIB_TYPE(point3d_t) *r, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Apply matrix to matrix (calculate matrix multiplication)
 *
 * @param r target matrix
 * @param s source matrix
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mm2, LSP_DSP_LIB_TYPE(matrix3d_t) *r, const LSP_DSP_LIB_TYPE(matrix3d_t) *s, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Apply matrix to matrix (calculate matrix multiplication)
 *
 * @param r target matrix
 * @param m matrix
 */
LSP_DSP_LIB_SYMBOL(void, apply_matrix3d_mm1, LSP_DSP_LIB_TYPE(matrix3d_t) *r, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Transpose matrix
 *
 * @param r target matrix
 */
LSP_DSP_LIB_SYMBOL(void, transpose_matrix3d1, LSP_DSP_LIB_TYPE(matrix3d_t) *r);

/** Transpose matrix
 *
 * @param r target matrix
 * @param m source matrix
 */
LSP_DSP_LIB_SYMBOL(void, transpose_matrix3d2, LSP_DSP_LIB_TYPE(matrix3d_t) *r, const LSP_DSP_LIB_TYPE(matrix3d_t) *m);

/** Analyze that two vectors and the normal vector organize the left triplet
 *
 * @param p1 start point of the first vector
 * @param p2 end point of the first vector, start point of the second vector
 * @param p3 end point of the second vector
 * @param n normal vector
 * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
 */
LSP_DSP_LIB_SYMBOL(float, check_triplet3d_p3n, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2, const LSP_DSP_LIB_TYPE(point3d_t) *p3, const LSP_DSP_LIB_TYPE(vector3d_t) *n);

/** Analyze that two vectors and the normal vector organize the left triplet
 *
 * @param pv array of three points, each previous point is start of vector, each next point is end of vector
 * @param n normal vector
 * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
 */
LSP_DSP_LIB_SYMBOL(float, check_triplet3d_pvn, const LSP_DSP_LIB_TYPE(point3d_t) *pv, const LSP_DSP_LIB_TYPE(vector3d_t) *n);

/** Analyze that two vectors and the normal vector organize the left triplet
 *
 * @param v1 first vector
 * @param v2 second vector
 * @param n normal vector
 * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
 */
LSP_DSP_LIB_SYMBOL(float, check_triplet3d_v2n, const LSP_DSP_LIB_TYPE(vector3d_t) *v1, const LSP_DSP_LIB_TYPE(vector3d_t) *v2, const LSP_DSP_LIB_TYPE(vector3d_t) *n);

/** Analyze that two vectors and the normal vector organize the left triplet
 *
 * @param v array of two vectors
 * @param n normal vector
 * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
 */
LSP_DSP_LIB_SYMBOL(float, check_triplet3d_vvn, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *n);

/** Analyze that three vectors organize the left triplet
 *
 * @param v array of three vectors
 * @param n normal vector
 * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
 */
LSP_DSP_LIB_SYMBOL(float, check_triplet3d_vv, const LSP_DSP_LIB_TYPE(vector3d_t) *v);

/**
 * Analyze point location relative to the triangle of three points.
 * All points should be considered being co-planar.
 *
 * @param t array of three triangle points
 * @param p point
 * @return value > 0 if point is inside of the triangle,
 *         value < 0 if point is outside of the triangle,
 *         value = 0 if point is on the edge of the triangle
 */
LSP_DSP_LIB_SYMBOL(float, check_point3d_on_triangle_pvp, const LSP_DSP_LIB_TYPE(point3d_t) *t, const LSP_DSP_LIB_TYPE(point3d_t) *p);

/**
 * Analyze point location relative to the triangle of three points.
 * All points should be considered being co-planar.
 *
 * @param p1 triangle point 1
 * @param p2 triangle point 2
 * @param p3 triangle point 3
 * @param p point
 * @return value > 0 if point is inside of the triangle,
 *         value < 0 if point is outside of the triangle,
 *         value = 0 if point is on the edge of the triangle
 */
LSP_DSP_LIB_SYMBOL(float, check_point3d_on_triangle_p3p, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2, const LSP_DSP_LIB_TYPE(point3d_t) *p3, const LSP_DSP_LIB_TYPE(point3d_t) *p);


/** Return the index of longest edge between three points
 *
 * @param p1 point 1
 * @param p2 point 2
 * @param p3 point 3
 * @return 0 if edge between points 1 and 2 is longest, 1 if between points 2 and 3, 2 if between ponts 3 and 1
 */
LSP_DSP_LIB_SYMBOL(size_t, longest_edge3d_p3, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2, const LSP_DSP_LIB_TYPE(point3d_t) *p3);

/** Return the index of longest edge between three points
 *
 * @param p array of three points (0, 1, 2)
 * @return 0 if edge between points 0 and 1 is longest, 1 if between points 1 and 2, 2 if between ponts 2 and 0
 */
LSP_DSP_LIB_SYMBOL(size_t, longest_edge3d_pv, const LSP_DSP_LIB_TYPE(point3d_t) *p);

/** Calculate normal for triangle described by three points
 *
 * @param n normal
 * @param p1 point 1
 * @param p2 point 2
 * @param p3 point 3
 */
LSP_DSP_LIB_SYMBOL(void, calc_normal3d_p3, LSP_DSP_LIB_TYPE(vector3d_t) *n, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2, const LSP_DSP_LIB_TYPE(point3d_t) *p3);

/** Calculate normal for triangle described by array of three points
 *
 * @param n normal
 * @param pv array of three points
 */
LSP_DSP_LIB_SYMBOL(void, calc_normal3d_pv, LSP_DSP_LIB_TYPE(vector3d_t) *n, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/** Calculate normal for triangle described by two vectors
 *
 * @param n normal
 * @param v1 vector 1
 * @param v2 vector 2
 */
LSP_DSP_LIB_SYMBOL(void, calc_normal3d_v2, LSP_DSP_LIB_TYPE(vector3d_t) *n, const LSP_DSP_LIB_TYPE(vector3d_t) *v1, const LSP_DSP_LIB_TYPE(vector3d_t) *v2);

/** Calculate normal for triangle described by array two vectors
 *
 * @param n normal
 * @param vv array of two vectors
 */
LSP_DSP_LIB_SYMBOL(void, calc_normal3d_vv, LSP_DSP_LIB_TYPE(vector3d_t) *n, const LSP_DSP_LIB_TYPE(vector3d_t) *vv);

/**
 * Add vector to point
 * @param p point
 * @param dv vector to add
 */
LSP_DSP_LIB_SYMBOL(void, add_vector_pv1, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(vector3d_t) *dv);

/**
 * Add vector to point
 * @param p point
 * @param dv vector to add
 */
LSP_DSP_LIB_SYMBOL(void, add_vector_pv2, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(vector3d_t) *dv);

/**
 * Add scaled vector to point: p = p + dv * k
 * @param p target point
 * @param dv vector to add
 * @param k scale factor
 */
LSP_DSP_LIB_SYMBOL(void, add_vector_pvk1, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(vector3d_t) *dv, float k);

/**
 * Add scaled vector to point: p = sp + dv * k
 * @param p point
 * @param sp source point
 * @param dv vector to add
 * @param k scale factor
 */
LSP_DSP_LIB_SYMBOL(void, add_vector_pvk2, LSP_DSP_LIB_TYPE(point3d_t) *p, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(vector3d_t) *dv, float k);

/**
 * Compute plane equation using three points
 * @param v pointer to store plane equation
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_plane_p3, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute plane equation using three points
 * @param v pointer to store plane equation
 * @param pv array of three points that lay on the plane
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_plane_pv, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Compute plane equation using vector and two points
 * @param v vector to store plane equation
 * @param v0 vector
 * @param p0 point 0
 * @param p1 point 1
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_plane_v1p2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(vector3d_t) *v0, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1);

/**
 * Orient plane to have source point below the plane
 * @param v target plane equation vector
 * @param sp source point
 * @param pl source plane equation vector
 * @return distance from point to the plane
 */
LSP_DSP_LIB_SYMBOL(float, orient_plane_v1p1, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(vector3d_t) *pl);

/**
 * Compute plane equation using three points and set the proper direction so the orienting point is always 'below'
 * the plane
 * @param v pointer to store plane equation
 * @param sp orienting point
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_oriented_plane_p3, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute plane equation using three points and set the proper direction so the orienting point is always 'below'
 * the plane
 * @param v pointer to store plane equation
 * @param sp orienting point
 * @param pv array of three points that lay on the plane
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_oriented_plane_pv, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Compute plane equation using three points and set the proper direction so the orienting point is always 'above'
 * the plane
 * @param v pointer to store plane equation
 * @param sp orienting point
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_rev_oriented_plane_p3, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute plane equation using three points and set the proper direction so the orienting point is always 'above'
 * the plane
 * @param v pointer to store plane equation
 * @param sp orienting point
 * @param pv array of three points that lay on the plane
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_rev_oriented_plane_pv, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Compute plane equation for parallel plane that contains sp and pp points and is parallel to the line formed from p0 and p1 points
 * @param v pointer to store plane equation
 * @param sp source (projection) point
 * @param pp point that lays on the plane
 * @param p0 line point 0
 * @param p1 line point 1
 * @return the length of the original normal vector
 */
LSP_DSP_LIB_SYMBOL(float, calc_parallel_plane_p2p2, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *pp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1);

/**
 * Estimate the area of parallelogram formed by three points
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 * @return area of parallelogram
 */
LSP_DSP_LIB_SYMBOL(float, calc_area_p3, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Estimate the area of parallelogram formed by three points
 * @param pv array of three points that form prarallelogram
 * @return area of parallelogram
 */
LSP_DSP_LIB_SYMBOL(float, calc_area_pv, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Estimate the shortest distance to triangle
 * @param sp projection point
 * @param p0 point 0 of triangle
 * @param p1 point 1 of triangle
 * @param p2 point 2 of triangle
 * @return shortest distance
 */
LSP_DSP_LIB_SYMBOL(float, calc_min_distance_p3, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Estimate the shortest distance to triangle
 * @param sp projection point
 * @param p0 point 0 of triangle
 * @param p1 point 1 of triangle
 * @param p2 point 2 of triangle
 * @return shortest distance
 */
LSP_DSP_LIB_SYMBOL(float, calc_min_distance_pv, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Estimate the average distance to triangle
 * @param sp projection point
 * @param p0 point 0 of triangle
 * @param p1 point 1 of triangle
 * @param p2 point 2 of triangle
 * @return average distance
 */
LSP_DSP_LIB_SYMBOL(float, calc_avg_distance_p3, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute distance between two pointes
 * @param p1 point 1
 * @param p2 point 2
 * @return distance
 */
LSP_DSP_LIB_SYMBOL(float, calc_distance_p2, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute square of distance between two points
 * @param p1 point 1
 * @param p2 point 2
 * @return square value of distance
 */
LSP_DSP_LIB_SYMBOL(float, calc_sqr_distance_p2, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute distance between two points
 * @param pv array of two points
 * @return distance between two points
 */
LSP_DSP_LIB_SYMBOL(float, calc_distance_pv, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Compute the distance (actually, the length of the vector)
 * @param v vector
 * @return distance (vector length)
 */
LSP_DSP_LIB_SYMBOL(float, calc_distance_v1, const LSP_DSP_LIB_TYPE(vector3d_t) *v);

/**
 * Compute square of distance between two points
 * @param pv array of two points
 * @return square of distance between two points
 */
LSP_DSP_LIB_SYMBOL(float, calc_sqr_distance_pv, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Compute intersection point of line and plane,
 * the method is safe from providing the same pointer of ip to l0 and/or l1
 * @param ip target point to store coordinates
 * @param l0 line point 0
 * @param l1 line point 1
 * @param pl vector containing plane equation
 */
LSP_DSP_LIB_SYMBOL(void, calc_split_point_p2v1, LSP_DSP_LIB_TYPE(point3d_t) *ip, const LSP_DSP_LIB_TYPE(point3d_t) *l0, const LSP_DSP_LIB_TYPE(point3d_t) *l1, const LSP_DSP_LIB_TYPE(vector3d_t) *pl);

/**
 * Compute intersection point of line and plane,
 * the method is safe from providing the same pointer of ip to l0 and/or l1
 * @param ip target point to store coordinates
 * @param lv line points (2 elements)
 * @param pl vector containing plane equation
 */
LSP_DSP_LIB_SYMBOL(void, calc_split_point_pvv1, LSP_DSP_LIB_TYPE(point3d_t) *ip, const LSP_DSP_LIB_TYPE(point3d_t) *lv, const LSP_DSP_LIB_TYPE(vector3d_t) *pl);

/**
 * Split raw triangle with plane, generates output set of triangles into out (triangles above split plane)
 * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
 * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
 *
 * @param out array of vertexes above plane
 * @param n_out counter of triangles above plane, should be initialized
 * @param in array of vertexes below plane
 * @param n_in counter of triangles below plane, should be initialized
 * @param pl plane equation
 * @param pv triangle to perform the split
 */
LSP_DSP_LIB_SYMBOL(void, split_triangle_raw,
        LSP_DSP_LIB_TYPE(raw_triangle_t) *out,
        size_t *n_out,
        LSP_DSP_LIB_TYPE(raw_triangle_t) *in,
        size_t *n_in,
        const LSP_DSP_LIB_TYPE(vector3d_t) *pl,
        const LSP_DSP_LIB_TYPE(raw_triangle_t) *pv
    );

/**
 * Cull raw triangle with plane, generates set of triangles below the split plane.
 * For every triangle, points 1 and 2 are the points that lay on the split plane,
 * the first triangle ALWAYS has 2 common points with split plane (1 and 2)
 *
 * @param in array of vertexes below plane
 * @param n_in counter of triangles below plane, should be initialized
 * @param pl plane equation
 * @param pv triangle to perform the split
 */
LSP_DSP_LIB_SYMBOL(void, cull_triangle_raw,
        LSP_DSP_LIB_TYPE(raw_triangle_t) *in,
        size_t *n_in,
        const LSP_DSP_LIB_TYPE(vector3d_t) *pl,
        const LSP_DSP_LIB_TYPE(raw_triangle_t) *pv
    );

/**
 * Check colocation of two points and a plane
 * @param v vector that contains plane equation
 * @param p1 point 1
 * @param p3 point 3
 * @return bit mask: 2 groups of 2 bits, describing state of each point, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   1001 - point 0 lays above the plane, point 1 lays on the plane
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x2_v1p2, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1);

/**
 * Check colocation of three points and a plane
 * @param v vector that contains plane equation
 * @param pv array of two points
 * @return bit mask: 2 groups of 2 bits, describing state of each point, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   1001 - point 0 lays above the plane, point 1 lays on the plane
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x2_v1pv, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Check colocation of three points and a plane
 * @param v vector that contains plane equation
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 * @return bit mask: 3 groups of 2 bits, describing state of each point, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   100100 - point 0 lays above the plane, point 1 lays on the plane, point 2 lays below the plane
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x3_v1p3, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Check colocation of three points and a plane
 * @param v vector that contains plane equation
 * @param pv array of three points
 * @return bit mask: 3 groups of 2 bits, describing state of each point, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   100100 - point 0 lays above the plane, point 1 lays on the plane, point 2 lays below the plane
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x3_v1pv, const LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

/**
 * Check colocation of three planes and a point
 * @param v0 plane 0
 * @param v1 plane 1
 * @param v2 plane 2
 * @param p point
 * @return bit mask: 3 groups of 2 bits, describing state of point relative to each plane, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   100100 - point lays above the plane 0, on the plane 1 and below the plane 2
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x3_v3p1, const LSP_DSP_LIB_TYPE(vector3d_t) *v0, const LSP_DSP_LIB_TYPE(vector3d_t) *v1, const LSP_DSP_LIB_TYPE(vector3d_t) *v2, const LSP_DSP_LIB_TYPE(point3d_t) *p);

/**
 * Check colocation of three planes and a point
 * @param vv array of three vectors
 * @param p point
 * @return bit mask: 3 groups of 2 bits, describing state of point relative to each plane, proper values are:
 *   00 - if point is above the plane
 *   01 - if point is on the plane
 *   10 - if point is below the plane
 *   11 - non-permitted value, won't be produced
 *   The example state:
 *   100100 - point lays above the plane 0, on the plane 1 and below the plane 2
 */
LSP_DSP_LIB_SYMBOL(size_t, colocation_x3_vvp1, const LSP_DSP_LIB_TYPE(vector3d_t) *vv, const LSP_DSP_LIB_TYPE(point3d_t) *p);

/**
 * Compute unit vector from source point to center of triangle
 * @param v target to store vector
 * @param sp source point
 * @param p0 point 0
 * @param p1 point 1
 * @param p2 point 2
 */
LSP_DSP_LIB_SYMBOL(void, unit_vector_p1p3, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *p0, const LSP_DSP_LIB_TYPE(point3d_t) *p1, const LSP_DSP_LIB_TYPE(point3d_t) *p2);

/**
 * Compute unit vector from source point to center of triangle
 * @param v target to store vector
 * @param sp source point
 * @param pv array of three points
 */
LSP_DSP_LIB_SYMBOL(void, unit_vector_p1pv, LSP_DSP_LIB_TYPE(vector3d_t) *v, const LSP_DSP_LIB_TYPE(point3d_t) *sp, const LSP_DSP_LIB_TYPE(point3d_t) *pv);

#endif /* LSP_PLUG_IN_DSP_COMMON_3DMATH_H_ */
