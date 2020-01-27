// BUG: unable to handle kernel NULL pointer dereference in map_vm_area
// https://syzkaller.appspot.com/bug?id=7813bd36527d8bdae9c16e40b15ca4c2fe1931e5
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
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
#include <string.h>

const int kFailStatus = 67;
const int kRetryStatus = 69;

static void exitf(const char* msg, ...)
{
  int e = errno;
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, " (errno %d)\n", e);
  doexit(kRetryStatus);
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

static int inject_fault(int nth)
{
  int fd;
  char buf[16];

  fd = open("/proc/thread-self/fail-nth", O_RDWR);
  if (fd == -1)
    exitf("failed to open /proc/thread-self/fail-nth");
  sprintf(buf, "%d", nth + 1);
  if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf))
    exitf("failed to write /proc/thread-self/fail-nth");
  return fd;
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

uint64_t r[1] = {0xffffffffffffffff};
unsigned long long procid;
void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    res = syscall(__NR_socket, 0xa, 1, 0);
    if (res != -1)
      r[0] = res;
    break;
  case 1:
    syscall(__NR_ioctl, r[0], 0x4000008912, 0x20000000);
    break;
  case 2:
    memcpy((void*)0x20000440, "big_key", 8);
    *(uint8_t*)0x20000480 = 0x73;
    *(uint8_t*)0x20000481 = 0x79;
    *(uint8_t*)0x20000482 = 0x7a;
    *(uint8_t*)0x20000483 = 0;
    *(uint8_t*)0x20000484 = 0;
    memcpy(
        (void*)0x200004c0,
        "\x72\xf3\x53\x01\xa2\x8f\x8e\x81\x15\x4f\xf1\x05\x65\x0c\xd1\x79\xfd"
        "\xae\x1e\xc4\xd8\xe7\x82\xa6\x85\xaf\xc5\x8f\xe9\xa9\x80\x8c\xf7\x1e"
        "\x59\xe8\x6f\xd4\xdf\x45\xd7\x03\x35\x46\x5e\xb3\x52\x47\x94\x82\x0a"
        "\xc7\xe7\x74\x13\x4e\x43\x46\x3c\xfc\xda\xe5\x10\x8f\x84\x9b\x08\xb2"
        "\x00\x50\xe5\xa7\x84\x02\x11\xe1\x3b\x70\x2b\x36\x94\x42\x79\x58\x64"
        "\xa8\xeb\x34\xb2\xa7\x59\x69\x81\x04\x68\x5e\x73\x31\x70\xb7\x12\x0d"
        "\x8d\x60\x7f\x46\xe3\xb2\x92\x0a\xf4\x5a\x3a\xeb\x25\x45\x01\x7f\xaf"
        "\x8f\x44\xe3\x7f\x08\x4d\x1c\xe3\xd8\xfd\xae\x82\x22\xed\x9d\x45\x75"
        "\xd2\x2a\x81\x56\x2d\xaf\x28\xa1\x41\xc2\x3e\xea\x6b\x6b\x07\x7a\x37"
        "\xf4\x9f\xdb\xca\x0f\xe3\x6b\x5c\xc7\x03\x1f\xe6\x2d\xf1\xf4\xf7\xf1"
        "\x64\x00\xe6\xb9\x82\x64\x13\xf8\x46\xe1\x42\x6d\xdb\xf1\x27\x2c\x70"
        "\x0c\x85\x94\xe4\x1f\xa3\xdf\x20\xaf\x26\xe5\xa8\x65\x58\x3c\x46\x33"
        "\x0d\x22\x5e\x90\x6b\xf3\xbb\x13\x92\x9c\x2a\x2d\xf7\x68\xc8\x68\x4a"
        "\xd1\x39\xee\xb3\x3d\xa5\xcf\xb7\x60\xf7\x25\x84\xfd\xcb\xdc\x3f\xde"
        "\x07\x9d\x4b\x28\x90\x88\xe0\x16\x6d\xc6\x79\x39\x3e\x8b\x0c\xf9\x62"
        "\x17\x18\xc7\x1e\x9d\x5f\x86\xdf\x69\xd0\xe5\x69\xd5\x71\x2f\x93\x5c"
        "\x2c\x37\x17\x70\x92\x69\x49\x1d\x97\xdd\x82\x0b\x84\x3c\x0d\xf0\x80"
        "\xfc\xe0\xf0\x19\x4e\x09\xd1\xd2\x2d\x69\x7e\x1f\x11\xe0\x4a\x6a\xa5"
        "\xec\x47\xba\xfc\xba\xb9\x72\x7c\x9e\xa4\xcb\xfe\x56\x87\xff\x6f\xb7"
        "\x74\x18\x01\xda\xfc\xe4\x97\xd7\x5b\x4d\xe8\xf6\x43\x8c\x03\xdf\x05"
        "\x6a\x77\x65\x35\x10\x0c\x6f\xab\x84\xd2\xd5\xc4\x5a\xd7\x43\xfe\xdc"
        "\x3f\xf0\x58\xd3\xcc\xff\xc3\xe1\xa8\x54\x6c\xee\x79\xce\xc1\xac\x31"
        "\xe0\x16\xe5\x6d\x0b\xd9\x38\x87\x7d\x3a\xd0\x9f\xa0\xab\x30\xa9\x1c"
        "\x0e\x57\xf6\xdc\x36\x72\x5f\x5f\xc2\x89\x27\x06\x1d\x84\x83\x8e\xe9"
        "\x40\x97\x52\x16\x52\xbb\x92\x06\x55\xe8\x86\x56\x1e\x2d\x7d\x4e\xce"
        "\x20\xb7\x00\xbd\x0a\x4c\xd8\xe7\x9e\x03\xdf\x72\x18\x28\x09\xf1\xf7"
        "\x3b\x62\x00\x38\x8a\xc6\x56\x56\xab\x67\x25\xa7\x7a\xcf\xe6\x30\x08"
        "\x05\xe6\x10\xa4\x61\x36\xdf\xe2\x06\xc7\xf9\xe3\x33\x9e\x13\xf3\xae"
        "\x41\x79\xe8\x6e\x82\xa4\x7b\x7a\xb7\xa2\xf1\xc4\xde\x29\xf7\x45\x5c"
        "\xdf\x30\xee\x6f\xd2\xe4\xf1\x07\xf1\x85\x9e\x43\xd3\xc9\x11\xf3\x08"
        "\xd7\x3f\x69\xdd\x73\x90\x53\x51\x81\xac\x89\x07\x72\x38\x42\xb9\x23"
        "\x6c\x0e\xd6\xd2\xa3\x21\x3b\xaa\xb8\x8e\xc8\xcb\xf3\xe3\xe8\x1c\x93"
        "\x98\x77\xf8\x1f\x7c\x24\x48\xf5\x6f\x0c\xde\xa6\x70\xc4\xf7\x2b\xb7"
        "\xfb\x80\xf0\xa6\x4c\x6e\x9c\x11\xa1\x87\x77\x09\x86\x99\x3b\x52\xd5"
        "\xeb\x8d\x98\x70\x4d\xac\x18\x09\x0a\xad\xca\xec\x0f\x64\xa9\xe2\xa2"
        "\x69\xe9\xaf\x38\x65\x43\xa7\xf8\x1c\x96\x34\x10\x68\x36\xdf\x04\x81"
        "\x94\xf9\xf8\x7b\x85\x01\x12\xd4\xb7\x56\x4c\xd2\x72\x78\x9b\x1b\xe6"
        "\x5e\xe6\x76\x08\x98\x5c\xe8\x6d\x62\x35\x4c\x8a\x48\x61\x3d\x96\x8c"
        "\x76\xb8\x19\x1b\xb4\x6a\x8c\x8b\xcf\x11\xbd\xdb\x67\x87\x20\x43\xf7"
        "\x60\x54\x93\xf3\xb7\x1b\x83\xef\xe4\x5d\x92\xd9\x06\x6d\x69\x2a\x13"
        "\xeb\x52\x79\x8d\x87\x7e\xf1\x27\x49\xce\x41\xeb\xfe\x0a\x61\x2f\xf6"
        "\x27\x8a\x39\x04\x25\x69\xc0\x09\x57\x56\x15\xec\x00\x79\xa8\x4f\xd5"
        "\xaa\xcd\x6d\xf4\x93\xc4\xb1\x7c\x9c\xac\xe7\xd4\x36\xbf\xb2\x65\x38"
        "\x55\x22\xc9\xe9\xa1\x10\xb9\x02\xd0\xa4\x0c\x6e\xb6\x5d\x20\x6a\x21"
        "\x5e\x6a\x1a\x29\x30\xc1\x8f\xe6\xe6\xbf\x1d\x29\xb8\x7d\x04\xc1\x54"
        "\x3f\x4b\x8d\xb5\x73\xf2\xca\x1e\xf9\xe6\x7c\x90\xa7\xd3\xaf\xca\x75"
        "\x6c\xea\x38",
        785);
    write_file("/sys/kernel/debug/failslab/ignore-gfp-wait", "N");
    write_file("/sys/kernel/debug/fail_futex/ignore-private", "N");
    inject_fault(8);
    syscall(__NR_add_key, 0x20000440, 0x20000480, 0x200004c0, 0x311,
            0xfffffffb);
    break;
  }
}

void execute_one()
{
  execute(3);
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