// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

unsigned long long procid;

static void sleep_ms(uint64_t ms)
{
  usleep(ms * 1000);
}

static uint64_t current_time_ms(void)
{
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    exit(1);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

#define BITMASK(bf_off, bf_len) (((1ull << (bf_len)) - 1) << (bf_off))
#define STORE_BY_BITMASK(type, htobe, addr, val, bf_off, bf_len)               \
  *(type*)(addr) =                                                             \
      htobe((htobe(*(type*)(addr)) & ~BITMASK((bf_off), (bf_len))) |           \
            (((type)(val) << (bf_off)) & BITMASK((bf_off), (bf_len))))

static bool write_file(const char* file, const char* what, ...)
{
  char buf[1024];
  va_list args;
  va_start(args, what);
  vsnprintf(buf, sizeof(buf), what, args);
  va_end(args);
  buf[sizeof(buf) - 1] = 0;
  int len = strlen(buf);
  int fd = open(file, O_WRONLY | O_CLOEXEC);
  if (fd == -1)
    return false;
  if (write(fd, buf, len) != len) {
    int err = errno;
    close(fd);
    errno = err;
    return false;
  }
  close(fd);
  return true;
}

static void kill_and_wait(int pid, int* status)
{
  kill(-pid, SIGKILL);
  kill(pid, SIGKILL);
  int i;
  for (i = 0; i < 100; i++) {
    if (waitpid(-1, status, WNOHANG | __WALL) == pid)
      return;
    usleep(1000);
  }
  DIR* dir = opendir("/sys/fs/fuse/connections");
  if (dir) {
    for (;;) {
      struct dirent* ent = readdir(dir);
      if (!ent)
        break;
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        continue;
      char abort[300];
      snprintf(abort, sizeof(abort), "/sys/fs/fuse/connections/%s/abort",
               ent->d_name);
      int fd = open(abort, O_WRONLY);
      if (fd == -1) {
        continue;
      }
      if (write(fd, abort, 1) < 0) {
      }
      close(fd);
    }
    closedir(dir);
  } else {
  }
  while (waitpid(-1, status, __WALL) != pid) {
  }
}

static void setup_test()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  write_file("/proc/self/oom_score_adj", "1000");
}

static void execute_one(void);

#define WAIT_FLAGS __WALL

static void loop(void)
{
  int iter;
  for (iter = 0;; iter++) {
    int pid = fork();
    if (pid < 0)
      exit(1);
    if (pid == 0) {
      setup_test();
      execute_one();
      exit(0);
    }
    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      if (waitpid(-1, &status, WNOHANG | WAIT_FLAGS) == pid)
        break;
      sleep_ms(1);
      if (current_time_ms() - start < 5 * 1000)
        continue;
      kill_and_wait(pid, &status);
      break;
    }
  }
}

uint64_t r[3] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

