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

.type Putc %function
.global Putc
Putc:
  ldr r10, =PutcProxy
  svc 0
  bx lr

.type Getc %function
.global Getc
Getc:
  ldr r10, =GetcProxy
  svc 0
  bx lr

.type Send %function
.global Send
Send:
  ldr r10, =SendProxy
  svc 0
  bx lr

.type Receive %function
.global Receive
Receive:
  ldr r10, =ReceiveProxy
  svc 0
  bx lr

.type Reply %function
.global Reply
Reply:
  ldr r10, =ReplyProxy
  svc 0
  bx lr
