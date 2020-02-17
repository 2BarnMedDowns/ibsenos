.section .text

# Kernel entry point.
.globl _loader
_loader:
    pushl   %ecx
    call    _start
    hlt

