// panic: invalid segment range [ADDR, ADDR)
// https://syzkaller.appspot.com/bug?id=dcf23438ebf087bad89dd8f151869db4fbe00e9a
// status:fixed
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
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

static uintptr_t syz_open_procfs(uintptr_t a0, uintptr_t a1)
{

  char buf[128];
  memset(buf, 0, sizeof(buf));
  if (a0 == 0) {
    snprintf(buf, sizeof(buf), "/proc/self/%s", (char*)a1);
  } else if (a0 == (uintptr_t)-1) {
    snprintf(buf, sizeof(buf), "/proc/thread-self/%s", (char*)a1);
  } else {
    snprintf(buf, sizeof(buf), "/proc/self/task/%d/%s", (int)a0, (char*)a1);
  }
  int fd = open(buf, O_RDWR);
  if (fd == -1)
    fd = open(buf, O_RDONLY);
  return fd;
}

static void loop();

static void sandbox_common()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  setsid();

  struct rlimit rlim;
  rlim.rlim_cur = rlim.rlim_max = 160 << 20;
  setrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 8 << 20;
  setrlimit(RLIMIT_MEMLOCK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 136 << 20;
  setrlimit(RLIMIT_FSIZE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_STACK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);

  if (unshare(CLONE_NEWNS)) {
  }
  if (unshare(CLONE_NEWIPC)) {
  }
  if (unshare(0x02000000)) {
  }
  if (unshare(CLONE_NEWUTS)) {
  }
  if (unshare(CLONE_SYSVSEM)) {
  }
}

static int do_sandbox_none(void)
{
  if (unshare(CLONE_NEWPID)) {
  }
  int pid = fork();
  if (pid < 0)
    fail("sandbox fork failed");
  if (pid)
    return pid;

  sandbox_common();
  if (unshare(CLONE_NEWNET)) {
  }
  loop();
  doexit(1);
}

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
  long res = 0;
  memcpy((void*)0x200000c0, "\x2f\x65\x78\x65\x00\x00\x00\x00\x00\x00", 10);
  res = syz_open_procfs(0, 0x200000c0);
  if (res != -1)
    r[0] = res;
  syscall(__NR_ftruncate, r[0], 7);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  for (procid = 0; procid < 4; procid++) {
    if (fork() == 0) {
      for (;;) {
        int pid = do_sandbox_none();
        int status = 0;
        while (waitpid(pid, &status, __WALL) != pid) {
        }
      }
    }
  }
  sleep(1000000);
  return 0;
}
