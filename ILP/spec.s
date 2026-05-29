	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 5
	.globl	__Z15sum_conditionalPKii        ; -- Begin function _Z15sum_conditionalPKii
	.p2align	2
__Z15sum_conditionalPKii:               ; @_Z15sum_conditionalPKii
	.cfi_startproc
; %bb.0:
	cmp	w1, #1
	b.lt	LBB0_4
; %bb.1:
	mov	x8, #0                          ; =0x0
	mov	w9, w1
LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x0], #4
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x8, x8, x10
	subs	x9, x9, #1
	b.ne	LBB0_2
; %bb.3:
	mov	x0, x8
	ret
LBB0_4:
	mov	x8, #0                          ; =0x0
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z14sum_branchlessPKii         ; -- Begin function _Z14sum_branchlessPKii
	.p2align	2
__Z14sum_branchlessPKii:                ; @_Z14sum_branchlessPKii
	.cfi_startproc
; %bb.0:
	cmp	w1, #1
	b.lt	LBB1_4
; %bb.1:
	mov	x8, #0                          ; =0x0
	mov	w9, w1
LBB1_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x0], #4
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x8, x8, w10, sxtw
	subs	x9, x9, #1
	b.ne	LBB1_2
; %bb.3:
	mov	x0, x8
	ret
