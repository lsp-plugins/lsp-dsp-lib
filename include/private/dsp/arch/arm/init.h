/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 7 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_INIT_H_
#define PRIVATE_DSP_ARCH_ARM_INIT_H_

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/cpuid.h>

#if defined(ARCH_ARM6) || defined(ARCH_ARM7)

#define LSP_DSP_CPU_NAMESPACE           arm

namespace lsp
{
    namespace arm
    {
        void dsp_init(const cpuid_t *f);
    } /* namespace arm */

} /* namespace lsp */

#endif /* defined(ARCH_ARM6) || defined(ARCH_ARM7) */

#endif /* PRIVATE_DSP_ARCH_ARM_INIT_H_ */
