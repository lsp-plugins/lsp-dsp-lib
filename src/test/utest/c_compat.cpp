/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 авг. 2023 г.
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

#define LSP_DSP_LIB_USE_C_IFACE

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>

static constexpr size_t ARRAY_SIZE  = 0x1000;

UTEST_BEGIN("dsp", c_compat)
    UTEST_MAIN
    {
    // Optionally: output information about the system
        lsp_dsp_info_t *info = lsp_dsp_info();
        if (info != NULL)
        {
            printf("Architecture:   %s\n", info->arch);
            printf("Processor:      %s\n", info->cpu);
            printf("Model:          %s\n", info->model);
            printf("Features:       %s\n", info->features);

            free(info);
        }

        // For faster computing we can tune CPU by updating thread context.
        // This will enable Flush-to-Zero and Denormals-are-Zero flags on
        // CPUs that support them. This is thread-local change and should
        // be called in each individual processing thread
        lsp_dsp_context_t ctx;
        lsp_dsp_start(&ctx);

        // Here we call some dsp functions, for example lsp_dsp_fill_zero
        float *v = static_cast<float *>(malloc(ARRAY_SIZE * sizeof(float)));
        lsp_finally { free(v); };
        lsp_dsp_fill_zero(v, ARRAY_SIZE);

        // At the end, we need to restore the context and reset CPU settings to defaults
        lsp_dsp_finish(&ctx);
    }

UTEST_END;


