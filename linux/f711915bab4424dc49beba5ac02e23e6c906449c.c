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

static int inject_fault(int nth)
{
  int fd;
  fd = open("/proc/thread-self/fail-nth", O_RDWR);
  if (fd == -1)
    exit(1);
  char buf[16];
  sprintf(buf, "%d", nth + 1);
  if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf))
    exit(1);
  return fd;
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

static void setup_fault()
{
  static struct {
    const char* file;
    const char* val;
    bool fatal;
  } files[] = {
      {"/sys/kernel/debug/failslab/ignore-gfp-wait", "N", true},
      {"/sys/kernel/debug/fail_futex/ignore-private", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-highmem", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-wait", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/min-order", "0", false},
  };
  unsigned i;
  for (i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    if (!write_file(files[i].file, files[i].val)) {
      if (files[i].fatal)
        exit(1);
    }
  }
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

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

void execute_one(void)
{
  *(uint32_t*)0x20000080 = 0xe;
  *(uint32_t*)0x20000084 = 3;
  *(uint64_t*)0x20000088 = 0x20001fd8;
  *(uint8_t*)0x20001fd8 = 0x85;
  STORE_BY_BITMASK(uint8_t, , 0x20001fd9, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, , 0x20001fd9, 0, 4, 4);
  *(uint16_t*)0x20001fda = 0;
  *(uint32_t*)0x20001fdc = 8;
  *(uint8_t*)0x20001fe0 = 0x3d;
  *(uint8_t*)0x20001fe1 = 0;
  *(uint16_t*)0x20001fe2 = 0;
  *(uint32_t*)0x20001fe4 = 0;
  *(uint8_t*)0x20001fe8 = 0x95;
  *(uint8_t*)0x20001fe9 = 0;
  *(uint16_t*)0x20001fea = 0;
  *(uint32_t*)0x20001fec = 0;
  *(uint64_t*)0x20000090 = 0x20000000;
  memcpy((void*)0x20000000, "GPL\000", 4);
  *(uint32_t*)0x20000098 = 5;
  *(uint32_t*)0x2000009c = 0x252;
  *(uint64_t*)0x200000a0 = 0x2000cf3d;
  *(uint32_t*)0x200000a8 = 0;
  *(uint32_t*)0x200000ac = 0;
  *(uint8_t*)0x200000b0 = 0;
  *(uint8_t*)0x200000b1 = 0;
  *(uint8_t*)0x200000b2 = 0;
  *(uint8_t*)0x200000b3 = 0;
  *(uint8_t*)0x200000b4 = 0;
  *(uint8_t*)0x200000b5 = 0;
  *(uint8_t*)0x200000b6 = 0;
  *(uint8_t*)0x200000b7 = 0;
  *(uint8_t*)0x200000b8 = 0;
  *(uint8_t*)0x200000b9 = 0;
  *(uint8_t*)0x200000ba = 0;
  *(uint8_t*)0x200000bb = 0;
  *(uint8_t*)0x200000bc = 0;
  *(uint8_t*)0x200000bd = 0;
  *(uint8_t*)0x200000be = 0;
  *(uint8_t*)0x200000bf = 0;
  *(uint32_t*)0x200000c0 = 0;
  *(uint32_t*)0x200000c4 = 0;
  *(uint32_t*)0x200000c8 = -1;
  *(uint32_t*)0x200000cc = 8;
  *(uint64_t*)0x200000d0 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x200000d8 = 0x262;
  *(uint32_t*)0x200000dc = 0x10;
  *(uint64_t*)0x200000e0 = 0x20000000;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint32_t*)0x2000000c = 0;
  *(uint32_t*)0x200000e8 = 0;
  *(uint32_t*)0x200000ec = 0;
  *(uint32_t*)0x200000f0 = -1;
  inject_fault(21);
  syscall(__NR_bpf, 5ul, 0x20000080ul, 0x48ul);
}
int main(void)
{
  inject_fault(21);
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  setup_fault();
  loop();
  return 0;
}
