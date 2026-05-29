	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 5
	.globl	__Z10sum_serialPKdi             ; -- Begin function _Z10sum_serialPKdi
	.p2align	2
__Z10sum_serialPKdi:                    ; @_Z10sum_serialPKdi
	.cfi_startproc
; %bb.0:
	cmp	w1, #1
	b.lt	LBB0_4
; %bb.1:
	mov	w8, w1
	movi	d0, #0000000000000000
LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
	ldr	d1, [x0], #8
	fadd	d0, d0, d1
	subs	x8, x8, #1
	b.ne	LBB0_2
; %bb.3:
	ret
LBB0_4:
	movi	d0, #0000000000000000
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z8sum_2accPKdi                ; -- Begin function _Z8sum_2accPKdi
	.p2align	2
__Z8sum_2accPKdi:                       ; @_Z8sum_2accPKdi
	.cfi_startproc
; %bb.0:
	and	w8, w1, #0x1
	cmp	w1, #0
	cneg	w8, w8, lt
	sub	w9, w1, w8
	cmp	w9, #1
	b.lt	LBB1_6
; %bb.1:
	mov	x10, #0                         ; =0x0
	add	x11, x0, #8
	movi	d0, #0000000000000000
	movi	d1, #0000000000000000
LBB1_2:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d2, d3, [x11, #-8]
	fadd	d0, d0, d2
	fadd	d1, d1, d3
	add	x10, x10, #2
	add	x11, x11, #16
	cmp	x10, x9
	b.lo	LBB1_2
; %bb.3:
	cmp	w8, #0
	b.le	LBB1_5
LBB1_4:
	add	x9, x0, w1, sxtw #3
	neg	w8, w8
	ldr	d2, [x9, w8, sxtw #3]
	fadd	d0, d0, d2
LBB1_5:
	fadd	d0, d1, d0
	ret
LBB1_6:
	movi	d1, #0000000000000000
	movi	d0, #0000000000000000
	cmp	w8, #0
	b.gt	LBB1_4
	b	LBB1_5
	.cfi_endproc
                                        ; -- End function
	.globl	__Z8sum_4accPKdi                ; -- Begin function _Z8sum_4accPKdi
	.p2align	2
__Z8sum_4accPKdi:                       ; @_Z8sum_4accPKdi
	.cfi_startproc
; %bb.0:
                                        ; kill: def $w1 killed $w1 def $x1
	negs	w8, w1
	and	w8, w8, #0x3
	and	w9, w1, #0x3
	csneg	w8, w9, w8, mi
	sub	w9, w1, w8
	cmp	w9, #1
	b.lt	LBB2_4
; %bb.1:
	mov	x10, #0                         ; =0x0
	add	x11, x0, #16
	movi	d0, #0000000000000000
	movi	d1, #0000000000000000
	movi	d2, #0000000000000000
	movi	d3, #0000000000000000
LBB2_2:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d4, d5, [x11, #-16]
	fadd	d0, d0, d4
	fadd	d3, d3, d5
	ldp	d4, d5, [x11], #32
	fadd	d2, d2, d4
	fadd	d1, d1, d5
	add	x10, x10, #4
	cmp	x10, x9
	b.lo	LBB2_2
; %bb.3:
	cmp	w8, #1
	b.ge	LBB2_5
	b	LBB2_7
LBB2_4:
	movi	d3, #0000000000000000
	movi	d2, #0000000000000000
	movi	d1, #0000000000000000
	movi	d0, #0000000000000000
	cmp	w8, #1
	b.lt	LBB2_7
LBB2_5:
	sxtw	x9, w1
	neg	w8, w8
	add	x8, x9, w8, sxtw
LBB2_6:                                 ; =>This Inner Loop Header: Depth=1
	ldr	d4, [x0, x8, lsl #3]
	fadd	d0, d0, d4
	add	x8, x8, #1
	cmp	x8, x9
	b.lt	LBB2_6
LBB2_7:
	fadd	d0, d3, d0
	fadd	d0, d2, d0
	fadd	d0, d1, d0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z8sum_8accPKdi                ; -- Begin function _Z8sum_8accPKdi
	.p2align	2
__Z8sum_8accPKdi:                       ; @_Z8sum_8accPKdi
	.cfi_startproc
; %bb.0:
                                        ; kill: def $w1 killed $w1 def $x1
	negs	w8, w1
	and	w8, w8, #0x7
	and	w9, w1, #0x7
	csneg	w8, w9, w8, mi
	sub	w9, w1, w8
	cmp	w9, #1
	b.lt	LBB3_4
; %bb.1:
	mov	x10, #0                         ; =0x0
	movi	d0, #0000000000000000
	movi	d1, #0000000000000000
	add	x11, x0, #32
	movi	d2, #0000000000000000
	movi	d3, #0000000000000000
	movi	d4, #0000000000000000
	movi	d5, #0000000000000000
	movi	d6, #0000000000000000
	movi	d7, #0000000000000000
LBB3_2:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d16, d17, [x11, #-32]
	fadd	d0, d0, d16
	fadd	d7, d7, d17
	ldp	d16, d17, [x11, #-16]
	fadd	d6, d6, d16
	fadd	d5, d5, d17
	ldp	d16, d17, [x11]
	fadd	d4, d4, d16
	fadd	d3, d3, d17
	ldp	d16, d17, [x11, #16]
	fadd	d2, d2, d16
	fadd	d1, d1, d17
	add	x10, x10, #8
	add	x11, x11, #64
	cmp	x10, x9
	b.lo	LBB3_2
; %bb.3:
	cmp	w8, #1
	b.ge	LBB3_5
	b	LBB3_7
LBB3_4:
	movi	d7, #0000000000000000
	movi	d6, #0000000000000000
	movi	d5, #0000000000000000
	movi	d4, #0000000000000000
	movi	d3, #0000000000000000
	movi	d2, #0000000000000000
	movi	d1, #0000000000000000
	movi	d0, #0000000000000000
	cmp	w8, #1
	b.lt	LBB3_7
LBB3_5:
	sxtw	x9, w1
	neg	w8, w8
	add	x8, x9, w8, sxtw
LBB3_6:                                 ; =>This Inner Loop Header: Depth=1
	ldr	d16, [x0, x8, lsl #3]
	fadd	d0, d0, d16
	add	x8, x8, #1
	cmp	x8, x9
	b.lt	LBB3_6
LBB3_7:
	fadd	d0, d7, d0
	fadd	d0, d6, d0
	fadd	d0, d5, d0
	fadd	d0, d4, d0
	fadd	d0, d3, d0
	fadd	d0, d2, d0
	fadd	d0, d1, d0
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
Lloh0:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh2:
	ldr	x8, [x8]
	str	x8, [sp, #40]
	mov	w0, #536870912                  ; =0x20000000
	bl	__Znwm
	mov	x28, x0
	mov	w1, #536870912                  ; =0x20000000
	bl	_bzero
	mov	w8, #0                          ; =0x0
	mov	w9, #67108864                   ; =0x4000000
	mov	w10, #19923                     ; =0x4dd3
	movk	w10, #4194, lsl #16
	mov	w11, #1000                      ; =0x3e8
	mov	x12, #43516                     ; =0xa9fc
	movk	x12, #54001, lsl #16
	movk	x12, #25165, lsl #32
	movk	x12, #16208, lsl #48
	fmov	d0, x12
	fmov	d1, #1.00000000
	mov	x12, x28
LBB4_1:                                 ; =>This Inner Loop Header: Depth=1
	umull	x13, w8, w10
	lsr	x13, x13, #38
	msub	w13, w13, w11, w8
	ucvtf	d2, w13
	fmadd	d2, d2, d0, d1
	str	d2, [x12], #8
	add	w8, w8, #1
	subs	x9, x9, #1
	b.ne	LBB4_1
; %bb.2:
	movi	d0, #0000000000000000
	mov	w8, #67108864                   ; =0x4000000
	mov	x9, x28
LBB4_3:                                 ; =>This Inner Loop Header: Depth=1
	ldr	d1, [x9], #8
	fadd	d0, d0, d1
	subs	x8, x8, #1
	b.ne	LBB4_3
; %bb.4:
	add	x8, x28, #8
	movi	d1, #0000000000000000
	mov	x9, #-2                         ; =0xfffffffffffffffe
	mov	w10, #67108862                  ; =0x3fffffe
	movi	d2, #0000000000000000
LBB4_5:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d3, d4, [x8, #-8]
	fadd	d1, d1, d3
	fadd	d2, d2, d4
	add	x9, x9, #2
	add	x8, x8, #16
	cmp	x9, x10
	b.lo	LBB4_5
; %bb.6:
	add	x8, x28, #16
	movi	d4, #0000000000000000
	mov	x9, #-4                         ; =0xfffffffffffffffc
	mov	w10, #67108860                  ; =0x3fffffc
	movi	d3, #0000000000000000
	movi	d5, #0000000000000000
	movi	d6, #0000000000000000
LBB4_7:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d7, d16, [x8, #-16]
	fadd	d4, d4, d7
	fadd	d6, d6, d16
	ldp	d7, d16, [x8], #32
	fadd	d5, d5, d7
	fadd	d3, d3, d16
	add	x9, x9, #4
	cmp	x9, x10
	b.lo	LBB4_7
; %bb.8:
	fadd	d4, d4, d6
	fadd	d4, d5, d4
	add	x8, x28, #32
	movi	d5, #0000000000000000
	mov	x9, #-8                         ; =0xfffffffffffffff8
	mov	w10, #67108856                  ; =0x3fffff8
	movi	d6, #0000000000000000
	movi	d7, #0000000000000000
	movi	d16, #0000000000000000
	movi	d17, #0000000000000000
	movi	d18, #0000000000000000
	movi	d19, #0000000000000000
	movi	d20, #0000000000000000
LBB4_9:                                 ; =>This Inner Loop Header: Depth=1
	ldp	d21, d22, [x8, #-32]
	fadd	d5, d5, d21
	fadd	d20, d20, d22
	ldp	d21, d22, [x8, #-16]
	fadd	d19, d19, d21
	fadd	d18, d18, d22
	ldp	d21, d22, [x8]
	fadd	d17, d17, d21
	fadd	d16, d16, d22
	ldp	d21, d22, [x8, #16]
	fadd	d7, d7, d21
	fadd	d6, d6, d22
	add	x9, x9, #8
	add	x8, x8, #64
	cmp	x9, x10
	b.lo	LBB4_9
; %bb.10:
	fadd	d1, d1, d2
	fadd	d2, d3, d4
	fadd	d3, d5, d20
	fadd	d3, d19, d3
	fadd	d3, d18, d3
	fadd	d3, d17, d3
	fadd	d3, d16, d3
	fadd	d3, d7, d3
	fadd	d3, d6, d3
	fabd	d1, d0, d1
	fabd	d2, d0, d2
	stp	d1, d2, [sp, #16]
	fabd	d0, d0, d3
	str	d0, [sp, #32]
	add	x8, sp, #16
	mov	w9, #8                          ; =0x8
	add	x10, sp, #16
LBB4_11:                                ; =>This Inner Loop Header: Depth=1
	add	x11, x8, x9
	ldr	d0, [x10]
	ldr	d1, [x11]
	fcmp	d0, d1
	csel	x10, x11, x10, mi
	add	x9, x9, #8
	cmp	x9, #24
	b.ne	LBB4_11
; %bb.12:
	ldr	d8, [x10]
Ltmp0:
Lloh3:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh4:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh5:
	adrp	x1, l_.str@PAGE
Lloh6:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #29                         ; =0x1d
	str	x28, [sp, #8]                   ; 8-byte Folded Spill
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp1:
; %bb.13:
	ldr	x8, [x0]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	ldr	w9, [x8, #8]
	and	w9, w9, #0xfffffffb
	orr	w9, w9, #0x100
	str	w9, [x8, #8]
Ltmp2:
	fmov	d0, d8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp3:
; %bb.14:
Ltmp4:
Lloh7:
	adrp	x1, l_.str.1@PAGE
Lloh8:
	add	x1, x1, l_.str.1@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp5:
; %bb.15:
Lloh9:
	adrp	x8, l_.str.3@PAGE
Lloh10:
	add	x8, x8, l_.str.3@PAGEOFF
Lloh11:
	adrp	x9, l_.str.2@PAGE
Lloh12:
	add	x9, x9, l_.str.2@PAGEOFF
	fmov	d0, #1.00000000
	fcmp	d8, d0
	csel	x1, x9, x8, mi
Ltmp6:
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp7:
; %bb.16:
Ltmp8:
Lloh13:
	adrp	x1, l_.str.4@PAGE
Lloh14:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp9:
; %bb.17:
	ldr	x8, [x0]
	ldur	x8, [x8, #-24]
	add	x8, x0, x8
	ldr	w9, [x8, #8]
	mov	w10, #-261                      ; =0xfffffefb
	and	w9, w9, w10
	str	w9, [x8, #8]
Ltmp11:
Lloh15:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh16:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh17:
	adrp	x1, l_.str.5@PAGE
Lloh18:
	add	x1, x1, l_.str.5@PAGEOFF
	mov	w2, #9                          ; =0x9
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp12:
; %bb.18:
Ltmp13:
	mov	w1, #64                         ; =0x40
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp14:
; %bb.19:
Ltmp15:
Lloh19:
	adrp	x1, l_.str.6@PAGE
Lloh20:
	add	x1, x1, l_.str.6@PAGEOFF
	mov	w2, #12                         ; =0xc
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp16:
; %bb.20:
Ltmp17:
	mov	w1, #512                        ; =0x200
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp18:
; %bb.21:
Ltmp19:
Lloh21:
	adrp	x1, l_.str.7@PAGE
Lloh22:
	add	x1, x1, l_.str.7@PAGEOFF
	mov	w2, #5                          ; =0x5
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp20:
; %bb.22:
Ltmp21:
Lloh23:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh24:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh25:
	adrp	x1, l_.str.8@PAGE
Lloh26:
	add	x1, x1, l_.str.8@PAGEOFF
	mov	w2, #31                         ; =0x1f
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp22:
; %bb.23:
Ltmp23:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp24:
; %bb.24:
Ltmp25:
Lloh27:
	adrp	x1, l_.str.9@PAGE
Lloh28:
	add	x1, x1, l_.str.9@PAGEOFF
	mov	w2, #36                         ; =0x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp26:
; %bb.25:
Ltmp27:
	mov	w1, #3                          ; =0x3
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEi
Ltmp28:
; %bb.26:
Ltmp29:
Lloh29:
	adrp	x1, l_.str.10@PAGE
Lloh30:
	add	x1, x1, l_.str.10@PAGEOFF
	mov	w2, #14                         ; =0xe
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp30:
; %bb.27:
	mov	w19, #0                         ; =0x0
	mov	x23, #9223372036854775807       ; =0x7fffffffffffffff
	add	x21, sp, #16
	mov	x25, #13531                     ; =0x34db
	movk	x25, #55222, lsl #16
	movk	x25, #56962, lsl #32
	movk	x25, #17179, lsl #48
LBB4_28:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB4_29 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	movi	d0, #0000000000000000
	mov	x8, x28
	mov	w9, #67108864                   ; =0x4000000
LBB4_29:                                ;   Parent Loop BB4_28 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	d1, [x8], #8
	fadd	d0, d0, d1
	subs	x9, x9, #1
	b.ne	LBB4_29
; %bb.30:                               ;   in Loop: Header=BB4_28 Depth=1
	str	d0, [sp, #16]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	smulh	x8, x8, x25
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x23
	csel	x23, x8, x23, lt
	add	w19, w19, #1
	cmp	w19, #5
	b.ne	LBB4_28
; %bb.31:
	mov	w19, #0                         ; =0x0
	add	x21, x28, #8
	mov	x22, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w24, #67108862                  ; =0x3fffffe
	add	x26, sp, #16
LBB4_32:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB4_33 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	movi	d0, #0000000000000000
	mov	x8, #-2                         ; =0xfffffffffffffffe
	mov	x9, x21
	movi	d1, #0000000000000000
LBB4_33:                                ;   Parent Loop BB4_32 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	d2, d3, [x9, #-8]
	fadd	d0, d0, d2
	fadd	d1, d1, d3
	add	x8, x8, #2
	add	x9, x9, #16
	cmp	x8, x24
	b.lo	LBB4_33
; %bb.34:                               ;   in Loop: Header=BB4_32 Depth=1
	fadd	d0, d0, d1
	str	d0, [sp, #16]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	smulh	x8, x8, x25
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x22
	csel	x22, x8, x22, lt
	add	w19, w19, #1
	cmp	w19, #5
	b.ne	LBB4_32
; %bb.35:
	mov	w19, #0                         ; =0x0
	add	x24, x28, #16
	mov	x21, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w26, #67108860                  ; =0x3fffffc
	add	x27, sp, #16
LBB4_36:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB4_37 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	movi	d0, #0000000000000000
	mov	x8, #-4                         ; =0xfffffffffffffffc
	mov	x9, x24
	movi	d1, #0000000000000000
	movi	d2, #0000000000000000
	movi	d3, #0000000000000000
LBB4_37:                                ;   Parent Loop BB4_36 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	d4, d5, [x9, #-16]
	fadd	d0, d0, d4
	fadd	d3, d3, d5
	ldp	d4, d5, [x9], #32
	fadd	d2, d2, d4
	fadd	d1, d1, d5
	add	x8, x8, #4
	cmp	x8, x26
	b.lo	LBB4_37
; %bb.38:                               ;   in Loop: Header=BB4_36 Depth=1
	fadd	d0, d0, d3
	fadd	d0, d2, d0
	fadd	d0, d1, d0
	str	d0, [sp, #16]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	smulh	x8, x8, x25
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x21
	csel	x21, x8, x21, lt
	add	w19, w19, #1
	cmp	w19, #5
	b.ne	LBB4_36
; %bb.39:
	mov	w26, #0                         ; =0x0
	add	x27, x28, #32
	mov	x20, #9223372036854775807       ; =0x7fffffffffffffff
	mov	w28, #67108856                  ; =0x3fffff8
	add	x19, sp, #16
LBB4_40:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB4_41 Depth 2
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x24, x0
	movi	d0, #0000000000000000
	movi	d1, #0000000000000000
	mov	x8, #-8                         ; =0xfffffffffffffff8
	mov	x9, x27
	movi	d2, #0000000000000000
	movi	d3, #0000000000000000
	movi	d4, #0000000000000000
	movi	d5, #0000000000000000
	movi	d6, #0000000000000000
	movi	d7, #0000000000000000
LBB4_41:                                ;   Parent Loop BB4_40 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	d16, d17, [x9, #-32]
	fadd	d0, d0, d16
	fadd	d7, d7, d17
	ldp	d16, d17, [x9, #-16]
	fadd	d6, d6, d16
	fadd	d5, d5, d17
	ldp	d16, d17, [x9]
	fadd	d4, d4, d16
	fadd	d3, d3, d17
	ldp	d16, d17, [x9, #16]
	fadd	d2, d2, d16
	fadd	d1, d1, d17
	add	x8, x8, #8
	add	x9, x9, #64
	cmp	x8, x28
	b.lo	LBB4_41
; %bb.42:                               ;   in Loop: Header=BB4_40 Depth=1
	fadd	d0, d0, d7
	fadd	d0, d6, d0
	fadd	d0, d5, d0
	fadd	d0, d4, d0
	fadd	d0, d3, d0
	fadd	d0, d2, d0
	fadd	d0, d1, d0
	str	d0, [sp, #16]
	; InlineAsm Start
	; InlineAsm End
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x24
	smulh	x8, x8, x25
	asr	x9, x8, #18
	add	x8, x9, x8, lsr #63
	cmp	x8, x20
	csel	x20, x8, x20, lt
	add	w26, w26, #1
	cmp	w26, #5
	b.ne	LBB4_40
; %bb.43:
Ltmp32:
Lloh31:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh32:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh33:
	adrp	x1, l_.str.4@PAGE
Lloh34:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	ldr	x19, [sp, #8]                   ; 8-byte Folded Reload
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp33:
; %bb.44:
Ltmp34:
	mov	x24, x0
	mov	w0, #64                         ; =0x40
	bl	__Znwm
Ltmp35:
; %bb.45:
	mov	x25, #64                        ; =0x40
	movk	x25, #32768, lsl #48
	mov	w8, #60                         ; =0x3c
	str	x0, [sp, #16]
	stp	x8, x25, [sp, #24]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	str	q0, [x0, #32]
	stur	q0, [x0, #44]
	strb	wzr, [x0, #60]
	ldrsb	w9, [sp, #39]
	add	x10, sp, #16
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp37:
	and	x2, x8, #0xff
	mov	x0, x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp38:
; %bb.46:
Ltmp39:
Lloh35:
	adrp	x1, l_.str.4@PAGE
Lloh36:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp40:
; %bb.47:
	ldrsb	w8, [sp, #39]
	tbz	w8, #31, LBB4_49
; %bb.48:
	ldr	x0, [sp, #16]
	bl	__ZdlPv
LBB4_49:
Ltmp42:
Lloh37:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh38:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh39:
	adrp	x1, l_.str.11@PAGE
Lloh40:
	add	x1, x1, l_.str.11@PAGEOFF
	mov	w2, #49                         ; =0x31
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp43:
; %bb.50:
Ltmp45:
	mov	w0, #64                         ; =0x40
	bl	__Znwm
Ltmp46:
; %bb.51:
	mov	w8, #60                         ; =0x3c
	str	x0, [sp, #16]
	stp	x8, x25, [sp, #24]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	str	q0, [x0, #32]
	stur	q0, [x0, #44]
	strb	wzr, [x0, #60]
	ldrsb	w9, [sp, #39]
	add	x10, sp, #16
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp48:
Lloh41:
	adrp	x24, __ZNSt3__14coutE@GOTPAGE
Lloh42:
	ldr	x24, [x24, __ZNSt3__14coutE@GOTPAGEOFF]
	and	x2, x8, #0xff
	mov	x0, x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp49:
; %bb.52:
Ltmp50:
Lloh43:
	adrp	x1, l_.str.4@PAGE
Lloh44:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp51:
; %bb.53:
	ldrsb	w8, [sp, #39]
	tbz	w8, #31, LBB4_55
; %bb.54:
	ldr	x0, [sp, #16]
	bl	__ZdlPv
LBB4_55:
	ldr	x8, [x24]
	ldur	x9, [x8, #-24]
	add	x9, x24, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x24, x8
	mov	w9, #28                         ; =0x1c
	str	x9, [x8, #24]
Ltmp53:
Lloh45:
	adrp	x1, l_.str.12@PAGE
Lloh46:
	add	x1, x1, l_.str.12@PAGEOFF
	mov	x0, x24
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp54:
; %bb.56:
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
Ltmp56:
Lloh47:
	adrp	x1, l_.str.13@PAGE
Lloh48:
	add	x1, x1, l_.str.13@PAGEOFF
	mov	w2, #4                          ; =0x4
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp57:
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
	mov	w9, #10                         ; =0xa
	str	x9, [x8, #24]
Ltmp59:
Lloh49:
	adrp	x1, l_.str.14@PAGE
Lloh50:
	add	x1, x1, l_.str.14@PAGEOFF
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp60:
; %bb.58:
Ltmp61:
Lloh51:
	adrp	x1, l_.str.4@PAGE
Lloh52:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp62:
; %bb.59:
Ltmp64:
	mov	w0, #64                         ; =0x40
	bl	__Znwm
Ltmp65:
; %bb.60:
	mov	w8, #60                         ; =0x3c
	str	x0, [sp, #16]
	stp	x8, x25, [sp, #24]
	movi.16b	v0, #45
	stp	q0, q0, [x0]
	str	q0, [x0, #32]
	stur	q0, [x0, #44]
	strb	wzr, [x0, #60]
	ldrsb	w9, [sp, #39]
	add	x10, sp, #16
	cmp	w9, #0
	csel	w8, w8, w9, lt
	csel	x1, x0, x10, lt
Ltmp67:
	and	x2, x8, #0xff
	mov	x0, x24
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp68:
; %bb.61:
Ltmp69:
Lloh53:
	adrp	x1, l_.str.4@PAGE
Lloh54:
	add	x1, x1, l_.str.4@PAGEOFF
	mov	w2, #1                          ; =0x1
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp70:
; %bb.62:
	ldrsb	w8, [sp, #39]
	tbz	w8, #31, LBB4_64
; %bb.63:
	ldr	x0, [sp, #16]
	bl	__ZdlPv
LBB4_64:
	ldr	x8, [x24]
	ldur	x9, [x8, #-24]
	add	x9, x24, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x24, x8
	mov	w9, #28                         ; =0x1c
	str	x9, [x8, #24]
Ltmp72:
Lloh55:
	adrp	x1, l_.str.15@PAGE
Lloh56:
	add	x1, x1, l_.str.15@PAGEOFF
	mov	x0, x24
	mov	w2, #22                         ; =0x16
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp73:
; %bb.65:
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
Ltmp74:
	mov	x1, x23
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp75:
; %bb.66:
Ltmp76:
Lloh57:
	adrp	x1, l_.str.20@PAGE
Lloh58:
	add	x1, x1, l_.str.20@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp77:
; %bb.67:
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
	mov	w9, #2                          ; =0x2
	scvtf	d8, x23
	str	x9, [x8, #16]
	cmp	x23, #1
	csinc	x8, x23, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp78:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp79:
; %bb.68:
Ltmp80:
Lloh59:
	adrp	x1, l_.str.21@PAGE
Lloh60:
	add	x1, x1, l_.str.21@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp81:
; %bb.69:
	ldr	x8, [x24]
	ldur	x9, [x8, #-24]
	add	x9, x24, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x24, x8
	mov	w9, #28                         ; =0x1c
	str	x9, [x8, #24]
Ltmp82:
Lloh61:
	adrp	x1, l_.str.16@PAGE
Lloh62:
	add	x1, x1, l_.str.16@PAGEOFF
	mov	x0, x24
	mov	w2, #14                         ; =0xe
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp83:
; %bb.70:
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
Ltmp84:
	mov	x1, x22
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp85:
; %bb.71:
Ltmp86:
Lloh63:
	adrp	x1, l_.str.20@PAGE
Lloh64:
	add	x1, x1, l_.str.20@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp87:
; %bb.72:
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
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	cmp	x22, #1
	csinc	x8, x22, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp88:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp89:
; %bb.73:
Ltmp90:
Lloh65:
	adrp	x1, l_.str.21@PAGE
Lloh66:
	add	x1, x1, l_.str.21@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp91:
; %bb.74:
	ldr	x8, [x24]
	ldur	x9, [x8, #-24]
	add	x9, x24, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x24, x8
	mov	w9, #28                         ; =0x1c
	str	x9, [x8, #24]
Ltmp92:
Lloh67:
	adrp	x1, l_.str.17@PAGE
Lloh68:
	add	x1, x1, l_.str.17@PAGEOFF
	mov	x0, x24
	mov	w2, #14                         ; =0xe
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp93:
; %bb.75:
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
Ltmp94:
	mov	x1, x21
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp95:
; %bb.76:
Ltmp96:
Lloh69:
	adrp	x1, l_.str.20@PAGE
Lloh70:
	add	x1, x1, l_.str.20@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp97:
; %bb.77:
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
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	cmp	x21, #1
	csinc	x8, x21, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp98:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp99:
; %bb.78:
Ltmp100:
Lloh71:
	adrp	x1, l_.str.21@PAGE
Lloh72:
	add	x1, x1, l_.str.21@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp101:
; %bb.79:
	ldr	x8, [x24]
	ldur	x9, [x8, #-24]
	add	x9, x24, x9
	ldr	w10, [x9, #8]
	mov	w11, #-177                      ; =0xffffff4f
	and	w10, w10, w11
	orr	w10, w10, #0x20
	str	w10, [x9, #8]
	ldur	x8, [x8, #-24]
	add	x8, x24, x8
	mov	w9, #28                         ; =0x1c
	str	x9, [x8, #24]
Ltmp102:
Lloh73:
	adrp	x1, l_.str.18@PAGE
Lloh74:
	add	x1, x1, l_.str.18@PAGEOFF
	mov	x0, x24
	mov	w2, #14                         ; =0xe
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp103:
; %bb.80:
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
Ltmp104:
	mov	x1, x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Ltmp105:
; %bb.81:
Ltmp106:
Lloh75:
	adrp	x1, l_.str.20@PAGE
Lloh76:
	add	x1, x1, l_.str.20@PAGEOFF
	mov	w2, #3                          ; =0x3
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp107:
; %bb.82:
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
	mov	w9, #2                          ; =0x2
	str	x9, [x8, #16]
	cmp	x20, #1
	csinc	x8, x20, xzr, gt
	ucvtf	d0, x8
	fdiv	d0, d8, d0
Ltmp108:
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEd
Ltmp109:
; %bb.83:
Ltmp110:
Lloh77:
	adrp	x1, l_.str.21@PAGE
Lloh78:
	add	x1, x1, l_.str.21@PAGEOFF
	mov	w2, #2                          ; =0x2
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp111:
; %bb.84:
Ltmp112:
Lloh79:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh80:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh81:
	adrp	x1, l_.str.19@PAGE
Lloh82:
	add	x1, x1, l_.str.19@PAGEOFF
	mov	w2, #120                        ; =0x78
	bl	__ZNSt3__124__put_character_sequenceB8ne190102IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp113:
; %bb.85:
	mov	x0, x19
	bl	__ZdlPv
	ldr	x8, [sp, #40]
Lloh83:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh84:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh85:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB4_87
; %bb.86:
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
LBB4_87:
	bl	___stack_chk_fail
LBB4_88:
Ltmp66:
	b	LBB4_103
LBB4_89:
Ltmp58:
	b	LBB4_103
LBB4_90:
Ltmp55:
	b	LBB4_103
LBB4_91:
Ltmp47:
	b	LBB4_103
LBB4_92:
Ltmp36:
	b	LBB4_103
LBB4_93:
Ltmp71:
	b	LBB4_97
LBB4_94:
Ltmp63:
	b	LBB4_103
LBB4_95:
Ltmp52:
	b	LBB4_97
LBB4_96:
Ltmp41:
LBB4_97:
	mov	x20, x0
	ldrsb	w8, [sp, #39]
	tbz	w8, #31, LBB4_104
; %bb.98:
	ldr	x0, [sp, #16]
	bl	__ZdlPv
	b	LBB4_104
LBB4_99:
Ltmp44:
	b	LBB4_103
LBB4_100:
Ltmp10:
	b	LBB4_103
LBB4_101:
Ltmp31:
	b	LBB4_103
LBB4_102:
Ltmp114:
LBB4_103:
	mov	x20, x0
LBB4_104:
	ldr	x0, [sp, #8]                    ; 8-byte Folded Reload
	bl	__ZdlPv
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGotLdr	Lloh0, Lloh1, Lloh2
	.loh AdrpAdd	Lloh5, Lloh6
	.loh AdrpLdrGot	Lloh3, Lloh4
	.loh AdrpAdd	Lloh7, Lloh8
	.loh AdrpAdd	Lloh11, Lloh12
	.loh AdrpAdd	Lloh9, Lloh10
	.loh AdrpAdd	Lloh13, Lloh14
	.loh AdrpAdd	Lloh17, Lloh18
	.loh AdrpLdrGot	Lloh15, Lloh16
	.loh AdrpAdd	Lloh19, Lloh20
	.loh AdrpAdd	Lloh21, Lloh22
	.loh AdrpAdd	Lloh25, Lloh26
	.loh AdrpLdrGot	Lloh23, Lloh24
	.loh AdrpAdd	Lloh27, Lloh28
	.loh AdrpAdd	Lloh29, Lloh30
	.loh AdrpAdd	Lloh33, Lloh34
	.loh AdrpLdrGot	Lloh31, Lloh32
	.loh AdrpAdd	Lloh35, Lloh36
	.loh AdrpAdd	Lloh39, Lloh40
	.loh AdrpLdrGot	Lloh37, Lloh38
	.loh AdrpLdrGot	Lloh41, Lloh42
	.loh AdrpAdd	Lloh43, Lloh44
	.loh AdrpAdd	Lloh45, Lloh46
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
	.loh AdrpAdd	Lloh71, Lloh72
	.loh AdrpAdd	Lloh73, Lloh74
	.loh AdrpAdd	Lloh75, Lloh76
	.loh AdrpAdd	Lloh77, Lloh78
	.loh AdrpAdd	Lloh81, Lloh82
	.loh AdrpLdrGot	Lloh79, Lloh80
	.loh AdrpLdrGotLdr	Lloh83, Lloh84, Lloh85
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table4:
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
	.uleb128 Ltmp9-Ltmp0                    ;   Call between Ltmp0 and Ltmp9
	.uleb128 Ltmp10-Lfunc_begin0            ;     jumps to Ltmp10
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp11-Lfunc_begin0            ; >> Call Site 3 <<
	.uleb128 Ltmp30-Ltmp11                  ;   Call between Ltmp11 and Ltmp30
	.uleb128 Ltmp31-Lfunc_begin0            ;     jumps to Ltmp31
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp32-Lfunc_begin0            ; >> Call Site 4 <<
	.uleb128 Ltmp33-Ltmp32                  ;   Call between Ltmp32 and Ltmp33
	.uleb128 Ltmp44-Lfunc_begin0            ;     jumps to Ltmp44
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp34-Lfunc_begin0            ; >> Call Site 5 <<
	.uleb128 Ltmp35-Ltmp34                  ;   Call between Ltmp34 and Ltmp35
	.uleb128 Ltmp36-Lfunc_begin0            ;     jumps to Ltmp36
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp37-Lfunc_begin0            ; >> Call Site 6 <<
	.uleb128 Ltmp40-Ltmp37                  ;   Call between Ltmp37 and Ltmp40
	.uleb128 Ltmp41-Lfunc_begin0            ;     jumps to Ltmp41
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp42-Lfunc_begin0            ; >> Call Site 7 <<
	.uleb128 Ltmp43-Ltmp42                  ;   Call between Ltmp42 and Ltmp43
	.uleb128 Ltmp44-Lfunc_begin0            ;     jumps to Ltmp44
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp45-Lfunc_begin0            ; >> Call Site 8 <<
	.uleb128 Ltmp46-Ltmp45                  ;   Call between Ltmp45 and Ltmp46
	.uleb128 Ltmp47-Lfunc_begin0            ;     jumps to Ltmp47
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp48-Lfunc_begin0            ; >> Call Site 9 <<
	.uleb128 Ltmp51-Ltmp48                  ;   Call between Ltmp48 and Ltmp51
	.uleb128 Ltmp52-Lfunc_begin0            ;     jumps to Ltmp52
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp53-Lfunc_begin0            ; >> Call Site 10 <<
	.uleb128 Ltmp54-Ltmp53                  ;   Call between Ltmp53 and Ltmp54
	.uleb128 Ltmp55-Lfunc_begin0            ;     jumps to Ltmp55
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp56-Lfunc_begin0            ; >> Call Site 11 <<
	.uleb128 Ltmp57-Ltmp56                  ;   Call between Ltmp56 and Ltmp57
	.uleb128 Ltmp58-Lfunc_begin0            ;     jumps to Ltmp58
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp59-Lfunc_begin0            ; >> Call Site 12 <<
	.uleb128 Ltmp62-Ltmp59                  ;   Call between Ltmp59 and Ltmp62
	.uleb128 Ltmp63-Lfunc_begin0            ;     jumps to Ltmp63
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp64-Lfunc_begin0            ; >> Call Site 13 <<
	.uleb128 Ltmp65-Ltmp64                  ;   Call between Ltmp64 and Ltmp65
	.uleb128 Ltmp66-Lfunc_begin0            ;     jumps to Ltmp66
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp67-Lfunc_begin0            ; >> Call Site 14 <<
	.uleb128 Ltmp70-Ltmp67                  ;   Call between Ltmp67 and Ltmp70
	.uleb128 Ltmp71-Lfunc_begin0            ;     jumps to Ltmp71
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp72-Lfunc_begin0            ; >> Call Site 15 <<
	.uleb128 Ltmp113-Ltmp72                 ;   Call between Ltmp72 and Ltmp113
	.uleb128 Ltmp114-Lfunc_begin0           ;     jumps to Ltmp114
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp113-Lfunc_begin0           ; >> Call Site 16 <<
	.uleb128 Lfunc_end0-Ltmp113             ;   Call between Ltmp113 and Lfunc_end0
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
Ltmp115:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne190102EPKc
Ltmp116:
; %bb.1:
Lloh86:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh87:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh88:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh89:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB6_2:
Ltmp117:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh88, Lloh89
	.loh AdrpLdrGot	Lloh86, Lloh87
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table6:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Lfunc_begin1-Lfunc_begin1      ; >> Call Site 1 <<
	.uleb128 Ltmp115-Lfunc_begin1           ;   Call between Lfunc_begin1 and Ltmp115
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp115-Lfunc_begin1           ; >> Call Site 2 <<
	.uleb128 Ltmp116-Ltmp115                ;   Call between Ltmp115 and Ltmp116
	.uleb128 Ltmp117-Lfunc_begin1           ;     jumps to Ltmp117
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp116-Lfunc_begin1           ; >> Call Site 3 <<
	.uleb128 Lfunc_end1-Ltmp116             ;   Call between Ltmp116 and Lfunc_end1
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
Lloh90:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh91:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh90, Lloh91
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
Ltmp118:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp119:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB8_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w8, [x4, #144]
	cmn	w8, #1
	b.ne	LBB8_7
; %bb.3:
Ltmp121:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp122:
; %bb.4:
Ltmp123:
Lloh92:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh93:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp124:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp125:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp126:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB8_7:
	ldrsb	w5, [x4, #144]
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp128:
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne190102IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp129:
; %bb.8:
	cbnz	x0, LBB8_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp131:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp132:
LBB8_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB8_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB8_12:
Ltmp133:
	b	LBB8_15
LBB8_13:
Ltmp127:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB8_16
LBB8_14:
Ltmp130:
LBB8_15:
	mov	x20, x0
LBB8_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB8_18
LBB8_17:
Ltmp120:
	mov	x20, x0
LBB8_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp134:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp135:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB8_11
LBB8_20:
Ltmp136:
	mov	x19, x0
Ltmp137:
	bl	___cxa_end_catch
Ltmp138:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB8_22:
Ltmp139:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh92, Lloh93
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table8:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Ltmp118-Lfunc_begin2           ; >> Call Site 1 <<
	.uleb128 Ltmp119-Ltmp118                ;   Call between Ltmp118 and Ltmp119
	.uleb128 Ltmp120-Lfunc_begin2           ;     jumps to Ltmp120
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp121-Lfunc_begin2           ; >> Call Site 2 <<
	.uleb128 Ltmp122-Ltmp121                ;   Call between Ltmp121 and Ltmp122
	.uleb128 Ltmp130-Lfunc_begin2           ;     jumps to Ltmp130
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp123-Lfunc_begin2           ; >> Call Site 3 <<
	.uleb128 Ltmp126-Ltmp123                ;   Call between Ltmp123 and Ltmp126
	.uleb128 Ltmp127-Lfunc_begin2           ;     jumps to Ltmp127
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp128-Lfunc_begin2           ; >> Call Site 4 <<
	.uleb128 Ltmp129-Ltmp128                ;   Call between Ltmp128 and Ltmp129
	.uleb128 Ltmp130-Lfunc_begin2           ;     jumps to Ltmp130
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp131-Lfunc_begin2           ; >> Call Site 5 <<
	.uleb128 Ltmp132-Ltmp131                ;   Call between Ltmp131 and Ltmp132
	.uleb128 Ltmp133-Lfunc_begin2           ;     jumps to Ltmp133
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp132-Lfunc_begin2           ; >> Call Site 6 <<
	.uleb128 Ltmp134-Ltmp132                ;   Call between Ltmp132 and Ltmp134
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp134-Lfunc_begin2           ; >> Call Site 7 <<
	.uleb128 Ltmp135-Ltmp134                ;   Call between Ltmp134 and Ltmp135
	.uleb128 Ltmp136-Lfunc_begin2           ;     jumps to Ltmp136
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp135-Lfunc_begin2           ; >> Call Site 8 <<
	.uleb128 Ltmp137-Ltmp135                ;   Call between Ltmp135 and Ltmp137
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp137-Lfunc_begin2           ; >> Call Site 9 <<
	.uleb128 Ltmp138-Ltmp137                ;   Call between Ltmp137 and Ltmp138
	.uleb128 Ltmp139-Lfunc_begin2           ;     jumps to Ltmp139
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp138-Lfunc_begin2           ; >> Call Site 10 <<
	.uleb128 Lfunc_end2-Ltmp138             ;   Call between Ltmp138 and Lfunc_end2
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
	cbz	x0, LBB9_16
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
	b.lt	LBB9_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB9_15
LBB9_3:
	cmp	x23, #1
	b.lt	LBB9_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB9_17
; %bb.5:
	cmp	x23, #22
	b.hi	LBB9_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB9_8
LBB9_7:
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
LBB9_8:
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
Ltmp140:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp141:
; %bb.9:
	cmp	x0, x23
	csel	x19, x19, xzr, eq
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB9_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB9_15
	b	LBB9_12
LBB9_11:
	ldr	x8, [sp, #8]
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPv
	mov	x0, x24
	cmp	x0, x23
	b.ne	LBB9_15
LBB9_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB9_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB9_15
LBB9_14:
	str	xzr, [x20, #24]
	b	LBB9_16
LBB9_15:
	mov	x19, #0                         ; =0x0
LBB9_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB9_17:
	add	x0, sp, #8
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190102Ev
LBB9_18:
Ltmp142:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB9_20
; %bb.19:
	ldr	x0, [sp, #8]
	bl	__ZdlPv
LBB9_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table9:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Lfunc_begin3-Lfunc_begin3      ; >> Call Site 1 <<
	.uleb128 Ltmp140-Lfunc_begin3           ;   Call between Lfunc_begin3 and Ltmp140
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp140-Lfunc_begin3           ; >> Call Site 2 <<
	.uleb128 Ltmp141-Ltmp140                ;   Call between Ltmp140 and Ltmp141
	.uleb128 Ltmp142-Lfunc_begin3           ;     jumps to Ltmp142
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp141-Lfunc_begin3           ; >> Call Site 3 <<
	.uleb128 Lfunc_end3-Ltmp141             ;   Call between Ltmp141 and Lfunc_end3
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
Lloh94:
	adrp	x0, l_.str.23@PAGE
Lloh95:
	add	x0, x0, l_.str.23@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190102EPKc
	.loh AdrpAdd	Lloh94, Lloh95
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Correctness check: max_err = "

l_.str.1:                               ; @.str.1
	.asciz	"  "

l_.str.2:                               ; @.str.2
	.asciz	"PASS"

l_.str.3:                               ; @.str.3
	.asciz	"FAIL"

l_.str.4:                               ; @.str.4
	.asciz	"\n"

l_.str.5:                               ; @.str.5
	.asciz	"\nArray:  "

l_.str.6:                               ; @.str.6
	.asciz	"M doubles  ("

l_.str.7:                               ; @.str.7
	.asciz	" MB)\n"

l_.str.8:                               ; @.str.8
	.asciz	"FADD latency (Apple M-series): "

l_.str.9:                               ; @.str.9
	.asciz	" cycles  \342\206\222  pipeline-full at K >= "

l_.str.10:                              ; @.str.10
	.asciz	" accumulators\n"

l_.str.11:                              ; @.str.11
	.asciz	"Array sum  N=64M doubles  512 MB   -O1 (no SIMD)\n"

l_.str.12:                              ; @.str.12
	.asciz	"version"

l_.str.13:                              ; @.str.13
	.asciz	"time"

l_.str.14:                              ; @.str.14
	.asciz	"speedup"

l_.str.15:                              ; @.str.15
	.asciz	"serial (1 accumulator)"

l_.str.16:                              ; @.str.16
	.asciz	"2 accumulators"

l_.str.17:                              ; @.str.17
	.asciz	"4 accumulators"

l_.str.18:                              ; @.str.18
	.asciz	"8 accumulators"

l_.str.19:                              ; @.str.19
	.asciz	"\nNote: at -O2 the compiler auto-vectorises these loops (SIMD),\n      masking the ILP effect.  Build with -O1 to see it.\n"

l_.str.20:                              ; @.str.20
	.asciz	" ms"

l_.str.21:                              ; @.str.21
	.asciz	"x\n"

l_.str.23:                              ; @.str.23
	.asciz	"basic_string"

.subsections_via_symbols
