// KASAN: slab-out-of-bounds Read in tipc_nametbl_lookup_dst_nodes
// https://syzkaller.appspot.com/bug?id=0ac48b55b4d38313c7fe2e32cd63b9b3a802a435
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
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  long res = 0;
  res = syscall(__NR_socket, 0x1e, 5, 0);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20d80f80 = 0x1e;
  memcpy((void*)0x20d80f82,
         "\x01\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09"
         "\xa9\x79\xf3\x21\xb3\x0c\x7b\xc8\x79\x04\x05\xc7\xba\xd6\x2e\x0a\x43"
         "\xa6\x32\xed\x49\x38\xd3\x6d\x73\xfb\x8f\x84\x01\xa3\xff\x59\x82\x9a"
         "\x2b\x0a\xfe\x7c\xe4\x3a\x4b\x24\x70\xa0\xc5\x21\x66\x69\xca\x02\x1f"
         "\x6f\x65\xdc\xf1\x60\xe7\xe5\x8f\x35\x8c\x00\x02\xf0\x00\x01\x58\xd1"
         "\x9b\xcb\x31\xf1\x31\x4a\x8e\xf1\x51\x62\x2c\xa5\xbd\xb9\xc8\xea\xd2"
         "\x00\x00\x77\xae\xb8\x1c\x90\x00\x1d\x6d\x7c\x98\x0e\xe5\x90\xc8\xb9"
         "\xf7\x0d\xc1\x36\xcb\x18\x4a",
         126);
  syscall(__NR_bind, r[0], 0x20d80f80, 0x80);
  res = syscall(__NR_socket, 0x1e, 5, 0);
  if (res != -1)
    r[1] = res;
  *(uint64_t*)0x20316000 = 0x20dd7000;
  *(uint16_t*)0x20dd7000 = 0x1e;
  memcpy((void*)0x20dd7002,
         "\x01\x00\x00\x00\x00\x00\x00\x00\x0c\x00\x00\x01\xe5\x26\xcc\x57\x3c"
         "\x5b\xf8\x6c\x48\x37\x24\xc7\x1e\x14\xdd\x6a\x73\x9e\xff\xea\x1b\x48"
         "\x00\x6b\xe6\x1f\xfe\x06\xd7\x9f\x00\x00\x00\x00\x00\x00\x00\x07\x6c"
         "\x3f\x01\x00\x39\xd8\xf9\x86\xff\x03\x00\x00\x00\x00\x00\x00\xaf\x50"
         "\xd5\xfe\x32\xc4\x19\xd6\x7b\xcb\xc7\xe3\xad\x31\x6a\x19\x83\x56\xed"
         "\x00\x08\x34\x1c\x1f\xd4\x56\x24\x28\x1e\x27\x80\x0e\xce\x70\xb0\x76"
         "\xcf\x97\x9a\xc4\x00\x00\xbd\x76\x7e\x2e\x78\xa1\xdf\xd3\x00\x98\x1a"
         "\x15\x65\xb3\xb1\x6d\x74\x36",
         126);
  *(uint32_t*)0x20316008 = 0x2a;
  *(uint64_t*)0x20316010 = 0;
  *(uint64_t*)0x20316018 = 0;
  *(uint64_t*)0x20316020 = 0;
  *(uint64_t*)0x20316028 = 0;
  *(uint32_t*)0x20316030 = 0;
  syscall(__NR_sendmsg, r[1], 0x20316000, 0);
  return 0;
}