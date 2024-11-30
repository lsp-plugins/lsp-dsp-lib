/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#if defined(ARCH_ARM6) || defined(ARCH_ARM7)

#include <private/dsp/exports.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/stdlib/string.h>

// Test framework
#ifdef LSP_TESTING
    #include <lsp-plug.in/test-fw/test.h>
#else
    #define TEST_EXPORT(...)
#endif /* LSP_TESTING */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

// Platform-specific includes
#if defined(PLATFORM_FREEBSD)
    #include <sys/sysctl.h>
#elif defined(PLATFORM_MACOSX)
    #include <sys/sysctl.h>
#endif

#define PRIVATE_DSP_ARCH_ARM_IMPL
    #include <private/dsp/arch/arm/features.h>
    #include <private/dsp/arch/arm/fpscr.h>
#undef PRIVATE_DSP_ARCH_ARM_IMPL

namespace lsp
{
    namespace neon_d32
    {
        extern void dsp_init(const arm::cpu_features_t *f);
    }

    namespace arm
    {
        //    processor       : 0
        //    model name      : ARMv7 Processor rev 4 (v7l)
        //    BogoMIPS        : 38.40
        //    Features        : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32
        //    CPU implementer : 0x41
        //    CPU architecture: 7
        //    CPU variant     : 0x0
        //    CPU part        : 0xd03
        //    CPU revision    : 4

        typedef struct cpu_part_t
        {
            uint32_t    id;
            const char *name;
        } cpu_part_t;

        typedef struct feature_t
        {
            uint32_t    mask;
            const char *name;
        } feature_t;

        static const cpu_part_t cpu_parts[] =
        {
            { 0xb02, "ARM11 MPCore" },
            { 0xb36, "ARM1136" },
            { 0xb56, "ARM1156" },
            { 0xb76, "ARM1176" },

            { 0xc05, "Cortex-A5" },
            { 0xc07, "Cortex-A7" },
            { 0xc08, "Cortex-A8" },
            { 0xc09, "Cortex-A9" },
            { 0xc0d, "Cortex-A12" },
            { 0xc0e, "Cortex-A17" },
            { 0xc0f, "Cortex-A15" },
            { 0xc14, "Cortex-R4" },
            { 0xc15, "Cortex-R5" },
            { 0xc17, "Cortex-R7" },
            { 0xc18, "Cortex-R8" },

            { 0xc20, "Cortex-M0" },
            { 0xc21, "Cortex-M1" },
            { 0xc23, "Cortex-M3" },
            { 0xc24, "Cortex-M4" },
            { 0xc27, "Cortex-M7" },
            { 0xc60, "Cortex-M0+" },

            { 0xd01, "Cortex-A32" },
            { 0xd03, "Cortex-A53" },
            { 0xd04, "Cortex-A35" },
            { 0xd05, "Cortex-A55" },
            { 0xd07, "Cortex-A57" },
            { 0xd08, "Cortex-A72" },
            { 0xd09, "Cortex-A73" },
            { 0xd0a, "Cortex-A75" },
            { 0xd13, "Cortex-R52" },

            { 0xd20, "Cortex-M23" },
            { 0xd21, "Cortex-M33" }
        };

        static const feature_t cpu_features[] =
        {
            { HWCAP_ARM_SWP, "SWP" },
            { HWCAP_ARM_HALF, "HALF" },
            { HWCAP_ARM_THUMB, "THUMB" },
            { HWCAP_ARM_26BIT, "26BIT" },
            { HWCAP_ARM_FAST_MULT, "FAST_MULT" },
            { HWCAP_ARM_FPA, "FPA" },
            { HWCAP_ARM_VFP, "VFP" },
            { HWCAP_ARM_EDSP, "EDSP" },
            { HWCAP_ARM_JAVA, "JAVA" },
            { HWCAP_ARM_IWMMXT, "IWMMXT" },
            { HWCAP_ARM_CRUNCH, "CRUNCH" },
            { HWCAP_ARM_THUMBEE, "THUMBEE" },
            { HWCAP_ARM_NEON, "NEON" },
            { HWCAP_ARM_VFPv3, "VFPv3" },
            { HWCAP_ARM_VFPv3D16, "VFPv3D16" },
            { HWCAP_ARM_TLS, "TLS" },
            { HWCAP_ARM_VFPv4, "VFPv4" },
            { HWCAP_ARM_IDIVA, "IDIVA" },
            { HWCAP_ARM_IDIVT, "IDIVT" },
            { HWCAP_ARM_VFPD32, "VFPD32" },
            { HWCAP_ARM_LPAE, "LPAE" }
        };

