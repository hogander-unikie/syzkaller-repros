// general protection fault in rxrpc_connect_call
// https://syzkaller.appspot.com/bug?id=504c5ba516c3e24d8ecb8f0500b1ee3c122cba4a
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

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  intptr_t res = 0;
  res = syscall(__NR_socket, 0x2000000000000021, 2, 2);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20000040 = 0x21;
  *(uint16_t*)0x20000042 = 0;
  *(uint16_t*)0x20000044 = 2;
  *(uint16_t*)0x20000046 = 0x10;
  *(uint16_t*)0x20000048 = 2;
  *(uint16_t*)0x2000004a = htobe16(0);
  *(uint8_t*)0x2000004c = 0xac;
  *(uint8_t*)0x2000004d = 0x1e;
  *(uint8_t*)0x2000004e = 1;
  *(uint8_t*)0x2000004f = 0;
  syscall(__NR_connect, r[0], 0x20000040, 0x24);
  *(uint64_t*)0x20005c00 = 0;
  *(uint32_t*)0x20005c08 = 0;
  *(uint64_t*)0x20005c10 = 0;
  *(uint64_t*)0x20005c18 = 0;
  *(uint64_t*)0x20005c20 = 0x20000000;
  *(uint64_t*)0x20000000 = 0x18;
  *(uint32_t*)0x20000008 = 0x110;
  *(uint32_t*)0x2000000c = 1;
  memcpy((void*)0x20000010, "w", 1);
  *(uint64_t*)0x20005c28 = 0x18;
  *(uint32_t*)0x20005c30 = 0;
  *(uint32_t*)0x20005c38 = 0;
  syscall(__NR_sendmmsg, r[0], 0x20005c00, 1, 0);
  return 0;
}
