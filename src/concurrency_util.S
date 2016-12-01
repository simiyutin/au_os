

.text
.global switch_thread
switch_thread:
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushfq

    //movq %rsp, (%rdi)
    //movq %rsi, %rsp
    movq %rdi, %rsp

    popfq
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    ret

.global start_thread_handler
start_thread_handler:
    movq %r14, %rdi
    cld
    call %r15
