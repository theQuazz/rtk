.global svc_entry
svc_entry:
  /* Call svc */
	msr CPSR_c, #0xD3 /* Supervisor mode */
  push {r4-r9}
  blx r10
  mov a4, r0
  pop {r4-r9}

	/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
  mov a3, lr
  mov a2, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mrs a1, SPSR
	mov lr, pc
	ldr pc, =SaveTaskState

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
