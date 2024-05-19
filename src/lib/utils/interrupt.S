// https://grasslab.github.io/osdi/en/labs/lab3.html
// https://github.com/tianocore/edk2/blob/master/ArmPlatformPkg/PrePeiCore/AArch64/Helper.S
// https://developer.arm.com/documentation/ddi0500/j/System-Control/AArch64-register-descriptions/Hypervisor-Configuration-Register
// https://github.com/rockytriton/LLD/tree/main/rpi_bm/part9
// https://github.com/babbleberry/rpi4-osdev/tree/master/part13-interrupts

.globl interrupt_init
interrupt_init:
    mrs x0, hcr_el2             // Read EL2 Hypervisor Configuration Register
    orr x0, x0, #(1 << 4)       // Enable EL2 IRQ Routing
    msr hcr_el2, x0             // Write back to register

.globl interrupt_enable
interrupt_enable:
    msr daifclr, #2             // Unmasks interrupts
    ret

.globl interrupt_disable
interrupt_disable:
    msr daifset, #2             // Masks interrupts
    ret