LBB1_4:
	mov	x8, #0                          ; =0x0
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	stp	d9, d8, [sp, #-112]!            ; 16-byte Folded Spill
	stp	x28, x27, [sp, #16]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	sub	sp, sp, #2560
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
	mov	w20, #134217728                 ; =0x8000000
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
	mov	x19, x0
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
	stp	xzr, xzr, [x29, #-120]
	stur	xzr, [x29, #-104]
Ltmp0:
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
Ltmp1:
; %bb.1:
	stp	x0, x0, [x29, #-120]
	mov	w8, #134217728                  ; =0x8000000
	add	x21, x0, x8
	stur	x21, [x29, #-104]
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
	stur	x21, [x29, #-112]
	mov	w8, #42                         ; =0x2a
	str	w8, [sp, #32]
	mov	w9, #1                          ; =0x1
	mov	w10, #35173                     ; =0x8965
	movk	w10, #27655, lsl #16
	add	x11, sp, #32
LBB2_2:                                 ; =>This Inner Loop Header: Depth=1
	eor	w8, w8, w8, lsr #30
	madd	w8, w8, w10, w9
	str	w8, [x11, x9, lsl #2]
	add	x9, x9, #1
	cmp	x9, #624
	b.ne	LBB2_2
; %bb.3:
	mov	x21, #0                         ; =0x0
	str	xzr, [sp, #2528]
	mov	x8, #1095216660480              ; =0xff00000000
	str	x8, [sp, #24]
	mov	w22, #134217728                 ; =0x8000000
LBB2_4:                                 ; =>This Inner Loop Header: Depth=1
Ltmp3:
	add	x0, sp, #24
	add	x1, sp, #32
	add	x2, sp, #24
	bl	__ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE
Ltmp4:
; %bb.5:                                ;   in Loop: Header=BB2_4 Depth=1
	str	w0, [x19, x21]
	add	x21, x21, #4
	cmp	x21, x22
	b.ne	LBB2_4
; %bb.6:
Ltmp6:
	sub	x0, x29, #120
	add	x2, x19, x20
	mov	x1, x19
	mov	w3, #33554432                   ; =0x2000000
	bl	__ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l
Ltmp7:
; %bb.7:
	ldp	x0, x1, [x29, #-120]
Ltmp8:
	mov	x2, sp
	bl	__ZNSt3__16__sortIRNS_6__lessIiiEEPiEEvT0_S5_T_
Ltmp9:
; %bb.8:
	mov	x8, #0                          ; =0x0
	mov	x20, #0                         ; =0x0
	mov	w9, #134217728                  ; =0x8000000
LBB2_9:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x19, x8]
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x20, x20, x10
	add	x8, x8, #4
	cmp	x8, x9
	b.ne	LBB2_9
; %bb.10:
	mov	x21, #0                         ; =0x0
	ldur	x8, [x29, #-120]
	mov	w9, #33554432                   ; =0x2000000
LBB2_11:                                ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x8], #4
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x21, x21, x10
	subs	x9, x9, #1
	b.ne	LBB2_11
; %bb.12:
	mov	x8, #0                          ; =0x0
	mov	x23, #0                         ; =0x0
	mov	w9, #134217728                  ; =0x8000000
LBB2_13:                                ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x19, x8]
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x23, x23, w10, sxtw
	add	x8, x8, #4
	cmp	x8, x9
	b.ne	LBB2_13
; %bb.14:
Ltmp11:
Lloh0:
	adrp	x22, __ZNSt3__14coutE@GOTPAGE
Lloh1:
	ldr	x22, [x22, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh2:
	adrp	x1, l_.str@PAGE
Lloh3:
	add	x1, x1, l_.str@PAGEOFF
	mov	x0, x22
	mov	w2, #22                         ; =0x16
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp12:
; %bb.15:
Ltmp13:
	mov	x1, x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp14:
; %bb.16:
Ltmp15:
Lloh4:
	adrp	x1, l_.str.1@PAGE
Lloh5:
	add	x1, x1, l_.str.1@PAGEOFF
	mov	w2, #9                          ; =0x9
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp16:
; %bb.17:
Ltmp17:
	mov	x1, x21
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp18:
; %bb.18:
Ltmp19:
Lloh6:
	adrp	x1, l_.str.2@PAGE
Lloh7:
	add	x1, x1, l_.str.2@PAGEOFF
	mov	w2, #13                         ; =0xd
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp20:
; %bb.19:
Ltmp21:
	mov	x1, x23
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp22:
; %bb.20:
Ltmp23:
Lloh8:
	adrp	x1, l_.str.3@PAGE
Lloh9:
	add	x1, x1, l_.str.3@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp24:
; %bb.21:
	cmp	x20, x23
	ccmp	x20, x21, #0, eq
Lloh10:
	adrp	x8, l_.str.5@PAGE
Lloh11:
	add	x8, x8, l_.str.5@PAGEOFF
Lloh12:
	adrp	x9, l_.str.4@PAGE
Lloh13:
	add	x9, x9, l_.str.4@PAGEOFF
	csel	x1, x9, x8, eq
Ltmp25:
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp26:
; %bb.22:
Ltmp27:
Lloh14:
	adrp	x1, l_.str.6@PAGE
Lloh15:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp28:
; %bb.23:
	ldr	x8, [x22]
	ldur	x9, [x8, #-24]
	add	x9, x22, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xfffffeff
	orr	w10, w10, #0x4
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x22, x8
	str	xzr, [x8, #16]
Ltmp30:
Lloh16:
	adrp	x1, l_.str.7@PAGE
Lloh17:
	add	x1, x1, l_.str.7@PAGEOFF
	mov	x0, x22
	mov	w2, #8                          ; =0x8
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp31:
; %bb.24:
Ltmp32:
	mov	w1, #33                         ; =0x21
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp33:
; %bb.25:
Ltmp34:
Lloh18:
	adrp	x1, l_.str.8@PAGE
Lloh19:
	add	x1, x1, l_.str.8@PAGEOFF
	mov	w2, #29                         ; =0x1d
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp35:
; %bb.26:
Ltmp36:
	mov	w1, #128                        ; =0x80
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp37:
; %bb.27:
Ltmp38:
Lloh20:
	adrp	x1, l_.str.9@PAGE
Lloh21:
	add	x1, x1, l_.str.9@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp39:
; %bb.28:
Ltmp40:
	mov	w1, #50                         ; =0x32
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp41:
; %bb.29:
Ltmp42:
Lloh22:
	adrp	x1, l_.str.10@PAGE
Lloh23:
	add	x1, x1, l_.str.10@PAGEOFF
	mov	w2, #8                          ; =0x8
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp43:
; %bb.30:
Ltmp44:
Lloh24:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh25:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh26:
	adrp	x1, l_.str.11@PAGE
Lloh27:
	add	x1, x1, l_.str.11@PAGEOFF
	mov	w2, #45                         ; =0x2d
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp45:
; %bb.31:
Ltmp46:
	mov	x8, #26865                      ; =0x68f1
	movk	x8, #35043, lsl #16
	movk	x8, #63669, lsl #32
	movk	x8, #16468, lsl #48
	fmov	d0, x8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp47:
; %bb.32:
Ltmp48:
Lloh28:
	adrp	x1, l_.str.12@PAGE
Lloh29:
	add	x1, x1, l_.str.12@PAGEOFF
	mov	w2, #5                          ; =0x5
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp49:
; %bb.33:
Ltmp50:
Lloh30:
	adrp	x1, l_.str.13@PAGE
Lloh31:
	add	x1, x1, l_.str.13@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp51:
; %bb.34:
Ltmp52:
	mov	w1, #50                         ; =0x32
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp53:
; %bb.35:
Ltmp54:
Lloh32:
	adrp	x1, l_.str.14@PAGE
Lloh33:
	add	x1, x1, l_.str.14@PAGEOFF
	mov	w2, #16                         ; =0x10
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp55:
; %bb.36:
Ltmp56:
	mov	w1, #15                         ; =0xf
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp57:
; %bb.37:
Ltmp58:
Lloh34:
	adrp	x1, l_.str.15@PAGE
Lloh35:
	add	x1, x1, l_.str.15@PAGEOFF
	mov	w2, #34                         ; =0x22
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp59:
; %bb.38:
	mov	w21, #0                         ; =0x0
	mov	x22, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w23, #134217728                 ; =0x8000000
	mov	x25, sp
	mov	x24, #13531                     ; =0x34db
	movk	x24, #55222, lsl #16
	movk	x24, #56962, lsl #32
	movk	x24, #17179, lsl #48
LBB2_39:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_40 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	mov	x9, #0                          ; =0x0
	mov	x8, #0                          ; =0x0
LBB2_40:                                ;   Parent Loop BB2_39 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	w10, [x19, x9]
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x8, x8, x10
	add	x9, x9, #4
	cmp	x9, x23
	b.ne	LBB2_40
; %bb.41:                               ;   in Loop: Header=BB2_39 Depth=1
	str	x8, [sp]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	smulh	x8, x8, x24
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x22
	csel	x22, x8, x22, lt
	add	w21, w21, #1
	cmp	w21, #5
	b.ne	LBB2_39
; %bb.42:
	mov	w23, #0                         ; =0x0
	mov	x21, #9223372036854775807       ; =0x7fffffffffffffff
	mov	x25, sp
LBB2_43:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_44 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	mov	x8, #0                          ; =0x0
	ldur	x9, [x29, #-120]
	mov	w10, #33554432                  ; =0x2000000
LBB2_44:                                ;   Parent Loop BB2_43 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	w11, [x9], #4
	cmp	w11, #127
	csel	w11, w11, wzr, gt
	add	x8, x8, x11
	subs	x10, x10, #1
	b.ne	LBB2_44
; %bb.45:                               ;   in Loop: Header=BB2_43 Depth=1
	str	x8, [sp]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	smulh	x8, x8, x24
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x21
	csel	x21, x8, x21, lt
	add	w23, w23, #1
	cmp	w23, #5
	b.ne	LBB2_43
; %bb.46:
	mov	w25, #0                         ; =0x0
	mov	x20, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w26, #134217728                 ; =0x8000000
	mov	x27, sp
LBB2_47:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_48 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x23, x0
	mov	x9, #0                          ; =0x0
	mov	x8, #0                          ; =0x0
LBB2_48:                                ;   Parent Loop BB2_47 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	w10, [x19, x9]
	cmp	w10, #127
	csel	w10, w10, wzr, gt
	add	x8, x8, w10, sxtw
	add	x9, x9, #4
	cmp	x9, x26
	b.ne	LBB2_48
; %bb.49:                               ;   in Loop: Header=BB2_47 Depth=1
	str	x8, [sp]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x23
	smulh	x8, x8, x24
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x20
	csel	x20, x8, x20, lt
	add	w25, w25, #1
	cmp	w25, #5
	b.ne	LBB2_47
; %bb.50:
Ltmp61:
Lloh36:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh37:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh38:
	adrp	x1, l_.str.6@PAGE
Lloh39:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp62:
; %bb.51:
Ltmp63:
	mov	x23, x0
	mov	w0, #72                         ; =0x48
	bl	__Znwm
Ltmp64:
; %bb.52:
	mov	x24, #72                        ; =0x48
	movk	x24, #32768, lsl #48
	mov	w8, #64                         ; =0x40
	str	x0, [sp]
	stp	x8, x24, [sp, #8]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	strb	wzr, [x0, #64]
	ldrsb	w9, [sp, #23]
	mov	x10, sp
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp66:
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp67:
; %bb.53:
Ltmp68:
Lloh40:
	adrp	x1, l_.str.6@PAGE
Lloh41:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp69:
; %bb.54:
	ldrsb	w8, [sp, #23]
	tbz	w8, #31, LBB2_56
; %bb.55:
	ldr	x0, [sp]
	bl	__ZdlPv
LBB2_56:
Ltmp71:
Lloh42:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh43:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh44:
	adrp	x1, l_.str.16@PAGE
Lloh45:
	add	x1, x1, l_.str.16@PAGEOFF
	mov	w2, #52                         ; =0x34
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp72:
; %bb.57:
Ltmp74:
	mov	w0, #72                         ; =0x48
	bl	__Znwm
Ltmp75:
; %bb.58:
	mov	w8, #64                         ; =0x40
	str	x0, [sp]
	stp	x8, x24, [sp, #8]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	strb	wzr, [x0, #64]
	ldrsb	w9, [sp, #23]
	mov	x10, sp
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp77:
Lloh46:
	adrp	x23, __ZNSt3__14coutE@GOTPAGE
Lloh47:
	ldr	x23, [x23, __ZNSt3__14coutE@GOTPAGEOFF]
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp78:
; %bb.59:
Ltmp79:
Lloh48:
	adrp	x1, l_.str.6@PAGE
Lloh49:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp80:
; %bb.60:
	ldrsb	w8, [sp, #23]
	tbz	w8, #31, LBB2_62
; %bb.61:
	ldr	x0, [sp]
	bl	__ZdlPv
LBB2_62:
	ldr	x8, [x23]
	ldur	x9, [x8, #-24]
	add	x9, x23, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x23, x8
	mov	w9, #36                         ; =0x24
	str	x9, [x8, #24]
Ltmp82:
Lloh50:
	adrp	x1, l_.str.17@PAGE
Lloh51:
	add	x1, x1, l_.str.17@PAGEOFF
	mov	x0, x23
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp83:
; %bb.63:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #8                          ; =0x8
	str	x9, [x8, #24]
Ltmp85:
Lloh52:
	adrp	x1, l_.str.18@PAGE
Lloh53:
	add	x1, x1, l_.str.18@PAGEOFF
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp86:
; %bb.64:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #10                         ; =0xa
	str	x9, [x8, #24]
Ltmp88:
Lloh54:
	adrp	x1, l_.str.19@PAGE
Lloh55:
	add	x1, x1, l_.str.19@PAGEOFF
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp89:
; %bb.65:
Ltmp90:
Lloh56:
	adrp	x1, l_.str.6@PAGE
Lloh57:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp91:
; %bb.66:
Ltmp93:
	mov	w0, #72                         ; =0x48
	bl	__Znwm
Ltmp94:
; %bb.67:
	mov	w8, #64                         ; =0x40
	str	x0, [sp]
	stp	x8, x24, [sp, #8]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	strb	wzr, [x0, #64]
	ldrsb	w9, [sp, #23]
	mov	x10, sp
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp96:
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp97:
; %bb.68:
Ltmp98:
Lloh58:
	adrp	x1, l_.str.6@PAGE
Lloh59:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp99:
; %bb.69:
	ldrsb	w8, [sp, #23]
	tbz	w8, #31, LBB2_71
; %bb.70:
	ldr	x0, [sp]
	bl	__ZdlPv
LBB2_71:
	ldr	x8, [x23]
	ldur	x9, [x8, #-24]
	add	x9, x23, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x23, x8
	mov	w9, #36                         ; =0x24
	str	x9, [x8, #24]
Ltmp101:
Lloh60:
	adrp	x1, l_.str.20@PAGE
Lloh61:
	add	x1, x1, l_.str.20@PAGEOFF
	mov	x0, x23
	mov	w2, #37                         ; =0x25
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp102:
; %bb.72:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #7                          ; =0x7
	str	x9, [x8, #24]
Ltmp103:
	mov	x1, x22
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp104:
; %bb.73:
Ltmp105:
Lloh62:
	adrp	x1, l_.str.23@PAGE
Lloh63:
	add	x1, x1, l_.str.23@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp106:
; %bb.74:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	mov	w10, #9                         ; =0x9
	str	x10, [x9, #24]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	mov	w11, #-261                      ; =0xfffffefb
	and	w10, w10, w11
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #2                          ; =0x2
	scvtf	d8, x22
	str	x9, [x8, #16]
	cmp	x22, #1
	csinc	x8, x22, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp107:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp108:
; %bb.75:
Ltmp109:
Lloh64:
	adrp	x1, l_.str.24@PAGE
Lloh65:
	add	x1, x1, l_.str.24@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp110:
; %bb.76:
	ldr	x8, [x23]
	ldur	x9, [x8, #-24]
	add	x9, x23, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x23, x8
	mov	w9, #36                         ; =0x24
	str	x9, [x8, #24]
Ltmp111:
Lloh66:
	adrp	x1, l_.str.21@PAGE
Lloh67:
	add	x1, x1, l_.str.21@PAGEOFF
	mov	x0, x23
	mov	w2, #35                         ; =0x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp112:
; %bb.77:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #7                          ; =0x7
	str	x9, [x8, #24]
Ltmp113:
	mov	x1, x21
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp114:
; %bb.78:
Ltmp115:
Lloh68:
	adrp	x1, l_.str.23@PAGE
Lloh69:
	add	x1, x1, l_.str.23@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp116:
; %bb.79:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	mov	w10, #9                         ; =0x9
	str	x10, [x9, #24]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	mov	w11, #-261                      ; =0xfffffefb
	and	w10, w10, w11
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	cmp	x21, #1
	csinc	x8, x21, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp117:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp118:
; %bb.80:
Ltmp119:
Lloh70:
	adrp	x1, l_.str.24@PAGE
Lloh71:
	add	x1, x1, l_.str.24@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp120:
; %bb.81:
	ldr	x8, [x23]
	ldur	x9, [x8, #-24]
	add	x9, x23, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x23, x8
	mov	w9, #36                         ; =0x24
	str	x9, [x8, #24]
Ltmp121:
Lloh72:
	adrp	x1, l_.str.22@PAGE
Lloh73:
	add	x1, x1, l_.str.22@PAGEOFF
	mov	x0, x23
	mov	w2, #33                         ; =0x21
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp122:
; %bb.82:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #7                          ; =0x7
	str	x9, [x8, #24]
Ltmp123:
	mov	x1, x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp124:
; %bb.83:
Ltmp125:
Lloh74:
	adrp	x1, l_.str.23@PAGE
Lloh75:
	add	x1, x1, l_.str.23@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp126:
; %bb.84:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	mov	w10, #9                         ; =0x9
	str	x10, [x9, #24]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	mov	w11, #-261                      ; =0xfffffefb
	and	w10, w10, w11
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	cmp	x20, #1
	csinc	x8, x20, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp127:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp128:
; %bb.85:
Ltmp129:
Lloh76:
	adrp	x1, l_.str.24@PAGE
Lloh77:
	add	x1, x1, l_.str.24@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp130:
; %bb.86:
	ldur	x0, [x29, #-120]
	cbz	x0, LBB2_88
; %bb.87:
	stur	x0, [x29, #-112]
	bl	__ZdlPv
LBB2_88:
	mov	x0, x19
	bl	__ZdlPv
	mov	w0, #0                          ; =0x0
	add	sp, sp, #2560
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
LBB2_89:
Ltmp95:
	b	LBB2_107
LBB2_90:
Ltmp87:
	b	LBB2_107
LBB2_91:
Ltmp84:
	b	LBB2_107
LBB2_92:
Ltmp76:
	b	LBB2_107
LBB2_93:
Ltmp65:
	b	LBB2_107
LBB2_94:
Ltmp2:
	b	LBB2_107
LBB2_95:
Ltmp100:
	b	LBB2_99
LBB2_96:
Ltmp92:
	b	LBB2_107
LBB2_97:
Ltmp81:
	b	LBB2_99
LBB2_98:
Ltmp70:
LBB2_99:
	mov	x20, x0
	ldrsb	w8, [sp, #23]
	tbz	w8, #31, LBB2_108
; %bb.100:
	ldr	x0, [sp]
	bl	__ZdlPv
	b	LBB2_108
LBB2_101:
Ltmp73:
	b	LBB2_107
LBB2_102:
Ltmp10:
	b	LBB2_107
LBB2_103:
Ltmp29:
	b	LBB2_107
LBB2_104:
Ltmp131:
	b	LBB2_107
LBB2_105:
Ltmp60:
	b	LBB2_107
LBB2_106:
Ltmp5:
LBB2_107:
	mov	x20, x0
LBB2_108:
	ldur	x0, [x29, #-120]
	cbz	x0, LBB2_110
; %bb.109:
	stur	x0, [x29, #-112]
	bl	__ZdlPv
LBB2_110:
	mov	x0, x19
	bl	__ZdlPv
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpLdrGot	Lloh0, Lloh1
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh6, Lloh7
	.loh AdrpAdd	Lloh8, Lloh9
	.loh AdrpAdd	Lloh12, Lloh13
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpAdd	Lloh14, Lloh15
	.loh AdrpAdd	Lloh16, Lloh17
	.loh AdrpAdd	Lloh18, Lloh19
	.loh AdrpAdd	Lloh20, Lloh21
	.loh AdrpAdd	Lloh22, Lloh23
	.loh AdrpAdd	Lloh26, Lloh27
	.loh AdrpLdrGot	Lloh24, Lloh25
	.loh AdrpAdd	Lloh28, Lloh29
	.loh AdrpAdd	Lloh30, Lloh31
	.loh AdrpAdd	Lloh32, Lloh33
	.loh AdrpAdd	Lloh34, Lloh35
	.loh AdrpAdd	Lloh38, Lloh39
	.loh AdrpLdrGot	Lloh36, Lloh37
	.loh AdrpAdd	Lloh40, Lloh41
	.loh AdrpAdd	Lloh44, Lloh45
	.loh AdrpLdrGot	Lloh42, Lloh43
	.loh AdrpLdrGot	Lloh46, Lloh47
	.loh AdrpAdd	Lloh48, Lloh49
	.loh AdrpAdd	Lloh50, Lloh51
	.loh AdrpAdd	Lloh52, Lloh53
	.loh AdrpAdd	Lloh54, Lloh55
	.loh AdrpAdd	Lloh56, Lloh57
	.loh AdrpAdd	Lloh58, Lloh59
	.loh AdrpAdd	Lloh60, Lloh61
	.loh AdrpAdd	Lloh62, Lloh63
	.loh AdrpAdd	Lloh64, Lloh65
	.loh AdrpAdd	Lloh66, Lloh67
	.loh AdrpAdd	Lloh68, Lloh69
	.loh AdrpAdd	Lloh70, Lloh71
	.loh AdrpAdd	Lloh72, Lloh73
	.loh AdrpAdd	Lloh74, Lloh75
	.loh AdrpAdd	Lloh76, Lloh77
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table2:
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
	.uleb128 Ltmp1-Ltmp0                    ;   Call between Ltmp0 and Ltmp1
	.uleb128 Ltmp2-Lfunc_begin0             ;     jumps to Ltmp2
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp1-Lfunc_begin0             ; >> Call Site 3 <<
	.uleb128 Ltmp3-Ltmp1                    ;   Call between Ltmp1 and Ltmp3
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ; >> Call Site 4 <<
	.uleb128 Ltmp4-Ltmp3                    ;   Call between Ltmp3 and Ltmp4
	.uleb128 Ltmp5-Lfunc_begin0             ;     jumps to Ltmp5
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp6-Lfunc_begin0             ; >> Call Site 5 <<
	.uleb128 Ltmp9-Ltmp6                    ;   Call between Ltmp6 and Ltmp9
	.uleb128 Ltmp10-Lfunc_begin0            ;     jumps to Ltmp10
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp11-Lfunc_begin0            ; >> Call Site 6 <<
	.uleb128 Ltmp28-Ltmp11                  ;   Call between Ltmp11 and Ltmp28
	.uleb128 Ltmp29-Lfunc_begin0            ;     jumps to Ltmp29
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp30-Lfunc_begin0            ; >> Call Site 7 <<
	.uleb128 Ltmp59-Ltmp30                  ;   Call between Ltmp30 and Ltmp59
	.uleb128 Ltmp60-Lfunc_begin0            ;     jumps to Ltmp60
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp61-Lfunc_begin0            ; >> Call Site 8 <<
	.uleb128 Ltmp62-Ltmp61                  ;   Call between Ltmp61 and Ltmp62
	.uleb128 Ltmp73-Lfunc_begin0            ;     jumps to Ltmp73
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp63-Lfunc_begin0            ; >> Call Site 9 <<
	.uleb128 Ltmp64-Ltmp63                  ;   Call between Ltmp63 and Ltmp64
	.uleb128 Ltmp65-Lfunc_begin0            ;     jumps to Ltmp65
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp66-Lfunc_begin0            ; >> Call Site 10 <<
	.uleb128 Ltmp69-Ltmp66                  ;   Call between Ltmp66 and Ltmp69
	.uleb128 Ltmp70-Lfunc_begin0            ;     jumps to Ltmp70
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp71-Lfunc_begin0            ; >> Call Site 11 <<
	.uleb128 Ltmp72-Ltmp71                  ;   Call between Ltmp71 and Ltmp72
	.uleb128 Ltmp73-Lfunc_begin0            ;     jumps to Ltmp73
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp74-Lfunc_begin0            ; >> Call Site 12 <<
	.uleb128 Ltmp75-Ltmp74                  ;   Call between Ltmp74 and Ltmp75
	.uleb128 Ltmp76-Lfunc_begin0            ;     jumps to Ltmp76
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp77-Lfunc_begin0            ; >> Call Site 13 <<
	.uleb128 Ltmp80-Ltmp77                  ;   Call between Ltmp77 and Ltmp80
	.uleb128 Ltmp81-Lfunc_begin0            ;     jumps to Ltmp81
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp82-Lfunc_begin0            ; >> Call Site 14 <<
	.uleb128 Ltmp83-Ltmp82                  ;   Call between Ltmp82 and Ltmp83
	.uleb128 Ltmp84-Lfunc_begin0            ;     jumps to Ltmp84
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp85-Lfunc_begin0            ; >> Call Site 15 <<
	.uleb128 Ltmp86-Ltmp85                  ;   Call between Ltmp85 and Ltmp86
	.uleb128 Ltmp87-Lfunc_begin0            ;     jumps to Ltmp87
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp88-Lfunc_begin0            ; >> Call Site 16 <<
	.uleb128 Ltmp91-Ltmp88                  ;   Call between Ltmp88 and Ltmp91
	.uleb128 Ltmp92-Lfunc_begin0            ;     jumps to Ltmp92
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp93-Lfunc_begin0            ; >> Call Site 17 <<
	.uleb128 Ltmp94-Ltmp93                  ;   Call between Ltmp93 and Ltmp94
	.uleb128 Ltmp95-Lfunc_begin0            ;     jumps to Ltmp95
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp96-Lfunc_begin0            ; >> Call Site 18 <<
	.uleb128 Ltmp99-Ltmp96                  ;   Call between Ltmp96 and Ltmp99
	.uleb128 Ltmp100-Lfunc_begin0           ;     jumps to Ltmp100
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp101-Lfunc_begin0           ; >> Call Site 19 <<
	.uleb128 Ltmp130-Ltmp101                ;   Call between Ltmp101 and Ltmp130
	.uleb128 Ltmp131-Lfunc_begin0           ;     jumps to Ltmp131
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp130-Lfunc_begin0           ; >> Call Site 20 <<
	.uleb128 Lfunc_end0-Ltmp130             ;   Call between Ltmp130 and Lfunc_end0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
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
	.private_extern	__ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev ; -- Begin function _ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev
	.globl	__ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev
	.weak_def_can_be_hidden	__ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev
	.p2align	2
__ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev: ; @_ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh78:
	adrp	x0, l_.str.25@PAGE
Lloh79:
	add	x0, x0, l_.str.25@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh78, Lloh79
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__120__throw_length_errorB8ne190102EPKc ; -- Begin function _ZNSt3__120__throw_length_errorB8ne190102EPKc
	.globl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.weak_def_can_be_hidden	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.p2align	2
__ZNSt3__120__throw_length_errorB8ne190102EPKc: ; @_ZNSt3__120__throw_length_errorB8ne190102EPKc
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
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
Ltmp132:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne190102EPKc
Ltmp133:
; %bb.1:
Lloh80:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh81:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh82:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh83:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB5_2:
Ltmp134:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh82, Lloh83
	.loh AdrpLdrGot	Lloh80, Lloh81
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table5:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Lfunc_begin1-Lfunc_begin1      ; >> Call Site 1 <<
	.uleb128 Ltmp132-Lfunc_begin1           ;   Call between Lfunc_begin1 and Ltmp132
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp132-Lfunc_begin1           ; >> Call Site 2 <<
	.uleb128 Ltmp133-Ltmp132                ;   Call between Ltmp132 and Ltmp133
	.uleb128 Ltmp134-Lfunc_begin1           ;     jumps to Ltmp134
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp133-Lfunc_begin1           ; >> Call Site 3 <<
	.uleb128 Lfunc_end1-Ltmp133             ;   Call between Ltmp133 and Lfunc_end1
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end1:
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
Lloh84:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh85:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh84, Lloh85
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE ; -- Begin function _ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE
	.weak_def_can_be_hidden	__ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE
	.p2align	2
__ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE: ; @_ZNSt3__124uniform_int_distributionIiEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEEiRT_RKNS1_10param_typeE
	.cfi_startproc
; %bb.0:
	ldp	w0, w8, [x2]
	subs	w11, w8, w0
	b.eq	LBB7_5
; %bb.1:
	mov	w8, #-272236544                 ; =0xefc60000
	mov	w9, #22144                      ; =0x5680
	movk	w9, #40236, lsl #16
	mov	w10, #45279                     ; =0xb0df
	movk	w10, #39176, lsl #16
	add	w11, w11, #1
	cbz	w11, LBB7_6
; %bb.2:
	clz	w12, w11
	lsl	w13, w11, w12
	tst	w13, #0x7fffffff
	mov	w13, #31                        ; =0x1f
	cinc	x13, x13, ne
	sub	x12, x13, x12
	lsr	x13, x12, #5
	tst	x12, #0x1f
	cinc	x13, x13, ne
	cmp	x13, x12
	udiv	x12, x12, x13
	neg	w12, w12
	mov	w13, #-1                        ; =0xffffffff
	lsr	w12, w13, w12
	csel	w12, wzr, w12, hi
	ldr	x15, [x1, #2496]
	mov	x13, #3361                      ; =0xd21
	movk	x13, #8402, lsl #16
	movk	x13, #53773, lsl #32
	movk	x13, #3360, lsl #48
	mov	w14, #624                       ; =0x270
LBB7_3:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x15
	add	x15, x15, #1
	lsr	x17, x15, #4
	umulh	x17, x17, x13
	lsr	x17, x17, #1
	msub	x15, x17, x14, x15
	ldr	w17, [x1, x16, lsl #2]
	ldr	w0, [x1, x15, lsl #2]
	and	w17, w17, #0x80000000
	and	w3, w0, #0x7ffffffe
	orr	w17, w3, w17
	add	x3, x16, #397
	lsr	x4, x3, #4
	umulh	x4, x4, x13
	lsr	x4, x4, #1
	msub	x3, x4, x14, x3
	ldr	w3, [x1, x3, lsl #2]
	tst	w0, #0x1
	csel	w0, w10, wzr, ne
	eor	w0, w0, w3
	eor	w17, w0, w17, lsr #1
	str	w17, [x1, x16, lsl #2]
	eor	w16, w17, w17, lsr #11
	and	w17, w9, w16, lsl #7
	eor	w16, w17, w16
	and	w17, w8, w16, lsl #15
	eor	w16, w17, w16
	eor	w16, w16, w16, lsr #18
	and	w16, w16, w12
	cmp	w16, w11
	b.hs	LBB7_3
; %bb.4:
	str	x15, [x1, #2496]
	ldr	w8, [x2]
	add	w0, w8, w16
LBB7_5:
	ret
LBB7_6:
	ldr	x11, [x1, #2496]
	add	x12, x11, #1
	lsr	x13, x12, #4
	mov	x14, #3361                      ; =0xd21
	movk	x14, #8402, lsl #16
	movk	x14, #53773, lsl #32
	movk	x14, #3360, lsl #48
	umulh	x13, x13, x14
	lsr	x13, x13, #1
	mov	w15, #624                       ; =0x270
	msub	x12, x13, x15, x12
	ldr	w13, [x1, x11, lsl #2]
	and	w13, w13, #0x80000000
	ldr	w16, [x1, x12, lsl #2]
	and	w17, w16, #0x7ffffffe
	orr	w13, w17, w13
	add	x17, x11, #397
	lsr	x0, x17, #4
	umulh	x14, x0, x14
	lsr	x14, x14, #1
	msub	x14, x14, x15, x17
	ldr	w14, [x1, x14, lsl #2]
	tst	w16, #0x1
	csel	w10, w10, wzr, ne
	eor	w10, w10, w14
	eor	w10, w10, w13, lsr #1
	str	w10, [x1, x11, lsl #2]
	eor	w10, w10, w10, lsr #11
	str	x12, [x1, #2496]
	and	w9, w9, w10, lsl #7
	eor	w9, w9, w10
	and	w8, w8, w9, lsl #15
	eor	w8, w8, w9
	eor	w0, w8, w8, lsr #18
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l ; -- Begin function _ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l
	.weak_def_can_be_hidden	__ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l
	.p2align	2
__ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l: ; @_ZNSt3__16vectorIiNS_9allocatorIiEEE18__assign_with_sizeB8ne190102IPiS5_EEvT_T0_l
	.cfi_startproc
; %bb.0:
	stp	x24, x23, [sp, #-64]!           ; 16-byte Folded Spill
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	ldr	x8, [x0, #16]
	ldr	x0, [x0]
	sub	x8, x8, x0
	cmp	x3, x8, asr #2
	b.ls	LBB8_8
; %bb.1:
	cbz	x0, LBB8_3
; %bb.2:
	str	x0, [x19, #8]
	mov	x22, x3
	bl	__ZdlPv
	mov	x3, x22
	stp	xzr, xzr, [x19]
	str	xzr, [x19, #16]
LBB8_3:
	lsr	x8, x3, #62
	cbnz	x8, LBB8_18
; %bb.4:
	ldr	x8, [x19, #16]
	ldr	x9, [x19]
	mov	x10, #9223372036854775804       ; =0x7ffffffffffffffc
	sub	x8, x8, x9
	asr	x9, x8, #1
	cmp	x9, x3
	csel	x9, x9, x3, hi
	cmp	x8, x10
	mov	x8, #4611686018427387903        ; =0x3fffffffffffffff
	csel	x8, x9, x8, lo
	lsr	x9, x8, #62
	cbnz	x9, LBB8_18
; %bb.5:
	lsl	x23, x8, #2
	mov	x0, x23
	bl	__Znwm
	mov	x22, x0
	stp	x0, x0, [x19]
	add	x8, x0, x23
	str	x8, [x19, #16]
	subs	x21, x21, x20
	b.eq	LBB8_7
; %bb.6:
	mov	x0, x22
	mov	x1, x20
	mov	x2, x21
	bl	_memcpy
LBB8_7:
	add	x8, x22, x21
	b	LBB8_17
LBB8_8:
	ldr	x8, [x19, #8]
	sub	x2, x8, x0
	cmp	x3, x2, asr #2
	b.ls	LBB8_14
; %bb.9:
	add	x22, x20, x2
	cmp	x8, x0
	b.eq	LBB8_11
; %bb.10:
	mov	x1, x20
	bl	_memmove
LBB8_11:
	ldr	x20, [x19, #8]
	subs	x21, x21, x22
	b.eq	LBB8_13
; %bb.12:
	mov	x0, x20
	mov	x1, x22
	mov	x2, x21
	bl	_memmove
LBB8_13:
	add	x8, x20, x21
	b	LBB8_17
LBB8_14:
	subs	x21, x21, x20
	b.eq	LBB8_16
; %bb.15:
	mov	x22, x0
	mov	x1, x20
	mov	x2, x21
	bl	_memmove
	mov	x0, x22
LBB8_16:
	add	x8, x0, x21
LBB8_17:
	str	x8, [x19, #8]
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp], #64             ; 16-byte Folded Reload
	ret
LBB8_18:
	mov	x0, x19
	bl	__ZNKSt3__16vectorIiNS_9allocatorIiEEE20__throw_length_errorB8ne190102Ev
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
Ltmp135:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp136:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB9_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w8, [x4, #144]
	cmn	w8, #1
	b.ne	LBB9_7
; %bb.3:
Ltmp138:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp139:
; %bb.4:
Ltmp140:
Lloh86:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh87:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp141:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp142:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp143:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB9_7:
	ldrsb	w5, [x4, #144]
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp145:
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp146:
; %bb.8:
	cbnz	x0, LBB9_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp148:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp149:
LBB9_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB9_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB9_12:
Ltmp150:
	b	LBB9_15
LBB9_13:
Ltmp144:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB9_16
LBB9_14:
Ltmp147:
LBB9_15:
	mov	x20, x0
LBB9_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB9_18
LBB9_17:
Ltmp137:
	mov	x20, x0
LBB9_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp151:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp152:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB9_11
LBB9_20:
Ltmp153:
	mov	x19, x0
Ltmp154:
	bl	___cxa_end_catch
Ltmp155:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB9_22:
Ltmp156:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh86, Lloh87
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table9:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Ltmp135-Lfunc_begin2           ; >> Call Site 1 <<
	.uleb128 Ltmp136-Ltmp135                ;   Call between Ltmp135 and Ltmp136
	.uleb128 Ltmp137-Lfunc_begin2           ;     jumps to Ltmp137
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp138-Lfunc_begin2           ; >> Call Site 2 <<
	.uleb128 Ltmp139-Ltmp138                ;   Call between Ltmp138 and Ltmp139
	.uleb128 Ltmp147-Lfunc_begin2           ;     jumps to Ltmp147
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp140-Lfunc_begin2           ; >> Call Site 3 <<
	.uleb128 Ltmp143-Ltmp140                ;   Call between Ltmp140 and Ltmp143
	.uleb128 Ltmp144-Lfunc_begin2           ;     jumps to Ltmp144
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp145-Lfunc_begin2           ; >> Call Site 4 <<
	.uleb128 Ltmp146-Ltmp145                ;   Call between Ltmp145 and Ltmp146
	.uleb128 Ltmp147-Lfunc_begin2           ;     jumps to Ltmp147
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp148-Lfunc_begin2           ; >> Call Site 5 <<
	.uleb128 Ltmp149-Ltmp148                ;   Call between Ltmp148 and Ltmp149
	.uleb128 Ltmp150-Lfunc_begin2           ;     jumps to Ltmp150
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp149-Lfunc_begin2           ; >> Call Site 6 <<
	.uleb128 Ltmp151-Ltmp149                ;   Call between Ltmp149 and Ltmp151
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp151-Lfunc_begin2           ; >> Call Site 7 <<
	.uleb128 Ltmp152-Ltmp151                ;   Call between Ltmp151 and Ltmp152
	.uleb128 Ltmp153-Lfunc_begin2           ;     jumps to Ltmp153
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp152-Lfunc_begin2           ; >> Call Site 8 <<
	.uleb128 Ltmp154-Ltmp152                ;   Call between Ltmp152 and Ltmp154
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp154-Lfunc_begin2           ; >> Call Site 9 <<
	.uleb128 Ltmp155-Ltmp154                ;   Call between Ltmp154 and Ltmp155
	.uleb128 Ltmp156-Lfunc_begin2           ;     jumps to Ltmp156
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp155-Lfunc_begin2           ; >> Call Site 10 <<
	.uleb128 Lfunc_end2-Ltmp155             ;   Call between Ltmp155 and Lfunc_end2
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
	cbz	x0, LBB10_16
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
	b.lt	LBB10_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB10_15
LBB10_3:
	cmp	x23, #1
	b.lt	LBB10_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB10_17
; %bb.5:
	cmp	x23, #22
	b.hi	LBB10_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB10_8
LBB10_7:
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
LBB10_8:
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
Ltmp157:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp158:
; %bb.9:
	cmp	x0, x23
	csel	x19, x19, xzr, eq
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB10_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB10_15
	b	LBB10_12
LBB10_11:
	ldr	x8, [sp, #8]
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPv
	mov	x0, x24
	cmp	x0, x23
	b.ne	LBB10_15
LBB10_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB10_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB10_15
LBB10_14:
	str	xzr, [x20, #24]
	b	LBB10_16
LBB10_15:
	mov	x19, #0                         ; =0x0
LBB10_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB10_17:
	add	x0, sp, #8
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
LBB10_18:
Ltmp159:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB10_20
; %bb.19:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB10_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table10:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Lfunc_begin3-Lfunc_begin3      ; >> Call Site 1 <<
	.uleb128 Ltmp157-Lfunc_begin3           ;   Call between Lfunc_begin3 and Ltmp157
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp157-Lfunc_begin3           ; >> Call Site 2 <<
	.uleb128 Ltmp158-Ltmp157                ;   Call between Ltmp157 and Ltmp158
	.uleb128 Ltmp159-Lfunc_begin3           ;     jumps to Ltmp159
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp158-Lfunc_begin3           ; >> Call Site 3 <<
	.uleb128 Lfunc_end3-Ltmp158             ;   Call between Ltmp158 and Lfunc_end3
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
Lloh88:
	adrp	x0, l_.str.26@PAGE
Lloh89:
	add	x0, x0, l_.str.26@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh88, Lloh89
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Correctness: shuffled="

l_.str.1:                               ; @.str.1
	.asciz	"  sorted="

l_.str.2:                               ; @.str.2
	.asciz	"  branchless="

l_.str.3:                               ; @.str.3
	.asciz	"  "

l_.str.4:                               ; @.str.4
	.asciz	"PASS"

l_.str.5:                               ; @.str.5
	.asciz	"FAIL"

l_.str.6:                               ; @.str.6
	.asciz	"\n"

l_.str.7:                               ; @.str.7
	.asciz	"\nData:  "

l_.str.8:                               ; @.str.8
	.asciz	"M ints in [0,255]  threshold="

l_.str.9:                               ; @.str.9
	.asciz	"  ~"

l_.str.10:                              ; @.str.10
	.asciz	"% above\n"

l_.str.11:                              ; @.str.11
	.asciz	"Expected misprediction overhead (shuffled): ~"

l_.str.12:                              ; @.str.12
	.asciz	" ms  "

l_.str.13:                              ; @.str.13
	.asciz	"("

l_.str.14:                              ; @.str.14
	.asciz	"% mispredict \303\227 "

l_.str.15:                              ; @.str.15
	.asciz	" cycles \303\227 32M branches at 3 GHz)\n"

l_.str.16:                              ; @.str.16
	.asciz	"Conditional sum  N=32M ints  128 MB   threshold=128\n"

l_.str.17:                              ; @.str.17
	.asciz	"version"

l_.str.18:                              ; @.str.18
	.asciz	"time"

l_.str.19:                              ; @.str.19
	.asciz	"speedup"

l_.str.20:                              ; @.str.20
	.asciz	"branchy  + shuffled  (50% mispredict)"

l_.str.21:                              ; @.str.21
	.asciz	"branchy  + sorted    (1 mispredict)"

l_.str.22:                              ; @.str.22
	.asciz	"branchless + shuffled (no branch)"

l_.str.23:                              ; @.str.23
	.asciz	" ms"

l_.str.24:                              ; @.str.24
	.asciz	"x\n"

l_.str.25:                              ; @.str.25
	.asciz	"vector"

l_.str.26:                              ; @.str.26
	.asciz	"basic_string"

.subsections_via_symbols
