	.arch armv8.5-a
	.build_version macos,  15, 0
	.text
	.cstring
	.align	3
lC0:
	.ascii " ms\0"
	.align	3
lC1:
	.ascii "x\12\0"
	.text
	.align	2
__ZZ4mainENKUlPKcxxE_clES0_xx:
LFB4358:
	stp	x29, x30, [sp, -48]!
LCFI0:
	mov	x29, sp
LCFI1:
	stp	x19, x20, [sp, 16]
	str	d15, [sp, 32]
LCFI2:
	mov	x19, x2
	fmov	d15, x3
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	ldr	x4, [x0]
	ldr	x3, [x4, -24]
	add	x3, x3, x0
	ldr	w2, [x3, 24]
	mov	w20, -177
	and	w2, w2, w20
	orr	w2, w2, 32
	str	w2, [x3, 24]
	ldr	x2, [x4, -24]
	add	x2, x2, x0
	mov	x3, 36
	str	x3, [x2, 16]
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	x2, [x0]
	ldr	x3, [x2, -24]
	add	x3, x0, x3
	ldr	w2, [x3, 24]
	and	w2, w2, w20
	orr	w2, w2, 128
	str	w2, [x3, 24]
	ldr	x2, [x0]
	ldr	x2, [x2, -24]
	add	x1, x0, x2
	mov	x2, 7
	str	x2, [x1, 16]
	mov	x1, x19
	bl	__ZNSo9_M_insertIxEERSoT_
	adrp	x1, lC0@PAGE
	add	x1, x1, lC0@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	x1, [x0]
	ldr	x2, [x1, -24]
	add	x2, x0, x2
	ldr	w1, [x2, 24]
	and	w1, w1, w20
	orr	w1, w1, 128
	str	w1, [x2, 24]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	mov	x2, 9
	str	x2, [x1, 16]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	w2, [x1, 24]
	mov	w4, -261
	and	w2, w2, w4
	str	w2, [x1, 24]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x3, x0, x1
	mov	x1, 2
	str	x1, [x3, 8]
	scvtf	d0, d15
	cmp	x19, 0
	csinc	x19, x19, xzr, gt
	scvtf	d31, x19
	fdiv	d0, d0, d31
	bl	__ZNSo9_M_insertIdEERSoT_
	adrp	x1, lC1@PAGE
	add	x1, x1, lC1@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	d15, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
LCFI3:
	ret
LFE4358:
	.align	2
	.globl __Z15sum_conditionalPKii
__Z15sum_conditionalPKii:
LFB4352:
	cmp	w1, 0
	ble	L7
	mov	x2, x0
	add	x3, x0, w1, sxtw 2
	mov	x0, 0
	b	L6
L5:
	add	x2, x2, 4
	cmp	x2, x3
	beq	L3
L6:
	ldr	w1, [x2]
	cmp	w1, 127
	ble	L5
	add	x0, x0, w1, sxtw
	b	L5
L7:
	mov	x0, 0
L3:
	ret
LFE4352:
	.align	2
	.globl __Z14sum_branchlessPKii
__Z14sum_branchlessPKii:
LFB4353:
	cmp	w1, 0
	ble	L12
	mov	x2, x0
	add	x4, x0, w1, sxtw 2
	mov	x0, 0
L11:
	ldr	w3, [x2], 4
	sxtw	x1, w3
	cmp	w3, 127
	csel	x1, x1, xzr, gt
	add	x0, x0, x1
	cmp	x2, x4
	bne	L11
L9:
	ret
L12:
	mov	x0, 0
	b	L9
LFE4353:
	.align	2
	.globl __ZNSt12_Vector_baseIiSaIiEED2Ev
	.weak_definition __ZNSt12_Vector_baseIiSaIiEED2Ev
__ZNSt12_Vector_baseIiSaIiEED2Ev:
LFB4968:
	mov	x1, x0
	ldr	x0, [x0]
	cbz	x0, L17
	stp	x29, x30, [sp, -16]!
LCFI4:
	mov	x29, sp
LCFI5:
	ldr	x1, [x1, 16]
	sub	x1, x1, x0
	bl	__ZdlPvm
	ldp	x29, x30, [sp], 16
LCFI6:
	ret
L17:
	ret
LFE4968:
	.align	2
	.globl __ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EE11_M_gen_randEv
	.weak_definition __ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EE11_M_gen_randEv
__ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EE11_M_gen_randEv:
LFB5242:
	mov	x1, x0
	add	x7, x0, 908
	mov	x2, x0
	mov	w8, 45279
	movk	w8, 0x9908, lsl 16
	b	L22
L21:
	eor	w3, w3, w5
	str	w3, [x6]
	add	x2, x2, 4
	cmp	x2, x7
	beq	L28
L22:
	mov	x6, x2
	ldp	w4, w3, [x2]
	bfi	w4, w3, 0, 31
	ldr	w3, [x2, 1588]
	eor	w3, w3, w4, lsr 1
	and	w5, w4, 1
	tbz	x4, 0, L21
	mov	w5, w8
	b	L21
L28:
	add	x6, x0, 1584
	mov	w7, 45279
	movk	w7, 0x9908, lsl 16
	b	L24
