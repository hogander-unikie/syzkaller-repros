// INFO: task hung in do_ip_vs_set_ctl
// https://syzkaller.appspot.com/bug?id=d2a3376cf530f71e3f3a957f212e9a11f0c7deb5
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

static void execute_one();
extern unsigned long long procid;

void loop()
{
  while (1) {
    execute_one();
  }
}

uint64_t r[1] = {0xffffffffffffffff};
unsigned long long procid;
void execute_one()
{
  long res;
  res = syscall(__NR_socket, 2, 2, 0);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000080 = 2;
  memcpy((void*)0x20000084,
         "\x65\x71\x6c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         16);
  *(uint32_t*)0x20000094 = 0;
  syscall(__NR_setsockopt, r[0], 0, 0x48c, 0x20000080, 0x18);
  *(uint32_t*)0x200000c0 = 2;
  memcpy((void*)0x200000c4,
         "\x73\x69\x74\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         16);
  *(uint32_t*)0x200000d4 = 0;
  syscall(__NR_setsockopt, r[0], 0, 0x48b, 0x200000c0, 0x18);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (procid = 0; procid < 8; procid++) {
    if (fork() == 0) {
      for (;;) {
        loop();
      }
    }
  }
  sleep(1000000);
  return 0;
}
