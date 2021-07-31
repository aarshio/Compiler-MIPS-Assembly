; --------------wain--------------
PROCEDUREwain:
.import print
.import init
.import new
.import delete
lis $4
.word 4
lis $10
.word print
lis $11
.word 1
lis $12
.word init
sub $29, $30, $4
; Push $31
sw $31, -4($30)
sub $30, $30, $4
jalr $12
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Push $1
sw $1, 0($29)
sub $30, $30, $4
; Push $2
sw $2, -4($29)
sub $30, $30, $4
lis $5
.word 5
; Push $5
sw $5, -8($29)
sub $30, $30, $4

; Push $29
sw $29, -4($30)
sub $30, $30, $4
; Push $31
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word PROCEDUREfoo
jalr $5
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Pop $29
add $30, $30, $4
lw $29, -4($30)
; Push $3
sw $3, -4($30)
sub $30, $30, $4
; Push $29
sw $29, -4($30)
sub $30, $30, $4
; Push $31
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word PROCEDUREbar
jalr $5
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Pop $29
add $30, $30, $4
lw $29, -4($30)
; Pop $5
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3
; Push $3
sw $3, -4($30)
sub $30, $30, $4
lis $3
.word 5
; Pop $5
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3

add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
jr $31
; --------------bar--------------
PROCEDUREbar:
sub $29, $30, $4
; Push $5
sw $5, -4($30)
sub $30, $30, $4
; Push $6
sw $6, -4($30)
sub $30, $30, $4
; Push $7
sw $7, -4($30)
sub $30, $30, $4
lis $5
.word 8
; Push $5
sw $5, 0($29)
sub $30, $30, $4

; Code(local)
lw $3, 0($29)
; Push $3
sw $3, -4($30)
sub $30, $30, $4
; Push $29
sw $29, -4($30)
sub $30, $30, $4
; Push $31
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word PROCEDUREfoo
jalr $5
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Pop $29
add $30, $30, $4
lw $29, -4($30)
; Pop $5
add $30, $30, $4
lw $5, -4($30)
sub $3, $5, $3

; Pop $7
add $30, $30, $4
lw $7, -4($30)
; Pop $6
add $30, $30, $4
lw $6, -4($30)
; Pop $5
add $30, $30, $4
lw $5, -4($30)
add $30, $29, $4
jr $31
; --------------foo--------------
PROCEDUREfoo:
sub $29, $30, $4
; Push $5
sw $5, -4($30)
sub $30, $30, $4
; Push $6
sw $6, -4($30)
sub $30, $30, $4
; Push $7
sw $7, -4($30)
sub $30, $30, $4
lis $5
.word 99
; Push $5
sw $5, 0($29)
sub $30, $30, $4

lis $3
.word 62
; Push $3
sw $3, -4($30)
sub $30, $30, $4
; Code(local)
lw $3, 0($29)
; Pop $5
add $30, $30, $4
lw $5, -4($30)
add $3, $5, $3

; Pop $7
add $30, $30, $4
lw $7, -4($30)
; Pop $6
add $30, $30, $4
lw $6, -4($30)
; Pop $5
add $30, $30, $4
lw $5, -4($30)
add $30, $29, $4
jr $31
