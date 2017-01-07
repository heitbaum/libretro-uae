// based upon uae4all2 5c514dd6d8bb374bd22e2ee423bad99539207cbd (neon_helper.s)
// sed -r "s/^(.*)$/\"\1\\\n\"/"


#if defined(__thumb__)
#define DECL_ARMMODE(x) "  .align 2\n" "  .global " x "\n" "  .thumb\n" "  .thumb_func\n" "  .type " x ", %function\n" x ":\n"
#else
#define DECL_ARMMODE(x) "  .align 4\n" "  .global " x "\n" "  .arm\n" x ":\n"
#endif


asm(
    "  .text\n"
    "  .align 2\n"
    "Lookup_doline_n1:\n"
    "  .long 0x00000000, 0x01000000, 0x00010000, 0x01010000\n"
    "  .long 0x00000100, 0x01000100, 0x00010100, 0x01010100\n"
    "  .long 0x00000001, 0x01000001, 0x00010001, 0x01010001\n"
    "  .long 0x00000101, 0x01000101, 0x00010101, 0x01010101\n"    
    );

asm(
    "@----------------------------------------------------------------\n"
    "@ ARM_doline_n1\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void ARM_doline_n1(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("ARM_doline_n1")
    "  stmdb     sp!, {r4-r9, lr}\n"
    "\n"
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r3, r3, r2           @ real_bplpt[0]\n"
    "\n"
    "  ldr       r4, Lookup_doline_n1_ptr\n"
    "\n"
    "  sub       r3, r3, #4\n"
    "  \n"
    "ARM_doline_n1_loop:\n"
    "  ldr       r2, [r3, #4]!\n"
    "@  add       r3, r3, #4\n"
    "\n"
    "  ubfx      r5, r2, #28, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r6, [r5]\n"
    "  \n"
    "  ubfx      r5, r2, #24, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r7, [r5]\n"
    "\n"
    "  ubfx      r5, r2, #20, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r8, [r5]\n"
    "\n"
    "  ubfx      r5, r2, #16, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r9, [r5]\n"
    "  stmia     r0!, {r6-r9}\n"
    "  \n"
    "  ubfx      r5, r2, #12, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r6, [r5]\n"
    "\n"
    "  ubfx      r5, r2, #8, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r7, [r5]\n"
    "\n"
    "  ubfx      r5, r2, #4, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r8, [r5]\n"
    "\n"
    "  ubfx      r5, r2, #0, #4\n"
    "  add       r5, r4, r5, lsl #2\n"
    "  ldr       r9, [r5]\n"
    "  stmia     r0!, {r6-r9}\n"
    "\n"
    "  subs      r1, r1, #1\n"
    "  bgt       ARM_doline_n1_loop\n"
    "  \n"
    "  ldmia     sp!, {r4-r9, pc}\n"
    );

asm(
    "  .align 2\n"
    "Lookup_doline_n1_ptr:\n"
    "  .word	Lookup_doline_n1\n"
    "ptr_line_data:\n"
    "  .word	line_data\n"
    );