L23:
	eor	w2, w2, w4
	str	w2, [x5, 908]
	add	x1, x1, 4
	cmp	x1, x6
	beq	L29
L24:
	mov	x5, x1
	ldr	w3, [x1, 908]
	ldr	w2, [x1, 912]
	bfi	w3, w2, 0, 31
	ldr	w2, [x1]
	eor	w2, w2, w3, lsr 1
	and	w4, w3, 1
	tbz	x3, 0, L23
	mov	w4, w7
	b	L23
L29:
	ldr	w2, [x0, 2492]
	ldr	w1, [x0]
	bfi	w2, w1, 0, 31
	ldr	w1, [x0, 1584]
	eor	w1, w1, w2, lsr 1
	and	w3, w2, 1
	tbz	x2, 0, L25
	mov	w3, 45279
	movk	w3, 0x9908, lsl 16
L25:
	eor	w1, w1, w3
	str	w1, [x0, 2492]
	str	xzr, [x0, 2496]
	ret
LFE5242:
	.align	2
	.globl __ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEclEv
	.weak_definition __ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEclEv
__ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEclEv:
LFB5132:
	stp	x29, x30, [sp, -32]!
LCFI7:
	mov	x29, sp
LCFI8:
	str	x19, [sp, 16]
LCFI9:
	mov	x19, x0
	ldr	x0, [x0, 2496]
	cmp	x0, 623
	bhi	L33
L31:
	ldr	x0, [x19, 2496]
	add	x1, x0, 1
	str	x1, [x19, 2496]
	ldr	w0, [x19, x0, lsl 2]
	eor	w0, w0, w0, lsr 11
	mov	w1, 22144
	movk	w1, 0x9d2c, lsl 16
	and	w1, w1, w0, lsl 7
	eor	w1, w1, w0
	mov	w0, -272236544
	and	w0, w0, w1, lsl 15
	eor	w0, w0, w1
	eor	w0, w0, w0, lsr 18
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
LCFI10:
	ret
L33:
LCFI11:
	mov	x0, x19
	bl	__ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EE11_M_gen_randEv
	b	L31
LFE5132:
	.align	2
	.globl __ZSt16__insertion_sortIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEENS0_5__ops15_Iter_less_iterEEvT_S9_T0_
	.weak_definition __ZSt16__insertion_sortIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEENS0_5__ops15_Iter_less_iterEEvT_S9_T0_
__ZSt16__insertion_sortIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEENS0_5__ops15_Iter_less_iterEEvT_S9_T0_:
LFB5259:
	cmp	x1, x0
	beq	L46
	stp	x29, x30, [sp, -48]!
LCFI12:
	mov	x29, sp
LCFI13:
	stp	x21, x22, [sp, 32]
LCFI14:
	mov	x21, x0
	mov	x22, x1
	add	x0, x0, 4
	cmp	x1, x0
	beq	L34
	stp	x19, x20, [x29, 16]
LCFI15:
	mov	x20, x0
	b	L42
L37:
	beq	L49
L38:
	str	w19, [x21]
L39:
	add	x20, x20, 4
	cmp	x22, x20
	beq	L50
L42:
	mov	x4, x20
	ldr	w19, [x20]
	ldr	w1, [x21]
	cmp	w19, w1
	bge	L36
	sub	x2, x20, x21
	lsl	x3, x2, 62
	sub	x3, x3, x2
	add	x3, x3, 4
	add	x0, x20, x3
	cmp	x2, 4
	ble	L37
	mov	x1, x21
	bl	_memmove
	b	L38
L49:
	str	w1, [x0]
	b	L38
L36:
	ldr	w3, [x20, -4]
	cmp	w19, w3
	bge	L40
	sub	x2, x20, #4
L41:
	str	w3, [x2, 4]
	mov	x4, x2
	ldr	w3, [x2, -4]!
	cmp	w19, w3
	blt	L41
L40:
	str	w19, [x4]
	b	L39
L50:
	ldp	x19, x20, [x29, 16]
LCFI16:
L34:
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
LCFI17:
	ret
L46:
	ret
LFE5259:
	.align	2
	.globl __ZSt13__adjust_heapIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEEliNS0_5__ops15_Iter_less_iterEEvT_T0_SA_T1_T2_
	.weak_definition __ZSt13__adjust_heapIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEEliNS0_5__ops15_Iter_less_iterEEvT_T0_SA_T1_T2_
__ZSt13__adjust_heapIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEEliNS0_5__ops15_Iter_less_iterEEvT_T0_SA_T1_T2_:
LFB5343:
	sub	x9, x2, #1
	add	x9, x9, x9, lsr 63
	asr	x9, x9, 1
	cmp	x1, x9
	bge	L52
	mov	x6, x1
	b	L54
L53:
	ldr	w5, [x0, x4, lsl 2]
	str	w5, [x0, x6, lsl 2]
	cmp	x4, x9
	bge	L63
	mov	x6, x4
L54:
	add	x5, x6, 1
	lsl	x5, x5, 1
	sub	x4, x5, #1
	ldr	w8, [x0, x5, lsl 2]
	ldr	w7, [x0, x4, lsl 2]
	cmp	w8, w7
	blt	L53
	mov	x4, x5
	b	L53
