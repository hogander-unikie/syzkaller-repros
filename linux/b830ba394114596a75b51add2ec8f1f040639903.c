// KASAN: use-after-free Read in _copy_from_iter
// https://syzkaller.appspot.com/bug?id=b830ba394114596a75b51add2ec8f1f040639903
// status:fixed
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

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    memcpy((void*)0x20000100, "./file0", 8);
    syscall(__NR_mknod, 0x20000100, 0, 0);
    break;
  case 1:
    res = syscall(__NR_pipe, 0x20000080);
    if (res != -1) {
      r[0] = *(uint32_t*)0x20000080;
      r[1] = *(uint32_t*)0x20000084;
    }
    break;
  case 2:
    syscall(__NR_write, r[1], 0x200001c0, 0xfffffef3);
    break;
  case 3:
    memcpy((void*)0x20000300, "./file0", 8);
    memcpy((void*)0x20000340, "9p", 3);
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
    syscall(__NR_mount, 0, 0x20000300, 0x20000340, 0, 0x20000380);
    break;
  case 4:
    syscall(__NR_read, r[0], 0x20000200, 0x50c7e3e3);
    break;
  }
}

void loop()
{
  execute(5);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  use_temporary_dir();
  loop();
  return 0;
}