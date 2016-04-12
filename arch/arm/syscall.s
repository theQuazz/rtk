.type Create %function
.global Create
Create:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =CreateTaskSafe
	svc 0

.type Pass %function
.global Pass
Pass:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =Nop
	svc 0

.type Exit %function
.global Exit
Exit:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =ExitTask
  svc 0

.type MyTid %function
.global MyTid
MyTid:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =GetCurrentTid
  svc 0

.type MyParentTid %function
.global MyParentTid
MyParentTid:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =GetParentTid
  svc 0

.type Putc %function
.global Putc
Putc:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =PutcProxy
  svc 0

.type Getc %function
.global Getc
Getc:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =GetcProxy
  svc 0

.type Send %function
.global Send
Send:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r4, [sp,#56]
  ldr r10, =SendProxy
  svc 0

.type Receive %function
.global Receive
Receive:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =ReceiveProxy
  svc 0

.type Reply %function
.global Reply
Reply:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =ReplyProxy
  svc 0
