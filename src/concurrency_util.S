

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

.global build_stack
build_stack:
    movq %rsp, %rax
    movq %rdi, %rsp
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushfq
    movq %rax, %rsp

    ret
