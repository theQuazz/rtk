.type Create %function
.global Create
Create:
  ldr r10, =CreateTaskSafe
	svc 0
	bx lr

.type Pass %function
.global Pass
Pass:
  ldr r10, =Nop
	svc 0
	bx lr

.type Exit %function
.global Exit
Exit:
  ldr r10, =ExitTask
  svc 0
  bx lr

.type MyTid %function
.global MyTid
MyTid:
  ldr r10, =GetCurrentTid
  svc 0
  bx lr

.type MyParentTid %function
.global MyParentTid
MyParentTid:
  ldr r10, =GetParentTid
  svc 0
  bx lr
