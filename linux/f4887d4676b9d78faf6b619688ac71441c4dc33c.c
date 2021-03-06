// KASAN: use-after-free Read in bpf_test_finish
// https://syzkaller.appspot.com/bug?id=f4887d4676b9d78faf6b619688ac71441c4dc33c
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

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  long res = 0;
  *(uint32_t*)0x20000ac0 = 0xc;
  *(uint32_t*)0x20000ac4 = 0xe;
  *(uint64_t*)0x20000ac8 = 0x20000000;
  memcpy((void*)0x20000000,
         "\xb7\x02\x00\x00\x13\x00\x00\x00\xbf\xa3\x00\x00\x00\x00\x00\x00\x07"
         "\x03\x00\x00\x00\xfe\xff\xff\x7a\x0a\xf0\xff\xf8\xff\xff\xff\x79\xa4"
         "\xf0\xff\x00\x00\x00\x00\xb7\x06\x00\x00\xff\xff\xff\xff\x2d\x64\x05"
         "\x00\x00\x00\x00\x00\x65\x04\x04\x00\x01\x00\x00\x00\x04\x04\x00\x00"
         "\x01\x00\x00\x00\xb7\x03\x00\x00\x00\x00\x00\x00\x6a\x0a\x00\xfe\x00"
         "\x00\x00\x00\x85\x00\x00\x00\x2b\x00\x00\x00\xb7\x00\x00\x00\x00\x00"
         "\x00\x00\x95\x00\x00\x00\x00\x00\x00\x00",
         112);
  *(uint64_t*)0x20000ad0 = 0x200000c0;
  memcpy((void*)0x200000c0, "syzkaller", 10);
  *(uint32_t*)0x20000ad8 = 0;
  *(uint32_t*)0x20000adc = 0;
  *(uint64_t*)0x20000ae0 = 0;
  *(uint32_t*)0x20000ae8 = 0;
  *(uint32_t*)0x20000aec = 0;
  *(uint8_t*)0x20000af0 = 0;
  *(uint8_t*)0x20000af1 = 0;
  *(uint8_t*)0x20000af2 = 0;
  *(uint8_t*)0x20000af3 = 0;
  *(uint8_t*)0x20000af4 = 0;
  *(uint8_t*)0x20000af5 = 0;
  *(uint8_t*)0x20000af6 = 0;
  *(uint8_t*)0x20000af7 = 0;
  *(uint8_t*)0x20000af8 = 0;
  *(uint8_t*)0x20000af9 = 0;
  *(uint8_t*)0x20000afa = 0;
  *(uint8_t*)0x20000afb = 0;
  *(uint8_t*)0x20000afc = 0;
  *(uint8_t*)0x20000afd = 0;
  *(uint8_t*)0x20000afe = 0;
  *(uint8_t*)0x20000aff = 0;
  *(uint32_t*)0x20000b00 = 0;
  *(uint32_t*)0x20000b04 = 0;
  res = syscall(__NR_bpf, 5, 0x20000ac0, 0x48);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000300 = r[0];
  *(uint32_t*)0x20000304 = 0;
  *(uint32_t*)0x20000308 = 0xc5;
  *(uint32_t*)0x2000030c = 0xe2;
  *(uint64_t*)0x20000310 = 0x20000100;
  memcpy((void*)0x20000100,
         "\x54\x00\x05\x6c\xcf\x6a\x32\x14\xe5\xf9\xfc\xbf\xd1\x98\xa2\x0f\x6b"
         "\xc3\x33\xdd\xe7\xfd\x29\x53\x2f\x51\x7a\x8a\xf7\x23\xf5\xf5\x68\x7c"
         "\x80\x8b\x77\xc8\x3f\x18\xda\x26\xdb\x27\x55\xd2\x91\x5e\xf5\x15\x86"
         "\x14\xd5\x36\x46\xb8\xe3\xd9\x7a\xa3\x62\x0c\x42\xd2\xd7\xe0\x5d\xbd"
         "\x91\x71\xe0\xbf\x37\xb7\xf7\xa7\x59\xbd\xc6\xcd\xb9\x4e\x2f\x8b\x95"
         "\x39\x85\xd9\x3c\x0f\xed\xee\xa9\xe4\xa4\x58\xb7\x2c\x8b\x07\x4c\x01"
         "\x79\xe6\x26\x14\x4c\x0b\xd3\x33\x3b\x98\xdb\x94\x90\x16\xd6\xc6\xb7"
         "\x0e\xc6\x1f\xc9\xac\x97\xdd\x31\x0d\xaf\xf6\xbd\x42\xcc\x2d\x19\x84"
         "\x93\x91\x69\x0d\xb0\x24\x09\xc2\x8c\xda\xdd\x74\x23\xcd\x86\x7d\x36"
         "\xff\xba\x42\xfb\x5a\x24\xc9\x9a\xee\x18\x8d\x6e\xb0\xe7\x33\x2a\x9e"
         "\x34\x60\xb6\x4a\xfd\xd6\x08\x0c\xfb\x50\x5a\x13\x4f\x68\x15\xfb\x3e"
         "\x20\x11\x6c\xd4\x38\x94\x97\x09\x34\x5d",
         197);
  *(uint64_t*)0x20000318 = 0x20000200;
  *(uint32_t*)0x20000320 = 0x64a1;
  *(uint32_t*)0x20000324 = 0;
  syscall(__NR_bpf, 0xa, 0x20000300, 0x28);
  return 0;
}