        static dsp::start_t     dsp_start       = NULL;
        static dsp::finish_t    dsp_finish      = NULL;

        const char *find_cpu_name(uint32_t id)
        {
            ssize_t first = 0, last = (sizeof(cpu_parts) / sizeof(cpu_part_t)) - 1;
            while (first <= last)
            {
                ssize_t mid     = (first + last) >> 1;
                uint32_t xmid   = cpu_parts[mid].id;
                if (id < xmid)
                    last = mid - 1;
                else if (id > xmid)
                    first = mid + 1;
                else
                    return cpu_parts[mid].name;
            }
            return "Generic ARM processor";
        }

    #ifdef PLATFORM_LINUX
        static void read_cpu_info(cpu_features_t *f)
        {
            // Example contents:
            // processor       : 0
            // BogoMIPS        : 38.40
            // Features        : fp asimd evtstrm crc32 cpuid
            // CPU implementer : 0x41
            // CPU architecture: 8
            // CPU variant     : 0x0
            // CPU part        : 0xd03
            // CPU revision    : 4

            // Read /proc/cpuinfo
            FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
            if (cpuinfo == NULL)
                return;  // handle error
            lsp_finally {
                fclose(cpuinfo);
            };

            size_t size = 0;
            char *line  = NULL;
            lsp_finally {
                if (line != NULL)
                    free(line);
            };

            while (getline(&line, &size, cpuinfo) >= 0)
            {
                size_t *field = NULL;

                // Find field
                if (strncasecmp(line, "CPU implementer", 15) == 0)
                    field = &f->implementer;
                else if (strncasecmp(line, "CPU architecture", 16) == 0)
                    field = &f->architecture;
                else if (strncasecmp(line, "CPU variant", 11) == 0)
                    field = &f->variant;
                else if (strncasecmp(line, "CPU part", 8) == 0)
                    field = &f->part;
                else if (strncasecmp(line, "CPU revision", 12) == 0)
                    field = &f->revision;

                if (field == NULL) // Field not found ?
                    continue;
                char *colon = strchr(line, ':'); // Colon not found ?
                if (colon++ == NULL)
                    continue;

                while ((*colon) == ' ')
                    colon++;
                if ((*colon) == '\0') // No data ?
                    continue;

                // Detect number base
                size_t base = 10;
                if (strncasecmp(colon, "0x", 2) == 0)
                {
                    colon  += 2;
                    base    = 16;
                }

                // Parse value
                errno       = 0;
                long value  = strtol(colon, &colon, base);
                if (errno != 0) // Failed parse ?
                    continue;
                if (((*colon) != '\0') && (*colon) != '\n') // Additional data?
                    continue;

                // Store parsed value
                *field      = value;
            }
        }
    #endif /* PLATFORM_LINUX */

    #ifdef PLATFORM_MACOSX
        static void read_hwcap(cpu_features_t *f)
        {
            int val = 0;
            size_t len = sizeof(val);
            if (sysctlbyname("hw.optional.AdvSIMD", &val, &len, NULL, 0) == 0)
            {
                if (val != 0)
                    f->hwcap       |= HWCAP_NEON | HWCAP_ARM_VFPD32;
            }

            val = 0;
            if (sysctlbyname("hw.optional.neon", &val, &len, NULL, 0) == 0)
            {
                if (val != 0)
                    f->hwcap       |= HWCAP_NEON | HWCAP_ARM_VFPD32;
            }
        }

        static void read_cpu_name(cpu_features_t *f)
        {
            size_t len = sizeof(f->cpu_name);
            sysctlbyname("machdep.cpu.brand_string", f->cpu_name, &len, nullptr, 0);
        }
    #endif /* PLATFORM_MACOSX */

    #ifdef PLATFORM_FREEBSD
        static void read_cpu_name(cpu_features_t *f)
        {
            size_t len = sizeof(f->cpu_name);
            sysctlbyname("hw.model", f->cpu_name, &len, nullptr, 0);
        }
    #endif /* PLATFORM_FREEBSD */

