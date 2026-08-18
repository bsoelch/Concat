  .intel_syntax noprefix
  .text
  .globl cctThreadYield
cctThreadYield:
  pause
  ret
/* try acquiring lock, return zero on success and non-zero on failure */
  .globl cctLockTryAcquire
cctLockTryAcquire:
  mov eax, 1
  xchg [rdi], eax
  ret
/* lock acquire */
  .globl cctLockAcquire
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
  pause /* notify processor that thread is within spinlock */
  jmp .spin
/* release lock */
  .globl cctLockRelease
cctLockRelease:
  xor eax, eax
  xchg [rdi], eax
  xor eax, 1
  ret

