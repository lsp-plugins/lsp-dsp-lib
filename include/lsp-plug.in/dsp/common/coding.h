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

#ifndef LSP_PLUG_IN_DSP_COMMON_CODING_H_
#define LSP_PLUG_IN_DSP_COMMON_CODING_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Encode base-64 value, the function does not generate padding '=' characters,
 * since the estimated size of output data is known, caller should control size
 * of input buffer to prevent the algorithm from emitting end-of-sequence
 * @param dst destination buffer, should be at least 4 bytes large for encoding
 * @param dst_left number of bytes left in destination buffer
 * @param src source buffer
 * @param src_left number of bytes left in source buffer
 * @return number of bytes of source buffer processed
 */
LSP_DSP_LIB_SYMBOL(size_t, base64_enc, void *dst, size_t *dst_left, const void *src, size_t *src_left);

/**
 * Decode base-64 value, the function treats '=' characters as error (for optimization purposes)
 * since the estimated size of output data is known, caller should control size
 * of input buffer to prevent the algorithm from emitting end-of-sequence
 * @param dst destination buffer, should be at leadt 3 bytes large for decoding
 * @param dst_left number of bytes left in destination buffer
 * @param src source buffer
 * @param src_left number of bytes left in source buffer
 * @return number of bytes decoded into destination buffer, negative value on illegal character in sequence
 */
LSP_DSP_LIB_SYMBOL(ssize_t, base64_dec, void *dst, size_t *dst_left, const void *src, size_t *src_left);

#endif /* LSP_PLUG_IN_DSP_COMMON_CODING_H_ */