void execute_one(void)
{
  intptr_t res = 0;
  res = syscall(__NR_socket, 0x10ul, 3ul, 0xcul);
  if (res != -1)
    r[0] = res;
  *(uint64_t*)0x20000300 = 0;
  *(uint32_t*)0x20000308 = 0;
  *(uint64_t*)0x20000310 = 0x200002c0;
  *(uint64_t*)0x200002c0 = 0x20000440;
  *(uint32_t*)0x20000440 = 0x54;
  *(uint8_t*)0x20000444 = 2;
  *(uint8_t*)0x20000445 = 6;
  *(uint16_t*)0x20000446 = 1;
  *(uint32_t*)0x20000448 = 0;
  *(uint32_t*)0x2000044c = 0;
  *(uint8_t*)0x20000450 = 0;
  *(uint8_t*)0x20000451 = 0;
  *(uint16_t*)0x20000452 = htobe16(0);
  *(uint16_t*)0x20000454 = 9;
  *(uint16_t*)0x20000456 = 2;
  memcpy((void*)0x20000458, "syz2\000", 5);
  *(uint16_t*)0x20000460 = 0xd;
  *(uint16_t*)0x20000462 = 3;
  memcpy((void*)0x20000464, "hash:net\000", 9);
  *(uint16_t*)0x20000470 = 0xc;
  STORE_BY_BITMASK(uint16_t, , 0x20000472, 7, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x20000473, 0, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x20000473, 1, 7, 1);
  *(uint16_t*)0x20000474 = 8;
  STORE_BY_BITMASK(uint16_t, , 0x20000476, 6, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x20000477, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x20000477, 0, 7, 1);
  *(uint32_t*)0x20000478 = htobe32(3);
  *(uint16_t*)0x2000047c = 5;
  *(uint16_t*)0x2000047e = 1;
  *(uint8_t*)0x20000480 = 7;
  *(uint16_t*)0x20000484 = 5;
  *(uint16_t*)0x20000486 = 4;
  *(uint8_t*)0x20000488 = 0;
  *(uint16_t*)0x2000048c = 5;
  *(uint16_t*)0x2000048e = 5;
  *(uint8_t*)0x20000490 = 2;
  *(uint64_t*)0x200002c8 = 0x54;
  *(uint64_t*)0x20000318 = 1;
  *(uint64_t*)0x20000320 = 0;
  *(uint64_t*)0x20000328 = 0;
  *(uint32_t*)0x20000330 = 0;
  syscall(__NR_sendmsg, r[0], 0x20000300ul, 0ul);
  syscall(__NR_sendmsg, -1, 0ul, 4ul);
  res = syscall(__NR_socket, 0x10ul, 3ul, 0xcul);
  if (res != -1)
    r[1] = res;
  *(uint64_t*)0x20000200 = 0;
  *(uint32_t*)0x20000208 = 0;
  *(uint64_t*)0x20000210 = 0x200001c0;
  *(uint64_t*)0x200001c0 = 0x20000340;
  *(uint32_t*)0x20000340 = 0x70;
  *(uint8_t*)0x20000344 = 0xb;
  *(uint8_t*)0x20000345 = 6;
  *(uint16_t*)0x20000346 = 1;
  *(uint32_t*)0x20000348 = 0;
  *(uint32_t*)0x2000034c = 0;
  *(uint8_t*)0x20000350 = 2;
  *(uint8_t*)0x20000351 = 0;
  *(uint16_t*)0x20000352 = htobe16(0);
  *(uint16_t*)0x20000354 = 9;
  *(uint16_t*)0x20000356 = 2;
  memcpy((void*)0x20000358, "syz2\000", 5);
  *(uint16_t*)0x20000360 = 5;
  *(uint16_t*)0x20000362 = 1;
  *(uint8_t*)0x20000364 = 7;
  *(uint16_t*)0x20000368 = 0x48;
  STORE_BY_BITMASK(uint16_t, , 0x2000036a, 7, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x2000036b, 0, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x2000036b, 1, 7, 1);
  *(uint16_t*)0x2000036c = 0xc;
  STORE_BY_BITMASK(uint16_t, , 0x2000036e, 0x18, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x2000036f, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x2000036f, 0, 7, 1);
  *(uint64_t*)0x20000370 = htobe64(0xc7);
  *(uint16_t*)0x20000378 = 0xc;
  STORE_BY_BITMASK(uint16_t, , 0x2000037a, 0x18, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x2000037b, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x2000037b, 0, 7, 1);
  *(uint64_t*)0x2000037c = htobe64(1);
  *(uint16_t*)0x20000384 = 8;
  STORE_BY_BITMASK(uint16_t, , 0x20000386, 9, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x20000387, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x20000387, 0, 7, 1);
  *(uint32_t*)0x20000388 = htobe32(9);
  *(uint16_t*)0x2000038c = 0x18;
  STORE_BY_BITMASK(uint16_t, , 0x2000038e, 2, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x2000038f, 0, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x2000038f, 1, 7, 1);
  *(uint16_t*)0x20000390 = 0x14;
  STORE_BY_BITMASK(uint16_t, , 0x20000392, 2, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x20000393, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x20000393, 0, 7, 1);
  *(uint8_t*)0x20000394 = 0xfe;
  *(uint8_t*)0x20000395 = 0x80;
  *(uint8_t*)0x20000396 = 0;
  *(uint8_t*)0x20000397 = 0;
  *(uint8_t*)0x20000398 = 0;
  *(uint8_t*)0x20000399 = 0;
  *(uint8_t*)0x2000039a = 0;
  *(uint8_t*)0x2000039b = 0;
  *(uint8_t*)0x2000039c = 0;
  *(uint8_t*)0x2000039d = 0;
  *(uint8_t*)0x2000039e = 0;
  *(uint8_t*)0x2000039f = 0;
  *(uint8_t*)0x200003a0 = 0;
  *(uint8_t*)0x200003a1 = 0;
  *(uint8_t*)0x200003a2 = 0;
  *(uint8_t*)0x200003a3 = 0x16;
  *(uint16_t*)0x200003a4 = 0xc;
  STORE_BY_BITMASK(uint16_t, , 0x200003a6, 0x19, 0, 14);
  STORE_BY_BITMASK(uint16_t, , 0x200003a7, 1, 6, 1);
  STORE_BY_BITMASK(uint16_t, , 0x200003a7, 0, 7, 1);
  *(uint64_t*)0x200003a8 = htobe64(8);
  *(uint64_t*)0x200001c8 = 0x70;
  *(uint64_t*)0x20000218 = 1;
  *(uint64_t*)0x20000220 = 0;
  *(uint64_t*)0x20000228 = 0;
  *(uint32_t*)0x20000230 = 0;
  syscall(__NR_sendmsg, r[1], 0x20000200ul, 0ul);
  syscall(__NR_sendmsg, -1, 0ul, 0ul);
  res = syscall(__NR_socket, 0x10ul, 3ul, 0xcul);
  if (res != -1)
    r[2] = res;
  syscall(__NR_sendmsg, r[2], 0ul, 0ul);
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  for (procid = 0; procid < 4; procid++) {
    if (fork() == 0) {
      loop();
    }
  }
  sleep(1000000);
  return 0;
}