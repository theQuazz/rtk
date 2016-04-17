.global irq_entry
PIC: .word 0x10140000
irq_entry:
  /* Save user stack & load sp */
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

  /* Call irq handler */
	msr CPSR_c, #0xD3 /* Supervisor mode */
  ldr a1, PIC
  ldr a1, [a1]
  clz a1, a1
	sub a1, a1, #31
  mov lr, pc
  ldr pc, =HandleIrq

	/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
  mov a2, sp

	msr CPSR_c, #0xD2 /* IRQ mode */
  sub a3, lr, #0x4
	mrs a1, SPSR

	msr CPSR_c, #0xD3 /* Supervisor mode */
  mov a4, #0 /* Ran till quantum ran out */
	mov lr, pc
	ldr pc, =SaveTaskState

  /* Jump into kernel */
	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.global svc_entry
svc_entry:
	msr CPSR_c, #0xD3 /* Supervisor mode */

  /* Save current timer value */
  /* TODO optimize with direct ldr */
  push {r0-r9}
  mov lr, pc
  ldr pc, =GetTimerTime
  mov ip, r0
  pop {r0-r9}

  /* Call svc */
  push {r4-r9}
  blx r10
  mov r10, r0
  pop {r4-r9}

	/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
  mov a3, lr
  mov a2, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

  mov a4, ip
	mrs a1, SPSR
	mov lr, pc
	ldr pc, =SaveTaskState

  mov a1, r10
  mov lr, pc
  ldr pc, =SetCurrentTaskReturnValue

  /* Jump into kernel */
	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.type Activate %function
.global Activate
Activate:
	/* Save kernel state */
	mov ip, sp
	push {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

	msr SPSR, a1
  mov lr, a3

	msr CPSR_c, #0xDF /* System mode */
	mov sp, a2
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	msr CPSR_c, #0xD3 /* Supervisor mode */

	movs pc, lr
