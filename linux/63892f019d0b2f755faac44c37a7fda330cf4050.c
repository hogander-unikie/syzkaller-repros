// KASAN: global-out-of-bounds Read in get_unique_tuple
// https://syzkaller.appspot.com/bug?id=63892f019d0b2f755faac44c37a7fda330cf4050
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
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  *(uint64_t*)0x20000280 = 0;
  *(uint32_t*)0x20000288 = 0;
  *(uint64_t*)0x20000290 = 0x20000240;
  *(uint64_t*)0x20000240 = 0x20000680;
  memcpy((void*)0x20000680,
         "\x70\x00\x00\x00\x00\x01\x19\x05\x03\x01\x02\xf7\xb1\xf8\x10\x52\xa1"
         "\xe6\x00\x00\x24\x00\x01\x00\x14\x00\x01\x00\x08\x00\x01\x00\xff\xff"
         "\xff\xff\x08\x00\x02\x00\xac\x1e\x00\x01\x0c\x00\x02\x00\x05\x00\x01"
         "\x00\x00\x00\x00\x00\x24\x00\x02\x00\x0c\x00\x02\x00\x05\x00\x01\x00"
         "\x00\x00\x00\x00\x14\x00\x01\x00\x08\x00\x01\x00\xe0\x00\x00\x02\x08"
         "\x00\x02\x00\x00\x00\x00\x00\x08\x00\x07\x00\x00\x00\x00\x00\x0c\x00"
         "\x06\x00\x08\x00\x01\x00\x7f\x00\x00\x01\x52\x41\xd9\x92\x8a\x46\x72"
         "\xb0\xc2\x0b\x3a\xca\xeb\xcf\x3e\x6d\xf1\x0d\xe3\xe4\x1b\x31\x76\x36"
         "\xdc\x46\xe4\x7f\x0a\x3f\x1c\xd4\xce\x77\xc7\x6e\x72\xfc\x1d\xdf\x1e"
         "\x3f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xbe"
         "\x09\x02\x48\xe4\xab\x74\x76\x4b\x11\x08\xe1\xe5\xc2\xb8\x39\xef\x2f"
         "\x56\xd5\x76\x0f\x63\x85\x41\x4f\xfa\x7e\xd2\xed\xc8\x46\x94\x6b\x78"
         "\x3a\x01\xe1\x48\xd7\x41\x82\x40\x51\xa6\xbe\x4f\x9e\x36\x7a\xad\xd8"
         "\x61\xca\xf9\x2b\xd7\xa0\x90\x77\x71\xb9\x66\x6d\x5e\x78\x91\xe1\xc4"
         "\x6f\xb9\x17\x64\x38\xae\x53\x0e\x52\x20\x64\x74\x59\x51\x61\x31\x66"
         "\xcf\xcd\x8b\xf4\x36\x6c\x0a\x7f\xdd\x6a",
         265);
  *(uint64_t*)0x20000248 = 0x70;
  *(uint64_t*)0x20000298 = 1;
  *(uint64_t*)0x200002a0 = 0;
  *(uint64_t*)0x200002a8 = 0;
  *(uint32_t*)0x200002b0 = 0;
  syscall(__NR_sendmsg, -1, 0x20000280ul, 0ul);
  res = syscall(__NR_socket, 0x10ul, 3ul, 0xcul);
  if (res != -1)
    r[0] = res;
  *(uint64_t*)0x20000640 = 0;
  *(uint32_t*)0x20000648 = 0;
  *(uint64_t*)0x20000650 = 0x20000200;
  *(uint64_t*)0x20000200 = 0;
  *(uint64_t*)0x20000208 = 0;
  *(uint64_t*)0x20000658 = 8;
  *(uint64_t*)0x20000660 = 0;
  *(uint64_t*)0x20000668 = 0;
  *(uint32_t*)0x20000670 = 0;
  syscall(__NR_sendmsg, r[0], 0x20000640ul, 0ul);
  return 0;
}