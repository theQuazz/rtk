.global svc_entry
svc_entry:
	/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  mov r6, lr
  mov r5, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mrs r4, SPSR
  push {r4-r6}

  blx r10

  mov a4, r0
  pop {a1-a3}
	mov lr, pc
	ldr pc, =SaveTaskState

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
