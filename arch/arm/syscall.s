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

.type Send %function
.global Send
Send:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r4, [sp, #56]
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

.type AwaitEvent %function
.global AwaitEvent
AwaitEvent:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =AwaitEventHandler
  svc 0

.type SetPriority %function
.global SetPriority
SetPriority:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =SetTaskPriority
  svc 0

.type GetPriority %function
.global GetPriority
GetPriority:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =GetTaskPriority
  svc 0

.type SetMyPriority %function
.global SetMyPriority
SetMyPriority:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =SetCurrentTaskPriority
  svc 0

.type GetMyPriority %function
.global GetMyPriority
GetMyPriority:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =GetCurrentTaskPriority
  svc 0

.type GetTaskStats %function
.global GetTaskStats
GetTaskStats:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =TaskStatsHandler
  svc 0

.type GetTasksStats %function
.global GetTasksStats
GetTasksStats:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =TasksStatsHandler
  svc 0

.type Destroy %function
.global Destroy
Destroy:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
  ldr r10, =DestroyHandler
  svc 0
