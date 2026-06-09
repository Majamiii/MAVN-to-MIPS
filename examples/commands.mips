.globl main

.data
m1: .word 6
m2: .word 5
m3: .word 10

.text
main:
	la	$t2, m1
	lw	$t1, 0($t2)
	la	$t0, m2
	lw	$t0, 0($t0)
	add	$t0, $t1, $t0
	and	$t0, $t1, $t0
	slt	$t0, $t1, $t0
	jr	$t2
