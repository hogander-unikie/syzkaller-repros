// BUG: unable to handle kernel (3)
// https://syzkaller.appspot.com/bug?id=1a237d589845a77a695b488e4f81821420d22bcc
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

#define BITMASK_LEN(type, bf_len) (type)((1ull << (bf_len)) - 1)

#define BITMASK_LEN_OFF(type, bf_off, bf_len)                                  \
  (type)(BITMASK_LEN(type, (bf_len)) << (bf_off))

#define STORE_BY_BITMASK(type, addr, val, bf_off, bf_len)                      \
  if ((bf_off) == 0 && (bf_len) == 0) {                                        \
    *(type*)(addr) = (type)(val);                                              \
  } else {                                                                     \
    type new_val = *(type*)(addr);                                             \
    new_val &= ~BITMASK_LEN_OFF(type, (bf_off), (bf_len));                     \
    new_val |= ((type)(val)&BITMASK_LEN(type, (bf_len))) << (bf_off);          \
    *(type*)(addr) = new_val;                                                  \
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

unsigned long long procid;
void execute_call(int call)
{
  switch (call) {
  case 0:
    *(uint32_t*)0x20c86f88 = 2;
    *(uint32_t*)0x20c86f8c = 0x70;
    *(uint8_t*)0x20c86f90 = 0xf4;
    *(uint8_t*)0x20c86f91 = 0;
    *(uint8_t*)0x20c86f92 = 0;
    *(uint8_t*)0x20c86f93 = 0;
    *(uint32_t*)0x20c86f94 = 0;
    *(uint64_t*)0x20c86f98 = 0;
    *(uint64_t*)0x20c86fa0 = 0;
    *(uint64_t*)0x20c86fa8 = 0;
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 0, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 1, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 2, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 3, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 4, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, -1, 5, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 6, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 7, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 8, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 9, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 10, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 11, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 12, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 13, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 14, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 15, 2);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 17, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 18, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 19, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 20, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 21, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 22, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 23, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 24, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 25, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 26, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 27, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 28, 1);
    STORE_BY_BITMASK(uint64_t, 0x20c86fb0, 0, 29, 35);
    *(uint32_t*)0x20c86fb8 = 0;
    *(uint32_t*)0x20c86fbc = 0;
    *(uint64_t*)0x20c86fc0 = 0x20000000;
    *(uint64_t*)0x20c86fc8 = 0;
    *(uint64_t*)0x20c86fd0 = 0;
    *(uint64_t*)0x20c86fd8 = 0;
    *(uint32_t*)0x20c86fe0 = 0;
    *(uint32_t*)0x20c86fe4 = 0;
    *(uint64_t*)0x20c86fe8 = 0;
    *(uint32_t*)0x20c86ff0 = 0;
    *(uint16_t*)0x20c86ff4 = 0;
    *(uint16_t*)0x20c86ff6 = 0;
    syscall(__NR_perf_event_open, 0x20c86f88, 0, 0, -1, 0);
    break;
  case 1:
    memcpy((void*)0x200000c0, "big_key", 8);
    *(uint8_t*)0x20000100 = 0x73;
    *(uint8_t*)0x20000101 = 0x79;
    *(uint8_t*)0x20000102 = 0x7a;
    *(uint8_t*)0x20000103 = 0;
    *(uint8_t*)0x20000104 = 0;
    memcpy(
        (void*)0x20000140,
        "\xf2\xb7\xbd\x3f\x0f\x47\x43\xf0\xe9\xa9\x49\x24\x8a\xba\x0c\x66\xa6"
        "\x6d\x8b\x0d\x84\x0a\x29\x80\x0b\x73\x1f\x49\x50\xf3\xa0\xf7\xc4\x38"
        "\x25\x00\x3d\x0e\x82\xee\x05\x2a\x46\x47\xaf\xdc\xc5\xf8\xd5\x6f\x67"
        "\x56\x02\x8c\x6b\x3d\xc5\xd7\x05\x49\x67\xab\x9a\x9a\x68\x20\xac\x54"
        "\xc7\xe3\x5d\x53\x94\x51\xd7\xc2\xd0\x73\x0f\x64\x92\xae\xa8\x4c\x94"
        "\x49\x96\x11\x74\x63\xf0\x7d\xed\x57\xbe\x2c\x70\xcd\xff\x19\x7f\xc3"
        "\x6c\x38\x14\x07\x4c\xb2\x10\xaf\xde\x00\xcf\xd3\x9b\xfc\x00\x28\xdc"
        "\x50\x13\xc3\x59\x19\xee\xf4\x59\x05\xa0\xfa\xf3\x2a\xb1\x47\x57\x8a"
        "\x96\x6a\xe9\x5a\x5f\x4a\x21\x48\x75\x2a\xfe\x93\x3e\xf9\x44\xae\xd8"
        "\xfe\xfb\x90\xb8\xe6\x9d\xe3\x3e\xe4\xd9\x8c\x3a\xb3\x33\x7f\x1a\x83"
        "\x53\x48\x32\xbf\xe0\x07\x0a\xa5\x4b\xe3\x7c\xc2\xe3\xbc\x1f\x36\x3d"
        "\x6f\x3f\x33\x07\xbb\xa7\xd6\xb0\xe0\xef\x13\xe4\xa4\x34\xa2\x9f\x9f"
        "\x55\x89\xa5\x2f\xf5\x50\x13\xf0\x8d\xa9\xd0\xce\x4d\xfa\x9b\xf1\xf9"
        "\x71\x83\x12\x87\x3c\x0f\xda\x7b\x6a\x90\xde\x03\x22\xe9\x4a\xa0\x63"
        "\x10\x0e\xc1\x3b\x51\xd9\x36\xce\x72\xb9\x3f\x12\x30\x9d\x1d\x73\x88"
        "\x27\x93\x1a\x13\x78\x80\x47\x0d\xbf\x85\xee\x86\xc5\x9c\xbe\xf9\x3a"
        "\x88\xc2\x23\xd7\xa3\x22\x0a\x8d\xe2\xea\x60\x3d\xc1\xeb\xed\x20\x7d"
        "\x3f\xa8\xf4\x30\x11\xf9\xac\x62\x34\x83\x1b\x68\x82\xf6\x71\xb9\x6a"
        "\x07\x52\x26\x47\xea\x82\x33\xc3\xe2\xc3\x43\x47\x52\x1f\x9c\xd6\x06"
        "\x52\xe7\x34\xcf\xfc\x40\x0d\x63\x5e\x78\x5e\x61\x63\xd7\x55\x6c\x49"
        "\x83\xba\xbd\x48\x64\x64\x83\x2e\xee\xd3\xa0\x41\x86\x04\x3c\xa2\x5e"
        "\x77\x86\x37\x14\x41\x66\x08\x8d\x04\xca\xc4\x50\x1f\x10\xaf\xf1\x1a"
        "\xe2\x9d\xaf\xc3\xfb\xf1\xbd\x4f\xe7\xfe\x82\xe8\xfd\x1f\xc4\x4e\xbb"
        "\x88\x91\x95\x3e\x26\x22\xb3\xe2\x42\x69\x50\x00\x0c\xbe\x0d\x34\x4e"
        "\xa3\x35\xb3\x84\x4f\x64\x04\x6d\x94\x79\x86\x30\xe5\x82\x63\x4e\xc9"
        "\x37\x99\x94\xd2\x87\x9a\xd6\x32\x8c\x69\x41\x0b\xb6\x3d\xb9\x05\x83"
        "\x48\x99\x34\x9f\xe0\x4a\x2f\x93\x3b\xee\x83\x5c\x5e\x24\xb9\xa6\x2a"
        "\xca\x48\xb9\x9a\x72\x3d\x94\x74\x3d\x9b\x0a\xc1\x37\xbf\x8d\xdc\x68"
        "\x28\x61\xb7\x02\xc2\xbf\x74\xa7\xad\xe8\x14\x65\x85\x80\x2c\x8a\x17"
        "\xf0\xb1\xc3\x3f\xfd\x2a\xfb\xad\x06\x7a\x6a\xa7\xb5\xd9\x21\x2c\x61"
        "\xe9\x47\x83\xf7\x37\x67\x54\x8c\xd9\x01\xe9\x86\x54\x73\xbd\x55\xf8"
        "\xeb\x71\x22\x49\x75\x5f\x9a\xd8\x3f\x6d\xa5\x5c\x7b\x8a\x02\xc2\x9d"
        "\x1a\x3f\x73\xbd\xc0\xbc\xd6\x4a\x25\x68\xf3\x29\x7e\xce\xfe\x9f\x85"
        "\xe3\x7a\xc2\x2b\x57\xf5\xc8\xb0\x76\x68\xe3\xe7\x59\x6f\x58\x88\x1e"
        "\xf5\x7e\xb2\x1d\xd3\x09\x7a\x68\xc7\x4f\xa5\xe2\x2b\x34\x24\x19\xa4"
        "\x27\x41\x1a\x68\xa0\x75\x56\x96\x40\xe5\xde\x4d\x54\x1b\x94\x94\x7c"
        "\x90\xc6\xab\xa6\xf2\xad\x88\x0d\xe9\xfa\x3a\x47\xae\x12\x11\xc1\xac"
        "\x87\xf8\x91\xaa\x34\x8b\xb6\xfc\x2d\xfb\x8e\xfd\x75\x1f\x4e\xbd\x73"
        "\x08\xe7\xc8\xf5\x0a\xaa\xcd\x83\x47\xbd\x6d\xea\x3f\xff\xac\x30\x8c"
        "\x80\x5f\xa2\xde\xa6\xd2\xbc\x4e\x4b\x63\x16\xa5\xc7\xef\x1c\x60\xc8"
        "\x9f\x7a\xf8\x83\xf8\x5e\xc9\x96\x1c\x3d\xc6\x1a\x7a\x7c\x93\xd7\xb2"
        "\xba\xb4\x86\xeb\x91\xb3\x0b\xa4\x31\xa5\xe9\x05\x78\xef\x51\xc0\x52"
        "\x35\x58\x49\x12\xa0\xb0\x1d\xcb\x8f\x3b\x17\x5f\xad\x72\xe8\x13\xc0"
        "\x18\xbe\xa2\xbf\x3a\x6d\x30\xbf\x25\x85\xf2\x95\x57\xa2\xa9\xe8\x3c"
        "\xb2\x49\xaa\x2c\x2c\xc1\x53\xb3\xa0\xf1\x97\xed\x63\x81\x10\xe4\x71"
        "\xb4\xd4\xef\xa7\x18\x89\x64\xae\xe0\x85\x95\xed\xfb\xfd\x0b\xe1\x0c"
        "\x69\xf3\xed",
        785);
    write_file("/sys/kernel/debug/failslab/ignore-gfp-wait", "N");
    write_file("/sys/kernel/debug/fail_futex/ignore-private", "N");
    inject_fault(8);
    syscall(__NR_add_key, 0x200000c0, 0x20000100, 0x20000140, 0x311,
            0xfffffffc);
    break;
  }
}

void execute_one()
{
  execute(2);
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
