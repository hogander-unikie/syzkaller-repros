// panic: Signal-induced panic
// https://syzkaller.appspot.com/bug?id=2668718853a3764ffe6976db2451c75e34780d67
// status:invalid
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

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  syscall(__NR_clone, 0x2102001ffc, 0, 0x9999999999999999, 0x20000640, -1);
  syscall(__NR_ioctl, -1, 0x8912, 0x400200);
  memcpy((void*)0x20000040, "./file0", 8);
  syscall(__NR_mkdir, 0x20000040, 0);
  memcpy((void*)0x200004c0, "./file0", 8);
  memcpy((void*)0x20000500, "cgroup", 7);
  memcpy((void*)0x20000540, "\x63\x70\x75\x73\x65\x74\x00\x94\xbf\x7f\x03\x18"
                            "\x1f\x5b\xe6\x75\x6b\xb6\xf8\xf4\x22\xb4\x46\x10"
                            "\x1a\x3e\x50\x53\x7f\xed\xf9\xdc\x3b\x60\xf0\x02"
                            "\x8a\x49\x3d\x36\x4b\x66\x92",
         43);
  syscall(__NR_mount, 0x20000480, 0x200004c0, 0x20000500, 0, 0x20000540);
  memcpy((void*)0x20000180, "\x00\x00\x00\x00", 4);
  syscall(__NR_mount, 0x20002240, 0, 0x20000180, 0xb8fe85bbb0f409f2, 0);
  syscall(__NR_socket, 1, 1, 0);
  *(uint16_t*)0x20987ff4 = 1;
  memcpy((void*)0x20987ff6,
         "\xe9\x1f\x71\x89\x59\x1e\x92\x33\x61\x4b\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00",
         108);
  syscall(__NR_connect, -1, 0x20987ff4, 0xc);
  return 0;
}