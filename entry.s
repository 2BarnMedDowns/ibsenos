.section .text


# Kernel entry point.
# Placed at address 0x2000
.globl _loader
_loader:
    call    _start
    hlt

