// general protection fault in string
// https://syzkaller.appspot.com/bug?id=a5b80da4b4e7ebf2467ad2c3c4eb83d86f7163e9
// status:dup
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <stdint.h>
#include <string.h>

static uintptr_t syz_open_procfs(uintptr_t a0, uintptr_t a1)
{

  char buf[128];
  memset(buf, 0, sizeof(buf));
  if (a0 == 0) {
    snprintf(buf, sizeof(buf), "/proc/self/%s", (char*)a1);
  } else if (a0 == (uintptr_t)-1) {
    snprintf(buf, sizeof(buf), "/proc/thread-self/%s", (char*)a1);
  } else {
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/%s", (int)a0,
             (char*)a1);
  }
  int fd = open(buf, O_RDWR);
  if (fd == -1)
    fd = open(buf, O_RDONLY);
  return fd;
}

long r[18];
void loop()
{
  memset(r, -1, sizeof(r));
  r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                 0xfffffffffffffffful, 0x0ul);
  *(uint64_t*)0x20000000 = (uint64_t)0x0;
  *(uint32_t*)0x20000008 = (uint32_t)0x27;
  *(uint32_t*)0x2000000c = (uint32_t)0x8;
  *(uint64_t*)0x20000010 = (uint64_t)0x206af000;
  *(uint64_t*)0x20000018 = (uint64_t)0x20baaf11;
  *(uint64_t*)0x20000020 = (uint64_t)0x0;
  *(uint64_t*)0x20000028 = (uint64_t)0x0;
  *(uint64_t*)0x20000030 = (uint64_t)0x0;
  *(uint64_t*)0x20000038 = (uint64_t)0x0;
  *(uint64_t*)0x20000040 = (uint64_t)0x0;
  *(uint64_t*)0x20000048 = (uint64_t)0x0;
  *(uint64_t*)0x20000050 = (uint64_t)0x0;
  *(uint64_t*)0x20000058 = (uint64_t)0x0;
  r[14] = syscall(__NR_timer_create, 0x0ul, 0x20000000ul, 0x20000000ul);
  memcpy((void*)0x20f99000, "\x74\x69\x6d\x65\x72\x73\x00", 7);
  r[16] = syz_open_procfs(0x0ul, 0x20f99000ul);
  r[17] = syscall(__NR_read, r[16], 0x20874000ul, 0x0ul);
}

int main()
{
  loop();
  return 0;
}
