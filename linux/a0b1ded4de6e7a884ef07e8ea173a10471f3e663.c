// general protection fault in __vb2_queue_free
// https://syzkaller.appspot.com/bug?id=a0b1ded4de6e7a884ef07e8ea173a10471f3e663
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
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

static __thread int skip_segv;
static __thread jmp_buf segv_env;

static void segv_handler(int sig, siginfo_t* info, void* ctx)
{
  uintptr_t addr = (uintptr_t)info->si_addr;
  const uintptr_t prog_start = 1 << 20;
  const uintptr_t prog_end = 100 << 20;
  if (__atomic_load_n(&skip_segv, __ATOMIC_RELAXED) &&
      (addr < prog_start || addr > prog_end)) {
    _longjmp(segv_env, 1);
  }
  exit(sig);
}

static void install_segv_handler(void)
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  syscall(SYS_rt_sigaction, 0x20, &sa, NULL, 8);
  syscall(SYS_rt_sigaction, 0x21, &sa, NULL, 8);
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = segv_handler;
  sa.sa_flags = SA_NODEFER | SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
}

#define NONFAILING(...)                                                        \
  {                                                                            \
    __atomic_fetch_add(&skip_segv, 1, __ATOMIC_SEQ_CST);                       \
    if (_setjmp(segv_env) == 0) {                                              \
      __VA_ARGS__;                                                             \
    }                                                                          \
    __atomic_fetch_sub(&skip_segv, 1, __ATOMIC_SEQ_CST);                       \
  }

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
  NONFAILING(*(uint32_t*)0x20000040 = 1);
  NONFAILING(*(uint32_t*)0x20000044 = 0x70);
  NONFAILING(*(uint8_t*)0x20000048 = 0);
  NONFAILING(*(uint8_t*)0x20000049 = 0);
  NONFAILING(*(uint8_t*)0x2000004a = 0);
  NONFAILING(*(uint8_t*)0x2000004b = 0);
  NONFAILING(*(uint32_t*)0x2000004c = 0);
  NONFAILING(*(uint64_t*)0x20000050 = 0x50d);
  NONFAILING(*(uint64_t*)0x20000058 = 0);
  NONFAILING(*(uint64_t*)0x20000060 = 0);
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 0, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 1, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 2, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 3, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 4, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 5, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 6, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 7, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 8, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 9, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 10, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 11, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 12, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 13, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 14, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 15, 2));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 17, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 18, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 19, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 20, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 21, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 22, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 23, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 24, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 25, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 26, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 27, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 28, 1));
  NONFAILING(STORE_BY_BITMASK(uint64_t, , 0x20000068, 0, 29, 35));
  NONFAILING(*(uint32_t*)0x20000070 = 0);
  NONFAILING(*(uint32_t*)0x20000074 = 0);
  NONFAILING(*(uint64_t*)0x20000078 = 0);
  NONFAILING(*(uint64_t*)0x20000080 = 0);
  NONFAILING(*(uint64_t*)0x20000088 = 0);
  NONFAILING(*(uint64_t*)0x20000090 = 0);
  NONFAILING(*(uint32_t*)0x20000098 = 0);
  NONFAILING(*(uint32_t*)0x2000009c = 0);
  NONFAILING(*(uint64_t*)0x200000a0 = 0);
  NONFAILING(*(uint32_t*)0x200000a8 = 0);
  NONFAILING(*(uint16_t*)0x200000ac = 0);
  NONFAILING(*(uint16_t*)0x200000ae = 0);
  syscall(__NR_perf_event_open, 0x20000040, 0, 0, -1, 0);
  NONFAILING(memcpy((void*)0x200000c0, "/dev/video35\000", 13));
  res = syscall(__NR_openat, 0xffffffffffffff9c, 0x200000c0, 2, 0);
  if (res != -1)
    r[0] = res;
  NONFAILING(*(uint32_t*)0x20000480 = 0);
  NONFAILING(*(uint32_t*)0x20000484 = 0xfffffffd);
  NONFAILING(*(uint32_t*)0x20000488 = 2);
  NONFAILING(*(uint32_t*)0x20000490 = 2);
  NONFAILING(memcpy(
      (void*)0x20000498,
      "\xc5\xd5\x7e\x14\x09\xc8\xc8\xbf\xc4\xb0\xa6\xa4\x86\xe6\x75\x9e\xc6\xb7"
      "\x17\x94\xff\x0a\xdc\x9e\xe3\x76\xa4\x08\x07\xfd\xd9\x77\x4a\xec\xe3\xe8"
      "\xb9\x5d\xa3\x4e\xe1\x75\xd3\x52\x9b\xc0\x3c\x34\x01\xcb\xd4\x67\xe3\xa0"
      "\x0b\x3f\x56\x5d\x83\xfc\xda\xa1\x8e\x13\x64\x95\x41\x92\xaa\x37\xc8\xb5"
      "\xd5\x2d\xe4\xdf\x78\xb6\xea\x9e\x89\xa2\xc1\x3f\x92\xb9\xf7\x65\x0b\x89"
      "\xfd\xf1\x2c\xa7\xc4\x87\x62\xbc\x81\x64\x6f\x92\xb9\xf2\x27\x0c\x1a\x32"
      "\x4c\x98\x20\xfd\x05\x25\xa8\xc3\x81\xce\xd9\x42\x47\xdf\xa0\x6a\x93\x62"
      "\xcd\xad\xe3\x6c\x5e\xe4\xe0\x46\x32\x88\xdc\xed\x37\x88\x01\xa8\xda\xd7"
      "\x13\x03\x6e\xcf\xa5\x1a\xdd\x03\x33\x7e\x4c\xc1\xd3\xd7\xcd\xea\x59\x9d"
      "\x98\x66\x41\x33\x44\x04\xb6\x7b\xd6\x60\xb0\x34\xeb\xd5\x65\xd3\x06\xb6"
      "\x51\xd8\xb3\xa7\xd0\x52\x56\xfd\x53\x03\x99\x28\x49\x8f\x4a\xa4\x80\x44"
      "\x81\x83",
      200));
  NONFAILING(*(uint32_t*)0x20000560 = 0);
  NONFAILING(*(uint32_t*)0x20000564 = 0);
  NONFAILING(*(uint32_t*)0x20000568 = 0);
  NONFAILING(*(uint32_t*)0x2000056c = 0);
  NONFAILING(*(uint32_t*)0x20000570 = 0);
  NONFAILING(*(uint32_t*)0x20000574 = 0);
  NONFAILING(*(uint32_t*)0x20000578 = 0);
  NONFAILING(*(uint32_t*)0x2000057c = 0);
  syscall(__NR_ioctl, r[0], 0xc100565c, 0x20000480);
  NONFAILING(*(uint32_t*)0x200001c0 = 0);
  NONFAILING(*(uint32_t*)0x200001c4 = 0x7fff);
  NONFAILING(*(uint32_t*)0x200001c8 = 1);
  NONFAILING(*(uint32_t*)0x200001d0 = 2);
  NONFAILING(*(uint32_t*)0x200001d8 = 0xff);
  NONFAILING(*(uint32_t*)0x200001dc = 0xff);
  NONFAILING(*(uint32_t*)0x200001e0 = 5);
  NONFAILING(*(uint32_t*)0x200001e4 = 0x32315241);
  NONFAILING(*(uint32_t*)0x200001e8 = 0xd512);
  NONFAILING(*(uint32_t*)0x200001ec = 0x5a2e00bc);
  NONFAILING(*(uint32_t*)0x200001f0 = 7);
  NONFAILING(*(uint32_t*)0x200001f4 = 0x384024fc);
  NONFAILING(*(uint32_t*)0x200001f8 = 0x13a);
  NONFAILING(*(uint32_t*)0x200001fc = 0);
  NONFAILING(*(uint32_t*)0x20000200 = 0);
  NONFAILING(*(uint32_t*)0x200002a0 = 0);
  NONFAILING(*(uint32_t*)0x200002a4 = 0);
  NONFAILING(*(uint32_t*)0x200002a8 = 0);
  NONFAILING(*(uint32_t*)0x200002ac = 0);
  NONFAILING(*(uint32_t*)0x200002b0 = 0);
  NONFAILING(*(uint32_t*)0x200002b4 = 0);
  NONFAILING(*(uint32_t*)0x200002b8 = 0);
  NONFAILING(*(uint32_t*)0x200002bc = 0);
  syscall(__NR_ioctl, r[0], 0xc100565c, 0x200001c0);
  syscall(__NR_splice, -1, 0, -1, 0, 0x100000000000a, 2);
  NONFAILING(memcpy((void*)0x20000200, "/dev/loop-control\000", 18));
  res = syscall(__NR_openat, 0xffffffffffffff9c, 0x20000200, 0, 0);
  if (res != -1)
    r[1] = res;
  syscall(__NR_openat, 0xffffffffffffff9c, 0, 0, 0);
  syscall(__NR_fcntl, -1, 8, 0);
  syscall(__NR_ioctl, r[1], 0x4c82, 0);
  syscall(__NR_ioctl, -1, 0x40345410, 0);
  res = syscall(__NR_openat, 0xffffffffffffff9c, 0, 0, 0);
  if (res != -1)
    r[2] = res;
  NONFAILING(*(uint32_t*)0x20000340 = 0);
  syscall(__NR_getsockopt, r[2], 0x84, 0x12, 0, 0x20000340);
  syscall(__NR_ioctl, -1, 0x40345410, 0);
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  install_segv_handler();
  for (procid = 0; procid < 6; procid++) {
    if (fork() == 0) {
      loop();
    }
  }
  sleep(1000000);
  return 0;
}
