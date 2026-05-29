	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 5
	.globl	__Z6reportPKcS0_xS0_x           ; -- Begin function _Z6reportPKcS0_xS0_x
	.p2align	2
__Z6reportPKcS0_xS0_x:                  ; @_Z6reportPKcS0_xS0_x
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	sub	sp, sp, #128
	stp	d9, d8, [sp, #32]               ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset b8, -88
	.cfi_offset b9, -96
	mov	x19, x4
	mov	x20, x3
	mov	x21, x2
	mov	x22, x1
	mov	x23, x0
	mov	x25, #72                        ; =0x48
	movk	x25, #32768, lsl #48
Lloh0:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh1:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh2:
	adrp	x1, l_.str@PAGE
Lloh3:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	x24, x0
	mov	w0, #72                         ; =0x48
	bl	__Znwm
	mov	w8, #64                         ; =0x40
	str	x0, [sp, #8]
	stp	x8, x25, [sp, #16]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	strb	wzr, [x0, #64]
	ldrsb	w9, [sp, #31]
	add	x10, sp, #8
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp0:
	and	x2, x8, #0xff
	mov	x0, x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp1:
; %bb.1:
Ltmp2:
Lloh4:
	adrp	x1, l_.str@PAGE
Lloh5:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp3:
; %bb.2:
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB0_4
; %bb.3:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB0_4:
	mov	x0, x23
	bl	_strlen
	mov	x2, x0
Lloh6:
	adrp	x24, __ZNSt3__14coutE@GOTPAGE
Lloh7:
	ldr	x24, [x24, __ZNSt3__14coutE@GOTPAGEOFF]
	mov	x0, x24
	mov	x1, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lloh8:
	adrp	x1, l_.str@PAGE
Lloh9:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	w0, #72                         ; =0x48
	bl	__Znwm
	mov	w8, #64                         ; =0x40
	str	x0, [sp, #8]
	stp	x8, x25, [sp, #16]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	strb	wzr, [x0, #64]
	ldrsb	w9, [sp, #31]
	add	x10, sp, #8
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp5:
	and	x2, x8, #0xff
	mov	x0, x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp6:
; %bb.5:
Ltmp7:
Lloh10:
	adrp	x1, l_.str@PAGE
Lloh11:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp8:
; %bb.6:
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB0_8
; %bb.7:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB0_8:
	scvtf	d0, x21
	cmp	x19, #1
	csinc	x8, x19, xzr, gt
	ucvtf	d1, x8
	fdiv	d8, d0, d1
Lloh12:
	adrp	x23, __ZNSt3__14coutE@GOTPAGE
Lloh13:
	ldr	x23, [x23, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh14:
	adrp	x1, l_.str.1@PAGE
Lloh15:
	add	x1, x1, l_.str.1@PAGEOFF
	mov	x0, x23
	mov	w2, #11                         ; =0xb
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	x24, x0
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	mov	w25, #-177                      ; =0xffffff4f
	and	w10, w10, w25
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w26, #44                        ; =0x2c
	str	x26, [x8, #24]
	mov	x0, x22
	bl	_strlen
	mov	x2, x0
	mov	x0, x24
	mov	x1, x22
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w24, #5                         ; =0x5
	str	x24, [x8, #24]
	mov	x1, x21
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Lloh16:
	adrp	x21, l_.str.2@PAGE
Lloh17:
	add	x21, x21, l_.str.2@PAGEOFF
	mov	x1, x21
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lloh18:
	adrp	x1, l_.str.3@PAGE
Lloh19:
	add	x1, x1, l_.str.3@PAGEOFF
	mov	x0, x23
	mov	w2, #11                         ; =0xb
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	x22, x0
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, w25
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x26, [x8, #24]
	mov	x0, x20
	bl	_strlen
	mov	x2, x0
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x24, [x8, #24]
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
	mov	x1, x21
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lloh20:
	adrp	x1, l_.str.4@PAGE
Lloh21:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	x0, x23
	mov	w2, #11                         ; =0xb
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xfffffeff
	orr	w10, w10, #0x4
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	fmov	d0, d8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Lloh22:
	adrp	x1, l_.str.5@PAGE
Lloh23:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #32]               ; 16-byte Folded Reload
	add	sp, sp, #128
	ret
LBB0_9:
Ltmp9:
	b	LBB0_11
LBB0_10:
Ltmp4:
LBB0_11:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB0_13
; %bb.12:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB0_13:
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpLdrGot	Lloh0, Lloh1
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh8, Lloh9
	.loh AdrpLdrGot	Lloh6, Lloh7
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpAdd	Lloh22, Lloh23
	.loh AdrpAdd	Lloh20, Lloh21
	.loh AdrpAdd	Lloh18, Lloh19
	.loh AdrpAdd	Lloh16, Lloh17
	.loh AdrpAdd	Lloh14, Lloh15
	.loh AdrpLdrGot	Lloh12, Lloh13
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table0:
Lexception0:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ; >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ;   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ; >> Call Site 2 <<
	.uleb128 Ltmp3-Ltmp0                    ;   Call between Ltmp0 and Ltmp3
	.uleb128 Ltmp4-Lfunc_begin0             ;     jumps to Ltmp4
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ; >> Call Site 3 <<
	.uleb128 Ltmp5-Ltmp3                    ;   Call between Ltmp3 and Ltmp5
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp5-Lfunc_begin0             ; >> Call Site 4 <<
	.uleb128 Ltmp8-Ltmp5                    ;   Call between Ltmp5 and Ltmp8
	.uleb128 Ltmp9-Lfunc_begin0             ;     jumps to Ltmp9
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp8-Lfunc_begin0             ; >> Call Site 5 <<
	.uleb128 Lfunc_end0-Ltmp8               ;   Call between Ltmp8 and Lfunc_end0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z15correlate_fusedPKfPKS0_Pfi ; -- Begin function _Z15correlate_fusedPKfPKS0_Pfi
	.p2align	2
__Z15correlate_fusedPKfPKS0_Pfi:        ; @_Z15correlate_fusedPKfPKS0_Pfi
	.cfi_startproc
; %bb.0:
                                        ; kill: def $w3 killed $w3 def $x3
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x2, #32]
	stp	q0, q0, [x2]
	cmp	w3, #1
	b.lt	LBB1_4
; %bb.1:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	mov	x8, #0                          ; =0x0
	ldp	x9, x10, [x1]
	ldp	x11, x12, [x1, #16]
	ldp	x13, x14, [x1, #32]
	ldp	x15, x16, [x1, #48]
	ldp	x17, x4, [x1, #64]
	ldp	x5, x6, [x1, #80]
	ldp	x7, x19, [x1, #96]
	ldp	x20, x1, [x1, #112]
	ldp	s0, s1, [x2]
	ldp	s2, s3, [x2, #8]
	ldp	s4, s5, [x2, #16]
	ldp	s6, s7, [x2, #24]
	ldp	s16, s17, [x2, #32]
	ldp	s18, s19, [x2, #40]
	ldp	s20, s21, [x2, #48]
	ubfiz	x3, x3, #2, #32
	ldp	s22, s23, [x2, #56]
LBB1_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s24, [x0, x8]
	ldr	s25, [x9, x8]
	fmadd	s0, s24, s25, s0
	str	s0, [x2]
	ldr	s25, [x10, x8]
	fmadd	s1, s24, s25, s1
	str	s1, [x2, #4]
	ldr	s25, [x11, x8]
	fmadd	s2, s24, s25, s2
	str	s2, [x2, #8]
	ldr	s25, [x12, x8]
	fmadd	s3, s24, s25, s3
	str	s3, [x2, #12]
	ldr	s25, [x13, x8]
	fmadd	s4, s24, s25, s4
	str	s4, [x2, #16]
	ldr	s25, [x14, x8]
	fmadd	s5, s24, s25, s5
	str	s5, [x2, #20]
	ldr	s25, [x15, x8]
	fmadd	s6, s24, s25, s6
	str	s6, [x2, #24]
	ldr	s25, [x16, x8]
	fmadd	s7, s24, s25, s7
	str	s7, [x2, #28]
	ldr	s25, [x17, x8]
	fmadd	s16, s24, s25, s16
	str	s16, [x2, #32]
	ldr	s25, [x4, x8]
	fmadd	s17, s24, s25, s17
	str	s17, [x2, #36]
	ldr	s25, [x5, x8]
	fmadd	s18, s24, s25, s18
	str	s18, [x2, #40]
	ldr	s25, [x6, x8]
	fmadd	s19, s24, s25, s19
	str	s19, [x2, #44]
	ldr	s25, [x7, x8]
	fmadd	s20, s24, s25, s20
	str	s20, [x2, #48]
	ldr	s25, [x19, x8]
	fmadd	s21, s24, s25, s21
	str	s21, [x2, #52]
	ldr	s25, [x20, x8]
	fmadd	s22, s24, s25, s22
	str	s22, [x2, #56]
	ldr	s25, [x1, x8]
	fmadd	s23, s24, s25, s23
	str	s23, [x2, #60]
	add	x8, x8, #4
	cmp	x3, x8
	b.ne	LBB1_2
; %bb.3:
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
LBB1_4:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z17correlate_fissionPKfPKS0_Pfi ; -- Begin function _Z17correlate_fissionPKfPKS0_Pfi
	.p2align	2
__Z17correlate_fissionPKfPKS0_Pfi:      ; @_Z17correlate_fissionPKfPKS0_Pfi
	.cfi_startproc
; %bb.0:
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x2, #32]
	stp	q0, q0, [x2]
	cmp	w3, #1
	b.lt	LBB2_6
; %bb.1:
	ldp	x9, x10, [x1]
	ldp	x11, x12, [x1, #16]
	ldp	x13, x14, [x1, #32]
	ldp	x15, x16, [x1, #48]
	ldp	s0, s1, [x2]
	ldp	s2, s3, [x2, #8]
	ldp	s4, s5, [x2, #16]
	mov	w8, w3
	mov	x17, x8
	mov	x4, x0
	ldp	s6, s7, [x2, #24]
LBB2_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s16, [x4], #4
	ldr	s17, [x9], #4
	fmadd	s0, s16, s17, s0
	str	s0, [x2]
	ldr	s17, [x10], #4
	fmadd	s1, s16, s17, s1
	str	s1, [x2, #4]
	ldr	s17, [x11], #4
	fmadd	s2, s16, s17, s2
	str	s2, [x2, #8]
	ldr	s17, [x12], #4
	fmadd	s3, s16, s17, s3
	str	s3, [x2, #12]
	ldr	s17, [x13], #4
	fmadd	s4, s16, s17, s4
	str	s4, [x2, #16]
	ldr	s17, [x14], #4
	fmadd	s5, s16, s17, s5
	str	s5, [x2, #20]
	ldr	s17, [x15], #4
	fmadd	s6, s16, s17, s6
	str	s6, [x2, #24]
	ldr	s17, [x16], #4
	fmadd	s7, s16, s17, s7
	str	s7, [x2, #28]
	subs	x17, x17, #1
	b.ne	LBB2_2
; %bb.3:
	cmp	w3, #1
	b.lt	LBB2_6
; %bb.4:
	ldp	x9, x10, [x1, #64]
	ldp	x11, x12, [x1, #80]
	ldp	x13, x14, [x1, #96]
	ldp	x15, x16, [x1, #112]
	ldp	s0, s1, [x2, #32]
	ldp	s2, s3, [x2, #40]
	ldp	s4, s5, [x2, #48]
	ldp	s6, s7, [x2, #56]
LBB2_5:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s16, [x0], #4
	ldr	s17, [x9], #4
	fmadd	s0, s16, s17, s0
	str	s0, [x2, #32]
	ldr	s17, [x10], #4
	fmadd	s1, s16, s17, s1
	str	s1, [x2, #36]
	ldr	s17, [x11], #4
	fmadd	s2, s16, s17, s2
	str	s2, [x2, #40]
	ldr	s17, [x12], #4
	fmadd	s3, s16, s17, s3
	str	s3, [x2, #44]
	ldr	s17, [x13], #4
	fmadd	s4, s16, s17, s4
	str	s4, [x2, #48]
	ldr	s17, [x14], #4
	fmadd	s5, s16, s17, s5
	str	s5, [x2, #52]
	ldr	s17, [x15], #4
	fmadd	s6, s16, s17, s6
	str	s6, [x2, #56]
	ldr	s17, [x16], #4
	fmadd	s7, s16, s17, s7
	str	s7, [x2, #60]
	subs	x8, x8, #1
	b.ne	LBB2_5
LBB2_6:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
; %bb.0:
	sub	sp, sp, #160
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #80]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #96]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	w0, #23040                      ; =0x5a00
	movk	w0, #610, lsl #16
	bl	__Znwm
	mov	x19, x0
	mov	w1, #23040                      ; =0x5a00
	movk	w1, #610, lsl #16
	bl	_bzero
	stp	xzr, xzr, [sp]
	str	xzr, [sp, #16]
Ltmp10:
	mov	w0, #23040                      ; =0x5a00
	movk	w0, #610, lsl #16
	bl	__Znwm
Ltmp11:
; %bb.1:
	stp	x0, x0, [sp]
	mov	w8, #23040                      ; =0x5a00
	movk	w8, #610, lsl #16
	add	x20, x0, x8
	str	x20, [sp, #16]
	mov	w1, #23040                      ; =0x5a00
	movk	w1, #610, lsl #16
	bl	_bzero
	str	x20, [sp, #8]
Ltmp13:
	add	x0, sp, #24
	mov	x2, sp
	mov	w1, #16                         ; =0x10
	bl	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_
Ltmp14:
; %bb.2:
	ldr	x0, [sp]
	cbz	x0, LBB3_4
; %bb.3:
	str	x0, [sp, #8]
	bl	__ZdlPv
LBB3_4:
	mov	x21, #0                         ; =0x0
	mov	w20, #38528                     ; =0x9680
	movk	w20, #152, lsl #16
	mov	w8, #4719                       ; =0x126f
	movk	w8, #14979, lsl #16
	fmov	s8, w8
LBB3_5:                                 ; =>This Inner Loop Header: Depth=1
	ucvtf	s0, w21
	fmul	s0, s0, s8
	bl	_sinf
	str	s0, [x19, x21, lsl #2]
	add	x21, x21, #1
	cmp	x20, x21
	b.ne	LBB3_5
; %bb.6:
	mov	x23, #0                         ; =0x0
	ldr	x21, [sp, #24]
	mov	w22, #24                        ; =0x18
	mov	w8, #4719                       ; =0x126f
	movk	w8, #14979, lsl #16
	fmov	s8, w8
LBB3_7:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_8 Depth 2
	mov	x24, #0                         ; =0x0
	mul	x8, x23, x22
	add	x23, x23, #1
	ucvtf	s9, w23
	ldr	x25, [x21, x8]
LBB3_8:                                 ;   Parent Loop BB3_7 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ucvtf	s0, w24
	fmul	s0, s0, s8
	fmul	s0, s0, s9
	bl	_sinf
	str	s0, [x25, x24, lsl #2]
	add	x24, x24, #1
	cmp	x20, x24
	b.ne	LBB3_8
; %bb.9:                                ;   in Loop: Header=BB3_7 Depth=1
	cmp	x23, #16
	b.ne	LBB3_7
; %bb.10:
Ltmp16:
	mov	w0, #128                        ; =0x80
	bl	__Znwm
Ltmp17:
; %bb.11:
	mov	x20, x0
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x0, #96]
	stp	q0, q0, [x0, #64]
	stp	q0, q0, [x0, #32]
	stp	q0, q0, [x0]
	ldr	x9, [sp, #24]
LBB3_12:                                ; =>This Inner Loop Header: Depth=1
	ldr	x10, [x9], #24
	str	x10, [x20, x8]
	add	x8, x8, #8
	cmp	x8, #128
	b.ne	LBB3_12
; %bb.13:
Ltmp19:
	mov	w0, #64                         ; =0x40
	bl	__Znwm
Ltmp20:
; %bb.14:
	mov	x21, x0
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x0, #32]
	stp	q0, q0, [x0]
Ltmp22:
	mov	w0, #64                         ; =0x40
	bl	__Znwm
Ltmp23:
; %bb.15:
	mov	x22, x0
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x0, #32]
	stp	q0, q0, [x0]
	mov	x0, x19
	mov	x1, x20
	mov	x2, x21
	mov	w3, #38528                      ; =0x9680
	movk	w3, #152, lsl #16
	bl	__Z15correlate_fusedPKfPKS0_Pfi
	mov	x0, x19
	mov	x1, x20
	mov	x2, x22
	mov	w3, #38528                      ; =0x9680
	movk	w3, #152, lsl #16
	bl	__Z17correlate_fissionPKfPKS0_Pfi
Ltmp25:
Lloh24:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh25:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh26:
	adrp	x1, l_.str.6@PAGE
Lloh27:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #19                         ; =0x13
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp26:
; %bb.16:
	mov	x23, #0                         ; =0x0
	mov	w27, #1                         ; =0x1
	mov	w8, #4719                       ; =0x126f
	movk	w8, #14979, lsl #16
	fmov	s8, w8
Lloh28:
	adrp	x24, __ZNSt3__14coutE@GOTPAGE
Lloh29:
	ldr	x24, [x24, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh30:
	adrp	x25, l_.str.7@PAGE
Lloh31:
	add	x25, x25, l_.str.7@PAGEOFF
Lloh32:
	adrp	x26, l_.str@PAGE
Lloh33:
	add	x26, x26, l_.str@PAGEOFF
	b	LBB3_19
LBB3_17:                                ;   in Loop: Header=BB3_19 Depth=1
	mov	w27, #0                         ; =0x0
LBB3_18:                                ;   in Loop: Header=BB3_19 Depth=1
	add	x23, x23, #1
	cmp	x23, #16
	b.eq	LBB3_23
LBB3_19:                                ; =>This Inner Loop Header: Depth=1
	ldr	s0, [x21, x23, lsl #2]
	ldr	s1, [x22, x23, lsl #2]
	fabd	s0, s0, s1
	fcmp	s0, s8
	b.le	LBB3_18
; %bb.20:                               ;   in Loop: Header=BB3_19 Depth=1
Ltmp28:
	mov	x0, x24
	mov	x1, x25
	mov	w2, #9                          ; =0x9
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp29:
; %bb.21:                               ;   in Loop: Header=BB3_19 Depth=1
Ltmp30:
	mov	x1, x23
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp31:
; %bb.22:                               ;   in Loop: Header=BB3_19 Depth=1
Ltmp32:
	mov	x1, x26
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp33:
	b	LBB3_17
LBB3_23:
Ltmp35:
Lloh34:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh35:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh36:
	adrp	x1, l_.str.8@PAGE
Lloh37:
	add	x1, x1, l_.str.8@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp36:
; %bb.24:
Lloh38:
	adrp	x8, l_.str.10@PAGE
Lloh39:
	add	x8, x8, l_.str.10@PAGEOFF
Lloh40:
	adrp	x9, l_.str.9@PAGE
Lloh41:
	add	x9, x9, l_.str.9@PAGEOFF
	tst	w27, #0x1
	csel	x1, x9, x8, ne
Ltmp37:
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp38:
; %bb.25:
Ltmp39:
Lloh42:
	adrp	x1, l_.str@PAGE
Lloh43:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp40:
; %bb.26:
	mov	x23, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w25, #5                         ; =0x5
	mov	x26, #13531                     ; =0x34db
	movk	x26, #55222, lsl #16
	movk	x26, #56962, lsl #32
	movk	x26, #17179, lsl #48
LBB3_27:                                ; =>This Inner Loop Header: Depth=1
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x24, x0
	mov	x0, x19
	mov	x1, x20
	mov	x2, x21
	mov	w3, #38528                      ; =0x9680
	movk	w3, #152, lsl #16
	bl	__Z15correlate_fusedPKfPKS0_Pfi
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x24
	smulh	x8, x8, x26
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x23
	csel	x23, x8, x23, lt
	subs	w25, w25, #1
	b.ne	LBB3_27
; %bb.28:
	mov	x24, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w27, #5                         ; =0x5
LBB3_29:                                ; =>This Inner Loop Header: Depth=1
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x25, x0
	mov	x0, x19
	mov	x1, x20
	mov	x2, x22
	mov	w3, #38528                      ; =0x9680
	movk	w3, #152, lsl #16
	bl	__Z17correlate_fissionPKfPKS0_Pfi
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x25
	smulh	x8, x8, x26
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x24
	csel	x24, x8, x24, lt
	subs	w27, w27, #1
	b.ne	LBB3_29
; %bb.30:
Ltmp42:
Lloh44:
	adrp	x0, l_.str.11@PAGE
Lloh45:
	add	x0, x0, l_.str.11@PAGEOFF
Lloh46:
	adrp	x1, l_.str.12@PAGE
Lloh47:
	add	x1, x1, l_.str.12@PAGEOFF
Lloh48:
	adrp	x3, l_.str.13@PAGE
Lloh49:
	add	x3, x3, l_.str.13@PAGEOFF
	mov	x2, x23
	mov	x4, x24
	bl	__Z6reportPKcS0_xS0_x
Ltmp43:
; %bb.31:
Ltmp44:
Lloh50:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh51:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh52:
	adrp	x1, l_.str.14@PAGE
Lloh53:
	add	x1, x1, l_.str.14@PAGEOFF
	mov	w2, #101                        ; =0x65
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp45:
; %bb.32:
	mov	x0, x22
	bl	__ZdlPv
	mov	x0, x21
	bl	__ZdlPv
	mov	x0, x20
	bl	__ZdlPv
	ldr	x20, [sp, #24]
	cbz	x20, LBB3_39
; %bb.33:
	ldr	x8, [sp, #32]
	cmp	x8, x20
	b.eq	LBB3_38
; %bb.34:
	mov	x21, x8
	b	LBB3_36
LBB3_35:                                ;   in Loop: Header=BB3_36 Depth=1
	mov	x8, x21
	cmp	x21, x20
	b.eq	LBB3_38
LBB3_36:                                ; =>This Inner Loop Header: Depth=1
	ldr	x0, [x21, #-24]!
	cbz	x0, LBB3_35
; %bb.37:                               ;   in Loop: Header=BB3_36 Depth=1
	stur	x0, [x8, #-16]
	bl	__ZdlPv
	b	LBB3_35
LBB3_38:
	str	x20, [sp, #32]
	ldr	x0, [sp, #24]
	bl	__ZdlPv
LBB3_39:
	mov	x0, x19
	bl	__ZdlPv
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
LBB3_40:
Ltmp27:
	b	LBB3_51
LBB3_41:
Ltmp24:
	mov	x23, x0
	b	LBB3_52
LBB3_42:
Ltmp21:
	mov	x23, x0
	b	LBB3_53
LBB3_43:
Ltmp18:
	mov	x23, x0
	b	LBB3_54
LBB3_44:
Ltmp15:
	b	LBB3_46
LBB3_45:
Ltmp12:
LBB3_46:
	mov	x23, x0
	ldr	x0, [sp]
	cbz	x0, LBB3_55
; %bb.47:
	str	x0, [sp, #8]
	bl	__ZdlPv
	b	LBB3_55
LBB3_48:
Ltmp46:
	b	LBB3_51
LBB3_49:
Ltmp41:
	b	LBB3_51
LBB3_50:
Ltmp34:
LBB3_51:
	mov	x23, x0
	mov	x0, x22
	bl	__ZdlPv
LBB3_52:
	mov	x0, x21
	bl	__ZdlPv
LBB3_53:
	mov	x0, x20
	bl	__ZdlPv
LBB3_54:
	add	x0, sp, #24
	bl	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev
LBB3_55:
	mov	x0, x19
	bl	__ZdlPv
	mov	x0, x23
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh26, Lloh27
	.loh AdrpLdrGot	Lloh24, Lloh25
	.loh AdrpAdd	Lloh32, Lloh33
	.loh AdrpAdd	Lloh30, Lloh31
	.loh AdrpLdrGot	Lloh28, Lloh29
	.loh AdrpAdd	Lloh36, Lloh37
	.loh AdrpLdrGot	Lloh34, Lloh35
	.loh AdrpAdd	Lloh40, Lloh41
	.loh AdrpAdd	Lloh38, Lloh39
	.loh AdrpAdd	Lloh42, Lloh43
	.loh AdrpAdd	Lloh48, Lloh49
	.loh AdrpAdd	Lloh46, Lloh47
	.loh AdrpAdd	Lloh44, Lloh45
	.loh AdrpAdd	Lloh52, Lloh53
	.loh AdrpLdrGot	Lloh50, Lloh51
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table3:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Lfunc_begin1-Lfunc_begin1      ; >> Call Site 1 <<
	.uleb128 Ltmp10-Lfunc_begin1            ;   Call between Lfunc_begin1 and Ltmp10
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp10-Lfunc_begin1            ; >> Call Site 2 <<
	.uleb128 Ltmp11-Ltmp10                  ;   Call between Ltmp10 and Ltmp11
	.uleb128 Ltmp12-Lfunc_begin1            ;     jumps to Ltmp12
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp11-Lfunc_begin1            ; >> Call Site 3 <<
	.uleb128 Ltmp13-Ltmp11                  ;   Call between Ltmp11 and Ltmp13
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp13-Lfunc_begin1            ; >> Call Site 4 <<
	.uleb128 Ltmp14-Ltmp13                  ;   Call between Ltmp13 and Ltmp14
	.uleb128 Ltmp15-Lfunc_begin1            ;     jumps to Ltmp15
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp14-Lfunc_begin1            ; >> Call Site 5 <<
	.uleb128 Ltmp16-Ltmp14                  ;   Call between Ltmp14 and Ltmp16
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp16-Lfunc_begin1            ; >> Call Site 6 <<
	.uleb128 Ltmp17-Ltmp16                  ;   Call between Ltmp16 and Ltmp17
	.uleb128 Ltmp18-Lfunc_begin1            ;     jumps to Ltmp18
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp19-Lfunc_begin1            ; >> Call Site 7 <<
	.uleb128 Ltmp20-Ltmp19                  ;   Call between Ltmp19 and Ltmp20
	.uleb128 Ltmp21-Lfunc_begin1            ;     jumps to Ltmp21
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp22-Lfunc_begin1            ; >> Call Site 8 <<
	.uleb128 Ltmp23-Ltmp22                  ;   Call between Ltmp22 and Ltmp23
	.uleb128 Ltmp24-Lfunc_begin1            ;     jumps to Ltmp24
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp25-Lfunc_begin1            ; >> Call Site 9 <<
	.uleb128 Ltmp26-Ltmp25                  ;   Call between Ltmp25 and Ltmp26
	.uleb128 Ltmp27-Lfunc_begin1            ;     jumps to Ltmp27
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp28-Lfunc_begin1            ; >> Call Site 10 <<
	.uleb128 Ltmp33-Ltmp28                  ;   Call between Ltmp28 and Ltmp33
	.uleb128 Ltmp34-Lfunc_begin1            ;     jumps to Ltmp34
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp35-Lfunc_begin1            ; >> Call Site 11 <<
	.uleb128 Ltmp40-Ltmp35                  ;   Call between Ltmp35 and Ltmp40
	.uleb128 Ltmp41-Lfunc_begin1            ;     jumps to Ltmp41
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp42-Lfunc_begin1            ; >> Call Site 12 <<
	.uleb128 Ltmp45-Ltmp42                  ;   Call between Ltmp42 and Ltmp45
	.uleb128 Ltmp46-Lfunc_begin1            ;     jumps to Ltmp46
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp45-Lfunc_begin1            ; >> Call Site 13 <<
	.uleb128 Lfunc_end1-Ltmp45              ;   Call between Ltmp45 and Lfunc_end1
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end1:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev ; -- Begin function _ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev
	.globl	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev
	.weak_def_can_be_hidden	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev
	.p2align	2
__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev: ; @_ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEED1B8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x22, x21, [sp, #-48]!           ; 16-byte Folded Spill
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	mov	x19, x0
	ldr	x20, [x0]
	cbz	x20, LBB4_7
; %bb.1:
	ldr	x8, [x19, #8]
	cmp	x8, x20
	b.eq	LBB4_6
; %bb.2:
	mov	x21, x8
	b	LBB4_4
LBB4_3:                                 ;   in Loop: Header=BB4_4 Depth=1
	mov	x8, x21
	cmp	x21, x20
	b.eq	LBB4_6
LBB4_4:                                 ; =>This Inner Loop Header: Depth=1
	ldr	x0, [x21, #-24]!
	cbz	x0, LBB4_3
; %bb.5:                                ;   in Loop: Header=BB4_4 Depth=1
	stur	x0, [x8, #-16]
	bl	__ZdlPv
	b	LBB4_3
LBB4_6:
	str	x20, [x19, #8]
	ldr	x0, [x19]
	bl	__ZdlPv
LBB4_7:
	mov	x0, x19
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp], #48             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	___clang_call_terminate ; -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	2
___clang_call_terminate:                ; @__clang_call_terminate
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	___cxa_begin_catch
	bl	__ZSt9terminatev
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m ; -- Begin function _ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.globl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.weak_def_can_be_hidden	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.p2align	2
__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m: ; @_ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
Ltmp47:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp48:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB6_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w8, [x4, #144]
	cmn	w8, #1
	b.ne	LBB6_7
; %bb.3:
Ltmp50:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp51:
; %bb.4:
Ltmp52:
Lloh54:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh55:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp53:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp54:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp55:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB6_7:
	ldrsb	w5, [x4, #144]
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp57:
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp58:
; %bb.8:
	cbnz	x0, LBB6_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp60:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp61:
LBB6_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB6_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB6_12:
Ltmp62:
	b	LBB6_15
LBB6_13:
Ltmp56:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB6_16
LBB6_14:
Ltmp59:
LBB6_15:
	mov	x20, x0
LBB6_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB6_18
LBB6_17:
Ltmp49:
	mov	x20, x0
LBB6_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp63:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp64:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB6_11
LBB6_20:
Ltmp65:
	mov	x19, x0
Ltmp66:
	bl	___cxa_end_catch
Ltmp67:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB6_22:
Ltmp68:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh54, Lloh55
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table6:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Ltmp47-Lfunc_begin2            ; >> Call Site 1 <<
	.uleb128 Ltmp48-Ltmp47                  ;   Call between Ltmp47 and Ltmp48
	.uleb128 Ltmp49-Lfunc_begin2            ;     jumps to Ltmp49
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp50-Lfunc_begin2            ; >> Call Site 2 <<
	.uleb128 Ltmp51-Ltmp50                  ;   Call between Ltmp50 and Ltmp51
	.uleb128 Ltmp59-Lfunc_begin2            ;     jumps to Ltmp59
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp52-Lfunc_begin2            ; >> Call Site 3 <<
	.uleb128 Ltmp55-Ltmp52                  ;   Call between Ltmp52 and Ltmp55
	.uleb128 Ltmp56-Lfunc_begin2            ;     jumps to Ltmp56
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp57-Lfunc_begin2            ; >> Call Site 4 <<
	.uleb128 Ltmp58-Ltmp57                  ;   Call between Ltmp57 and Ltmp58
	.uleb128 Ltmp59-Lfunc_begin2            ;     jumps to Ltmp59
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp60-Lfunc_begin2            ; >> Call Site 5 <<
	.uleb128 Ltmp61-Ltmp60                  ;   Call between Ltmp60 and Ltmp61
	.uleb128 Ltmp62-Lfunc_begin2            ;     jumps to Ltmp62
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp61-Lfunc_begin2            ; >> Call Site 6 <<
	.uleb128 Ltmp63-Ltmp61                  ;   Call between Ltmp61 and Ltmp63
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp63-Lfunc_begin2            ; >> Call Site 7 <<
	.uleb128 Ltmp64-Ltmp63                  ;   Call between Ltmp63 and Ltmp64
	.uleb128 Ltmp65-Lfunc_begin2            ;     jumps to Ltmp65
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp64-Lfunc_begin2            ; >> Call Site 8 <<
	.uleb128 Ltmp66-Ltmp64                  ;   Call between Ltmp64 and Ltmp66
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp66-Lfunc_begin2            ; >> Call Site 9 <<
	.uleb128 Ltmp67-Ltmp66                  ;   Call between Ltmp66 and Ltmp67
	.uleb128 Ltmp68-Lfunc_begin2            ;     jumps to Ltmp68
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp67-Lfunc_begin2            ; >> Call Site 10 <<
	.uleb128 Lfunc_end2-Ltmp67              ;   Call between Ltmp67 and Lfunc_end2
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end2:
	.byte	1                               ; >> Action Record 1 <<
                                        ;   Catch TypeInfo 1
	.byte	0                               ;   No further actions
	.p2align	2, 0x0
                                        ; >> Catch TypeInfos <<
	.long	0                               ; TypeInfo 1
Lttbase0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_ ; -- Begin function _ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.globl	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.weak_def_can_be_hidden	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.p2align	2
__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_: ; @_ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Lfunc_begin3:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception3
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x19, x0
	cbz	x0, LBB7_16
; %bb.1:
	mov	x24, x5
	mov	x20, x4
	mov	x22, x3
	mov	x21, x2
	ldr	x8, [x4, #24]
	sub	x9, x3, x1
	subs	x8, x8, x9
	csel	x23, x8, xzr, gt
	sub	x25, x2, x1
	cmp	x25, #1
	b.lt	LBB7_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB7_15
LBB7_3:
	cmp	x23, #1
	b.lt	LBB7_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB7_17
; %bb.5:
	cmp	x23, #22
	b.hi	LBB7_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB7_8
LBB7_7:
	orr	x8, x23, #0x7
	cmp	x8, #23
	mov	w9, #25                         ; =0x19
	csinc	x26, x9, x8, eq
	mov	x0, x26
	bl	__Znwm
	mov	x25, x0
	orr	x8, x26, #0x8000000000000000
	stp	x23, x8, [sp, #16]
	str	x0, [sp, #8]
LBB7_8:
	mov	x0, x25
	mov	x1, x24
	mov	x2, x23
	bl	_memset
	strb	wzr, [x25, x23]
	ldrsb	w8, [sp, #31]
	ldr	x9, [sp, #8]
	cmp	w8, #0
	add	x8, sp, #8
	csel	x1, x9, x8, lt
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
Ltmp69:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp70:
; %bb.9:
	cmp	x0, x23
	csel	x19, x19, xzr, eq
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB7_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB7_15
	b	LBB7_12
LBB7_11:
	ldr	x8, [sp, #8]
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPv
	mov	x0, x24
	cmp	x0, x23
	b.ne	LBB7_15
LBB7_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB7_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB7_15
LBB7_14:
	str	xzr, [x20, #24]
	b	LBB7_16
LBB7_15:
	mov	x19, #0                         ; =0x0
LBB7_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB7_17:
	add	x0, sp, #8
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
LBB7_18:
Ltmp71:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB7_20
; %bb.19:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB7_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table7:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Lfunc_begin3-Lfunc_begin3      ; >> Call Site 1 <<
	.uleb128 Ltmp69-Lfunc_begin3            ;   Call between Lfunc_begin3 and Ltmp69
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp69-Lfunc_begin3            ; >> Call Site 2 <<
	.uleb128 Ltmp70-Ltmp69                  ;   Call between Ltmp69 and Ltmp70
	.uleb128 Ltmp71-Lfunc_begin3            ;     jumps to Ltmp71
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp70-Lfunc_begin3            ; >> Call Site 3 <<
	.uleb128 Lfunc_end3-Ltmp70              ;   Call between Ltmp70 and Lfunc_end3
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end3:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev ; -- Begin function _ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
	.globl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
	.weak_def_can_be_hidden	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
	.p2align	2
__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev: ; @_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh56:
	adrp	x0, l_.str.15@PAGE
Lloh57:
	add	x0, x0, l_.str.15@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh56, Lloh57
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__120__throw_length_errorB8ne190102EPKc ; -- Begin function _ZNSt3__120__throw_length_errorB8ne190102EPKc
	.globl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.weak_def_can_be_hidden	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.p2align	2
__ZNSt3__120__throw_length_errorB8ne190102EPKc: ; @_ZNSt3__120__throw_length_errorB8ne190102EPKc
Lfunc_begin4:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception4
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	w0, #16                         ; =0x10
	bl	___cxa_allocate_exception
	mov	x19, x0
Ltmp72:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne190102EPKc
Ltmp73:
; %bb.1:
Lloh58:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh59:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh60:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh61:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB9_2:
Ltmp74:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh60, Lloh61
	.loh AdrpLdrGot	Lloh58, Lloh59
Lfunc_end4:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table9:
Lexception4:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end4-Lcst_begin4
Lcst_begin4:
	.uleb128 Lfunc_begin4-Lfunc_begin4      ; >> Call Site 1 <<
	.uleb128 Ltmp72-Lfunc_begin4            ;   Call between Lfunc_begin4 and Ltmp72
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp72-Lfunc_begin4            ; >> Call Site 2 <<
	.uleb128 Ltmp73-Ltmp72                  ;   Call between Ltmp72 and Ltmp73
	.uleb128 Ltmp74-Lfunc_begin4            ;     jumps to Ltmp74
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp73-Lfunc_begin4            ; >> Call Site 3 <<
	.uleb128 Lfunc_end4-Ltmp73              ;   Call between Ltmp73 and Lfunc_end4
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end4:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt12length_errorC1B8ne190102EPKc ; -- Begin function _ZNSt12length_errorC1B8ne190102EPKc
	.globl	__ZNSt12length_errorC1B8ne190102EPKc
	.weak_def_can_be_hidden	__ZNSt12length_errorC1B8ne190102EPKc
	.p2align	2
__ZNSt12length_errorC1B8ne190102EPKc:   ; @_ZNSt12length_errorC1B8ne190102EPKc
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	__ZNSt11logic_errorC2EPKc
Lloh62:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh63:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh62, Lloh63
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev ; -- Begin function _ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev
	.globl	__ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev
	.weak_def_can_be_hidden	__ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev
	.p2align	2
__ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev: ; @_ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh64:
	adrp	x0, l_.str.16@PAGE
Lloh65:
	add	x0, x0, l_.str.16@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh64, Lloh65
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_ ; -- Begin function _ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_
	.globl	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_
	.weak_def_can_be_hidden	__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_
	.p2align	2
__ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_: ; @_ZNSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEEC2B8ne190102EmRKS3_
Lfunc_begin5:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception5
; %bb.0:
	sub	sp, sp, #96
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x19, x0
	stp	xzr, xzr, [x0]
	str	xzr, [x0, #16]
	str	x0, [sp]
	strb	wzr, [sp, #8]
	cbz	x1, LBB12_10
; %bb.1:
	mov	x8, #-6148914691236517206       ; =0xaaaaaaaaaaaaaaaa
	movk	x8, #43691
	movk	x8, #2730, lsl #48
	cmp	x1, x8
	b.hs	LBB12_12
; %bb.2:
	mov	x23, x2
	add	x8, x1, x1, lsl #1
	lsl	x21, x8, #3
Ltmp75:
	mov	x0, x21
	bl	__Znwm
Ltmp76:
; %bb.3:
	mov	x20, x0
	stp	x0, x0, [x19]
	add	x8, x0, x21
	str	x8, [x19, #16]
	ldp	x22, x24, [x23]
	sub	x23, x24, x22
	b	LBB12_5
LBB12_4:                                ;   in Loop: Header=BB12_5 Depth=1
	add	x20, x20, #24
	subs	x21, x21, #24
	b.eq	LBB12_9
LBB12_5:                                ; =>This Inner Loop Header: Depth=1
	stp	xzr, xzr, [x20]
	str	xzr, [x20, #16]
	cmp	x24, x22
	b.eq	LBB12_4
; %bb.6:                                ;   in Loop: Header=BB12_5 Depth=1
	tbnz	x23, #63, LBB12_11
; %bb.7:                                ;   in Loop: Header=BB12_5 Depth=1
Ltmp77:
	mov	x0, x23
	bl	__Znwm
Ltmp78:
; %bb.8:                                ;   in Loop: Header=BB12_5 Depth=1
	stp	x0, x0, [x20]
	add	x25, x0, x23
	str	x25, [x20, #16]
	mov	x1, x22
	mov	x2, x23
	bl	_memcpy
	str	x25, [x20, #8]
	b	LBB12_4
LBB12_9:
	str	x20, [x19, #8]
LBB12_10:
	mov	x0, x19
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #96
	ret
LBB12_11:
Ltmp80:
	mov	x0, x20
	bl	__ZNKSt3__16vectorIfNS_9allocatorIfEEE20__throw_length_errorB8ne190102Ev
Ltmp81:
	b	LBB12_13
LBB12_12:
Ltmp83:
	mov	x0, x19
	bl	__ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev
Ltmp84:
LBB12_13:
	brk	#0x1
LBB12_14:
Ltmp85:
	mov	x21, x0
	b	LBB12_20
LBB12_15:
Ltmp79:
	b	LBB12_17
LBB12_16:
Ltmp82:
LBB12_17:
	mov	x21, x0
	ldr	x0, [x20]
	cbz	x0, LBB12_19
; %bb.18:
	str	x0, [x20, #8]
	bl	__ZdlPv
LBB12_19:
	str	x20, [x19, #8]
LBB12_20:
	mov	x0, sp
	bl	__ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev
	mov	x0, x21
	bl	__Unwind_Resume
Lfunc_end5:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table12:
Lexception5:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end5-Lcst_begin5
Lcst_begin5:
	.uleb128 Ltmp75-Lfunc_begin5            ; >> Call Site 1 <<
	.uleb128 Ltmp76-Ltmp75                  ;   Call between Ltmp75 and Ltmp76
	.uleb128 Ltmp85-Lfunc_begin5            ;     jumps to Ltmp85
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp77-Lfunc_begin5            ; >> Call Site 2 <<
	.uleb128 Ltmp78-Ltmp77                  ;   Call between Ltmp77 and Ltmp78
	.uleb128 Ltmp79-Lfunc_begin5            ;     jumps to Ltmp79
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp78-Lfunc_begin5            ; >> Call Site 3 <<
	.uleb128 Ltmp80-Ltmp78                  ;   Call between Ltmp78 and Ltmp80
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp80-Lfunc_begin5            ; >> Call Site 4 <<
	.uleb128 Ltmp81-Ltmp80                  ;   Call between Ltmp80 and Ltmp81
	.uleb128 Ltmp82-Lfunc_begin5            ;     jumps to Ltmp82
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp83-Lfunc_begin5            ; >> Call Site 5 <<
	.uleb128 Ltmp84-Ltmp83                  ;   Call between Ltmp83 and Ltmp84
	.uleb128 Ltmp85-Lfunc_begin5            ;     jumps to Ltmp85
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp84-Lfunc_begin5            ; >> Call Site 6 <<
	.uleb128 Lfunc_end5-Ltmp84              ;   Call between Ltmp84 and Lfunc_end5
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end5:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev ; -- Begin function _ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev
	.globl	__ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev
	.weak_def_can_be_hidden	__ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev
	.p2align	2
__ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev: ; @_ZNSt3__128__exception_guard_exceptionsINS_6vectorINS1_IfNS_9allocatorIfEEEENS2_IS4_EEE16__destroy_vectorEED1B8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x22, x21, [sp, #-48]!           ; 16-byte Folded Spill
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	mov	x19, x0
	ldrb	w8, [x0, #8]
	tbnz	w8, #0, LBB13_8
; %bb.1:
	ldr	x20, [x19]
	ldr	x21, [x20]
	cbz	x21, LBB13_8
; %bb.2:
	ldr	x8, [x20, #8]
	cmp	x8, x21
	b.eq	LBB13_7
; %bb.3:
	mov	x22, x8
	b	LBB13_5
LBB13_4:                                ;   in Loop: Header=BB13_5 Depth=1
	mov	x8, x22
	cmp	x22, x21
	b.eq	LBB13_7
LBB13_5:                                ; =>This Inner Loop Header: Depth=1
	ldr	x0, [x22, #-24]!
	cbz	x0, LBB13_4
; %bb.6:                                ;   in Loop: Header=BB13_5 Depth=1
	stur	x0, [x8, #-16]
	bl	__ZdlPv
	b	LBB13_4
LBB13_7:
	str	x21, [x20, #8]
	ldr	x8, [x19]
	ldr	x0, [x8]
	bl	__ZdlPv
LBB13_8:
	mov	x0, x19
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp], #48             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev ; -- Begin function _ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev
	.globl	__ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev
	.weak_def_can_be_hidden	__ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev
	.p2align	2
__ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev: ; @_ZNKSt3__16vectorINS0_IfNS_9allocatorIfEEEENS1_IS3_EEE20__throw_length_errorB8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh66:
	adrp	x0, l_.str.16@PAGE
Lloh67:
	add	x0, x0, l_.str.16@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh66, Lloh67
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"\n"

l_.str.1:                               ; @.str.1
	.asciz	"  FUSED    "

l_.str.2:                               ; @.str.2
	.asciz	" ms\n"

l_.str.3:                               ; @.str.3
	.asciz	"  FISSION  "

l_.str.4:                               ; @.str.4
	.asciz	"  Speedup: "

l_.str.5:                               ; @.str.5
	.asciz	"x\n"

l_.str.6:                               ; @.str.6
	.asciz	"Correctness check:\n"

l_.str.7:                               ; @.str.7
	.asciz	"  FAIL k="

l_.str.8:                               ; @.str.8
	.asciz	"  "

l_.str.9:                               ; @.str.9
	.asciz	"PASS"

l_.str.10:                              ; @.str.10
	.asciz	"FAIL"

l_.str.11:                              ; @.str.11
	.asciz	"16 correlations \342\200\224 register pressure vs loop fission"

l_.str.12:                              ; @.str.12
	.asciz	"1 loop  (16 accumulators, spills to stack)"

l_.str.13:                              ; @.str.13
	.asciz	"2 loops ( 8 accumulators each, no spills)"

l_.str.14:                              ; @.str.14
	.asciz	"\nTo inspect spills:\n  g++ -O1 -S -o fission.s loop_fission.cpp\n  grep -A1 'rsp' fission.s | head -40\n"

l_.str.15:                              ; @.str.15
	.asciz	"basic_string"

l_.str.16:                              ; @.str.16
	.asciz	"vector"

.subsections_via_symbols
