// KASAN: use-after-free Read in radix_tree_next_chunk
// https://syzkaller.appspot.com/bug?id=a7e7703f43319750ed88a5c0d930d283b51f4e77
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  res = syscall(__NR_socket, 0x10ul, 3ul, 0ul);
  if (res != -1)
    r[0] = res;
  *(uint64_t*)0x20000000 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint64_t*)0x20000010 = 0x20000140;
  *(uint64_t*)0x20000140 = 0x20000040;
  memcpy((void*)0x20000040, "\x3c\x00\x00\x00\x10\x00\x01\x04\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00",
         20);
  *(uint32_t*)0x20000054 = -1;
  memcpy((void*)0x20000058, "\x19\x00\x00\x00\x00\x00\x00\x00\x1c\x00\x12\x00"
                            "\x0c\x00\x01\x00\x62\x72\x69\x64\x67\x65\x00\x00"
                            "\x0c\x00\x02\x00\x08\x00\x05\x00\x01\x00\x00\x00",
         36);
  *(uint64_t*)0x20000148 = 0x3c;
  *(uint64_t*)0x20000018 = 1;
  *(uint64_t*)0x20000020 = 0;
  *(uint64_t*)0x20000028 = 0;
  *(uint32_t*)0x20000030 = 0;
  syscall(__NR_sendmsg, r[0], 0x20000000ul, 0ul);
  res = syscall(__NR_socket, 0x10ul, 3ul, 0ul);
  if (res != -1)
    r[1] = res;
  *(uint64_t*)0x20000140 = 0;
  *(uint32_t*)0x20000148 = 0x3b7;
  *(uint64_t*)0x20000150 = 0x20000000;
  *(uint64_t*)0x20000000 = 0x200000c0;
  memcpy((void*)0x200000c0, "\x3c\x00\x00\x00\x10\x00\x81\x05\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00",
         20);
  *(uint32_t*)0x200000d4 = -1;
  memcpy((void*)0x200000d8, "\x0d\x2f\x25\xd1\x00\x00\x00\x00\x14\x00\x12\x00"
                            "\x0c\x00\x01\x00\x69\x70\x76\x6c\x61\x6e\x00\x00"
                            "\x00\x00\x02\x00\x08\x00\x05\x00",
         32);
  *(uint32_t*)0x200000f8 = -1;
  *(uint64_t*)0x20000008 = 0x3c;
  *(uint64_t*)0x20000158 = 1;
  *(uint64_t*)0x20000160 = 0;
  *(uint64_t*)0x20000168 = 0;
  *(uint32_t*)0x20000170 = 0;
  syscall(__NR_sendmsg, r[1], 0x20000140ul, 0ul);
  return 0;
}