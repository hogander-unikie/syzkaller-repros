// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <endian.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

static long syz_open_dev(long a0, long a1, long a2)
{
  if (a0 == 0xc || a0 == 0xb) {
    char buf[128];
    sprintf(buf, "/dev/%s/%d:%d", a0 == 0xc ? "char" : "block", (uint8_t)a1,
            (uint8_t)a2);
    return open(buf, O_RDWR, 0);
  } else {
    char buf[1024];
    char* hash;
    strncpy(buf, (char*)a0, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    while ((hash = strchr(buf, '#'))) {
      *hash = '0' + (char)(a1 % 10);
      a1 /= 10;
    }
    return open(buf, a2, 0);
  }
}

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  long res = 0;
  memcpy((void*)0x20000480, "/dev/video#", 12);
  res = syz_open_dev(0x20000480, 0x100000007fffffff, 0);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000240 = 0;
  *(uint32_t*)0x20000244 = 7;
  *(uint32_t*)0x20000248 = 2;
  *(uint32_t*)0x20000250 = 1;
  *(uint16_t*)0x20000258 = 0;
  *(uint16_t*)0x2000025a = 0;
  *(uint16_t*)0x2000025c = 0;
  *(uint16_t*)0x2000025e = 0;
  *(uint16_t*)0x20000260 = 0;
  *(uint16_t*)0x20000262 = 0;
  *(uint16_t*)0x20000264 = 0;
  *(uint16_t*)0x20000266 = 0;
  *(uint16_t*)0x20000268 = 0;
  *(uint16_t*)0x2000026a = 0;
  *(uint16_t*)0x2000026c = 0;
  *(uint16_t*)0x2000026e = 0x1000;
  *(uint16_t*)0x20000270 = 0;
  *(uint16_t*)0x20000272 = 0;
  *(uint16_t*)0x20000274 = 0;
  *(uint16_t*)0x20000276 = 0;
  *(uint16_t*)0x20000278 = 0;
  *(uint16_t*)0x2000027a = 0;
  *(uint16_t*)0x2000027c = 0;
  *(uint16_t*)0x2000027e = 0;
  *(uint16_t*)0x20000280 = 0;
  *(uint16_t*)0x20000282 = 0;
  *(uint16_t*)0x20000284 = 0;
  *(uint16_t*)0x20000286 = 0;
  *(uint16_t*)0x20000288 = 0;
  *(uint16_t*)0x2000028a = 0;
  *(uint16_t*)0x2000028c = 0;
  *(uint16_t*)0x2000028e = 0;
  *(uint16_t*)0x20000290 = 0;
  *(uint16_t*)0x20000292 = 0;
  *(uint16_t*)0x20000294 = 0;
  *(uint16_t*)0x20000296 = 0;
  *(uint16_t*)0x20000298 = 0;
  *(uint16_t*)0x2000029a = 0;
  *(uint16_t*)0x2000029c = 0;
  *(uint16_t*)0x2000029e = 0;
  *(uint16_t*)0x200002a0 = 0;
  *(uint16_t*)0x200002a2 = 0;
  *(uint16_t*)0x200002a4 = 0;
  *(uint16_t*)0x200002a6 = 0;
  *(uint16_t*)0x200002a8 = 0;
  *(uint16_t*)0x200002aa = 0;
  *(uint16_t*)0x200002ac = 0;
  *(uint16_t*)0x200002ae = 0;
  *(uint16_t*)0x200002b0 = 0;
  *(uint16_t*)0x200002b2 = 0;
  *(uint16_t*)0x200002b4 = 0;
  *(uint16_t*)0x200002b6 = 0;
  *(uint16_t*)0x200002b8 = 0;
  *(uint32_t*)0x200002bc = 0;
  *(uint32_t*)0x200002c0 = 0;
  *(uint32_t*)0x200002c4 = 0;
  *(uint32_t*)0x20000320 = 0;
  *(uint32_t*)0x20000324 = 0;
  *(uint32_t*)0x20000328 = 0;
  *(uint32_t*)0x2000032c = 0;
  *(uint32_t*)0x20000330 = 0;
  *(uint32_t*)0x20000334 = 0;
  *(uint32_t*)0x20000338 = 0;
  *(uint32_t*)0x2000033c = 0;
  syscall(__NR_ioctl, r[0], 0xc100565c, 0x20000240);
  *(uint32_t*)0x200000c0 = 0;
  *(uint32_t*)0x200000c4 = 0x8000;
  *(uint32_t*)0x200000c8 = 1;
  *(uint32_t*)0x200000d0 = 1;
  *(uint32_t*)0x200000d8 = 0;
  *(uint32_t*)0x200000dc = 0;
  *(uint32_t*)0x200000e0 = 0;
  *(uint32_t*)0x200000e4 = 0;
  *(uint32_t*)0x200000e8 = 0;
  *(uint32_t*)0x200000ec = 0x15182c;
  *(uint32_t*)0x200000f0 = 0;
  *(uint32_t*)0x200000f4 = 0;
  *(uint32_t*)0x200000f8 = 0;
  *(uint32_t*)0x200000fc = 0;
  *(uint32_t*)0x20000100 = 0;
  *(uint32_t*)0x20000104 = 0;
  *(uint32_t*)0x200001a0 = 0;
  *(uint32_t*)0x200001a4 = 0;
  *(uint32_t*)0x200001a8 = 0;
  *(uint32_t*)0x200001ac = 0;
  *(uint32_t*)0x200001b0 = 0;
  *(uint32_t*)0x200001b4 = 0;
  *(uint32_t*)0x200001b8 = 0;
  *(uint32_t*)0x200001bc = 0;
  syscall(__NR_ioctl, r[0], 0xc100565c, 0x200000c0);
  return 0;
}