L63:
	tbnz	x2, 0, L55
L58:
	sub	x2, x2, #2
	cmp	x4, x2, asr 1
	beq	L64
L55:
	sub	x5, x4, #1
	add	x5, x5, x5, lsr 63
	asr	x5, x5, 1
	cmp	x4, x1
	bgt	L57
	b	L56
L64:
	lsl	x2, x4, 1
	add	x2, x2, 1
	ldr	w5, [x0, x2, lsl 2]
	str	w5, [x0, x4, lsl 2]
	mov	x4, x2
	b	L55
L60:
	mov	x5, x2
L57:
	ldr	w2, [x0, x5, lsl 2]
	cmp	w3, w2
	ble	L56
	str	w2, [x0, x4, lsl 2]
	sub	x2, x5, #1
	add	x2, x2, x2, lsr 63
	asr	x2, x2, 1
	mov	x4, x5
	cmp	x1, x5
	blt	L60
L56:
	str	w3, [x0, x4, lsl 2]
	ret
L52:
	mov	x4, x1
	tbnz	x2, 0, L56
	mov	x4, x1
	b	L58
LFE5343:
	.align	2
	.globl __ZSt16__introsort_loopIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEElNS0_5__ops15_Iter_less_iterEEvT_S9_T0_T1_
	.weak_definition __ZSt16__introsort_loopIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEElNS0_5__ops15_Iter_less_iterEEvT_S9_T0_T1_
__ZSt16__introsort_loopIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEElNS0_5__ops15_Iter_less_iterEEvT_S9_T0_T1_:
LFB5156:
	stp	x29, x30, [sp, -48]!
LCFI18:
	mov	x29, sp
LCFI19:
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
LCFI20:
	mov	x19, x0
	mov	x21, x2
	mov	x20, x1
	sub	x0, x1, x0
	cmp	x0, 64
	bgt	L85
	b	L65
L92:
	asr	x22, x0, 2
	add	x21, x22, x0, lsr 63
	asr	x21, x21, 1
	sub	x21, x21, #1
	b	L69
L91:
	sub	x21, x21, #1
L69:
	mov	w4, 0
	ldr	w3, [x19, x21, lsl 2]
	mov	x2, x22
	mov	x1, x21
	mov	x0, x19
	bl	__ZSt13__adjust_heapIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEEliNS0_5__ops15_Iter_less_iterEEvT_T0_SA_T1_T2_
	cbnz	x21, L91
	sub	x0, x20, x19
	cmp	x0, 4
	ble	L65
	sub	x20, x20, #4
L71:
	ldr	w3, [x20]
	ldr	w0, [x19]
	str	w0, [x20]
	sub	x21, x20, x19
	mov	w4, 0
	asr	x2, x21, 2
	mov	x1, 0
	mov	x0, x19
	bl	__ZSt13__adjust_heapIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEEliNS0_5__ops15_Iter_less_iterEEvT_T0_SA_T1_T2_
	sub	x20, x20, #4
	cmp	x21, 4
	bgt	L71
L65:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
LCFI21:
	ret
L73:
LCFI22:
	cmp	w2, w3
	bge	L75
	ldr	w0, [x19]
	str	w3, [x19]
	str	w0, [x20, -4]
	b	L74
L75:
	ldr	w0, [x19]
	stp	w2, w0, [x19]
	b	L74
L72:
	ldr	w3, [x20, -4]
	cmp	w2, w3
	bge	L77
	ldr	w0, [x19]
	stp	w2, w0, [x19]
	b	L74
L77:
	cmp	w1, w3
	bge	L78
	ldr	w0, [x19]
	str	w3, [x19]
	str	w0, [x20, -4]
	b	L74
L78:
	ldr	w2, [x19]
	str	w1, [x19]
	str	w2, [x19, x0]
	b	L74
L81:
	sub	x4, x4, #4
	b	L83
L93:
	mov	w3, 0
	mov	x2, x21
	mov	x1, x20
	mov	x0, x22
	bl	__ZSt16__introsort_loopIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEElNS0_5__ops15_Iter_less_iterEEvT_S9_T0_T1_
	sub	x0, x22, x19
	cmp	x0, 64
	ble	L65
	mov	x20, x22
L85:
	cbz	x21, L92
	sub	x21, x21, #1
	lsr	x1, x0, 63
	add	x0, x1, x0, asr 2
	asr	x0, x0, 1
	lsl	x0, x0, 2
	add	x22, x19, 4
	ldr	w2, [x19, 4]
	ldr	w1, [x19, x0]
	cmp	w2, w1
	bge	L72
	ldr	w3, [x20, -4]
	cmp	w1, w3
	bge	L73
	ldr	w2, [x19]
	str	w1, [x19]
	str	w2, [x19, x0]
L74:
	mov	x4, x20
L76:
	ldr	w2, [x22]
	ldr	w1, [x19]
	cmp	w2, w1
	bge	L79
	add	x0, x22, 4
L80:
	mov	x3, x0
	ldr	w2, [x0], 4
	cmp	w1, w2
	bgt	L80
	mov	x22, x3
