/*
 * transform.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_

#include <lsp-plug.in/dsp/types.h>

#include <lsp-plug.in/dsp/common/filters/types.h>

namespace lsp
{
    namespace dsp
    {
        //---------------------------------------------------------------------------------------
        // Bilinear transformation of dynamic filters
        //---------------------------------------------------------------------------------------
        /** Perform bilinear transformation of one filter bank
         *
         * @param bf memory-aligned target transformed biquad x1 filters
         * @param bc memory-aligned source analog bilinear filter cascades
         * @param kf frequency shift coefficient
         * @param count number of cascades  to process
         */
        extern void (* bilinear_transform_x1)(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);

        /** Perform bilinear transformation of two filter banks
         *
         * @param bf memory-aligned target transformed biquad x2 filters
         * @param bc memory-aligned source analog bilinear filter cascades matrix
         * @param kf frequency shift coefficient
         * @param count number of matrix rows to process
         */
        extern void (* bilinear_transform_x2)(biquad_x2_t *bf, const f_cascade_t *bc, float kf, size_t count);

        /** Perform bilinear transformation of four filter banks
         *
         * @param bf memory-aligned target transformed biquad x4 filters
         * @param bc memory-aligned source analog bilinear filter cascades matrix
         * @param kf frequency shift coefficient
         * @param count number of matrix rows to process
         */
        extern void (* bilinear_transform_x4)(biquad_x4_t *bf, const f_cascade_t *bc, float kf, size_t count);

        /** Perform bilinear transformation of eight filter banks
         *
         * @param bf memory-aligned target transformed biquad x8 filters
         * @param bc memory-aligned source analog bilinear filter cascades matrix
         * @param kf frequency shift coefficient
         * @param count number of matrix rows to process
         */
        extern void (* bilinear_transform_x8)(biquad_x8_t *bf, const f_cascade_t *bc, float kf, size_t count);

        //---------------------------------------------------------------------------------------
        // Matched Z transformation of dynamic filters
        //---------------------------------------------------------------------------------------
        /** Perform matched Z transformation of one filter bank,
         * function modifies contents of the bc memory chunk
         *
         * @param bf memory-aligned target transformed biquad x1 filters
         * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
         * @param kf frequency shift coefficient (filter frequency)
         * @param td tranformation coefficient (2 * pi / sample rate)
         * @param count number of cascades  to process
         */
        extern void (* matched_transform_x1)(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

        /** Perform matched Z transformation of two filter banks,
         * function modifies contents of the bc memory chunk
         *
         * @param bf memory-aligned target transformed biquad x2 filters
         * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
         * @param kf frequency shift coefficient (filter frequency)
         * @param td tranformation coefficient (2 * pi / sample rate)
         * @param count number of cascades  to process
         */
        extern void (* matched_transform_x2)(biquad_x2_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

        /** Perform matched Z transformation of four filter banks,
         * function modifies contents of the bc memory chunk
         *
         * @param bf memory-aligned target transformed biquad x4 filters
         * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
         * @param kf frequency shift coefficient (filter frequency)
         * @param td tranformation coefficient (2 * pi / sample rate)
         * @param count number of cascades  to process
         */
        extern void (* matched_transform_x4)(biquad_x4_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

        /** Perform matched Z transformation of eight filter banks,
         * function modifies contents of the bc memory chunk
         *
         * @param bf memory-aligned target transformed biquad x8 filters
         * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
         * @param kf frequency shift coefficient (filter frequency)
         * @param td tranformation coefficient (2 * pi / sample rate)
         * @param count number of cascades  to process
         */
        extern void (* matched_transform_x8)(biquad_x8_t *bf, f_cascade_t *bc, float kf, float td, size_t count);

    }
}


#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_ */
