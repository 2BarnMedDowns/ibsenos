.section .text

# Kernel entry point.
.globl _loader
_loader:
    call    _start
    hlt