L79:
	ldr	w3, [x4, -4]
	cmp	w1, w3
	bge	L81
	sub	x0, x4, #8
L82:
	mov	x4, x0
	ldr	w3, [x0], -4
	cmp	w1, w3
	blt	L82
L83:
	cmp	x4, x22
	bls	L93
	str	w3, [x22], 4
	str	w2, [x4]
	b	L76
LFE5156:
	.cstring
	.align	3
lC2:
	.ascii "PASS\0"
	.align	3
lC3:
	.ascii "FAIL\0"
	.align	3
lC4:
	.ascii "Correctness: shuffled=\0"
	.align	3
lC5:
	.ascii "  sorted=\0"
	.align	3
lC6:
	.ascii "  branchless=\0"
	.align	3
lC7:
	.ascii "  \0"
	.align	3
lC8:
	.ascii "\12\0"
	.align	3
lC9:
	.ascii "\12Data:  \0"
	.align	3
lC10:
	.ascii "M ints in [0,255]  threshold=\0"
	.align	3
lC11:
	.ascii "  ~\0"
	.align	3
lC12:
	.ascii "% above\12\0"
	.align	3
lC13:
	.ascii "Expected misprediction overhead (shuffled): ~\0"
	.align	3
lC14:
	.ascii " ms  \0"
	.align	3
lC15:
	.ascii "(\0"
	.align	3
lC16:
	.ascii "% mispredict \303\227 \0"
	.align	3
lC17:
	.ascii " cycles \303\227 32M branches at 3 GHz)\12\0"
	.align	3
lC18:
	.ascii "Conditional sum  N=32M ints  128 MB   threshold=128\12\0"
	.align	3
lC19:
	.ascii "version\0"
	.align	3
lC20:
	.ascii "time\0"
	.align	3
lC21:
	.ascii "speedup\0"
	.align	3
lC22:
	.ascii "branchy  + shuffled  (50% mispredict)\0"
	.align	3
lC23:
	.ascii "branchy  + sorted    (1 mispredict)\0"
	.align	3
lC24:
	.ascii "branchless + shuffled (no branch)\0"
	.section __TEXT,__text_startup,regular,pure_instructions
	.align	2
	.globl _main
_main:
LFB4354:
	sub	sp, sp, #2704
LCFI23:
	stp	x29, x30, [sp]
LCFI24:
	mov	x29, sp
LCFI25:
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	str	x25, [sp, 64]
LCFI26:
	mov	x0, 134217728
LEHB0:
	bl	__Znwm
LEHE0:
	mov	x20, x0
	str	x0, [x29, 2656]
	mov	x1, 134217728
	add	x1, x0, x1
	str	x1, [x29, 2672]
	str	wzr, [x0], 4
L95:
	str	wzr, [x0], 4
	cmp	x0, x1
	bne	L95
	str	x1, [x29, 2664]
	mov	x0, 134217728
LEHB1:
	bl	__Znwm
LEHE1:
	mov	x21, x0
	str	x0, [x29, 2632]
	mov	x19, 134217728
	add	x19, x0, x19
	str	x19, [x29, 2648]
	str	wzr, [x0], 4
L96:
	str	wzr, [x0], 4
	cmp	x19, x0
	bne	L96
	str	x19, [x29, 2640]
	mov	w0, 42
	str	w0, [x29, 128]
	add	x0, x29, 132
	mov	x2, 1
	mov	w5, 35173
	movk	w5, 0x6c07, lsl 16
	mov	w4, 624
L97:
	ldr	w1, [x0, -4]
	eor	w1, w1, w1, lsr 30
	udiv	w3, w2, w4
	msub	w3, w3, w4, w2
	madd	w1, w1, w5, w3
	str	w1, [x0], 4
	add	x2, x2, 1
	cmp	x2, 624
	bne	L97
	mov	x0, 624
	str	x0, [x29, 2624]
	mov	x22, x20
	mov	x23, 134217728
	add	x23, x20, x23
	add	x24, x29, 128
L98:
	mov	x0, x24
	bl	__ZNSt23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEclEv
	lsr	w0, w0, 24
	str	w0, [x22], 4
	cmp	x22, x23
	bne	L98
	mov	x2, 134217728
	mov	x1, x20
	mov	x0, x21
	bl	_memcpy
	mov	w3, 0
	mov	x2, 50
	mov	x1, x19
	mov	x0, x21
	bl	__ZSt16__introsort_loopIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEElNS0_5__ops15_Iter_less_iterEEvT_S9_T0_T1_
	add	x22, x21, 64
	mov	w2, 0
	mov	x1, x22
	mov	x0, x21
	bl	__ZSt16__insertion_sortIN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEENS0_5__ops15_Iter_less_iterEEvT_S9_T0_
L101:
	ldp	w1, w2, [x22, -4]
	cmp	w2, w1
	bge	L111
	sub	x0, x22, #4
L100:
	str	w1, [x0, 4]
	mov	x3, x0
	ldr	w1, [x0, -4]!
	cmp	w2, w1
	blt	L100
