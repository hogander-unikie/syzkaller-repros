// WARNING in xt_compat_add_offset
// https://syzkaller.appspot.com/bug?id=28b6bf730a5e8d288db5c794d5c6ccc49f746d74
// status:fixed
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

#ifndef __NR_mmap
#define __NR_mmap 192
#endif
#ifndef __NR_setsockopt
#define __NR_setsockopt 366
#endif
#ifndef __NR_socket
#define __NR_socket 359
#endif
#undef __NR_mmap
#define __NR_mmap __NR_mmap2

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  intptr_t res = 0;
  res = syscall(__NR_socket, 2, 3, 1);
  if (res != -1)
    r[0] = res;
  memcpy((void*)0x20000000, "filter\000\000\000\000\000\000\000\000\000\000\000"
                            "\000\000\b\000\000\000\000\000\000\000\000\000\000"
                            "\000\000",
         32);
  *(uint32_t*)0x20000020 = 0xe;
  *(uint32_t*)0x20000024 = 0;
  *(uint32_t*)0x20000028 = 0x1ac;
  *(uint32_t*)0x2000002c = 0;
  *(uint32_t*)0x20000030 = 0x200000c0;
  *(uint32_t*)0x20000034 = 0x200000f0;
  *(uint32_t*)0x20000038 = 0x2000023c;
  *(uint32_t*)0x2000003c = 0;
  *(uint32_t*)0x20000040 = 0;
  *(uint32_t*)0x20000044 = 0;
  *(uint32_t*)0x20000048 = 0;
  *(uint32_t*)0x2000004c = 0x200000c0;
  memcpy(
      (void*)0x200000c0,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\xfe\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff"
      "\xff\xff\x01\x00\x00\x00\x19\x00\x00\x00\x00\x00\x00\x00\x00\x00\x76\x65"
      "\x74\x61\x31\x5f\x74\x6f\x5f\x04\x00\x00\x00\x00\x00\x00\x65\x72\x73\x70"
      "\x61\x6e\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x62\x72\x69\x64\x66\x65"
      "\x5f\x73\x6c\x61\x76\x65\x5f\x31\x00\x00\x69\x65\x3e\x84\x19\x01\xe5\x0a"
      "\xc9\x00\x00\x00\x00\x00\x00\x00\x01\x80\xc2\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\xaa\xaa\xaa\xaa\xaa\x00\x00\x00\x00\x00\x00\x00\x00\x00\xd4\x00"
      "\x00\x00\xd4\x00\x00\x00\x1c\x01\x00\x00\x63\x6c\x75\x73\x74\x65\x72\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x72\x65\x61\x6c\x6d\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x6c\x6f\x67\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00"
      "\x00\x00\x00\x46\xae\xfd\xc2\xee\xda\xf8\xff\x6f\xcb\xbf\xa6\x7e\xe3\x1e"
      "\x6b\xfa\x4c\xf9\xf0\x76\x1f\xfd\xac\x96\xc7\xd3\x00\xbc\x1d\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x01\x00\x00\x00\xfe\xff\xff\xff\x00\x00\x00\x00",
      428);
  syscall(__NR_setsockopt, (intptr_t)r[0], 0, 0x80, 0x20000000, 0x1fc);
  return 0;
}