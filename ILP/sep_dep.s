	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 5
	.globl	__Z13compute_naivePKfPfi        ; -- Begin function _Z13compute_naivePKfPfi
	.p2align	2
__Z13compute_naivePKfPfi:               ; @_Z13compute_naivePKfPfi
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB0_3
; %bb.1:
	mov	w8, w2
	fmov	s0, #1.00000000
LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s1, [x0], #4
	fmul	s2, s1, s1
	fmul	s1, s1, s2
	fadd	s1, s1, s0
	str	s1, [x1], #4
	subs	x8, x8, #1
	b.ne	LBB0_2
LBB0_3:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z12compute_sep2PKfPfi         ; -- Begin function _Z12compute_sep2PKfPfi
	.p2align	2
__Z12compute_sep2PKfPfi:                ; @_Z12compute_sep2PKfPfi
	.cfi_startproc
; %bb.0:
                                        ; kill: def $w2 killed $w2 def $x2
	and	w8, w2, #0xfffffffe
	cmp	w8, #1
	b.lt	LBB1_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	add	x10, x1, #4
	add	x11, x0, #4
	fmov	s0, #1.00000000
LBB1_2:                                 ; =>This Inner Loop Header: Depth=1
	ldp	s1, s2, [x11, #-4]
	fmul	s3, s1, s1
	fmul	s4, s2, s2
	fmul	s1, s1, s3
	fmul	s2, s2, s4
	fadd	s1, s1, s0
	fadd	s2, s2, s0
	stp	s1, s2, [x10, #-4]
	add	x9, x9, #2
	add	x10, x10, #8
	add	x11, x11, #8
	cmp	x9, x8
	b.lo	LBB1_2
LBB1_3:
	tbz	w2, #0, LBB1_5
; %bb.4:
	sbfiz	x8, x2, #2, #32
	sub	x8, x8, #4
	ldr	s0, [x0, x8]
	fmul	s1, s0, s0
	fmov	s2, #1.00000000
	fmadd	s0, s1, s0, s2
	str	s0, [x1, x8]
LBB1_5:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z12compute_sep4PKfPfi         ; -- Begin function _Z12compute_sep4PKfPfi
	.p2align	2
__Z12compute_sep4PKfPfi:                ; @_Z12compute_sep4PKfPfi
	.cfi_startproc
; %bb.0:
                                        ; kill: def $w2 killed $w2 def $x2
	and	w8, w2, #0xfffffffc
	cmp	w8, #1
	b.lt	LBB2_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	add	x10, x0, #8
	add	x11, x1, #8
	fmov	s0, #1.00000000
LBB2_2:                                 ; =>This Inner Loop Header: Depth=1
	ldp	s1, s2, [x10, #-8]
	fmul	s3, s1, s1
	fmul	s4, s2, s2
	ldp	s5, s6, [x10], #16
	fmul	s7, s5, s5
	fmul	s16, s6, s6
	fmul	s1, s1, s3
	fmul	s2, s2, s4
	fmul	s3, s5, s7
	fmul	s4, s6, s16
	fadd	s1, s1, s0
	fadd	s2, s2, s0
	stp	s1, s2, [x11, #-8]
	fadd	s1, s3, s0
	fadd	s2, s4, s0
	stp	s1, s2, [x11], #16
	add	x9, x9, #4
	cmp	x9, x8
	b.lo	LBB2_2
LBB2_3:
	cmp	w8, w2
	b.eq	LBB2_6
; %bb.4:
	sxtw	x8, w8
	sxtw	x9, w2
	fmov	s0, #1.00000000
LBB2_5:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s1, [x0, x8, lsl #2]
	fmul	s2, s1, s1
	fmadd	s1, s2, s1, s0
	str	s1, [x1, x8, lsl #2]
	add	x8, x8, #1
	cmp	x8, x9
	b.lt	LBB2_5
LBB2_6:
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
	sub	sp, sp, #192
	stp	d11, d10, [sp, #64]             ; 16-byte Folded Spill
	stp	d9, d8, [sp, #80]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #96]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #112]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #128]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #144]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #160]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #176]            ; 16-byte Folded Spill
	add	x29, sp, #176
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
	.cfi_offset b10, -120
	.cfi_offset b11, -128
Lloh0:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh2:
	ldr	x8, [x8]
	str	x8, [sp, #56]
	mov	w20, #134217728                 ; =0x8000000
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
	mov	x19, x0
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
	mov	x8, #0                          ; =0x0
	mov	w9, #0                          ; =0x0
	mov	w10, #19923                     ; =0x4dd3
	movk	w10, #4194, lsl #16
	mov	w11, #1000                      ; =0x3e8
	mov	w12, #4719                      ; =0x126f
	movk	w12, #14979, lsl #16
	fmov	s0, w12
	fmov	s1, #0.50000000
LBB3_1:                                 ; =>This Inner Loop Header: Depth=1
	umull	x12, w9, w10
	lsr	x12, x12, #38
	msub	w12, w12, w11, w9
	ucvtf	s2, w12
	fmadd	s2, s2, s0, s1
	str	s2, [x19, x8]
	add	w9, w9, #1
	add	x8, x8, #4
	cmp	x8, x20
	b.ne	LBB3_1
; %bb.2:
Ltmp0:
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
Ltmp1:
; %bb.3:
	mov	x20, x0
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
Ltmp3:
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
Ltmp4:
; %bb.4:
	mov	x21, x0
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
Ltmp6:
	mov	w0, #134217728                  ; =0x8000000
	bl	__Znwm
Ltmp7:
; %bb.5:
	mov	x22, x0
	mov	w23, #134217728                 ; =0x8000000
	mov	w1, #134217728                  ; =0x8000000
	bl	_bzero
	mov	x8, #0                          ; =0x0
	fmov	s0, #1.00000000
LBB3_6:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s1, [x19, x8]
	fmul	s2, s1, s1
	fmul	s1, s1, s2
	fadd	s1, s1, s0
	str	s1, [x20, x8]
	add	x8, x8, #4
	cmp	x8, x23
	b.ne	LBB3_6
; %bb.7:
	mov	x8, #0                          ; =0x0
	fmov	s0, #1.00000000
	mov	w9, #33554430                   ; =0x1fffffe
LBB3_8:                                 ; =>This Inner Loop Header: Depth=1
	lsl	x10, x8, #2
	add	x11, x19, x10
	ldp	s1, s2, [x11]
	fmul	s3, s1, s1
	fmul	s4, s2, s2
	fmul	s1, s1, s3
	fmul	s2, s2, s4
	fadd	s1, s1, s0
	add	x10, x21, x10
	fadd	s2, s2, s0
	stp	s1, s2, [x10]
	add	x10, x8, #2
	cmp	x8, x9
	mov	x8, x10
	b.lo	LBB3_8
; %bb.9:
	mov	x8, #-4                         ; =0xfffffffffffffffc
	mov	w9, #8                          ; =0x8
	fmov	s0, #1.00000000
	mov	w10, #33554428                  ; =0x1fffffc
LBB3_10:                                ; =>This Inner Loop Header: Depth=1
	add	x11, x19, x9
	ldp	s1, s2, [x11, #-8]
	fmul	s3, s1, s1
	fmul	s4, s2, s2
	ldp	s5, s6, [x11]
	fmul	s7, s5, s5
	fmul	s16, s6, s6
	fmul	s1, s1, s3
	fmul	s2, s2, s4
	fmul	s3, s5, s7
	fmul	s4, s6, s16
	fadd	s1, s1, s0
	add	x11, x22, x9
	fadd	s2, s2, s0
	stp	s1, s2, [x11, #-8]
	fadd	s1, s3, s0
	fadd	s2, s4, s0
	stp	s1, s2, [x11]
	add	x8, x8, #4
	add	x9, x9, #16
	cmp	x8, x10
	b.lo	LBB3_10
; %bb.11:
	mov	x8, #0                          ; =0x0
	movi	d8, #0000000000000000
	mov	w9, #134217728                  ; =0x8000000
	movi	d9, #0000000000000000
LBB3_12:                                ; =>This Inner Loop Header: Depth=1
	ldr	s0, [x20, x8]
	ldr	s1, [x21, x8]
	fabd	s1, s0, s1
	fcmp	s8, s1
	ldr	s2, [x22, x8]
	fcsel	s8, s1, s8, mi
	fabd	s0, s0, s2
	fcmp	s9, s0
	fcsel	s9, s0, s9, mi
	add	x8, x8, #4
	cmp	x8, x9
	b.ne	LBB3_12
; %bb.13:
Ltmp9:
Lloh3:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh4:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh5:
	adrp	x1, l_.str@PAGE
Lloh6:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #26                         ; =0x1a
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp10:
; %bb.14:
Ltmp11:
	fmov	s0, s8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp12:
; %bb.15:
Ltmp13:
Lloh7:
	adrp	x1, l_.str.1@PAGE
Lloh8:
	add	x1, x1, l_.str.1@PAGEOFF
	mov	w2, #15                         ; =0xf
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp14:
; %bb.16:
Ltmp15:
	fmov	s0, s9
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp16:
; %bb.17:
Ltmp17:
Lloh9:
	adrp	x1, l_.str.2@PAGE
Lloh10:
	add	x1, x1, l_.str.2@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp18:
; %bb.18:
	fcmp	s9, #0.0
	movi	d0, #0000000000000000
	fccmp	s8, s0, #0, eq
Lloh11:
	adrp	x8, l_.str.4@PAGE
Lloh12:
	add	x8, x8, l_.str.4@PAGEOFF
Lloh13:
	adrp	x9, l_.str.3@PAGE
Lloh14:
	add	x9, x9, l_.str.3@PAGEOFF
	csel	x1, x9, x8, eq
Ltmp19:
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp20:
; %bb.19:
Ltmp21:
Lloh15:
	adrp	x1, l_.str.5@PAGE
Lloh16:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp22:
; %bb.20:
Ltmp24:
Lloh17:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh18:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh19:
	adrp	x1, l_.str.6@PAGE
Lloh20:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #8                          ; =0x8
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp25:
; %bb.21:
Ltmp26:
	mov	w1, #32                         ; =0x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp27:
; %bb.22:
Ltmp28:
Lloh21:
	adrp	x1, l_.str.7@PAGE
Lloh22:
	add	x1, x1, l_.str.7@PAGEOFF
	mov	w2, #11                         ; =0xb
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp29:
; %bb.23:
Ltmp30:
	mov	w1, #128                        ; =0x80
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp31:
; %bb.24:
Ltmp32:
Lloh23:
	adrp	x1, l_.str.8@PAGE
Lloh24:
	add	x1, x1, l_.str.8@PAGEOFF
	mov	w2, #5                          ; =0x5
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp33:
; %bb.25:
Ltmp34:
Lloh25:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh26:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh27:
	adrp	x1, l_.str.9@PAGE
Lloh28:
	add	x1, x1, l_.str.9@PAGEOFF
	mov	w2, #31                         ; =0x1f
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp35:
; %bb.26:
Ltmp36:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp37:
; %bb.27:
Ltmp38:
Lloh29:
	adrp	x1, l_.str.10@PAGE
Lloh30:
	add	x1, x1, l_.str.10@PAGEOFF
	mov	w2, #33                         ; =0x21
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp39:
; %bb.28:
Ltmp40:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp41:
; %bb.29:
Ltmp42:
Lloh31:
	adrp	x1, l_.str.11@PAGE
Lloh32:
	add	x1, x1, l_.str.11@PAGEOFF
	mov	w2, #9                          ; =0x9
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp43:
; %bb.30:
	mov	w25, #0                         ; =0x0
	mov	x24, #9223372036854775807       ; =0x7fffffffffffffff
	fmov	s8, #1.00000000
	mov	w26, #134217728                 ; =0x8000000
	mov	x28, #13531                     ; =0x34db
	movk	x28, #55222, lsl #16
	movk	x28, #56962, lsl #32
	movk	x28, #17179, lsl #48
LBB3_31:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_32 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x23, x0
	mov	x8, #0                          ; =0x0
LBB3_32:                                ;   Parent Loop BB3_31 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	s0, [x19, x8]
	fmul	s1, s0, s0
	fmul	s0, s0, s1
	fadd	s0, s0, s8
	str	s0, [x20, x8]
	add	x8, x8, #4
	cmp	x8, x26
	b.ne	LBB3_32
; %bb.33:                               ;   in Loop: Header=BB3_31 Depth=1
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x23
	smulh	x8, x8, x28
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x24
	csel	x24, x8, x24, lt
	add	w25, w25, #1
	cmp	w25, #5
	b.ne	LBB3_31
; %bb.34:
	mov	w25, #0                         ; =0x0
	mov	x26, #9223372036854775807       ; =0x7fffffffffffffff
	fmov	s8, #1.00000000
	mov	w27, #33554430                  ; =0x1fffffe
LBB3_35:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_36 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x23, x0
	mov	x8, #0                          ; =0x0
LBB3_36:                                ;   Parent Loop BB3_35 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	lsl	x9, x8, #2
	add	x10, x19, x9
	ldp	s0, s1, [x10]
	fmul	s2, s0, s0
	fmul	s3, s1, s1
	fmul	s0, s0, s2
	fmul	s1, s1, s3
	fadd	s0, s0, s8
	add	x9, x21, x9
	fadd	s1, s1, s8
	stp	s0, s1, [x9]
	add	x9, x8, #2
	cmp	x8, x27
	mov	x8, x9
	b.lo	LBB3_36
; %bb.37:                               ;   in Loop: Header=BB3_35 Depth=1
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x23
	smulh	x8, x8, x28
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x26
	csel	x26, x8, x26, lt
	add	w25, w25, #1
	cmp	w25, #5
	b.ne	LBB3_35
; %bb.38:
	mov	w28, #0                         ; =0x0
	mov	x27, #9223372036854775807       ; =0x7fffffffffffffff
	fmov	s8, #1.00000000
	mov	w25, #33554428                  ; =0x1fffffc
LBB3_39:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_40 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x23, x0
	mov	x8, #-4                         ; =0xfffffffffffffffc
	mov	w9, #8                          ; =0x8
LBB3_40:                                ;   Parent Loop BB3_39 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x19, x9
	ldp	s0, s1, [x10, #-8]
	fmul	s2, s0, s0
	fmul	s3, s1, s1
	ldp	s4, s5, [x10]
	fmul	s6, s4, s4
	fmul	s7, s5, s5
	fmul	s0, s0, s2
	fmul	s1, s1, s3
	fmul	s2, s4, s6
	fmul	s3, s5, s7
	fadd	s0, s0, s8
	add	x10, x22, x9
	fadd	s1, s1, s8
	stp	s0, s1, [x10, #-8]
	fadd	s0, s2, s8
	fadd	s1, s3, s8
	stp	s0, s1, [x10]
	add	x8, x8, #4
	add	x9, x9, #16
	cmp	x8, x25
	b.lo	LBB3_40
; %bb.41:                               ;   in Loop: Header=BB3_39 Depth=1
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x23
	mov	x9, #13531                      ; =0x34db
	movk	x9, #55222, lsl #16
	movk	x9, #56962, lsl #32
	movk	x9, #17179, lsl #48
	smulh	x8, x8, x9
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x27
	csel	x27, x8, x27, lt
	add	w28, w28, #1
	cmp	w28, #5
	b.ne	LBB3_39
; %bb.42:
	stp	x24, x26, [sp, #32]
	str	x27, [sp, #48]
Ltmp45:
Lloh33:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh34:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh35:
	adrp	x1, l_.str.5@PAGE
Lloh36:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp46:
; %bb.43:
Ltmp47:
	mov	x23, x0
	mov	w0, #80                         ; =0x50
	bl	__Znwm
Ltmp48:
; %bb.44:
	mov	x25, #80                        ; =0x50
	movk	x25, #32768, lsl #48
	mov	w8, #76                         ; =0x4c
	str	x0, [sp, #8]
	stp	x8, x25, [sp, #16]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	stur	q0, [x0, #60]
	strb	wzr, [x0, #76]
	ldrsb	w9, [sp, #31]
	add	x10, sp, #8
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp50:
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp51:
; %bb.45:
Ltmp52:
Lloh37:
	adrp	x1, l_.str.5@PAGE
Lloh38:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp53:
; %bb.46:
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB3_48
; %bb.47:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB3_48:
Ltmp55:
Lloh39:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh40:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh41:
	adrp	x1, l_.str.12@PAGE
Lloh42:
	add	x1, x1, l_.str.12@PAGEOFF
	mov	w2, #56                         ; =0x38
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp56:
; %bb.49:
Ltmp57:
	fmov	d0, #4.00000000
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp58:
; %bb.50:
Ltmp59:
Lloh43:
	adrp	x1, l_.str.13@PAGE
Lloh44:
	add	x1, x1, l_.str.13@PAGEOFF
	mov	w2, #5                          ; =0x5
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp60:
; %bb.51:
Ltmp62:
	mov	w0, #80                         ; =0x50
	bl	__Znwm
Ltmp63:
; %bb.52:
	mov	w8, #76                         ; =0x4c
	str	x0, [sp, #8]
	stp	x8, x25, [sp, #16]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	stur	q0, [x0, #60]
	strb	wzr, [x0, #76]
	ldrsb	w9, [sp, #31]
	add	x10, sp, #8
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp65:
Lloh45:
	adrp	x23, __ZNSt3__14coutE@GOTPAGE
Lloh46:
	ldr	x23, [x23, __ZNSt3__14coutE@GOTPAGEOFF]
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp66:
; %bb.53:
Ltmp67:
Lloh47:
	adrp	x1, l_.str.5@PAGE
Lloh48:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp68:
; %bb.54:
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB3_56
; %bb.55:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB3_56:
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
	mov	w9, #26                         ; =0x1a
	str	x9, [x8, #24]
Ltmp70:
Lloh49:
	adrp	x1, l_.str.14@PAGE
Lloh50:
	add	x1, x1, l_.str.14@PAGEOFF
	mov	x0, x23
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp71:
; %bb.57:
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
Ltmp73:
Lloh51:
	adrp	x1, l_.str.15@PAGE
Lloh52:
	add	x1, x1, l_.str.15@PAGEOFF
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp74:
; %bb.58:
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	mov	w9, #12                         ; =0xc
	str	x9, [x8, #24]
Ltmp76:
Lloh53:
	adrp	x1, l_.str.16@PAGE
Lloh54:
	add	x1, x1, l_.str.16@PAGEOFF
	mov	w2, #11                         ; =0xb
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp77:
; %bb.59:
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
Ltmp79:
Lloh55:
	adrp	x1, l_.str.17@PAGE
Lloh56:
	add	x1, x1, l_.str.17@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp80:
; %bb.60:
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
Ltmp82:
Lloh57:
	adrp	x1, l_.str.18@PAGE
Lloh58:
	add	x1, x1, l_.str.18@PAGEOFF
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp83:
; %bb.61:
Ltmp84:
Lloh59:
	adrp	x1, l_.str.5@PAGE
Lloh60:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp85:
; %bb.62:
Ltmp87:
	mov	w0, #80                         ; =0x50
	bl	__Znwm
Ltmp88:
; %bb.63:
	mov	w8, #76                         ; =0x4c
	str	x0, [sp, #8]
	stp	x8, x25, [sp, #16]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	stp	q0, q0, [x0, #32]
	stur	q0, [x0, #60]
	strb	wzr, [x0, #76]
	ldrsb	w9, [sp, #31]
	add	x10, sp, #8
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp90:
	and	x2, x8, #0xff
	mov	x0, x23
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp91:
; %bb.64:
Ltmp92:
Lloh61:
	adrp	x1, l_.str.5@PAGE
Lloh62:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp93:
; %bb.65:
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB3_67
; %bb.66:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB3_67:
	mov	x27, #0                         ; =0x0
	scvtf	d9, x24
	mov	w28, #7                         ; =0x7
	add	x24, sp, #32
	mov	w25, #2                         ; =0x2
	fmov	d10, #4.00000000
LBB3_68:                                ; =>This Inner Loop Header: Depth=1
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
	mov	w9, #26                         ; =0x1a
	str	x9, [x8, #24]
Lloh63:
	adrp	x8, l___const.main.labels@PAGE
Lloh64:
	add	x8, x8, l___const.main.labels@PAGEOFF
	ldr	x26, [x8, x27]
	mov	x0, x26
	bl	_strlen
	mov	x2, x0
Ltmp95:
	mov	x0, x23
	mov	x1, x26
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp96:
; %bb.69:                               ;   in Loop: Header=BB3_68 Depth=1
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x28, [x8, #24]
	ldr	x26, [x24, x27]
Ltmp98:
	mov	x1, x26
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp99:
; %bb.70:                               ;   in Loop: Header=BB3_68 Depth=1
Ltmp100:
Lloh65:
	adrp	x1, l_.str.22@PAGE
Lloh66:
	add	x1, x1, l_.str.22@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp101:
; %bb.71:                               ;   in Loop: Header=BB3_68 Depth=1
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	mov	w10, #11                        ; =0xb
	str	x10, [x9, #24]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xfffffeff
	orr	w10, w10, #0x4
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x25, [x8, #16]
	ldr	d0, [x24, x27]
	scvtf	d0, d0
	fmul	d0, d0, d10
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fmul	d0, d0, d1
	mov	x8, #4494592428115755008        ; =0x3e60000000000000
	fmov	d1, x8
	fmul	d8, d0, d1
Ltmp103:
	fmov	d0, d8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp104:
; %bb.72:                               ;   in Loop: Header=BB3_68 Depth=1
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xffffffcf
	orr	w10, w10, #0x80
	str	w10, [x9, #8]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	str	x28, [x9, #24]
	ldur	x9, [x8, #-24]
	add	x9, x0, x9
	ldr	w10, [x9, #8]
	and	w10, w10, #0xfffffeff
	orr	w10, w10, #0x4
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x25, [x8, #16]
Lloh67:
	adrp	x8, __ZL5INSNS@PAGE
Lloh68:
	add	x8, x8, __ZL5INSNS@PAGEOFF
	ldr	d0, [x8, x27]
	fdiv	d0, d8, d0
Ltmp106:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp107:
; %bb.73:                               ;   in Loop: Header=BB3_68 Depth=1
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
	and	w10, w10, #0xfffffeff
	orr	w10, w10, #0x4
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	str	x25, [x8, #16]
	cmp	x26, #1
	csinc	x8, x26, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d9, d0
Ltmp109:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp110:
; %bb.74:                               ;   in Loop: Header=BB3_68 Depth=1
Ltmp111:
Lloh69:
	adrp	x1, l_.str.23@PAGE
Lloh70:
	add	x1, x1, l_.str.23@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp112:
; %bb.75:                               ;   in Loop: Header=BB3_68 Depth=1
	add	x27, x27, #8
	cmp	x27, #24
	b.ne	LBB3_68
; %bb.76:
Ltmp114:
Lloh71:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh72:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh73:
	adrp	x1, l_.str.24@PAGE
Lloh74:
	add	x1, x1, l_.str.24@PAGEOFF
	mov	w2, #33                         ; =0x21
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp115:
; %bb.77:
Ltmp116:
Lloh75:
	adrp	x1, l_.str.25@PAGE
Lloh76:
	add	x1, x1, l_.str.25@PAGEOFF
	mov	w2, #20                         ; =0x14
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp117:
; %bb.78:
Ltmp118:
	mov	w1, #2                          ; =0x2
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp119:
; %bb.79:
Ltmp120:
Lloh77:
	adrp	x1, l_.str.26@PAGE
Lloh78:
	add	x1, x1, l_.str.26@PAGEOFF
	mov	w2, #16                         ; =0x10
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp121:
; %bb.80:
Ltmp122:
	mov	w1, #7                          ; =0x7
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp123:
; %bb.81:
Ltmp124:
Lloh79:
	adrp	x1, l_.str.27@PAGE
Lloh80:
	add	x1, x1, l_.str.27@PAGEOFF
	mov	w2, #13                         ; =0xd
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp125:
; %bb.82:
Ltmp126:
Lloh81:
	adrp	x1, l_.str.28@PAGE
Lloh82:
	add	x1, x1, l_.str.28@PAGEOFF
	mov	w2, #57                         ; =0x39
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp127:
; %bb.83:
Ltmp128:
	mov	w1, #4                          ; =0x4
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp129:
; %bb.84:
Ltmp130:
Lloh83:
	adrp	x1, l_.str.27@PAGE
Lloh84:
	add	x1, x1, l_.str.27@PAGEOFF
	mov	w2, #13                         ; =0xd
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp131:
; %bb.85:
Ltmp132:
Lloh85:
	adrp	x1, l_.str.29@PAGE
Lloh86:
	add	x1, x1, l_.str.29@PAGEOFF
	mov	w2, #9                          ; =0x9
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp133:
; %bb.86:
Ltmp134:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp135:
; %bb.87:
Ltmp136:
Lloh87:
	adrp	x1, l_.str.30@PAGE
Lloh88:
	add	x1, x1, l_.str.30@PAGEOFF
	mov	w2, #19                         ; =0x13
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp137:
; %bb.88:
Ltmp138:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp139:
; %bb.89:
Ltmp140:
Lloh89:
	adrp	x1, l_.str.31@PAGE
Lloh90:
	add	x1, x1, l_.str.31@PAGEOFF
	mov	w2, #41                         ; =0x29
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp141:
; %bb.90:
Ltmp142:
Lloh91:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh92:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh93:
	adrp	x1, l_.str.32@PAGE
Lloh94:
	add	x1, x1, l_.str.32@PAGEOFF
	mov	w2, #126                        ; =0x7e
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp143:
; %bb.91:
	mov	x0, x22
	bl	__ZdlPv
	mov	x0, x21
	bl	__ZdlPv
	mov	x0, x20
	bl	__ZdlPv
	mov	x0, x19
	bl	__ZdlPv
	ldr	x8, [sp, #56]
Lloh95:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh96:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh97:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB3_93
; %bb.92:
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #176]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #160]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #144]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #128]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #112]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #96]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #80]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #192
	ret
LBB3_93:
	bl	___stack_chk_fail
LBB3_94:
Ltmp89:
	b	LBB3_119
LBB3_95:
Ltmp81:
	b	LBB3_119
LBB3_96:
Ltmp78:
	b	LBB3_119
LBB3_97:
Ltmp75:
	b	LBB3_119
LBB3_98:
Ltmp72:
	b	LBB3_119
LBB3_99:
Ltmp64:
	b	LBB3_119
LBB3_100:
Ltmp49:
	b	LBB3_119
LBB3_101:
Ltmp8:
	mov	x23, x0
	b	LBB3_121
LBB3_102:
Ltmp5:
	mov	x23, x0
	b	LBB3_122
LBB3_103:
Ltmp2:
	mov	x23, x0
	b	LBB3_123
LBB3_104:
Ltmp94:
	b	LBB3_108
LBB3_105:
Ltmp86:
	b	LBB3_119
LBB3_106:
Ltmp69:
	b	LBB3_108
LBB3_107:
Ltmp54:
LBB3_108:
	mov	x23, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB3_120
; %bb.109:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
	b	LBB3_120
LBB3_110:
Ltmp61:
	b	LBB3_119
LBB3_111:
Ltmp23:
	b	LBB3_119
LBB3_112:
Ltmp44:
	b	LBB3_119
LBB3_113:
Ltmp144:
	b	LBB3_119
LBB3_114:
Ltmp97:
	b	LBB3_119
LBB3_115:
Ltmp105:
	b	LBB3_119
LBB3_116:
Ltmp108:
	b	LBB3_119
LBB3_117:
Ltmp102:
	b	LBB3_119
LBB3_118:
Ltmp113:
LBB3_119:
	mov	x23, x0
LBB3_120:
	mov	x0, x22
	bl	__ZdlPv
LBB3_121:
	mov	x0, x21
	bl	__ZdlPv
LBB3_122:
	mov	x0, x20
	bl	__ZdlPv
LBB3_123:
	mov	x0, x19
	bl	__ZdlPv
	mov	x0, x23
	bl	__Unwind_Resume
	.loh AdrpLdrGotLdr	Lloh0, Lloh1, Lloh2
	.loh AdrpAdd	Lloh5, Lloh6
	.loh AdrpLdrGot	Lloh3, Lloh4
	.loh AdrpAdd	Lloh7, Lloh8
	.loh AdrpAdd	Lloh9, Lloh10
	.loh AdrpAdd	Lloh13, Lloh14
	.loh AdrpAdd	Lloh11, Lloh12
	.loh AdrpAdd	Lloh15, Lloh16
	.loh AdrpAdd	Lloh19, Lloh20
	.loh AdrpLdrGot	Lloh17, Lloh18
	.loh AdrpAdd	Lloh21, Lloh22
	.loh AdrpAdd	Lloh23, Lloh24
	.loh AdrpAdd	Lloh27, Lloh28
	.loh AdrpLdrGot	Lloh25, Lloh26
	.loh AdrpAdd	Lloh29, Lloh30
	.loh AdrpAdd	Lloh31, Lloh32
	.loh AdrpAdd	Lloh35, Lloh36
	.loh AdrpLdrGot	Lloh33, Lloh34
	.loh AdrpAdd	Lloh37, Lloh38
	.loh AdrpAdd	Lloh41, Lloh42
	.loh AdrpLdrGot	Lloh39, Lloh40
	.loh AdrpAdd	Lloh43, Lloh44
	.loh AdrpLdrGot	Lloh45, Lloh46
	.loh AdrpAdd	Lloh47, Lloh48
	.loh AdrpAdd	Lloh49, Lloh50
	.loh AdrpAdd	Lloh51, Lloh52
	.loh AdrpAdd	Lloh53, Lloh54
	.loh AdrpAdd	Lloh55, Lloh56
	.loh AdrpAdd	Lloh57, Lloh58
	.loh AdrpAdd	Lloh59, Lloh60
	.loh AdrpAdd	Lloh61, Lloh62
	.loh AdrpAdd	Lloh63, Lloh64
	.loh AdrpAdd	Lloh65, Lloh66
	.loh AdrpAdd	Lloh67, Lloh68
	.loh AdrpAdd	Lloh69, Lloh70
	.loh AdrpAdd	Lloh73, Lloh74
	.loh AdrpLdrGot	Lloh71, Lloh72
	.loh AdrpAdd	Lloh75, Lloh76
	.loh AdrpAdd	Lloh77, Lloh78
	.loh AdrpAdd	Lloh79, Lloh80
	.loh AdrpAdd	Lloh81, Lloh82
	.loh AdrpAdd	Lloh83, Lloh84
	.loh AdrpAdd	Lloh85, Lloh86
	.loh AdrpAdd	Lloh87, Lloh88
	.loh AdrpAdd	Lloh89, Lloh90
	.loh AdrpAdd	Lloh93, Lloh94
	.loh AdrpLdrGot	Lloh91, Lloh92
	.loh AdrpLdrGotLdr	Lloh95, Lloh96, Lloh97
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table3:
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
	.uleb128 Ltmp4-Lfunc_begin0             ; >> Call Site 5 <<
	.uleb128 Ltmp6-Ltmp4                    ;   Call between Ltmp4 and Ltmp6
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp6-Lfunc_begin0             ; >> Call Site 6 <<
	.uleb128 Ltmp7-Ltmp6                    ;   Call between Ltmp6 and Ltmp7
	.uleb128 Ltmp8-Lfunc_begin0             ;     jumps to Ltmp8
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp7-Lfunc_begin0             ; >> Call Site 7 <<
	.uleb128 Ltmp9-Ltmp7                    ;   Call between Ltmp7 and Ltmp9
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp9-Lfunc_begin0             ; >> Call Site 8 <<
	.uleb128 Ltmp22-Ltmp9                   ;   Call between Ltmp9 and Ltmp22
	.uleb128 Ltmp23-Lfunc_begin0            ;     jumps to Ltmp23
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp24-Lfunc_begin0            ; >> Call Site 9 <<
	.uleb128 Ltmp43-Ltmp24                  ;   Call between Ltmp24 and Ltmp43
	.uleb128 Ltmp44-Lfunc_begin0            ;     jumps to Ltmp44
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp45-Lfunc_begin0            ; >> Call Site 10 <<
	.uleb128 Ltmp46-Ltmp45                  ;   Call between Ltmp45 and Ltmp46
	.uleb128 Ltmp61-Lfunc_begin0            ;     jumps to Ltmp61
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp47-Lfunc_begin0            ; >> Call Site 11 <<
	.uleb128 Ltmp48-Ltmp47                  ;   Call between Ltmp47 and Ltmp48
	.uleb128 Ltmp49-Lfunc_begin0            ;     jumps to Ltmp49
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp50-Lfunc_begin0            ; >> Call Site 12 <<
	.uleb128 Ltmp53-Ltmp50                  ;   Call between Ltmp50 and Ltmp53
	.uleb128 Ltmp54-Lfunc_begin0            ;     jumps to Ltmp54
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp55-Lfunc_begin0            ; >> Call Site 13 <<
	.uleb128 Ltmp60-Ltmp55                  ;   Call between Ltmp55 and Ltmp60
	.uleb128 Ltmp61-Lfunc_begin0            ;     jumps to Ltmp61
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp62-Lfunc_begin0            ; >> Call Site 14 <<
	.uleb128 Ltmp63-Ltmp62                  ;   Call between Ltmp62 and Ltmp63
	.uleb128 Ltmp64-Lfunc_begin0            ;     jumps to Ltmp64
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp65-Lfunc_begin0            ; >> Call Site 15 <<
	.uleb128 Ltmp68-Ltmp65                  ;   Call between Ltmp65 and Ltmp68
	.uleb128 Ltmp69-Lfunc_begin0            ;     jumps to Ltmp69
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp70-Lfunc_begin0            ; >> Call Site 16 <<
	.uleb128 Ltmp71-Ltmp70                  ;   Call between Ltmp70 and Ltmp71
	.uleb128 Ltmp72-Lfunc_begin0            ;     jumps to Ltmp72
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp73-Lfunc_begin0            ; >> Call Site 17 <<
	.uleb128 Ltmp74-Ltmp73                  ;   Call between Ltmp73 and Ltmp74
	.uleb128 Ltmp75-Lfunc_begin0            ;     jumps to Ltmp75
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp76-Lfunc_begin0            ; >> Call Site 18 <<
	.uleb128 Ltmp77-Ltmp76                  ;   Call between Ltmp76 and Ltmp77
	.uleb128 Ltmp78-Lfunc_begin0            ;     jumps to Ltmp78
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp79-Lfunc_begin0            ; >> Call Site 19 <<
	.uleb128 Ltmp80-Ltmp79                  ;   Call between Ltmp79 and Ltmp80
	.uleb128 Ltmp81-Lfunc_begin0            ;     jumps to Ltmp81
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp82-Lfunc_begin0            ; >> Call Site 20 <<
	.uleb128 Ltmp85-Ltmp82                  ;   Call between Ltmp82 and Ltmp85
	.uleb128 Ltmp86-Lfunc_begin0            ;     jumps to Ltmp86
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp87-Lfunc_begin0            ; >> Call Site 21 <<
	.uleb128 Ltmp88-Ltmp87                  ;   Call between Ltmp87 and Ltmp88
	.uleb128 Ltmp89-Lfunc_begin0            ;     jumps to Ltmp89
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp90-Lfunc_begin0            ; >> Call Site 22 <<
	.uleb128 Ltmp93-Ltmp90                  ;   Call between Ltmp90 and Ltmp93
	.uleb128 Ltmp94-Lfunc_begin0            ;     jumps to Ltmp94
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp95-Lfunc_begin0            ; >> Call Site 23 <<
	.uleb128 Ltmp96-Ltmp95                  ;   Call between Ltmp95 and Ltmp96
	.uleb128 Ltmp97-Lfunc_begin0            ;     jumps to Ltmp97
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp98-Lfunc_begin0            ; >> Call Site 24 <<
	.uleb128 Ltmp101-Ltmp98                 ;   Call between Ltmp98 and Ltmp101
	.uleb128 Ltmp102-Lfunc_begin0           ;     jumps to Ltmp102
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp103-Lfunc_begin0           ; >> Call Site 25 <<
	.uleb128 Ltmp104-Ltmp103                ;   Call between Ltmp103 and Ltmp104
	.uleb128 Ltmp105-Lfunc_begin0           ;     jumps to Ltmp105
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp106-Lfunc_begin0           ; >> Call Site 26 <<
	.uleb128 Ltmp107-Ltmp106                ;   Call between Ltmp106 and Ltmp107
	.uleb128 Ltmp108-Lfunc_begin0           ;     jumps to Ltmp108
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp109-Lfunc_begin0           ; >> Call Site 27 <<
	.uleb128 Ltmp112-Ltmp109                ;   Call between Ltmp109 and Ltmp112
	.uleb128 Ltmp113-Lfunc_begin0           ;     jumps to Ltmp113
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp114-Lfunc_begin0           ; >> Call Site 28 <<
	.uleb128 Ltmp143-Ltmp114                ;   Call between Ltmp114 and Ltmp143
	.uleb128 Ltmp144-Lfunc_begin0           ;     jumps to Ltmp144
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp143-Lfunc_begin0           ; >> Call Site 29 <<
	.uleb128 Lfunc_end0-Ltmp143             ;   Call between Ltmp143 and Lfunc_end0
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
Ltmp145:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne190102EPKc
Ltmp146:
; %bb.1:
Lloh98:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh99:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh100:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh101:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB5_2:
Ltmp147:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh100, Lloh101
	.loh AdrpLdrGot	Lloh98, Lloh99
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
	.uleb128 Ltmp145-Lfunc_begin1           ;   Call between Lfunc_begin1 and Ltmp145
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp145-Lfunc_begin1           ; >> Call Site 2 <<
	.uleb128 Ltmp146-Ltmp145                ;   Call between Ltmp145 and Ltmp146
	.uleb128 Ltmp147-Lfunc_begin1           ;     jumps to Ltmp147
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp146-Lfunc_begin1           ; >> Call Site 3 <<
	.uleb128 Lfunc_end1-Ltmp146             ;   Call between Ltmp146 and Lfunc_end1
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
Lloh102:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh103:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh102, Lloh103
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
Ltmp148:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp149:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB7_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w8, [x4, #144]
	cmn	w8, #1
	b.ne	LBB7_7
; %bb.3:
Ltmp151:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp152:
; %bb.4:
Ltmp153:
Lloh104:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh105:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp154:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp155:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp156:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB7_7:
	ldrsb	w5, [x4, #144]
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp158:
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp159:
; %bb.8:
	cbnz	x0, LBB7_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp161:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp162:
LBB7_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB7_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB7_12:
Ltmp163:
	b	LBB7_15
LBB7_13:
Ltmp157:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB7_16
LBB7_14:
Ltmp160:
LBB7_15:
	mov	x20, x0
LBB7_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB7_18
LBB7_17:
Ltmp150:
	mov	x20, x0
LBB7_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp164:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp165:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB7_11
LBB7_20:
Ltmp166:
	mov	x19, x0
Ltmp167:
	bl	___cxa_end_catch
Ltmp168:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB7_22:
Ltmp169:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh104, Lloh105
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table7:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Ltmp148-Lfunc_begin2           ; >> Call Site 1 <<
	.uleb128 Ltmp149-Ltmp148                ;   Call between Ltmp148 and Ltmp149
	.uleb128 Ltmp150-Lfunc_begin2           ;     jumps to Ltmp150
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp151-Lfunc_begin2           ; >> Call Site 2 <<
	.uleb128 Ltmp152-Ltmp151                ;   Call between Ltmp151 and Ltmp152
	.uleb128 Ltmp160-Lfunc_begin2           ;     jumps to Ltmp160
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp153-Lfunc_begin2           ; >> Call Site 3 <<
	.uleb128 Ltmp156-Ltmp153                ;   Call between Ltmp153 and Ltmp156
	.uleb128 Ltmp157-Lfunc_begin2           ;     jumps to Ltmp157
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp158-Lfunc_begin2           ; >> Call Site 4 <<
	.uleb128 Ltmp159-Ltmp158                ;   Call between Ltmp158 and Ltmp159
	.uleb128 Ltmp160-Lfunc_begin2           ;     jumps to Ltmp160
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp161-Lfunc_begin2           ; >> Call Site 5 <<
	.uleb128 Ltmp162-Ltmp161                ;   Call between Ltmp161 and Ltmp162
	.uleb128 Ltmp163-Lfunc_begin2           ;     jumps to Ltmp163
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp162-Lfunc_begin2           ; >> Call Site 6 <<
	.uleb128 Ltmp164-Ltmp162                ;   Call between Ltmp162 and Ltmp164
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp164-Lfunc_begin2           ; >> Call Site 7 <<
	.uleb128 Ltmp165-Ltmp164                ;   Call between Ltmp164 and Ltmp165
	.uleb128 Ltmp166-Lfunc_begin2           ;     jumps to Ltmp166
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp165-Lfunc_begin2           ; >> Call Site 8 <<
	.uleb128 Ltmp167-Ltmp165                ;   Call between Ltmp165 and Ltmp167
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp167-Lfunc_begin2           ; >> Call Site 9 <<
	.uleb128 Ltmp168-Ltmp167                ;   Call between Ltmp167 and Ltmp168
	.uleb128 Ltmp169-Lfunc_begin2           ;     jumps to Ltmp169
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp168-Lfunc_begin2           ; >> Call Site 10 <<
	.uleb128 Lfunc_end2-Ltmp168             ;   Call between Ltmp168 and Lfunc_end2
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
	cbz	x0, LBB8_16
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
	b.lt	LBB8_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB8_15
LBB8_3:
	cmp	x23, #1
	b.lt	LBB8_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB8_17
; %bb.5:
	cmp	x23, #22
	b.hi	LBB8_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB8_8
LBB8_7:
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
LBB8_8:
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
Ltmp170:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp171:
; %bb.9:
	cmp	x0, x23
	csel	x19, x19, xzr, eq
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB8_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB8_15
	b	LBB8_12
LBB8_11:
	ldr	x8, [sp, #8]
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPv
	mov	x0, x24
	cmp	x0, x23
	b.ne	LBB8_15
LBB8_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB8_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB8_15
LBB8_14:
	str	xzr, [x20, #24]
	b	LBB8_16
LBB8_15:
	mov	x19, #0                         ; =0x0
LBB8_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB8_17:
	add	x0, sp, #8
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
LBB8_18:
Ltmp172:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB8_20
; %bb.19:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB8_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table8:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Lfunc_begin3-Lfunc_begin3      ; >> Call Site 1 <<
	.uleb128 Ltmp170-Lfunc_begin3           ;   Call between Lfunc_begin3 and Ltmp170
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp170-Lfunc_begin3           ; >> Call Site 2 <<
	.uleb128 Ltmp171-Ltmp170                ;   Call between Ltmp170 and Ltmp171
	.uleb128 Ltmp172-Lfunc_begin3           ;     jumps to Ltmp172
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp171-Lfunc_begin3           ; >> Call Site 3 <<
	.uleb128 Lfunc_end3-Ltmp171             ;   Call between Ltmp171 and Lfunc_end3
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
Lloh106:
	adrp	x0, l_.str.34@PAGE
Lloh107:
	add	x0, x0, l_.str.34@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh106, Lloh107
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Correctness: sep2 max_err="

l_.str.1:                               ; @.str.1
	.asciz	"  sep4 max_err="

l_.str.2:                               ; @.str.2
	.asciz	"  "

l_.str.3:                               ; @.str.3
	.asciz	"PASS"

l_.str.4:                               ; @.str.4
	.asciz	"FAIL"

l_.str.5:                               ; @.str.5
	.asciz	"\n"

l_.str.6:                               ; @.str.6
	.asciz	"\nArray: "

l_.str.7:                               ; @.str.7
	.asciz	"M floats  ("

l_.str.8:                               ; @.str.8
	.asciz	" MB)\n"

l_.str.9:                               ; @.str.9
	.asciz	"FMUL latency (Apple M-series): "

l_.str.10:                              ; @.str.10
	.asciz	" cycles  \342\206\222  stall-free at K >= "

l_.str.11:                              ; @.str.11
	.asciz	" streams\n"

l_.str.12:                              ; @.str.12
	.asciz	"x[i]^3 + 1   N=32M floats  128 MB   -O1 (no SIMD)   CPU="

l_.str.13:                              ; @.str.13
	.asciz	" GHz\n"

l_.str.14:                              ; @.str.14
	.asciz	"version"

l_.str.15:                              ; @.str.15
	.asciz	"time"

l_.str.16:                              ; @.str.16
	.asciz	"cycles/elem"

l_.str.17:                              ; @.str.17
	.asciz	"CPI"

l_.str.18:                              ; @.str.18
	.asciz	"speedup"

l_.str.19:                              ; @.str.19
	.asciz	"naive (1 stream)"

l_.str.20:                              ; @.str.20
	.asciz	"sep2  (2 streams)"

l_.str.21:                              ; @.str.21
	.asciz	"sep4  (4 streams)"

	.section	__DATA,__const
	.p2align	3, 0x0                          ; @__const.main.labels
l___const.main.labels:
	.quad	l_.str.19
	.quad	l_.str.20
	.quad	l_.str.21

	.section	__TEXT,__cstring,cstring_literals
l_.str.22:                              ; @.str.22
	.asciz	" ms"

l_.str.23:                              ; @.str.23
	.asciz	"x\n"

l_.str.24:                              ; @.str.24
	.asciz	"\nTheoretical cycles per element:\n"

l_.str.25:                              ; @.str.25
	.asciz	"  naive: 3 ops + 2\303\227"

l_.str.26:                              ; @.str.26
	.asciz	" stall cycles = "

l_.str.27:                              ; @.str.27
	.asciz	" cycles/elem\n"

l_.str.28:                              ; @.str.28
	.asciz	"  sep2:  a1 fills 1 gap, 1 stall remains per hazard \342\206\222 ~"

l_.str.29:                              ; @.str.29
	.asciz	"  sep4:  "

l_.str.30:                              ; @.str.30
	.asciz	" streams fill each "

l_.str.31:                              ; @.str.31
	.asciz	"-cycle gap \342\206\222 ~3 cycles/elem (0 stalls)\n"

l_.str.32:                              ; @.str.32
	.asciz	"\nNote: at -O2/-O3 the compiler auto-vectorises (SIMD), masking\n      the scalar scheduling effect.  Build with -O1 to see it.\n"

	.section	__TEXT,__const
	.p2align	3, 0x0                          ; @_ZL5INSNS
__ZL5INSNS:
	.quad	0x4020000000000000              ; double 8
	.quad	0x401c000000000000              ; double 7
	.quad	0x4018000000000000              ; double 6

	.section	__TEXT,__cstring,cstring_literals
l_.str.34:                              ; @.str.34
	.asciz	"basic_string"

.subsections_via_symbols