L99:
	str	w2, [x3]
	add	x22, x22, 4
	cmp	x19, x22
	bne	L101
	mov	w1, 33554432
	mov	x0, x20
	bl	__Z15sum_conditionalPKii
	mov	x19, x0
	mov	w1, 33554432
	mov	x0, x21
	bl	__Z15sum_conditionalPKii
	mov	x21, x0
	mov	w1, 33554432
	mov	x0, x20
	bl	__Z14sum_branchlessPKii
	mov	x20, x0
	adrp	x1, lC4@PAGE
	add	x1, x1, lC4@PAGEOFF;
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
LEHB2:
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	b	L130
L111:
	mov	x3, x22
	b	L99
L130:
	mov	x1, x19
	bl	__ZNSo9_M_insertIxEERSoT_
	adrp	x1, lC5@PAGE
	add	x1, x1, lC5@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	x1, x21
	bl	__ZNSo9_M_insertIxEERSoT_
	adrp	x1, lC6@PAGE
	add	x1, x1, lC6@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	x1, x20
	bl	__ZNSo9_M_insertIxEERSoT_
	adrp	x1, lC7@PAGE
	add	x1, x1, lC7@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	cmp	x19, x21
	ccmp	x19, x20, 0, eq
	bne	L112
	adrp	x1, lC2@PAGE
	add	x1, x1, lC2@PAGEOFF;
	b	L102
L112:
	adrp	x1, lC3@PAGE
	add	x1, x1, lC3@PAGEOFF;