asm(
    "@----------------------------------------------------------------\n"
    "@ pfield_doline_n2\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void pfield_doline_n2(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("NEON_doline_n2")
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r2, r3, r2           @ real_bplpt[0]\n"
    "  add       r3, r2, #200\n"
    "\n"
    "@ Load masks to registers\n"
    "  vmov.u8   d18, #0x55\n"
    "  vmov.u8   d19, #0x33\n"
    "  vmov.u8   d20, #0x0f\n"
    "\n"
    "pfield_doline_n2_loop:\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r2!, {d4}\n"
    "  vldmia    r3!, {d6}\n"
    "\n"
    "@      MERGE (b6, b7, 0x55555555, 1);\n"
    "  vshr.u8   d16, d4, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d6, #1      @ tmpa = a << shift\n"
    "  vbit.u8   d6, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE_0(b4, b6, 0x33333333, 2);\n"
    "  vshr.u8   d16, d6, #2		  @ tmp = b >> shift\n"
    "  vand.8    d2, d16, d19     @ a = tmp & mask\n"
    "  vand.8    d6, d6, d19     @ b = b & mask\n"
    "@      MERGE_0(b5, b7, 0x33333333, 2);\n"
    "  vshr.u8   d16, d4, #2		  @ tmp = b >> shift\n"
    "  vand.8    d0, d16, d19     @ a = tmp & mask\n"
    "  vand.8    d4, d4, d19     @ b = b & mask\n"
    "\n"
    "@      MERGE_0(b0, b4, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d2, #4		  @ tmp = b >> shift\n"
    "  vand.8    d3, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d2, d2, d20     @ b = b & mask\n"
    "@      MERGE_0(b1, b5, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d0, #4			@ tmp = b >> shift\n"
    "  vand.8    d1, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d0, d0, d20     @ b = b & mask\n"
    "@      MERGE_0(b2, b6, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d6, #4			@ tmp = b >> shift\n"
    "  vand.8    d7, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d6, d6, d20     @ b = b & mask\n"
    "@      MERGE_0(b3, b7, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d4, #4			@ tmp = b >> shift\n"
    "  vand.8    d5, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d4, d4, d20     @ b = b & mask\n"
    "  \n"
    "  vzip.8    d3, d7\n"
    "  vzip.8    d1, d5\n"
    "  vzip.8    d2, d6\n"
    "  vzip.8    d0, d4\n"
    "\n"
    "  vzip.8    d3, d1\n"
    "  vzip.8    d2, d0\n"
    "  vzip.32   d3, d2\n"
    "  vzip.32   d1, d0\n"
    "\n"
    "  vst1.8    {d0, d1, d2, d3}, [r0]!\n"
    "  \n"
    "  cmp       r1, #1    @ Exit from here if odd number of words\n"
#ifdef __thumb__
    "  it	 eq\n"
#endif /*__thumb__*/
    "  bxeq      lr\n"
    "   \n"
    "  subs      r1, r1, #2    @ We handle 2 words (64 bit) per loop: wordcount -= 2\n"
    "\n"
    "  vzip.8    d7, d5\n"
    "  vzip.8    d6, d4\n"
    "  vzip.32   d7, d6\n"
    "  vzip.32   d5, d4\n"
    "\n"
    "  vst1.8    {d4, d5, d6, d7}, [r0]!\n"
    "\n"
    "  bgt       pfield_doline_n2_loop\n"
    "  \n"
    "pfield_doline_n2_exit:\n"
    "  bx        lr\n"
    );

