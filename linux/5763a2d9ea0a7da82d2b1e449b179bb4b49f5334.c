// WARNING in task_participate_group_stop
// https://syzkaller.appspot.com/bug?id=5763a2d9ea0a7da82d2b1e449b179bb4b49f5334
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
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

#include <linux/futex.h>

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

static void thread_start(void* (*fn)(void*), void* arg)
{
  pthread_t th;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 128 << 10);
  int i;
  for (i = 0; i < 100; i++) {
    if (pthread_create(&th, &attr, fn, arg) == 0) {
      pthread_attr_destroy(&attr);
      return;
    }
    if (errno == EAGAIN) {
      usleep(50);
      continue;
    }
    break;
  }
  exit(1);
}

typedef struct {
  int state;
} event_t;

static void event_init(event_t* ev)
{
  ev->state = 0;
}

static void event_reset(event_t* ev)
{
  ev->state = 0;
}

static void event_set(event_t* ev)
{
  if (ev->state)
    exit(1);
  __atomic_store_n(&ev->state, 1, __ATOMIC_RELEASE);
  syscall(SYS_futex, &ev->state, FUTEX_WAKE | FUTEX_PRIVATE_FLAG);
}

static void event_wait(event_t* ev)
{
  while (!__atomic_load_n(&ev->state, __ATOMIC_ACQUIRE))
    syscall(SYS_futex, &ev->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 0, 0);
}

static int event_isset(event_t* ev)
{
  return __atomic_load_n(&ev->state, __ATOMIC_ACQUIRE);
}

static int event_timedwait(event_t* ev, uint64_t timeout)
{
  uint64_t start = current_time_ms();
  uint64_t now = start;
  for (;;) {
    uint64_t remain = timeout - (now - start);
    struct timespec ts;
    ts.tv_sec = remain / 1000;
    ts.tv_nsec = (remain % 1000) * 1000 * 1000;
    syscall(SYS_futex, &ev->state, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 0, &ts);
    if (__atomic_load_n(&ev->state, __ATOMIC_RELAXED))
      return 1;
    now = current_time_ms();
    if (now - start > timeout)
      return 0;
  }
}

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

#define SYZ_HAVE_SETUP_TEST 1
static void setup_test()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  write_file("/proc/self/oom_score_adj", "1000");
}

#define SYZ_HAVE_RESET_TEST 1
static void reset_test()
{
  int fd;
  for (fd = 3; fd < 30; fd++)
    close(fd);
}

struct thread_t {
  int created, call;
  event_t ready, done;
};

static struct thread_t threads[16];
static void execute_call(int call);
static int running;

static void* thr(void* arg)
{
  struct thread_t* th = (struct thread_t*)arg;
  for (;;) {
    event_wait(&th->ready);
    event_reset(&th->ready);
    execute_call(th->call);
    __atomic_fetch_sub(&running, 1, __ATOMIC_RELAXED);
    event_set(&th->done);
  }
  return 0;
}