        void detect_cpu_features(cpu_features_t *f)  // must be at least 13 bytes
        {
            f->implementer      = 0;
            f->architecture     = 0;
            IF_ARCH_ARM6(f->architecture = 6);
            IF_ARCH_ARM7(f->architecture = 7);
            IF_ARCH_ARM8(f->architecture = 8);
            f->variant          = 0;
            f->part             = 0;
            f->revision         = 0;
            f->hwcap            = 0;

            strncpy(f->cpu_name, "Generic ARM processor", sizeof(f->cpu_name) - 1);

        #if defined(PLATFORM_LINUX)
            f->hwcap            = getauxval(AT_HWCAP);
            read_cpu_info(f);
            const char *cpu_name = find_cpu_name(f->part);
            if (cpu_name != NULL)
                strncpy(f->cpu_name, cpu_name, sizeof(f->cpu_name));

        #elif defined(PLATFORM_BSD)
            unsigned long __hwcap = 0;
            if (elf_aux_info(AT_HWCAP, &__hwcap, sizeof(__hwcap)) == 0)
                f->hwcap            = __hwcap;
            read_cpu_name(f);

        #elif defined(PLATFORM_MACOSX)
            read_hwcap(f);
            read_cpu_name(f);

        #endif

            f->cpu_name[sizeof(f->cpu_name) - 1] = '\0';
        }

        static size_t estimate_features_size(const cpu_features_t *f)
        {
            // Estimate the string length
            size_t estimate = 1; // End of string character
            for (size_t i = 0, n=sizeof(cpu_features)/sizeof(feature_t); i < n; i++)
            {
                if (!(f->hwcap & cpu_features[i].mask))
                    continue;

                if (estimate > 0)
                    estimate++;
                estimate += strlen(cpu_features[i].name);
            }
            return estimate;
        }

        static char *build_features_list(char *dst, const cpu_features_t *f)
        {
            // Build string
            char *s = dst;

            for (size_t i = 0, n=sizeof(cpu_features)/sizeof(feature_t); i < n; i++)
            {
                if (!(f->hwcap & cpu_features[i].mask))
                    continue;
                if (s != dst)
                    s = stpcpy(s, " ");
                s = stpcpy(s, cpu_features[i].name);
            }
            *s = '\0';

            return s;
        }

        void start(dsp::context_t *ctx)
        {
            dsp_start(ctx);
            uint32_t fpscr          = read_fpscr();
            ctx->data[ctx->top++]   = fpscr;
            write_fpscr(fpscr | FPSCR_FZ | FPSCR_DN);
        }

        void finish(dsp::context_t *ctx)
        {
            write_fpscr(ctx->data[--ctx->top]);
            dsp_finish(ctx);
        }

        dsp::info_t *info()
        {
            cpu_features_t f;
            detect_cpu_features(&f);

            char *model     = NULL;
            int n = asprintf(&model, "vendor=0x%x, architecture=%d, variant=%d, part=0x%x, revision=%d",
                    int(f.implementer), int(f.architecture), int(f.variant), int(f.part), int(f.revision));
            if ((n < 0) || (model == NULL))
                return NULL;

            size_t size     = sizeof(dsp::info_t);
            size           += strlen(ARCH_STRING) + 1;
            size           += strlen(f.cpu_name) + 1;
            size           += strlen(model) + 1;
            size           += estimate_features_size(&f);

            dsp::info_t *res = reinterpret_cast<dsp::info_t *>(malloc(size));
            if (res == NULL)
            {
                free(model);
                return res;
            }

            char *text      = reinterpret_cast<char *>(&res[1]);
            res->arch       = text;
            text            = stpcpy(text, ARCH_STRING) + 1;
            res->cpu        = text;
            text            = stpcpy(text, f.cpu_name) + 1;
            res->model      = text;
            text            = stpcpy(text, model) + 1;
            res->features   = text;
            build_features_list(text, &f);

            free(model);
            return res;
        }

        #define EXPORT2(function, export) \
        { \
            dsp::function                       = arm::export; \
            dsp::LSP_DSP_LIB_MANGLE(function)   = arm::export; \
            TEST_EXPORT(arm::export); \
        }
        #define EXPORT1(function)                   EXPORT2(function, function)

        void dsp_init(const arm::cpu_features_t *f)
        {
            // Export functions
            EXPORT1(info);

            if (f->hwcap & HWCAP_ARM_VFP)
            {
                // Save previous entry points
                dsp_start                       = dsp::start;
                dsp_finish                      = dsp::finish;

                // Export routines
                EXPORT1(start);
                EXPORT1(finish);
            }

            // Initialize support of NEON functions with D-32 registers
            neon_d32::dsp_init(f);
        }
    } /* namespace arm */
} /* namespace lsp */

#endif /* defined(ARCH_ARM6) || defined(ARCH_ARM7) */


