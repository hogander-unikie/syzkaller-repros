// general protection fault in __list_add_valid
// https://syzkaller.appspot.com/bug?id=0bf841c341d12f2a41a9c84aca15d4620074f8c2
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

__attribute__((noreturn)) static void doexit(int status)
{
  volatile unsigned i;
  syscall(__NR_exit_group, status);
  for (i = 0;; i++) {
  }
}

__attribute__((noreturn)) static void fail(const char* msg, ...)
{
  int e = errno;
  fflush(stdout);
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit((e == ENOMEM || e == EAGAIN) ? kRetryStatus : kFailStatus);
}

__attribute__((noreturn)) static void exitf(const char* msg, ...)
{
  int e = errno;
  fflush(stdout);
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit(kRetryStatus);
}

static uint64_t current_time_ms()
{
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    fail("clock_gettime failed");
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void test();

void loop()
{
  int iter;
  for (iter = 0;; iter++) {
    int pid = fork();
    if (pid < 0)
      fail("clone failed");
    if (pid == 0) {
      prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
      setpgrp();
      test();
      doexit(0);
    }
    int status = 0;
    uint64_t start = current_time_ms();
    for (;;) {
      int res = waitpid(-1, &status, __WALL | WNOHANG);
      if (res == pid)
        break;
      usleep(1000);
      if (current_time_ms() - start > 5 * 1000) {
        kill(-pid, SIGKILL);
        kill(pid, SIGKILL);
        while (waitpid(-1, &status, __WALL) != pid) {
        }
        break;
      }
    }
  }
}

long r[67];
void* thr(void* arg)
{
  switch ((long)arg) {
  case 0:
    r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                   0xfffffffffffffffful, 0x0ul);
    break;
  case 1:
    r[1] = syscall(__NR_socket, 0x2ul, 0x1ul, 0x0ul);
    break;
  case 2:
    *(uint32_t*)0x20b4cfd0 = (uint32_t)0x1;
    *(uint32_t*)0x20b4cfd4 = (uint32_t)0x2;
    *(uint64_t*)0x20b4cfd8 = (uint64_t)0x20ef3000;
    *(uint64_t*)0x20b4cfe0 = (uint64_t)0x20b4d000;
    *(uint32_t*)0x20b4cfe8 = (uint32_t)0x1;
    *(uint32_t*)0x20b4cfec = (uint32_t)0x80;
    *(uint64_t*)0x20b4cff0 = (uint64_t)0x2000a000;
    *(uint32_t*)0x20b4cff8 = (uint32_t)0x0;
    *(uint32_t*)0x20b4cffc = (uint32_t)0x0;
    *(uint8_t*)0x20ef3000 = (uint8_t)0x8db7;
    *(uint8_t*)0x20ef3001 = (uint8_t)0x0;
    *(uint16_t*)0x20ef3002 = (uint16_t)0x0;
    *(uint32_t*)0x20ef3004 = (uint32_t)0x0;
    *(uint8_t*)0x20ef3008 = (uint8_t)0xd395;
    *(uint8_t*)0x20ef3009 = (uint8_t)0x0;
    *(uint16_t*)0x20ef300a = (uint16_t)0x0;
    *(uint32_t*)0x20ef300c = (uint32_t)0x0;
    memcpy((void*)0x20b4d000, "\x73\x79\x73\x65\x4f\x00", 6);
    r[20] = syscall(__NR_bpf, 0x5ul, 0x20b4cfd0ul, 0x30ul);
    break;
  case 3:
    r[21] = syscall(__NR_socket, 0x29ul, 0x5ul, 0x0ul);
    break;
  case 4:
    r[22] = syscall(__NR_write, r[21], 0x20efd000ul, 0x0ul);
    break;
  case 5:
    *(uint16_t*)0x20305ff0 = (uint16_t)0x2;
    *(uint16_t*)0x20305ff2 = (uint16_t)0x204e;
    *(uint32_t*)0x20305ff4 = (uint32_t)0x100;
    *(uint8_t*)0x20305ff8 = (uint8_t)0x0;
    *(uint8_t*)0x20305ff9 = (uint8_t)0x0;
    *(uint8_t*)0x20305ffa = (uint8_t)0x0;
    *(uint8_t*)0x20305ffb = (uint8_t)0x0;
    *(uint8_t*)0x20305ffc = (uint8_t)0x0;
    *(uint8_t*)0x20305ffd = (uint8_t)0x0;
    *(uint8_t*)0x20305ffe = (uint8_t)0x0;
    *(uint8_t*)0x20305fff = (uint8_t)0x0;
    r[34] = syscall(__NR_bind, r[1], 0x20305ff0ul, 0x10ul);
    break;
  case 6:
    *(uint32_t*)0x20efcff8 = r[1];
    *(uint32_t*)0x20efcffc = r[20];
    r[37] = syscall(__NR_ioctl, r[21], 0x89e0ul, 0x20efcff8ul);
    break;
  case 7:
    *(uint16_t*)0x20ef4000 = (uint16_t)0x2;
    *(uint16_t*)0x20ef4002 = (uint16_t)0x204e;
    *(uint8_t*)0x20ef4004 = (uint8_t)0xac;
    *(uint8_t*)0x20ef4005 = (uint8_t)0x14;
    *(uint8_t*)0x20ef4006 = (uint8_t)0x0;
    *(uint8_t*)0x20ef4007 = (uint8_t)0xaa;
    *(uint8_t*)0x20ef4008 = (uint8_t)0x0;
    *(uint8_t*)0x20ef4009 = (uint8_t)0x0;
    *(uint8_t*)0x20ef400a = (uint8_t)0x0;
    *(uint8_t*)0x20ef400b = (uint8_t)0x0;
    *(uint8_t*)0x20ef400c = (uint8_t)0x0;
    *(uint8_t*)0x20ef400d = (uint8_t)0x0;
    *(uint8_t*)0x20ef400e = (uint8_t)0x0;
    *(uint8_t*)0x20ef400f = (uint8_t)0x0;
    r[52] = syscall(__NR_connect, r[1], 0x20ef4000ul, 0x10ul);
    break;
  case 8:
    *(uint32_t*)0x2076c000 = (uint32_t)0xf;
    *(uint32_t*)0x2076c004 = (uint32_t)0x4;
    *(uint32_t*)0x2076c008 = (uint32_t)0x4;
    *(uint32_t*)0x2076c00c = (uint32_t)0x81;
    *(uint32_t*)0x2076c010 = (uint32_t)0x0;
    *(uint32_t*)0x2076c014 = (uint32_t)0x0;
    *(uint32_t*)0x2076c018 = (uint32_t)0x0;
    r[60] = syscall(__NR_bpf, 0x0ul, 0x2076c000ul, 0x14ul);
    break;
  case 9:
    *(uint32_t*)0x204eefe0 = r[60];
    *(uint64_t*)0x204eefe8 = (uint64_t)0x20fe6000;
    *(uint64_t*)0x204eeff0 = (uint64_t)0x20eb0fff;
    *(uint64_t*)0x204eeff8 = (uint64_t)0x0;
    memcpy((void*)0x20eb0fff, "\x05", 1);
    r[66] = syscall(__NR_bpf, 0x2ul, 0x204eefe0ul, 0x20ul);
    break;
  }
  return 0;
}

void test()
{
  long i;
  pthread_t th[20];

  memset(r, -1, sizeof(r));
  for (i = 0; i < 10; i++) {
    pthread_create(&th[i], 0, thr, (void*)i);
    usleep(rand() % 10000);
  }
  usleep(rand() % 100000);
}

int main()
{
  int i;
  for (i = 0; i < 8; i++) {
    if (fork() == 0) {
      loop();
      return 0;
    }
  }
  sleep(1000000);
  return 0;
}
