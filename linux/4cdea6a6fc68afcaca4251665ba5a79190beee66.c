// KMSAN: uninit-value in __twofish_setkey
// https://syzkaller.appspot.com/bug?id=4cdea6a6fc68afcaca4251665ba5a79190beee66
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

uint64_t r[1] = {0xffffffffffffffff};
void loop()
{
  long res = 0;
  res = syscall(__NR_socket, 0x26, 5, 0);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x200000c0 = 0x26;
  memcpy((void*)0x200000c2,
         "\x68\x61\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 14);
  *(uint32_t*)0x200000d0 = 0;
  *(uint32_t*)0x200000d4 = 0;
  memcpy((void*)0x200000d8,
         "\x78\x63\x62\x63\x28\x74\x77\x6f\x66\x69\x73\x68\x2d\x61\x73\x6d\x29"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         64);
  syscall(__NR_bind, r[0], 0x200000c0, 0x58);
  memcpy((void*)0x20000400, "\xd5\x56\xb8\x95\x29\x89\x92\xea\x54\xd6\x00\x04"
                            "\x00\x2c\x5b\x53\x4c\x90\xc2\x00\x00\x00\x00\x00",
         24);
  syscall(__NR_setsockopt, r[0], 0x117, 1, 0x20000400, 0x18);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}
