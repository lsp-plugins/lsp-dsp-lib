/*
 * fmop_vv.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_VV_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_VV_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate dst[i] = dst[i] + a[i] * b[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmadd3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = dst[i] - a[i] * b[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmsub3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = a[i] * b[i] - dst[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmrsub3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = dst[i] * a[i] * b[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmmul3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = dst[i] / (a[i] * b[i])
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmdiv3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = (a[i] * b[i]) / dst[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmrdiv3, float *dst, const float *a, const float *b, size_t count);

/**
 * Calculate remainder: dst[i] = dst[i] - (a[i]*b[i]) * int(dst[i] / (a[i] * b[i]))
 * @param dst destination array
 * @param a first argument array
 * @param b second argument array
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void , fmmod3, float *dst, const float *a, const float *b, size_t count);

/**
 * Calculate reverse remainder: dst[i] = a[i]*b[i] - dst[i] * int((a[i] * b[i]) / dst[i])
 * @param dst destination array
 * @param a first argument array
 * @param b second argument array
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void , fmrmod3, float *dst, const float *a, const float *b, size_t count);

/** Calculate dst[i] = a[i] + b[i] * c[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmadd4, float *dst, const float *a, const float *b, const float *c, size_t count);

/** Calculate dst[i] = a[i] - b[i] * c[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmsub4, float *dst, const float *a, const float *b, const float *c, size_t count);

/** Calculate dst[i] = b[i] * c[i] - a[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmrsub4, float *dst, const float *a, const float *b, const float *c, size_t count);

/** Calculate dst[i] = a[i] * b[i] * c[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmmul4, float *dst, const float *a, const float *b, const float *c, size_t count);

/** Calculate dst[i] = a[i] / (b[i] * c[i])
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmdiv4, float *dst, const float *a, const float *b, const float *c, size_t count);

/** Calculate dst[i] = (b[i] * c[i]) / a[i]
 *
 * @param dst destination array
 * @param a source array
 * @param b source array
 * @param c source array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void , fmrdiv4, float *dst, const float *a, const float *b, const float *c, size_t count);

/**
 * Calculate remainder: dst[i] = a[i] - (b[i]*c[i]) * int(a[i] / (b[i] * c[i]))
 * @param dst destination array
 * @param a first argument array
 * @param b second argument array
 * @param c third argument array
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void , fmmod4, float *dst, const float *a, const float *b, const float *c, size_t count);

/**
 * Calculate reverse remainder: dst[i] = (b[i]*c[i]) - a[i] * int((b[i] * c[i]) / a[i])
 * @param dst destination array
 * @param a first argument array
 * @param b second argument array
 * @param c third argument array
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void , fmrmod4, float *dst, const float *a, const float *b, const float *c, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_VV_H_ */