asm(
    "@----------------------------------------------------------------\n"
    "@ pfield_doline_n3\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void pfield_doline_n3(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("NEON_doline_n3")
    "  stmdb     sp!, {r4, lr}\n"
    "\n"
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r2, r3, r2           @ real_bplpt[0]\n"
    "  add       r3, r2, #200\n"
    "  add       r4, r3, #200\n"
    "\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r4!, {d0}\n"
    "\n"
    "@ Load masks to registers\n"
    "  vmov.u8   d18, #0x55\n"
    "  vmov.u8   d19, #0x33\n"
    "  vmov.u8   d20, #0x0f\n"
    "\n"
    "pfield_doline_n3_loop:\n"
    "@ Load from real_bplpt (now loaded earlier)\n"
    "@  vld1.8    d0, [r4]!\n"
    "@  vld1.8    d4, [r2]!\n"
    "@  vld1.8    d6, [r3]!\n"
    "\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r2!, {d4}\n"
    "  vldmia    r3!, {d6}\n"
    "\n"
    "@      MERGE_0(b4, b5, 0x55555555, 1);\n"
    "  vshr.u8   d16, d0, #1		  @ tmp = b >> shift\n"
    "  vand.8    d2, d16, d18     @ a = tmp & mask\n"
    "  vand.8    d0, d0, d18     @ b = b & mask\n"
    "\n"
    "@      MERGE (b6, b7, 0x55555555, 1);\n"
    "  vshr.u8   d16, d4, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d6, #1      @ tmpa = a << shift\n"
    "  vbit.u8   d6, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE (b4, b6, 0x33333333, 2);\n"
    "  vshr.u8   d16, d6, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d2, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b5, b7, 0x33333333, 2);\n"
    "  vshr.u8   d16, d4, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d0, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d0, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE_0(b0, b4, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d2, #4		  @ tmp = b >> shift\n"
    "  vand.8    d3, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d2, d2, d20     @ b = b & mask\n"
    "@      MERGE_0(b1, b5, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d0, #4			@ tmp = b >> shift\n"
    "  vand.8    d1, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d0, d0, d20     @ b = b & mask\n"
    "@      MERGE_0(b2, b6, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d6, #4			@ tmp = b >> shift\n"
    "  vand.8    d7, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d6, d6, d20     @ b = b & mask\n"
    "@      MERGE_0(b3, b7, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d4, #4			@ tmp = b >> shift\n"
    "  vand.8    d5, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d4, d4, d20     @ b = b & mask\n"
    "  \n"
    "  vzip.8    d3, d7\n"
    "  vzip.8    d1, d5\n"
    "  vzip.8    d2, d6\n"
    "  vzip.8    d0, d4\n"
    "\n"
    "  vzip.8    d3, d1\n"
    "  vzip.8    d2, d0\n"
    "  vzip.32   d3, d2\n"
    "  vzip.32   d1, d0\n"
    "\n"
    "  vst1.8    {d0, d1, d2, d3}, [r0]!\n"
    "  \n"
    "  cmp       r1, #1    @ Exit from here if odd number of words\n"
#ifdef __thumb__
    "  it	 eq\n"
#endif /*__thumb__*/
    "  ldmeqia   sp!, {r4, pc}\n"
    "  \n"
    "  subs      r1, r1, #2    @ We handle 2 words (64 bit) per loop: wordcount -= 2\n"
    "\n"
    "  @ Load next data (if needed) as early as possible\n"
#ifdef __thumb__
    "  it	 gt\n"
#endif /*__thumb__*/
    "  vldmiagt  r4!, {d0}\n"
    "\n"
    "  vzip.8    d7, d5\n"
    "  vzip.8    d6, d4\n"
    "  vzip.32   d7, d6\n"
    "  vzip.32   d5, d4\n"
    "\n"
    "  vst1.8    {d4, d5, d6, d7}, [r0]!\n"
    "\n"
    "  bgt       pfield_doline_n3_loop\n"
    "  \n"
    "pfield_doline_n3_exit:\n"
    "  ldmia     sp!, {r4, pc}\n"
    );

