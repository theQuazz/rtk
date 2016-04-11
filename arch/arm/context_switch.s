.global svc_entry
svc_entry:
	/* Save user state */
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  mov a3, lr
  mov a2, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mrs a1, SPSR
	mov lr, pc
	ldr pc, =save_task_state

  mov a1, r4
  mov a2, r5
  mov a3, r6
  mov a4, r7
  blx r12

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.type activate %function
.global activate
activate:
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
