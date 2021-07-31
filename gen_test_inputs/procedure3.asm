beq $0, $0, PROCEDUREwain
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
; Push $2
sw $2, -4($30)
sub $30, $30, $4
lis $2
.word 0
jalr $12
; Pop $2
add $30, $30, $4
lw $2, -4($30)
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Push $1
sw $1, 0($29)
sub $30, $30, $4
; Push $2
sw $2, -4($29)
sub $30, $30, $4

; Push $29
sw $29, -4($30)
sub $30, $30, $4
; Push $31
sw $31, -4($30)
sub $30, $30, $4
; Code(a)
lw $3, 0($29)
; Push $3
sw $3, -4($30)
sub $30, $30, $4
; Code(b)
lw $3, -4($29)
; Push $3
sw $3, -4($30)
sub $30, $30, $4
lis $5
.word PROCEDUREx
jalr $5
; Pop $31
add $30, $30, $4
lw $31, -4($30)
; Pop $29
add $30, $30, $4
lw $29, -4($30)

add $30, $30, $4
add $30, $30, $4
jr $31
; --------------x--------------
PROCEDUREx:
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
