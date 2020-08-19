/*
 * op_vv.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_OP_VV_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_OP_VV_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate dst[i] = dst[i] + src[i]
 *
 * @param dst destination array
 * @param src source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , add2, float *dst, const float *src, size_t count);

/** Calculate dst[i] = dst[i] - src[i]
 *
 * @param dst destination array
 * @param src source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , sub2, float *dst, const float *src, size_t count);

/** Calculate dst[i] = src[i] - dst[i]
 *
 * @param dst destination array
 * @param src source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , rsub2, float *dst, const float *src, size_t count);

/** Multiply: dst[i] = dst[i] * src[i]
 *
 * @param dst destination
 * @param src first source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , mul2, float *dst, const float *src, size_t count);

/** Divide: dst[i] = dst[i] / src[i]
 *
 * @param dst destination
 * @param src first source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , div2, float *dst, const float *src, size_t count);

/** Divide: dst[i] = src[i] / dst[i]
 *
 * @param dst destination
 * @param src first source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , rdiv2, float *dst, const float *src, size_t count);

/** Compute remainder: dst[i] = dst[i] - src[i] * int(dst[i]/src[i])
 *
 * @param dst destination
 * @param src source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , mod2, float *dst, const float *src, size_t count);

/** Compute reverse remainder: dst[i] = src[i] - dst[i] * int(src[i]/dst[i])
 *
 * @param dst destination
 * @param src source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , rmod2, float *dst, const float *src, size_t count);

/** Calculate dst[i] = src1[i] + src2[i]
 *
 * @param dst destination array
 * @param src source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , add3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate dst[i] = src1[i] - src2[i]
 *
 * @param dst destination array
 * @param src source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , sub3, float *dst, const float *src1, const float *src2, size_t count);

/** Multiply: dst[i] = src1[i] * src2[i]
 *
 * @param dst destination
 * @param src1 first source
 * @param src2 second source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , mul3, float *dst, const float *src1, const float *src2, size_t count);

/** Divide: dst[i] = src1[i] / src2[i]
 *
 * @param dst destination
 * @param src1 first source
 * @param src2 second source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , div3, float *dst, const float *src1, const float *src2, size_t count);

/** Remainder: dst[i] = src1[i] - src2[i]*int(src1[i]/src2[i]);
 *
 * @param dst destination
 * @param src1 first source
 * @param src2 second source
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , mod3, float *dst, const float *src1, const float *src2, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_OP_VV_H_ */
