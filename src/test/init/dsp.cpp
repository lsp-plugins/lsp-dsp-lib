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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/init.h>

namespace lsp
{
    static __thread dsp::context_t *pTestContext = NULL;
}

INIT_BEGIN(dsp_initializer)

    INIT_FUNC
    {
        // Initialize DSP
        dsp::init();

        // Output information if necessary
        dsp::info_t *info = dsp::info();
        if (info != NULL)
        {
            printf("Architecture:   %s\n", info->arch);
            printf("Processor:      %s\n", info->cpu);
            printf("Model:          %s\n", info->model);
            printf("Features:       %s\n", info->features);

            ::free(info);
        }
    }

    BEFORE_FUNC
    {
        // Check test match
        if (::strstr(test, "dsp.") != test)
            return;

        // Update context
        pTestContext = reinterpret_cast<dsp::context_t *>(::malloc(sizeof(dsp::context_t)));
        if (pTestContext != NULL)
            dsp::start(pTestContext);
    }

    AFTER_FUNC
    {
        // Restore context
        if (pTestContext != NULL)
        {
            dsp::finish(pTestContext);
            ::free(pTestContext);
            pTestContext = NULL;
        }
    }

INIT_END


