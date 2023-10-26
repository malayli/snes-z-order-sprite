.include "hdr.asm"

.ramsection "ram.data" bank $7E

myindex DSB 1

.ends

.section ".decrement_function" superfree

; Mandatory: enable 16 bits support
.accu 16
.index 16
.16bit

decrement:
    LDX myindex
    DEX // decrement myindex
    STX myindex
    rtl

.ends
