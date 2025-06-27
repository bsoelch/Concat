format ELF64

section '.text' executable

; parameters order:
; r9	; 6th param
; r8	; 5th param
; r10	; 4th param ( SystemV: rcx )
; rdx	; 3rd param
; rsi	; 2nd param
; rdi	; 1st param
; eax	; syscall_number
; syscall

public sys_read
sys_read:
	mov	eax,0
	syscall
	ret

public sys_write
sys_write:
	mov	eax,1
	syscall
	ret

public sys_open
sys_open:
	mov	eax,2
	syscall
	ret

public sys_close
sys_close:
	mov	eax,3
	syscall
	ret

public sys_mmap
sys_mmap:
  mov r10, rcx
	mov	eax,9
	syscall
	ret

public sys_protect
sys_protect:
	mov	eax,10
	syscall
	ret

public sys_munmap
sys_munmap:
	mov	eax,11
	syscall
	ret

public sys_mremap
sys_mremap:
  mov r10, rcx
	mov	eax,25
	syscall
	ret
	
public sys_clone3
sys_clone3:
	mov	eax,435
	syscall
	ret

public sys_exit
sys_exit:
	mov	eax,60
	syscall
	ret

; helper function returns base pointer of caller
public getBasePointer
getBasePointer:
  mov rax, rbp
  ret
; helper function returns base pointer of caller
public cctThreadInit
cctThreadInit:
  pop rbp
  ret
; try acquiring lock, return zero on success and non-zero on failure
public cctLockTryAcquire
cctLockTryAcquire:
  mov eax, 1
  xchg [rdi], eax
  ret
; lock acquire 
public cctLockAcquire
cctLockAcquire:
  mov eax, 1
  xchg [rdi], eax
  test eax, eax
  jnz .spin
  ret
.spin:
  mov eax, [rdi]
  test eax, eax
  jz cctLockAcquire
  pause ; notify processor that thread is within spinlock
  jmp .spin
; release lock
public cctLockRelease
cctLockRelease:
  xor eax, eax
  xchg [rdi], eax
  xor eax, 1
  ret