L102:
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	ldr	x3, [x0]
	ldr	x2, [x3, -24]
	add	x2, x2, x0
	ldr	w1, [x2, 24]
	mov	w4, -261
	and	w1, w1, w4
	orr	w1, w1, 4
	str	w1, [x2, 24]
	ldr	x1, [x3, -24]
	add	x1, x1, x0
	str	xzr, [x1, 8]
	adrp	x1, lC9@PAGE
	add	x1, x1, lC9@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w1, 33
	bl	__ZNSolsEi
	adrp	x1, lC10@PAGE
	add	x1, x1, lC10@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w1, 128
	bl	__ZNSolsEi
	adrp	x1, lC11@PAGE
	add	x1, x1, lC11@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w1, 50
	bl	__ZNSolsEi
	adrp	x1, lC12@PAGE
	add	x1, x1, lC12@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x1, lC13@PAGE
	add	x1, x1, lC13@PAGEOFF;
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x1, lC25@PAGE
	ldr	d0, [x1, #lC25@PAGEOFF]
	bl	__ZNSo9_M_insertIdEERSoT_
	adrp	x1, lC14@PAGE
	add	x1, x1, lC14@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x1, lC15@PAGE
	add	x1, x1, lC15@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w1, 50
	bl	__ZNSolsEi
	adrp	x1, lC16@PAGE
	add	x1, x1, lC16@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w1, 15
	bl	__ZNSolsEi
	adrp	x1, lC17@PAGE
	add	x1, x1, lC17@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	w21, 5
	mov	x19, 9223372036854775807
	mov	w23, 33554432
	mov	x22, 13531
	movk	x22, 0xd7b6, lsl 16
	movk	x22, 0xde82, lsl 32
	movk	x22, 0x431b, lsl 48
L103:
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	mov	x20, x0
	mov	w1, w23
	ldr	x0, [x29, 2656]
	bl	__Z15sum_conditionalPKii
	str	x0, [x29, 2680]
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	sub	x0, x0, x20
	smulh	x1, x0, x22
	asr	x1, x1, 18
	sub	x0, x1, x0, asr 63
	cmp	x19, x0
	csel	x19, x19, x0, le
	subs	w21, w21, #1
	bne	L103
	mov	w20, 5
	mov	x21, 9223372036854775807
	mov	w23, 33554432
	mov	x22, 13531
	movk	x22, 0xd7b6, lsl 16
	movk	x22, 0xde82, lsl 32
	movk	x22, 0x431b, lsl 48
L104:
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	mov	x24, x0
	mov	w1, w23
	ldr	x0, [x29, 2632]
	bl	__Z15sum_conditionalPKii
	str	x0, [x29, 2688]
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	sub	x0, x0, x24
	smulh	x1, x0, x22
	asr	x1, x1, 18
	sub	x0, x1, x0, asr 63
	cmp	x21, x0
	csel	x21, x21, x0, le
	subs	w20, w20, #1
	bne	L104
	mov	w22, 5
	mov	x20, 9223372036854775807
	mov	w24, 33554432
	mov	x23, 13531
	movk	x23, 0xd7b6, lsl 16
	movk	x23, 0xde82, lsl 32
	movk	x23, 0x431b, lsl 48
L105:
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	mov	x25, x0
	mov	w1, w24
	ldr	x0, [x29, 2656]
	bl	__Z14sum_branchlessPKii
	str	x0, [x29, 2696]
	bl	__ZNSt6chrono3_V212system_clock3nowEv
	sub	x0, x0, x25
	smulh	x1, x0, x23
	asr	x1, x1, 18
	sub	x0, x1, x0, asr 63
	cmp	x20, x0
	csel	x20, x20, x0, le
	subs	w22, w22, #1
	bne	L105
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	mov	x22, x0
	add	x0, x29, 104
	str	x0, [x29, 88]
	mov	w2, 45
	mov	x1, 64
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
LEHE2:
	ldp	x1, x2, [x29, 88]
	mov	x0, x22
LEHB3:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
LEHE3:
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
	adrp	x1, lC18@PAGE
	add	x1, x1, lC18@PAGEOFF;
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
LEHB4:
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	add	x0, x29, 104
	str	x0, [x29, 88]
	mov	w2, 45
	mov	x1, 64
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
LEHE4:
	ldp	x1, x2, [x29, 88]
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
LEHB5:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
LEHE5:
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	ldr	x3, [x0]
	ldr	x2, [x3, -24]
	add	x2, x2, x0
	ldr	w1, [x2, 24]
	mov	w4, -177
	and	w1, w1, w4
	orr	w1, w1, 32
	str	w1, [x2, 24]
	ldr	x1, [x3, -24]
	add	x1, x1, x0
	mov	x2, 36
	str	x2, [x1, 16]
	adrp	x1, lC19@PAGE
	add	x1, x1, lC19@PAGEOFF;
LEHB6:
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	x2, [x0]
	ldr	x3, [x2, -24]
	add	x3, x0, x3
	ldr	w2, [x3, 24]
	mov	w4, -177
	and	w2, w2, w4
	orr	w2, w2, 128
	str	w2, [x3, 24]
	ldr	x2, [x0]
	ldr	x2, [x2, -24]
	add	x1, x0, x2
	mov	x2, 8
	str	x2, [x1, 16]
	adrp	x1, lC20@PAGE
	add	x1, x1, lC20@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	x2, [x0]
	ldr	x3, [x2, -24]
	add	x3, x0, x3
	ldr	w2, [x3, 24]
	mov	w4, -177
	and	w2, w2, w4
	orr	w2, w2, 128
	str	w2, [x3, 24]
	ldr	x2, [x0]
	ldr	x2, [x2, -24]
	add	x1, x0, x2
	mov	x2, 10
	str	x2, [x1, 16]
	adrp	x1, lC21@PAGE
	add	x1, x1, lC21@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	add	x0, x29, 104
	str	x0, [x29, 88]
	mov	w2, 45
	mov	x1, 64
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
LEHE6:
	ldp	x1, x2, [x29, 88]
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
LEHB7:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	adrp	x1, lC8@PAGE
	add	x1, x1, lC8@PAGEOFF;
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
LEHE7:
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
	mov	x3, x19
	mov	x2, x19
	adrp	x1, lC22@PAGE
	add	x1, x1, lC22@PAGEOFF;
	add	x0, x29, 120
LEHB8:
	bl	__ZZ4mainENKUlPKcxxE_clES0_xx
	mov	x3, x19
	mov	x2, x21
	adrp	x1, lC23@PAGE
	add	x1, x1, lC23@PAGEOFF;
	add	x0, x29, 120
	bl	__ZZ4mainENKUlPKcxxE_clES0_xx
	mov	x3, x19
	mov	x2, x20
	adrp	x1, lC24@PAGE
	add	x1, x1, lC24@PAGEOFF;
	add	x0, x29, 120
	bl	__ZZ4mainENKUlPKcxxE_clES0_xx
LEHE8:
	b	L131
L115:
	mov	x19, x0
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
L107:
	add	x0, x29, 2632
	bl	__ZNSt12_Vector_baseIiSaIiEED2Ev
L110:
	add	x0, x29, 2656
	bl	__ZNSt12_Vector_baseIiSaIiEED2Ev
	mov	x0, x19
LEHB9:
	bl	__Unwind_Resume
LEHE9:
L116:
	mov	x19, x0
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
	b	L107
L117:
	mov	x19, x0
	add	x0, x29, 88
	bl	__ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
	b	L107
L114:
	mov	x19, x0
	b	L107
L113:
	mov	x19, x0
	b	L110
L131:
	add	x0, x29, 2632
	bl	__ZNSt12_Vector_baseIiSaIiEED2Ev
	add	x0, x29, 2656
	bl	__ZNSt12_Vector_baseIiSaIiEED2Ev
	mov	w0, 0
	ldp	x29, x30, [sp]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldr	x25, [sp, 64]
	add	sp, sp, 2704
LCFI27:
	ret
LFE4354:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table0:
LLSDA4354:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE4354-LLSDACSB4354
LLSDACSB4354:
	.uleb128 LEHB0-LFB4354
	.uleb128 LEHE0-LEHB0
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB1-LFB4354
	.uleb128 LEHE1-LEHB1
	.uleb128 L113-LFB4354
	.uleb128 0
	.uleb128 LEHB2-LFB4354
	.uleb128 LEHE2-LEHB2
	.uleb128 L114-LFB4354
	.uleb128 0
	.uleb128 LEHB3-LFB4354
	.uleb128 LEHE3-LEHB3
	.uleb128 L115-LFB4354
	.uleb128 0
	.uleb128 LEHB4-LFB4354
	.uleb128 LEHE4-LEHB4
	.uleb128 L114-LFB4354
	.uleb128 0
	.uleb128 LEHB5-LFB4354
	.uleb128 LEHE5-LEHB5
	.uleb128 L116-LFB4354
	.uleb128 0
	.uleb128 LEHB6-LFB4354
	.uleb128 LEHE6-LEHB6
	.uleb128 L114-LFB4354
	.uleb128 0
	.uleb128 LEHB7-LFB4354
	.uleb128 LEHE7-LEHB7
	.uleb128 L117-LFB4354
	.uleb128 0
	.uleb128 LEHB8-LFB4354
	.uleb128 LEHE8-LEHB8
	.uleb128 L114-LFB4354
	.uleb128 0
	.uleb128 LEHB9-LFB4354
	.uleb128 LEHE9-LEHB9
	.uleb128 0
	.uleb128 0
LLSDACSE4354:
	.section __TEXT,__text_startup,regular,pure_instructions
	.section	__TEXT,__StaticInit,regular,pure_instructions
	.align	2
__GLOBAL__sub_I_speculative_execution.cpp:
LFB5356:
	stp	x29, x30, [sp, -32]!
LCFI28:
	mov	x29, sp
LCFI29:
	str	x19, [sp, 16]
LCFI30:
	adrp	x19, __ZStL8__ioinit@PAGE
	add	x19, x19, __ZStL8__ioinit@PAGEOFF;
	mov	x0, x19
	bl	__ZNSt8ios_base4InitC1Ev
	adrp	x2, ___dso_handle@PAGE
	add	x2, x2, ___dso_handle@PAGEOFF;
	mov	x1, x19
	adrp	x0, __ZNSt8ios_base4InitD1Ev@GOTPAGE
	ldr	x0, [x0, __ZNSt8ios_base4InitD1Ev@GOTPAGEOFF]
	bl	___cxa_atexit
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
LCFI31:
	ret
LFE5356:
	.zerofill __DATA,__bss,__ZStL8__ioinit,1,0
	.literal8
	.align	3
lC25:
	.word	-1998362383
	.word	1079310517
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x3
	.ascii "zPLR\0"
	.uleb128 0x1
	.sleb128 -8
	.uleb128 0x1e
	.uleb128 0x7
	.byte	0x9b
L_got_pcr0:
	.long	___gxx_personality_v0@GOT-L_got_pcr0
	.byte	0x10
	.byte	0x10
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB4358-.
	.set L$set$2,LFE4358-LFB4358
	.quad L$set$2
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB4358
	.long L$set$3
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0x93
	.uleb128 0x4
	.byte	0x94
	.uleb128 0x3
	.byte	0x5
	.uleb128 0x4f
	.uleb128 0x2
	.byte	0x4
	.set L$set$6,LCFI3-LCFI2
	.long L$set$6
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xd4
	.byte	0x6
	.uleb128 0x4f
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE1:
LSFDE3:
	.set L$set$7,LEFDE3-LASFDE3
	.long L$set$7
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB4352-.
	.set L$set$8,LFE4352-LFB4352
	.quad L$set$8
	.uleb128 0x8
	.quad	0
	.align	3
LEFDE3:
LSFDE5:
	.set L$set$9,LEFDE5-LASFDE5
	.long L$set$9
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB4353-.
	.set L$set$10,LFE4353-LFB4353
	.quad L$set$10
	.uleb128 0x8
	.quad	0
	.align	3
LEFDE5:
LSFDE7:
	.set L$set$11,LEFDE7-LASFDE7
	.long L$set$11
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB4968-.
	.set L$set$12,LFE4968-LFB4968
	.quad L$set$12
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$13,LCFI4-LFB4968
	.long L$set$13
	.byte	0xe
	.uleb128 0x10
	.byte	0x9d
	.uleb128 0x2
	.byte	0x9e
	.uleb128 0x1
	.byte	0x4
	.set L$set$14,LCFI5-LCFI4
	.long L$set$14
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$15,LCFI6-LCFI5
	.long L$set$15
	.byte	0xde
	.byte	0xdd
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE7:
LSFDE9:
	.set L$set$16,LEFDE9-LASFDE9
	.long L$set$16
LASFDE9:
	.long	LASFDE9-EH_frame1
	.quad	LFB5242-.
	.set L$set$17,LFE5242-LFB5242
	.quad L$set$17
	.uleb128 0x8
	.quad	0
	.align	3
LEFDE9:
LSFDE11:
	.set L$set$18,LEFDE11-LASFDE11
	.long L$set$18
LASFDE11:
	.long	LASFDE11-EH_frame1
	.quad	LFB5132-.
	.set L$set$19,LFE5132-LFB5132
	.quad L$set$19
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$20,LCFI7-LFB5132
	.long L$set$20
	.byte	0xe
	.uleb128 0x20
	.byte	0x9d
	.uleb128 0x4
	.byte	0x9e
	.uleb128 0x3
	.byte	0x4
	.set L$set$21,LCFI8-LCFI7
	.long L$set$21
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$22,LCFI9-LCFI8
	.long L$set$22
	.byte	0x93
	.uleb128 0x2
	.byte	0x4
	.set L$set$23,LCFI10-LCFI9
	.long L$set$23
	.byte	0xa
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$24,LCFI11-LCFI10
	.long L$set$24
	.byte	0xb
	.align	3
LEFDE11:
LSFDE13:
	.set L$set$25,LEFDE13-LASFDE13
	.long L$set$25
LASFDE13:
	.long	LASFDE13-EH_frame1
	.quad	LFB5259-.
	.set L$set$26,LFE5259-LFB5259
	.quad L$set$26
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$27,LCFI12-LFB5259
	.long L$set$27
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$28,LCFI13-LCFI12
	.long L$set$28
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$29,LCFI14-LCFI13
	.long L$set$29
	.byte	0x95
	.uleb128 0x2
	.byte	0x96
	.uleb128 0x1
	.byte	0x4
	.set L$set$30,LCFI15-LCFI14
	.long L$set$30
	.byte	0x94
	.uleb128 0x3
	.byte	0x93
	.uleb128 0x4
	.byte	0x4
	.set L$set$31,LCFI16-LCFI15
	.long L$set$31
	.byte	0xd4
	.byte	0xd3
	.byte	0x4
	.set L$set$32,LCFI17-LCFI16
	.long L$set$32
	.byte	0xde
	.byte	0xdd
	.byte	0xd5
	.byte	0xd6
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE13:
LSFDE15:
	.set L$set$33,LEFDE15-LASFDE15
	.long L$set$33
LASFDE15:
	.long	LASFDE15-EH_frame1
	.quad	LFB5343-.
	.set L$set$34,LFE5343-LFB5343
	.quad L$set$34
	.uleb128 0x8
	.quad	0
	.align	3
LEFDE15:
LSFDE17:
	.set L$set$35,LEFDE17-LASFDE17
	.long L$set$35
LASFDE17:
	.long	LASFDE17-EH_frame1
	.quad	LFB5156-.
	.set L$set$36,LFE5156-LFB5156
	.quad L$set$36
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$37,LCFI18-LFB5156
	.long L$set$37
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$38,LCFI19-LCFI18
	.long L$set$38
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$39,LCFI20-LCFI19
	.long L$set$39
	.byte	0x93
	.uleb128 0x4
	.byte	0x94
	.uleb128 0x3
	.byte	0x95
	.uleb128 0x2
	.byte	0x96
	.uleb128 0x1
	.byte	0x4
	.set L$set$40,LCFI21-LCFI20
	.long L$set$40
	.byte	0xa
	.byte	0xde
	.byte	0xdd
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$41,LCFI22-LCFI21
	.long L$set$41
	.byte	0xb
	.align	3
LEFDE17:
LSFDE19:
	.set L$set$42,LEFDE19-LASFDE19
	.long L$set$42
LASFDE19:
	.long	LASFDE19-EH_frame1
	.quad	LFB4354-.
	.set L$set$43,LFE4354-LFB4354
	.quad L$set$43
	.uleb128 0x8
	.quad	LLSDA4354-.
	.byte	0x4
	.set L$set$44,LCFI23-LFB4354
	.long L$set$44
	.byte	0xe
	.uleb128 0xa90
	.byte	0x4
	.set L$set$45,LCFI24-LCFI23
	.long L$set$45
	.byte	0x9d
	.uleb128 0x152
	.byte	0x9e
	.uleb128 0x151
	.byte	0x4
	.set L$set$46,LCFI25-LCFI24
	.long L$set$46
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$47,LCFI26-LCFI25
	.long L$set$47
	.byte	0x93
	.uleb128 0x150
	.byte	0x94
	.uleb128 0x14f
	.byte	0x95
	.uleb128 0x14e
	.byte	0x96
	.uleb128 0x14d
	.byte	0x97
	.uleb128 0x14c
	.byte	0x98
	.uleb128 0x14b
	.byte	0x99
	.uleb128 0x14a
	.byte	0x4
	.set L$set$48,LCFI27-LCFI26
	.long L$set$48
	.byte	0xd9
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xdd
	.byte	0xde
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE19:
LSFDE21:
	.set L$set$49,LEFDE21-LASFDE21
	.long L$set$49
LASFDE21:
	.long	LASFDE21-EH_frame1
	.quad	LFB5356-.
	.set L$set$50,LFE5356-LFB5356
	.quad L$set$50
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$51,LCFI28-LFB5356
	.long L$set$51
	.byte	0xe
	.uleb128 0x20
	.byte	0x9d
	.uleb128 0x4
	.byte	0x9e
	.uleb128 0x3
	.byte	0x4
	.set L$set$52,LCFI29-LCFI28
	.long L$set$52
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$53,LCFI30-LCFI29
	.long L$set$53
	.byte	0x93
	.uleb128 0x2
	.byte	0x4
	.set L$set$54,LCFI31-LCFI30
	.long L$set$54
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE21:
	.private_extern ___dso_handle
	.ident	"GCC: (Homebrew GCC 15.2.0) 15.2.0"
	.mod_init_func
_Mod.init:
	.align	3
	.xword	__GLOBAL__sub_I_speculative_execution.cpp
	.subsections_via_symbols
