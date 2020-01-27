// INFO: task hung in tls_push_record
// https://syzkaller.appspot.com/bug?id=5d6ccc2bd6da7bb1911a3151048f9dc9f93c7475
// status:invalid
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
  res = syscall(__NR_socket, 0xa, 1, 0);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x200000c0 = 1;
  syscall(__NR_setsockopt, r[0], 6, 0x13, 0x200000c0, 0x1d4);
  *(uint16_t*)0x20000080 = 0xa;
  *(uint16_t*)0x20000082 = htobe16(0);
  *(uint32_t*)0x20000084 = 0;
  *(uint8_t*)0x20000088 = 0;
  *(uint8_t*)0x20000089 = 0;
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
  *(uint8_t*)0x20000097 = 0;
  *(uint32_t*)0x20000098 = 0;
  syscall(__NR_connect, r[0], 0x20000080, 0x1c);
  memcpy((void*)0x20006580, "tls", 4);
  syscall(__NR_setsockopt, r[0], 6, 0x1f, 0x20006580, 0x95d59845436eab44);
  *(uint16_t*)0x20000100 = 0x303;
  *(uint16_t*)0x20000102 = 0x33;
  syscall(__NR_setsockopt, r[0], 0x11a, 1, 0x20000100, 0x28);
  memcpy((void*)0x20000200,
         "\xef\xd7\x42\x63\x61\xce\x00\x1a\xff\xbb\x77\xe5\x57\xfd\xee\xa7\xd8"
         "\x15\xd7\x19\xae\x1f\xe1\x97\x42\xb7\x86\xb5\x39\xe3\x0d\xe8\xae\xe9"
         "\x1f\xbf\x01\xa8\x8f\x28\x0f\x99\x8c\x25\xaa\x83\xde\x32\xa9\xc6\x28"
         "\x35\x5a\xd2\xa8\x99\x7b\x67\xa7\x03\x4c\x22\xc5\x75\x44\x4b\x20\x07"
         "\x73\x23\xac\x57\x81\xc8\x66\x3c\xb7\x23\xaa\x44\x16\x2b\x29\x99\xc0"
         "\x14\x5c\xd0\x2e\x42\xfe\xb3\xf4\xeb\x51\xa3\x14\x30\xc8\x65\x5e\x76"
         "\x83\x82\xf7\xd0\x60\xe8\xc2\x85\xd8\xfe\x46\xeb\x9e\x47\x67\x72\xd8"
         "\xe2\xe0\x8c\x6c\x1e\x02\xc2\x78\xce\xca\xfa\xf7\x36\x60\x8e\x1a\xe5"
         "\x74\x77\xfe\xef\x21\x9d\xd4\x66\x0c\x70\x40\x27\x0a\x6d\x59\xb1\x63"
         "\x8e\x05\x0e\xa9\xaa\xb5\x5e\xd9\xb2\x34\xe2\x99\x41\xc6\xba\x78\x80"
         "\x89\xab\xe9\x4e\x37\x13\x01\xbe\xce\x6e",
         180);
  *(uint16_t*)0x20000040 = 0xa;
  *(uint16_t*)0x20000042 = htobe16(0x4e22);
  *(uint32_t*)0x20000044 = 1;
  *(uint8_t*)0x20000048 = -1;
  *(uint8_t*)0x20000049 = 2;
  *(uint8_t*)0x2000004a = 0;
  *(uint8_t*)0x2000004b = 0;
  *(uint8_t*)0x2000004c = 0;
  *(uint8_t*)0x2000004d = 0;
  *(uint8_t*)0x2000004e = 0;
  *(uint8_t*)0x2000004f = 0;
  *(uint8_t*)0x20000050 = 0;
  *(uint8_t*)0x20000051 = 0;
  *(uint8_t*)0x20000052 = 0;
  *(uint8_t*)0x20000053 = 0;
  *(uint8_t*)0x20000054 = 0;
  *(uint8_t*)0x20000055 = 0;
  *(uint8_t*)0x20000056 = 0;
  *(uint8_t*)0x20000057 = 1;
  *(uint32_t*)0x20000058 = 9;
  syscall(__NR_sendto, r[0], 0x20000200, 0xb4, 0, 0x20000040, 0x1c);
  res = syscall(__NR_socket, 0x26, 5, 0);
  if (res != -1)
    r[1] = res;
  *(uint16_t*)0x20000140 = 0x26;
  memcpy((void*)0x20000142,
         "\x61\x65\x61\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 14);
  *(uint32_t*)0x20000150 = 0;
  *(uint32_t*)0x20000154 = 0;
  memcpy((void*)0x20000158,
         "\x70\x63\x72\x79\x70\x74\x28\x67\x63\x6d\x28\x61\x65\x73\x29\x29\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         64);
  syscall(__NR_bind, r[1], 0x20000140, 0x58);
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}