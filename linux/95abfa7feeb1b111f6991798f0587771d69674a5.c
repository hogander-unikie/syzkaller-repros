// BUG: unable to handle kernel NULL pointer dereference in l2tp_session_free
// https://syzkaller.appspot.com/bug?id=95abfa7feeb1b111f6991798f0587771d69674a5
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
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
}

#define SYZ_HAVE_RESET_TEST 1
static void reset_test()
{
  int fd;
  for (fd = 3; fd < 30; fd++)
    close(fd);
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

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

void execute_one(void)
{
  long res = 0;
  res = syscall(__NR_socket, 0xa, 2, 0);
  if (res != -1)
    r[0] = res;
  *(uint16_t*)0x20000080 = 0xa;
  *(uint16_t*)0x20000082 = htobe16(0);
  *(uint32_t*)0x20000084 = 0;
  *(uint8_t*)0x20000088 = 0xfe;
  *(uint8_t*)0x20000089 = 0x80;
  *(uint8_t*)0x2000008a = 0;
  *(uint8_t*)0x2000008b = 0;
  *(uint8_t*)0x2000008c = 0;
  *(uint8_t*)0x2000008d = 0;
  *(uint8_t*)0x2000008e = 0;
  *(uint8_t*)0x2000008f = 0;
  *(uint8_t*)0x20000090 = 0;
  *(uint8_t*)0x20000091 = 0;
  *(uint8_t*)0x20000092 = 0;
  *(uint8_t*)0x20000093 = 0;
  *(uint8_t*)0x20000094 = 0;
  *(uint8_t*)0x20000095 = 0;
  *(uint8_t*)0x20000096 = 0;
  *(uint8_t*)0x20000097 = 0xbb;
  *(uint32_t*)0x20000098 = 0xb;
  syscall(__NR_connect, r[0], 0x20000080, 0x1c);
  res = syscall(__NR_socket, 0x18, 1, 1);
  if (res != -1)
    r[1] = res;
  *(uint16_t*)0x205fafd2 = 0x18;
  *(uint32_t*)0x205fafd4 = 1;
  *(uint32_t*)0x205fafd8 = 0;
  *(uint32_t*)0x205fafdc = r[0];
  *(uint16_t*)0x205fafe0 = 2;
  *(uint16_t*)0x205fafe2 = htobe16(0);
  *(uint32_t*)0x205fafe4 = htobe32(0xe0000002);
  *(uint8_t*)0x205fafe8 = 0;
  *(uint8_t*)0x205fafe9 = 0;
  *(uint8_t*)0x205fafea = 0;
  *(uint8_t*)0x205fafeb = 0;
  *(uint8_t*)0x205fafec = 0;
  *(uint8_t*)0x205fafed = 0;
  *(uint8_t*)0x205fafee = 0;
  *(uint8_t*)0x205fafef = 0;
  *(uint32_t*)0x205faff0 = 4;
  *(uint32_t*)0x205faff4 = 0;
  *(uint32_t*)0x205faff8 = 0;
  *(uint32_t*)0x205faffc = 0;
  syscall(__NR_connect, r[1], 0x205fafd2, 0x2e);
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}
