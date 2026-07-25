                //---------------------------------------------------------------------
                // Cycle 1
                __ASM_EMIT("mov         %[dst], %[X_D]")
                __ASM_EMIT("mov         %[count], %[X_COUNT]")

                // Initialize mask
                // xmm0=tmp, xmm1={s,s2[4]}, xmm2=p1[4], xmm3=p2[4], xmm6=d0[4], xmm7=d1[4]
                __ASM_EMIT("mov         $1, %[mask]")
                __ASM_EMIT("movaps      %[X_MASK], %%xmm0")
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")
                __ASM_EMIT("movaps      %%xmm0, %[MASK]")

                // Load delay buffer
                __ASM_EMIT("movaps      0x00(%[f]), %%xmm6")                        // xmm6     = d0
                __ASM_EMIT("movaps      0x20(%[f]), %%xmm7")                        // xmm7     = d1

                // Process first 3 steps
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
                __ASM_EMIT("mulps       0x00 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
                __ASM_EMIT("mulps       0x20 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
                __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
                __ASM_EMIT("mulps       0x40 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
                __ASM_EMIT("mulps       0x60 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
                __ASM_EMIT("mulps       0x80 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

                // Shift buffer
                __ASM_EMIT("shufps      $0x90, %%xmm1, %%xmm1")                     // xmm1     = s2[0] s2[0] s2[1] s2[2]

                // Update delay only by mask
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
                __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = MASK
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
                __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
                __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
                __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
                __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
                __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
                __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
                __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

                // Repeat loop
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jz          4f")                                        // jump to completion
                __ASM_EMIT("lea         0x01(,%[mask], 2), %[mask]")                // mask     = (mask << 1) | 1
                __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("movaps      %%xmm0, %[MASK]")                           // store mask
                __ASM_EMIT("cmp         $0x0f, %[mask]")
                __ASM_EMIT("jne         1b")

                // 4x filter processing without mask
                __ASM_EMIT(".align 16")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       (%[src]), %%xmm0")                          // xmm0     = *src
                __ASM_EMIT("add         $4, %[src]")                                // src      ++
                __ASM_EMIT("movss       %%xmm0, %%xmm1")                            // xmm1     = s
                __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
                __ASM_EMIT("mulps       0x00 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
                __ASM_EMIT("mulps       0x20 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
                __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
                __ASM_EMIT("mulps       0x40 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
                __ASM_EMIT("mulps       0x60 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
                __ASM_EMIT("mulps       0x80 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            // xmm7     = p2
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            // xmm6     = p1 + d1

                // Shift buffer and repeat loop
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         3b")

                // Prepare last loop
                __ASM_EMIT("4:")
                __ASM_EMIT("movaps      %[MASK], %%xmm0")                           // xmm0     = m[0] m[1] m[2] m[3]
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]

                // Process steps
                __ASM_EMIT(".align 16")
                __ASM_EMIT("5:")
                __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            // xmm2     = s
                __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            // xmm3     = s
                __ASM_EMIT("mulps       0x00 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm1")   // xmm1     = s*a0
                __ASM_EMIT("mulps       0x20 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm2")   // xmm2     = s*a1
                __ASM_EMIT("addps       %%xmm6, %%xmm1")                            // xmm1     = s*a0+d0 = s2
                __ASM_EMIT("mulps       0x40 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm3")   // xmm3     = s*a2
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            // xmm4     = s2
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            // xmm5     = s2
                __ASM_EMIT("mulps       0x60 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm4")   // xmm4     = s2*b1
                __ASM_EMIT("mulps       0x80 + " LSP_DSP_BIQUAD_XN_SOFF "(%[f]), %%xmm5")   // xmm5     = s2*b2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                            // xmm2     = s*a1 + s2*b1 = p1
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                            // xmm3     = s*a2 + s2*b2 = p2

                // Shift buffer and store
                __ASM_EMIT("test        $0x8, %[mask]")
                __ASM_EMIT("shufps      $0x93, %%xmm1, %%xmm1")                     // xmm1     = s2[3] s2[0] s2[1] s2[2]
                __ASM_EMIT("jz          7f")
                __ASM_EMIT("movss       %%xmm1, (%[dst])")                          // *dst     = s2[3]
                __ASM_EMIT("add         $4, %[dst]")                                // dst      ++
                __ASM_EMIT("7:")

                // Update delay only by mask
                __ASM_EMIT("addps       %%xmm7, %%xmm2")                            // xmm2     = p1 + d1
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            // xmm4     = MASK
                __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            // xmm5     = MASK
                __ASM_EMIT("andps       %%xmm4, %%xmm2")                            // xmm2     = (p1 + d1) & MASK
                __ASM_EMIT("andps       %%xmm5, %%xmm3")                            // xmm3     = p2 & MASK
                __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            // xmm4     = d0 & ~MASK
                __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            // xmm5     = d1 & ~MASK
                __ASM_EMIT("orps        %%xmm2, %%xmm4")                            // xmm4     = (p1 + d1) & MASK | (d0 & ~MASK)
                __ASM_EMIT("orps        %%xmm3, %%xmm5")                            // xmm5     = (p2 & MASK) | (d1 & ~MASK)
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            // xmm6     = d0 & ~MASK
                __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            // xmm7     = d1 & ~MASK

                // Repeat loop
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                            // xmm2     = 0 0 0 0
                __ASM_EMIT("shl         $1, %[mask]")                               // mask     = mask << 1
                __ASM_EMIT("shufps      $0x90, %%xmm0, %%xmm0")                     // xmm0     = m[0] m[0] m[1] m[2]
                __ASM_EMIT("and         $0x0f, %[mask]")                            // mask     = (mask << 1) & 0x0f
                __ASM_EMIT("movss       %%xmm2, %%xmm0")                            // xmm0     = 0 m[0] m[1] m[2]
                __ASM_EMIT("jnz         5b")                                        // check that mask is not zero

                // Store delay buffer
                __ASM_EMIT("movaps      %%xmm6, 0x00(%[f])")                        // xmm6     = d0
                __ASM_EMIT("movaps      %%xmm7, 0x20(%[f])")                        // xmm7     = d1