asm(
    "@----------------------------------------------------------------\n"
    "@ pfield_doline_n4\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void pfield_doline_n4(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("NEON_doline_n4")
    "  stmdb     sp!, {r4-r5, lr}\n"
    "\n"
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r2, r3, r2           @ real_bplpt[0]\n"
    "  add       r3, r2, #200\n"
    "  add       r4, r3, #200\n"
    "  add       r5, r4, #200\n"
    "\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r4!, {d0}\n"
    "  vldmia    r5!, {d2}\n"
    "\n"
    "@ Load masks to registers\n"
    "  vmov.u8   d18, #0x55\n"
    "  vmov.u8   d19, #0x33\n"
    "  vmov.u8   d20, #0x0f\n"
    "\n"
    "pfield_doline_n4_loop:\n"
    "@ Load from real_bplpt (now loaded earlier)\n"
    "@  vld1.8    d0, [r4]!\n"
    "@  vld1.8    d2, [r5]!\n"
    "@  vld1.8    d4, [r2]!\n"
    "@  vld1.8    d6, [r3]!\n"
    "\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r2!, {d4}\n"
    "\n"
    "@      MERGE (b4, b5, 0x55555555, 1);\n"
    "  vshr.u8   d16, d0, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #1      @ tmpa = a << shift\n"
    "\n"
    "  vldmia    r3!, {d6}\n"
    "\n"
    "  vbit.u8   d2, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d0, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b6, b7, 0x55555555, 1);\n"
    "  vshr.u8   d16, d4, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d6, #1      @ tmpa = a << shift\n"
    "  vbit.u8   d6, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE (b4, b6, 0x33333333, 2);\n"
    "  vshr.u8   d16, d6, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d2, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b5, b7, 0x33333333, 2);\n"
    "  vshr.u8   d16, d4, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d0, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d0, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE_0(b0, b4, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d2, #4		  @ tmp = b >> shift\n"
    "  vand.8    d3, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d2, d2, d20     @ b = b & mask\n"
    "@      MERGE_0(b1, b5, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d0, #4			@ tmp = b >> shift\n"
    "  vand.8    d1, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d0, d0, d20     @ b = b & mask\n"
    "@      MERGE_0(b2, b6, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d6, #4			@ tmp = b >> shift\n"
    "  vand.8    d7, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d6, d6, d20     @ b = b & mask\n"
    "@      MERGE_0(b3, b7, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d4, #4			@ tmp = b >> shift\n"
    "  vand.8    d5, d16, d20     @ a = tmp & mask\n"
    "  vand.8    d4, d4, d20     @ b = b & mask\n"
    "  \n"
    "  vzip.8    d3, d7\n"
    "  vzip.8    d1, d5\n"
    "  vzip.8    d2, d6\n"
    "  vzip.8    d0, d4\n"
    "\n"
    "  vzip.8    d3, d1\n"
    "  vzip.8    d2, d0\n"
    "  vzip.32   d3, d2\n"
    "  vzip.32   d1, d0\n"
    "\n"
    "  vst1.8    {d0, d1, d2, d3}, [r0]!\n"
    "  \n"
    "  cmp       r1, #1    @ Exit from here if odd number of words\n"
#ifdef __thumb__
    "  it	 eq\n"
#endif /*__thumb__*/
    "  ldmeqia   sp!, {r4-r5, pc}\n"
    "  \n"
    "  subs      r1, r1, #2    @ We handle 2 words (64 bit) per loop: wordcount -= 2\n"
#ifdef __thumb__
    "  it	 gt\n"
#endif /*__thumb__*/
    "\n"
    "  @ Load next data (if needed) as early as possible\n"
    "  vldmiagt  r4!, {d0}\n"
    "\n"
    "  vzip.8    d7, d5\n"
    "  vzip.8    d6, d4\n"
    "\n"
#ifdef __thumb__
    "  it	 gt\n"
#endif /*__thumb__*/
    "  vldmiagt  r5!, {d2}\n"
    "\n"
    "  vzip.32   d7, d6\n"
    "  vzip.32   d5, d4\n"
    "\n"
    "  vst1.8    {d4, d5, d6, d7}, [r0]!\n"
    "\n"
    "  bgt       pfield_doline_n4_loop\n"
    "  \n"
    "pfield_doline_n4_exit:\n"
    "  ldmia     sp!, {r4-r5, pc}\n"
    );

