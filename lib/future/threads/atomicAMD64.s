  .intel_syntax noprefix
  .text
  .globl cctAtomicIncI32
cctAtomicIncI32:
  lock inc DWORD PTR [rdi]
  ret
