// KMSAN: uninit-value in p9_client_rpc
// https://syzkaller.appspot.com/bug?id=a90ca45133088ce07550f7cee0be028ee079c3f4
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

__attribute__((noreturn)) static void doexit(int status)
{
  volatile unsigned i;
  syscall(__NR_exit_group, status);
  for (i = 0;; i++) {
  }
}
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

static void fail(const char* msg, ...)
{
  int e = errno;
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit((e == ENOMEM || e == EAGAIN) ? kRetryStatus : kFailStatus);
}

static void use_temporary_dir()
{
  char tmpdir_template[] = "./syzkaller.XXXXXX";
  char* tmpdir = mkdtemp(tmpdir_template);
  if (!tmpdir)
    fail("failed to mkdtemp");
  if (chmod(tmpdir, 0777))
    fail("failed to chmod");
  if (chdir(tmpdir))
    fail("failed to chdir");
}

static void execute_one();
extern unsigned long long procid;

void loop()
{
  while (1) {
    execute_one();
  }
}

struct thread_t {
  int created, running, call;
  pthread_t th;
};

static struct thread_t threads[16];
static void execute_call(int call);
static int running;

static void* thr(void* arg)
{
  struct thread_t* th = (struct thread_t*)arg;
  for (;;) {
    while (!__atomic_load_n(&th->running, __ATOMIC_ACQUIRE))
      syscall(SYS_futex, &th->running, FUTEX_WAIT, 0, 0);
    execute_call(th->call);
    __atomic_fetch_sub(&running, 1, __ATOMIC_RELAXED);
    __atomic_store_n(&th->running, 0, __ATOMIC_RELEASE);
    syscall(SYS_futex, &th->running, FUTEX_WAKE);
  }
  return 0;
}

static void execute(int num_calls)
{
  int call, thread;
  running = 0;
  for (call = 0; call < num_calls; call++) {
    for (thread = 0; thread < sizeof(threads) / sizeof(threads[0]); thread++) {
      struct thread_t* th = &threads[thread];
      if (!th->created) {
        th->created = 1;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 128 << 10);
        pthread_create(&th->th, &attr, thr, th);
      }
      if (!__atomic_load_n(&th->running, __ATOMIC_ACQUIRE)) {
        th->call = call;
        __atomic_fetch_add(&running, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&th->running, 1, __ATOMIC_RELEASE);
        syscall(SYS_futex, &th->running, FUTEX_WAKE);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 20 * 1000 * 1000;
        syscall(SYS_futex, &th->running, FUTEX_WAIT, 1, &ts);
        if (__atomic_load_n(&running, __ATOMIC_RELAXED))
          usleep((call == num_calls - 1) ? 10000 : 1000);
        break;
      }
    }
  }
}

#ifndef __NR_bpf
#define __NR_bpf 321
#endif

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};
unsigned long long procid;
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    res = syscall(__NR_pipe2, 0x20000100, 0);
    if (res != -1) {
      r[0] = *(uint32_t*)0x20000100;
      r[1] = *(uint32_t*)0x20000104;
    }
    break;
  case 1:
    *(uint32_t*)0x200002c0 = 0;
    *(uint32_t*)0x200002c4 = 6;
    syscall(__NR_setsockopt, -1, 0x84, 0x71, 0x200002c0, 8);
    break;
  case 2:
    memcpy((void*)0x20000340, "./file0", 8);
    syscall(__NR_open, 0x20000340, 0x42, 0);
    break;
  case 3:
    memcpy((void*)0x200000c0, "./file0", 8);
    memcpy((void*)0x20000180, "9p", 3);
    memcpy((void*)0x200001c0, "trans=fd,", 9);
    memcpy((void*)0x200001c9, "rfdno", 5);
    *(uint8_t*)0x200001ce = 0x3d;
    sprintf((char*)0x200001cf, "0x%016llx", (long long)r[0]);
    *(uint8_t*)0x200001e1 = 0x2c;
    memcpy((void*)0x200001e2, "wfdno", 5);
    *(uint8_t*)0x200001e7 = 0x3d;
    sprintf((char*)0x200001e8, "0x%016llx", (long long)r[1]);
    *(uint8_t*)0x200001fa = 0x2c;
    *(uint8_t*)0x200001fb = 0;
    syscall(__NR_mount, 0, 0x200000c0, 0x20000180, 0, 0x200001c0);
    break;
  case 4:
    *(uint32_t*)0x20000140 = 9;
    *(uint8_t*)0x20000144 = 7;
    *(uint16_t*)0x20000145 = 1;
    *(uint16_t*)0x20000147 = 0x24;
    memcpy((void*)0x20000149, "security*+em1nodev(userkeyringcgroup", 36);
    syscall(__NR_write, r[1], 0x20000140, 0x2d);
    break;
  case 5:
    memcpy((void*)0x20000000, "./file0", 8);
    memcpy((void*)0x20000080, "9p", 3);
    memcpy((void*)0x20000380, "trans=fd,", 9);
    memcpy((void*)0x20000389, "rfdno", 5);
    *(uint8_t*)0x2000038e = 0x3d;
    sprintf((char*)0x2000038f, "0x%016llx", (long long)r[0]);
    *(uint8_t*)0x200003a1 = 0x2c;
    memcpy((void*)0x200003a2, "wfdno", 5);
    *(uint8_t*)0x200003a7 = 0x3d;
    sprintf((char*)0x200003a8, "0x%016llx", (long long)r[1]);
    *(uint8_t*)0x200003ba = 0x2c;
    *(uint8_t*)0x200003bb = 0;
    syscall(__NR_mount, 0, 0x20000000, 0x20000080, 0, 0x20000380);
    break;
  case 6:
    *(uint32_t*)0x20000040 = 5;
    *(uint32_t*)0x20000044 = 2;
    *(uint32_t*)0x20000048 = 0xa;
    *(uint32_t*)0x2000004c = 1;
    *(uint32_t*)0x20000050 = 0;
    *(uint32_t*)0x20000054 = -1;
    *(uint32_t*)0x20000058 = 0;
    *(uint8_t*)0x2000005c = 0;
    *(uint8_t*)0x2000005d = 0;
    *(uint8_t*)0x2000005e = 0;
    *(uint8_t*)0x2000005f = 0;
    *(uint8_t*)0x20000060 = 0;
    *(uint8_t*)0x20000061 = 0;
    *(uint8_t*)0x20000062 = 0;
    *(uint8_t*)0x20000063 = 0;
    *(uint8_t*)0x20000064 = 0;
    *(uint8_t*)0x20000065 = 0;
    *(uint8_t*)0x20000066 = 0;
    *(uint8_t*)0x20000067 = 0;
    *(uint8_t*)0x20000068 = 0;
    *(uint8_t*)0x20000069 = 0;
    *(uint8_t*)0x2000006a = 0;
    *(uint8_t*)0x2000006b = 0;
    syscall(__NR_bpf, 0, 0x20000040, 0x2c);
    break;
  }
}

void execute_one()
{
  execute(7);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  char* cwd = get_current_dir_name();
  for (procid = 0; procid < 8; procid++) {
    if (fork() == 0) {
      for (;;) {
        if (chdir(cwd))
          fail("failed to chdir");
        use_temporary_dir();
        loop();
      }
    }
  }
  sleep(1000000);
  return 0;
}
