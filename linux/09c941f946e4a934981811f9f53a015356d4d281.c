// WARNING: refcount bug in put_watch
// https://syzkaller.appspot.com/bug?id=09c941f946e4a934981811f9f53a015356d4d281
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

#ifndef __NR_watch_devices
#define __NR_watch_devices 436
#endif

uint64_t r[1] = {0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  memcpy((void*)0x20000000, "/dev/watch_queue\000", 17);
  res = syscall(__NR_openat, 0xffffffffffffff9cul, 0x20000000ul, 0ul, 0ul);
  if (res != -1)
    r[0] = res;
  syscall(__NR_watch_devices, r[0], 0x42ul, 0ul);
  return 0;
}
