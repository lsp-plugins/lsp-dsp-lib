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

#ifndef LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_
#define LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Convert stereo signal to mid-side signal
 *
 * @param m mid signal
 * @param s side signal
 * @param l left signal
 * @param r right signal
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, lr_to_ms, float *m, float *s, const float *l, const float *r, size_t count);

/** Convert stereo signal to middle signal
 *
 * @param m mid signal
 * @param l left channel
 * @param r right channel
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, lr_to_mid, float *m, const float *l, const float *r, size_t count);

/** Convert stereo signal to side signal
 *
 * @param s side signal
 * @param l left channel
 * @param r right channel
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, lr_to_side, float *s, const float *l, const float *r, size_t count);

/** Convert mid-side signal to left-right signal
 *
 * @param l left signal
 * @param r right signal
 * @param m mid signal
 * @param s side signal
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, ms_to_lr, float *l, float *r, const float *m, const float *s, size_t count);

/** Convert mid-side signal to left signal
 *
 * @param l left signal
 * @param m mid signal
 * @param s side signal
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, ms_to_left, float *l, const float *m, const float *s, size_t count);

/** Convert mid-side signal to right signal
 *
 * @param r right signal
 * @param m mid signal
 * @param s side signal
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, ms_to_right, float *r, const float *m, const float *s, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_ */