static void execute_one(void)
{
  int i, call, thread;
  int collide = 0;
again:
  for (call = 0; call < 17; call++) {
    for (thread = 0; thread < (int)(sizeof(threads) / sizeof(threads[0]));
         thread++) {
      struct thread_t* th = &threads[thread];
      if (!th->created) {
        th->created = 1;
        event_init(&th->ready);
        event_init(&th->done);
        event_set(&th->done);
        thread_start(thr, th);
      }
      if (!event_isset(&th->done))
        continue;
      event_reset(&th->done);
      th->call = call;
      __atomic_fetch_add(&running, 1, __ATOMIC_RELAXED);
      event_set(&th->ready);
      if (collide && (call % 2) == 0)
        break;
      event_timedwait(&th->done, 45);
      break;
    }
  }
  for (i = 0; i < 100 && __atomic_load_n(&running, __ATOMIC_RELAXED); i++)
    sleep_ms(1);
  if (!collide) {
    collide = 1;
    goto again;
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
      reset_test();
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
#ifndef __NR_execveat
#define __NR_execveat 322
#endif
#ifndef __NR_memfd_create
#define __NR_memfd_create 319
#endif

uint64_t r[5] = {0x0, 0xffffffffffffffff, 0xffffffffffffffff, 0x0,
                 0xffffffffffffffff};

void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    syscall(__NR_getsockopt, -1, 0, 0x11, 0, 0);
    break;
  case 1:
    syscall(__NR_unshare, 0x24020400);
    break;
  case 2:
    memcpy((void*)0x200056c0, "./file0\000", 8);
    syscall(__NR_mknod, 0x200056c0, 0x1040, 0);
    break;
  case 3:
    memcpy((void*)0x20f8aff8, "./file0\000", 8);
    syscall(__NR_execve, 0x20f8aff8, 0, 0);
    break;
  case 4:
    res = syscall(__NR_gettid);
    if (res != -1)
      r[0] = res;
    break;
  case 5:
    res = syscall(__NR_socketpair, 1, 5, 0, 0x2000d000);
    if (res != -1) {
      r[1] = *(uint32_t*)0x2000d000;
      r[2] = *(uint32_t*)0x2000d004;
    }
    break;
  case 6:
    *(uint64_t*)0x20008ff8 = 0x3f;
    syscall(__NR_ioctl, r[1], 0x5452, 0x20008ff8);
    break;
  case 7:
    res = syscall(__NR_getpid);
    if (res != -1)
      r[3] = res;
    break;
  case 8:
    memcpy((void*)0x20000000, "./bus\000", 6);
    syscall(__NR_execve, 0x20000000, 0, 0);
    break;
  case 9:
    syscall(__NR_fcntl, r[1], 8, r[3]);
    break;
  case 10:
    syscall(__NR_fcntl, r[1], 0xa, 0x12);
    break;
  case 11:
    syscall(__NR_dup2, r[1], r[2]);
    break;
  case 12:
    syscall(__NR_tkill, r[0], 0x16);
    break;
  case 13:
    memcpy((void*)0x20000040, "\000\000\006\000\000\000\000\000\000\000", 10);
    res = syscall(__NR_memfd_create, 0x20000040, 0);
    if (res != -1)
      r[4] = res;
    break;
  case 14:
    memcpy((void*)0x200003c0,
           "\x7f\x45\x4c\x46\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
           "\x03\x00\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00"
           "\x40\x00\x00\x00\x00\x00\x00\x00\x00\x19\xc9\x54\x05\x00\x07\xef"
           "\xa9\x73\xc9\x15\xf5\x3b\x38\x00\x02\x00\x00\x00\x00\x04\x00\x59"
           "\x38\xab\x4a\xf2\xdc\x3f\x9f\x00\x80\x00\xf8\x00\x00\x00\x00\x00"
           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
           "\x50\xd7\x9c\xea\x9c\x68\x81\x1f\x4d\xbc\x8b\xbc\x33\x6b\x4d\xd0"
           "\xa2\x29\x45\xbe\x3b\xee\xfb\xc3\x8d\xd4\x86\xd9\xb9\x96\x79\x6b"
           "\xd0\xc6\x53\x01\x32\x9e\x68\x30\xa4\x6c\x52\xdb\xdb\x20\xc2\xd7"
           "\x53\x10\xcc\x3b\x9b\x7e\xce\xbc\xeb\x9c\xa0\x11\x0c\x83\x07\x8c"
           "\xc6\x70\x87\xe9\x64\x8b\xf9\x01\x8c\x9b\x39\x79\x42\x11\x57\x41"
           "\xb9\x63\x96\xfe\xd1\x8b\x3c\x4b\x20\xd9\x7c\xa0\x58\x6d\x03\x30"
           "\x2d\x00\xd0\x23\x15\x45\xab\x2f\xa1\xb4\x73\x8e\x51\x0c\x07\x7a"
           "\x61\x59\x68\x42\x9f\x39\x04\x91\x98\xd4\x6f\x36\x71\x8f\x77\x4d"
           "\xbd\x27\x7e\x61\xc9\x74\xff\xaa\x6e\x1a\x05\x5e\x5f\xae\x78\x48"
           "\xbe\x12\x6c\x66\xc4\x7f\xb7\x45\x29\xde\x3b\x0a\x63\xad\x40\xa0"
           "\x43\xad\xed\xf0\x90\x74\xf4\xc2\xf0\xa1\x1e\xd4\x18\x8b\x59\x96"
           "\xd2\x64\x67\x32\x55\xcd\xd5\x98\xcc\xc4\x76\x1b\xd1\x1a\x94\x2c"
           "\x23\x43\x2c\x87\xc8\x7b\xb9\x07\x05\xae\x48\x54\x7f\xed\xba\x8a"
           "\x45\xed\xc1\x7c\x39\x52\x5f\x6c\x9d\x48\x67\x01\xf1",
           317);
    syscall(__NR_write, r[4], 0x200003c0, 0x13d);
    break;
  case 15:
    syscall(__NR_write, r[4], 0, 0);
    break;
  case 16:
    memcpy((void*)0x20000100, "\000", 1);
    syscall(__NR_execveat, r[4], 0x20000100, 0, 0, 0x1000);
    break;
  }
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (procid = 0; procid < 6; procid++) {
    if (fork() == 0) {
      loop();
    }
  }
  sleep(1000000);
  return 0;
}
