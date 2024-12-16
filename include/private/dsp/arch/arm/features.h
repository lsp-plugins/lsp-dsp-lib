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

#ifndef PRIVATE_DSP_ARCH_ARM_FEATURES_H_
#define PRIVATE_DSP_ARCH_ARM_FEATURES_H_

#include <lsp-plug.in/common/types.h>

#if defined(ARCH_ARM6) || defined(ARCH_ARM7)

    #if defined(PLATFORM_MACOSX)
        #define HWCAP_NEON              (1 << 0)
        #define HWCAP_ARM_VFPD32        (1 << 1)
    #elif defined(PLATFORM_POSIX)
        #include <sys/auxv.h>
    #endif /* PLATFORM_POSIX */


    /*
        Redefine constants to be compatible with GCC 6.x (Raspbian) and GCC 8.x (FreeBSD)
    */
    #if defined(HWCAP_SWP) && !defined(HWCAP_ARM_SWP)
        #define HWCAP_ARM_SWP HWCAP_SWP
    #endif
    #if defined(HWCAP_HALF) && !defined(HWCAP_ARM_HALF)
        #define HWCAP_ARM_HALF HWCAP_HALF
    #endif
    #if defined(HWCAP_THUMB) && !defined(HWCAP_ARM_THUMB)
        #define HWCAP_ARM_THUMB HWCAP_THUMB
    #endif
    #if defined(HWCAP_26BIT) && !defined(HWCAP_ARM_26BIT)
        #define HWCAP_ARM_26BIT HWCAP_26BIT
    #endif
    #if defined(HWCAP_FAST_MULT) && !defined(HWCAP_ARM_FAST_MULT)
        #define HWCAP_ARM_FAST_MULT HWCAP_FAST_MULT
    #endif
    #if defined(HWCAP_FPA) && !defined(HWCAP_ARM_FPA)
        #define HWCAP_ARM_FPA HWCAP_FPA
    #endif
    #if defined(HWCAP_VFP) && !defined(HWCAP_ARM_VFP)
        #define HWCAP_ARM_VFP HWCAP_VFP
    #endif
    #if defined(HWCAP_EDSP) && !defined(HWCAP_ARM_EDSP)
        #define HWCAP_ARM_EDSP HWCAP_EDSP
    #endif
    #if defined(HWCAP_JAVA) && !defined(HWCAP_ARM_JAVA)
        #define HWCAP_ARM_JAVA HWCAP_JAVA
    #endif
    #if defined(HWCAP_IWMMXT) && !defined(HWCAP_ARM_IWMMXT)
        #define HWCAP_ARM_IWMMXT HWCAP_IWMMXT
    #endif
    #if defined(HWCAP_CRUNCH) && !defined(HWCAP_ARM_CRUNCH)
        #define HWCAP_ARM_CRUNCH HWCAP_CRUNCH
    #endif
    #if defined(HWCAP_THUMBEE) && !defined(HWCAP_ARM_THUMBEE)
        #define HWCAP_ARM_THUMBEE HWCAP_THUMBEE
    #endif
    #if defined(HWCAP_NEON) && !defined(HWCAP_ARM_NEON)
        #define HWCAP_ARM_NEON HWCAP_NEON
    #endif
    #if defined(HWCAP_VFPv3) && !defined(HWCAP_ARM_VFPv3)
        #define HWCAP_ARM_VFPv3 HWCAP_VFPv3
    #endif
    #if defined(HWCAP_VFPv3D16) && !defined(HWCAP_ARM_VFPv3D16)
        #define HWCAP_ARM_VFPv3D16 HWCAP_VFPv3D16
    #endif
    #if defined(HWCAP_TLS) && !defined(HWCAP_ARM_TLS)
        #define HWCAP_ARM_TLS HWCAP_TLS
    #endif
    #if defined(HWCAP_VFPv4) && !defined(HWCAP_ARM_VFPv4)
        #define HWCAP_ARM_VFPv4 HWCAP_VFPv4
    #endif
    #if defined(HWCAP_IDIVA) && !defined(HWCAP_ARM_IDIVA)
        #define HWCAP_ARM_IDIVA HWCAP_IDIVA
    #endif
    #if defined(HWCAP_IDIVT) && !defined(HWCAP_ARM_IDIVT)
        #define HWCAP_ARM_IDIVT HWCAP_IDIVT
    #endif
    #if defined(HWCAP_VFPD32) && !defined(HWCAP_ARM_VFPD32)
        #define HWCAP_ARM_VFPD32 HWCAP_VFPD32
    #endif
    #if defined(HWCAP_LPAE) && !defined(HWCAP_ARM_LPAE)
        #define HWCAP_ARM_LPAE HWCAP_LPAE
    #endif
    #if defined(HWCAP_EVTSTRM) && !defined(HWCAP_ARM_EVTSTRM)
        #define HWCAP_ARM_EVTSTRM HWCAP_EVTSTRM
    #endif

    namespace lsp
    {
        namespace arm
        {
            typedef struct cpu_features_t
            {
                size_t      implementer;
                size_t      architecture;
                size_t      variant;
                size_t      part;
                size_t      revision;
                uint64_t    hwcap;
                char        cpu_name[64];
            } cpu_features_t;

            void detect_cpu_features(cpu_features_t *f);

            void dsp_init(const cpu_features_t *f);
        } /* namespace arm */

    } /* namespace lsp */

    #define LSP_DSP_CPU_NAMESPACE           arm

#endif /* defined(ARCH_ARM6) || defined(ARCH_ARM7) */

#endif /* PRIVATE_DSP_ARCH_ARM_FEATURES_H_ */
