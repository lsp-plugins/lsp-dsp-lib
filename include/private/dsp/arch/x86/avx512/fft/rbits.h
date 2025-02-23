#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FFT_RBITS_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FFT_RBITS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {

        #define FFT_REVERSE_BITS32(VA, VB, SHIFT, SRC)  \
            /* Round 1 */ \
            __ASM_EMIT("vpslld          $16, " SRC ", " VB)                         /* VB   = v << 16 */ \
            __ASM_EMIT("vpsrld          $16, " SRC ", " VA)                         /* VA   = v >> 16 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = v = (v << 16) | (v >> 16) */ \
            /* Round 2 */ \
            __ASM_EMIT("vpslld          $8, " VA ", " VB "")                        /* VB   = v << 8 */ \
            __ASM_EMIT("vpandd          0x00 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xff00ff00) */ \
            __ASM_EMIT("vpandd          0x00 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x00ff00ff) << 8 */ \
            __ASM_EMIT("vpsrld          $8, " VA ", " VA "")                        /* VA   = (v & 0xff00ff00) >> 8 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x00ff00ff) << 8) | ((v & 0xff00ff00) >> 8) */ \
            /* Round 3 */ \
            __ASM_EMIT("vpslld          $4, " VA ", " VB "")                        /* VB   = v << 4 */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xf0f0f0f0) */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x0f0f0f0f) << 4 */ \
            __ASM_EMIT("vpsrld          $4, " VA ", " VA "")                        /* VA   = (v & 0xff00ff00) >> 4 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x0f0f0f0f) << 4) | ((v & 0xf0f0f0f0) >> 4) */ \
            /* Round 4 */ \
            __ASM_EMIT("vpslld          $2, " VA ", " VB "")                        /* VB   = v << 2 */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xcccccccc) */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x33333333) << 2 */ \
            __ASM_EMIT("vpsrld          $2, " VA ", " VA "")                        /* VA   = (v & 0xcccccccc) >> 2 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x33333333) << 2) | ((v & 0xcccccccc) >> 2) */ \
            /* Round 4 */ \
            __ASM_EMIT("vpslld          $1, " VA ", " VB "")                        /* VB   = v << 1 */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xaaaaaaaa) */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x55555555) << 1 */ \
            __ASM_EMIT("vpsrld          $1, " VA ", " VA "")                        /* VA   = (v & 0xaaaaaaaa) >> 1 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x55555555) << 1) | ((v & 0xaaaaaaaa) >> 1) */ \
            /* Final round */ \
            __ASM_EMIT("vmovss          %[shift], " SHIFT) \
            __ASM_EMIT("vpsrld          " SHIFT " , " VA ", " VA)                   /* VA   = va = v >> shift */ \
            __ASM_EMIT("vpord           %[mask]%{1to16%}, " VA ", " VB)             /* VB   = vb = va | mask */

        #define FFT_REVERSE_BITS16(VA, VB, SHIFT, SRC)  \
            /* Round 1 */ \
            __ASM_EMIT("vpslld          $8, " SRC ", " VB "")                       /* VB   = v << 8 */ \
            __ASM_EMIT("vpandd          0x00 + %[CMD]%{1to16%}, " SRC ", " VA "")   /* VA   = (v & 0xff00ff00) */ \
            __ASM_EMIT("vpandd          0x00 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x00ff00ff) << 8 */ \
            __ASM_EMIT("vpsrld          $8, " VA ", " VA "")                        /* VA   = (v & 0xff00ff00) >> 8 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x00ff00ff) << 8) | ((v & 0xff00ff00) >> 8) */ \
            /* Round 2 */ \
            __ASM_EMIT("vpslld          $4, " VA ", " VB "")                        /* VB   = v << 4 */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xf0f0f0f0) */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x0f0f0f0f) << 4 */ \
            __ASM_EMIT("vpsrld          $4, " VA ", " VA "")                        /* VA   = (v & 0xff00ff00) >> 4 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x0f0f0f0f) << 4) | ((v & 0xf0f0f0f0) >> 4) */ \
            /* Round 3 */ \
            __ASM_EMIT("vpslld          $2, " VA ", " VB "")                        /* VB   = v << 2 */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xcccccccc) */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x33333333) << 2 */ \
            __ASM_EMIT("vpsrld          $2, " VA ", " VA "")                        /* VA   = (v & 0xcccccccc) >> 2 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x33333333) << 2) | ((v & 0xcccccccc) >> 2) */ \
            /* Round 4 */ \
            __ASM_EMIT("vpslld          $1, " VA ", " VB "")                        /* VB   = v << 1 */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xaaaaaaaa) */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x55555555) << 1 */ \
            __ASM_EMIT("vpsrld          $1, " VA ", " VA "")                        /* VA   = (v & 0xaaaaaaaa) >> 1 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x55555555) << 1) | ((v & 0xaaaaaaaa) >> 1) */ \
            /* Final round */ \
            __ASM_EMIT("vmovss          %[shift], " SHIFT) \
            __ASM_EMIT("vpsrld          " SHIFT " , " VA ", " VA)                   /* VA   = va = v >> shift */ \
            __ASM_EMIT("vpord           %[mask]%{1to16%}, " VA ", " VB)             /* VB   = vb = va | mask */

        #define FFT_REVERSE_BITS8(VA, VB, SHIFT, SRC)  \
            /* Round 1 */ \
            __ASM_EMIT("vpslld          $4, " SRC ", " VB "")                       /* VB   = v << 4 */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " SRC ", " VA "")   /* VA   = (v & 0xf0f0f0f0) */ \
            __ASM_EMIT("vpandd          0x04 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x0f0f0f0f) << 4 */ \
            __ASM_EMIT("vpsrld          $4, " VA ", " VA "")                        /* VA   = (v & 0xff00ff00) >> 4 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x0f0f0f0f) << 4) | ((v & 0xf0f0f0f0) >> 4) */ \
            /* Round 2 */ \
            __ASM_EMIT("vpslld          $2, " VA ", " VB "")                        /* VB   = v << 2 */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xcccccccc) */ \
            __ASM_EMIT("vpandd          0x08 + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x33333333) << 2 */ \
            __ASM_EMIT("vpsrld          $2, " VA ", " VA "")                        /* VA   = (v & 0xcccccccc) >> 2 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x33333333) << 2) | ((v & 0xcccccccc) >> 2) */ \
            /* Round 3 */ \
            __ASM_EMIT("vpslld          $1, " VA ", " VB "")                        /* VB   = v << 1 */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VA ", " VA "")    /* VA   = (v & 0xaaaaaaaa) */ \
            __ASM_EMIT("vpandd          0x0c + %[CMD]%{1to16%}, " VB ", " VB "")    /* VB   = (v & 0x55555555) << 1 */ \
            __ASM_EMIT("vpsrld          $1, " VA ", " VA "")                        /* VA   = (v & 0xaaaaaaaa) >> 1 */ \
            __ASM_EMIT("vpord           " VB ", " VA ", " VA "")                    /* VA   = ((v & 0x55555555) << 1) | ((v & 0xaaaaaaaa) >> 1) */ \
            /* Final round */ \
            __ASM_EMIT("vmovss          %[shift], " SHIFT) \
            __ASM_EMIT("vpsrld          " SHIFT " , " VA ", " VA)                   /* VA   = va = v >> shift */ \
            __ASM_EMIT("vpord           %[mask]%{1to16%}, " VA ", " VB)             /* VB   = vb = va | mask */

    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FFT_RBITS_H_ */