asm(
    "@----------------------------------------------------------------\n"
    "@ pfield_doline_n6\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void pfield_doline_n6(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("NEON_doline_n6")
    "  stmdb     sp!, {r4-r7, lr}\n"
    "  \n"
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r2, r3, r2           @ real_bplpt[0]\n"
    "  add       r3, r2, #200\n"
    "  add       r4, r3, #200\n"
    "  add       r5, r4, #200\n"
    "  add       r6, r5, #200\n"
    "  add       r7, r6, #200\n"
    "  \n"
    "@ Load masks to registers\n"
    "  vmov.u8   d18, #0x55\n"
    "  vmov.u8   d19, #0x33\n"
    "  vmov.u8   d20, #0x0f\n"
    "\n"
    "pfield_doline_n6_loop:\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r6!, {d5}\n"
    "  vldmia    r7!, {d7}\n"
    "\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r4!, {d0}\n"
    "@      MERGE (b2, b3, 0x55555555, 1);\n"
    "  vshr.u8   d16, d5, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d7, #1      @ tmpa = a << shift\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r5!, {d2}\n"
    "  vbit.u8   d7, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d5, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r2!, {d4}\n"
    "@      MERGE (b4, b5, 0x55555555, 1);\n"
    "  vshr.u8   d16, d0, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #1      @ tmpa = a << shift\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r3!, {d6}\n"
    "  vbit.u8   d2, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d0, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b6, b7, 0x55555555, 1);\n"
    "  vshr.u8   d16, d4, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d6, #1      @ tmpa = a << shift\n"
    "  vbit.u8   d6, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE_0(b0, b2, 0x33333333, 2);\n"
    "  vshr.u8   d16, d7, #2		  @ tmp = b >> shift\n"
    "  vand.8    d3, d16, d19     @ a = tmp & mask\n"
    "  vand.8    d7, d7, d19     @ b = b & mask\n"
    "@      MERGE_0(b1, b3, 0x33333333, 2);\n"
    "  vshr.u8   d16, d5, #2		  @ tmp = b >> shift\n"
    "  vand.8    d1, d16, d19     @ a = tmp & mask\n"
    "  vand.8    d5, d5, d19     @ b = b & mask\n"
    "@      MERGE (b4, b6, 0x33333333, 2);\n"
    "  vshr.u8   d16, d6, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d2, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b5, b7, 0x33333333, 2);\n"
    "  vshr.u8   d16, d4, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d0, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d0, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE (b0, b4, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d2, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d3, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d3, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d2, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b1, b5, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d0, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d1, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d1, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d0, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b2, b6, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d6, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d7, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d7, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b3, b7, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d4, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d5, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d5, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "  vzip.8    d3, d7\n"
    "  vzip.8    d1, d5\n"
    "  vzip.8    d2, d6\n"
    "  vzip.8    d0, d4\n"
    "\n"
    "  vzip.8    d3, d1\n"
    "  vzip.8    d2, d0\n"
    "  vzip.32   d3, d2\n"
    "  vzip.32   d1, d0\n"
    "\n"
    "  vst1.8    {d0, d1, d2, d3}, [r0]!\n"
    "  \n"
    "  cmp       r1, #1    @ Exit from here if odd number of words\n"
#ifdef __thumb__
    "  it	 eq\n"
#endif /*__thumb__*/
    "  ldmeqia   sp!, {r4-r7, pc}\n"
    "\n"
    "  subs      r1, r1, #2    @ We handle 2 words (64 bit) per loop: wordcount -= 2\n"
    "\n"
    "  vzip.8    d7, d5\n"
    "  vzip.8    d6, d4\n"
    "  vzip.32   d7, d6\n"
    "  vzip.32   d5, d4\n"
    "\n"
    "  vst1.8    {d4, d5, d6, d7}, [r0]!\n"
    "\n"
    "  bgt       pfield_doline_n6_loop\n"
    "  \n"
    "pfield_doline_n6_exit:\n"
    "  ldmia     sp!, {r4-r7, pc}\n");

