.globl main

.data
m1: .word 10
m2: .word 20
m3: .word 30
m4: .word 40

.text
main:
	la	$t3, m1
	lw	$t4, 0($t3)
	la	$t3, m2
	lw	$t2, 0($t3)
	add	$t0, $t4, $t2
	la	$t3, m3
	lw	$t1, 0($t3)
	sub	$t0, $t0, $t1
	add	$t0, $t0, $t4
	bltz	$t0, l_neg
	b	l_pos
l_neg:
	sub	$t0, $t4, $t2
	b	end
l_pos:
	add	$t0, $t2, $t1
end:
	sw	$t0, 0($t3)
	nop
	jr	$t3
