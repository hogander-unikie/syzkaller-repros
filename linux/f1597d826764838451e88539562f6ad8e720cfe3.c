// KMSAN: uninit-value in strnlen
// https://syzkaller.appspot.com/bug?id=f1597d826764838451e88539562f6ad8e720cfe3
// status:dup
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
  res = syscall(__NR_socket, 0x18, 1, 0);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20000040 = 0x18;
  *(uint32_t*)0x20000042 = 0;
  *(uint16_t*)0x20000046 = 3;
  memcpy((void*)0x20000048, "\x55\xcd\xaa\xbe\xd5\x81", 6);
  memcpy((void*)0x2000004e,
         "\x76\x63\xb6\xe3\xec\x25\x00\x00\x00\x00\x00\x79\x2d\xe2\x94\x00",
         16);
  syscall(__NR_connect, r[0], 0x20000040, 7);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}