#ifdef AGA
asm(
    "@----------------------------------------------------------------\n"
    "@ pfield_doline_n8\n"
    "@\n"
    "@ r0: uae_u32   *pixels\n"
    "@ r1: int       wordcount\n"
    "@ r2: int       lineno \n"
    "@\n"
    "@ void pfield_doline_n8(uae_u32 *pixels, int wordcount, int lineno);\n"
    "@\n"
    "@----------------------------------------------------------------\n"
    DECL_ARMMODE("NEON_doline_n8")
    "  stmdb     sp!, {r4-r9, lr}\n"
    "  \n"
    "  mov       r3, #1600\n"
    "  mul       r2, r2, r3\n"
    "  ldr       r3, ptr_line_data\n"
    "  add       r2, r3, r2           @ real_bplpt[0]\n"
    "  add       r3, r2, #200\n"
    "  add       r4, r3, #200\n"
    "  add       r5, r4, #200\n"
    "  add       r6, r5, #200\n"
    "  add       r7, r6, #200\n"
    "  add       r8, r7, #200\n"
    "  add       r9, r8, #200\n"
    "  \n"
    "  @ Load data as early as possible\n"
    "  vldmia    r8!, {d1}\n"
    "  vldmia    r9!, {d3}\n"
    "\n"
    "@ Load masks to registers\n"
    "  vmov.u8   d18, #0x55\n"
    "  vmov.u8   d19, #0x33\n"
    "  vmov.u8   d20, #0x0f\n"
    "\n"
    "pfield_doline_n8_loop:\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r6!, {d5}\n"
    "@      MERGE (b0, b1, 0x55555555, 1);\n"
    "  vshr.u8   d16, d1, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d3, #1      @ tmpa = a << shift\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r7!, {d7}\n"
    "  vbit.u8   d3, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d1, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r4!, {d0}\n"
    "@      MERGE (b2, b3, 0x55555555, 1);\n"
    "  vshr.u8   d16, d5, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d7, #1      @ tmpa = a << shift\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r5!, {d2}\n"
    "  vbit.u8   d7, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d5, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r2!, {d4}\n"
    "@      MERGE (b4, b5, 0x55555555, 1);\n"
    "  vshr.u8   d16, d0, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #1      @ tmpa = a << shift\n"
    "  @ Load data as early as possible\n"
    "  vldmia    r3!, {d6}\n"
    "  vbit.u8   d2, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d0, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b6, b7, 0x55555555, 1);\n"
    "  vshr.u8   d16, d4, #1      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d6, #1      @ tmpa = a << shift\n"
    "  vbit.u8   d6, d16, d18     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d18     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE (b0, b2, 0x33333333, 2);\n"
    "  vshr.u8   d16, d7, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d3, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d3, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d7, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b1, b3, 0x33333333, 2);\n"
    "  vshr.u8   d16, d5, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d1, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d1, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d5, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b4, b6, 0x33333333, 2);\n"
    "  vshr.u8   d16, d6, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d2, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d2, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b5, b7, 0x33333333, 2);\n"
    "  vshr.u8   d16, d4, #2      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d0, #2      @ tmpa = a << shift\n"
    "  vbit.u8   d0, d16, d19     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d19     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "@      MERGE (b0, b4, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d2, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d3, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d3, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d2, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b1, b5, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d0, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d1, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d1, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d0, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b2, b6, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d6, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d7, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d7, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d6, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "@      MERGE (b3, b7, 0x0f0f0f0f, 4);\n"
    "  vshr.u8   d16, d4, #4      @ tmpb = b >> shift\n"
    "  vshl.u8   d17, d5, #4      @ tmpa = a << shift\n"
    "  vbit.u8   d5, d16, d20     @ a = a and bit set from tmpb if mask is true \n"
    "  vbif.u8   d4, d17, d20     @ b = b and bit set from tmpa if mask is false\n"
    "\n"
    "  vzip.8    d3, d7\n"
    "  vzip.8    d1, d5\n"
    "  vzip.8    d2, d6\n"
    "  vzip.8    d0, d4\n"
    "\n"
    "  vzip.8    d3, d1\n"
    "  vzip.8    d2, d0\n"
    "  vzip.32   d3, d2\n"
    "  vzip.32   d1, d0\n"
    "\n"
    "  vst1.8    {d0, d1, d2, d3}, [r0]!\n"
    "  \n"
    "  cmp       r1, #1    @ Exit from here if odd number of words\n"
#ifdef __thumb__
    "  it	 eq\n"
#endif /*__thumb__*/
    "  ldmeqia   sp!, {r4-r9, pc}\n"
    "\n"
    "  subs      r1, r1, #2    @ We handle 2 words (64 bit) per loop: wordcount -= 2\n"
    "\n"
    "  @ Load data as early as possible\n"
#ifdef __thumb__
    "  it	 gt\n"
#endif /*__thumb__*/
    "  vldmiagt  r8!, {d1}\n"
    "  \n"
    "  vzip.8    d7, d5\n"
    "  vzip.8    d6, d4\n"
    "\n"
    "  @ Load data as early as possible\n"
#ifdef __thumb__
    "  it	 gt\n"
#endif /*__thumb__*/
    "  vldmiagt  r9!, {d3}\n"
    "\n"
    "  vzip.32   d7, d6\n"
    "  vzip.32   d5, d4\n"
    "\n"
    "  vst1.8    {d4, d5, d6, d7}, [r0]!\n"
    "\n"
    "  bgt       pfield_doline_n8_loop\n"
    "  \n"
    "pfield_doline_n8_exit:\n"
    "  ldmia     sp!, {r4-r9, pc}\n"
    );
#else /*AGA*/
#define NEON_doline_n8 pfield_doline_dummy
#endif /*AGA*/
