// KMSAN: uninit-value in bcmp
// https://syzkaller.appspot.com/bug?id=38933ca8abb1b8f0ee10c430f3a6c1f6a68a2519
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

static void setup_common()
{
  if (mount(0, "/sys/fs/fuse/connections", "fusectl", 0, 0)) {
  }
}

static void loop();

static void sandbox_common()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  setsid();
  struct rlimit rlim;
  rlim.rlim_cur = rlim.rlim_max = (200 << 20);
  setrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 32 << 20;
  setrlimit(RLIMIT_MEMLOCK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 136 << 20;
  setrlimit(RLIMIT_FSIZE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_STACK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 256;
  setrlimit(RLIMIT_NOFILE, &rlim);
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
  typedef struct {
    const char* name;
    const char* value;
  } sysctl_t;
  static const sysctl_t sysctls[] = {
      {"/proc/sys/kernel/shmmax", "16777216"},
      {"/proc/sys/kernel/shmall", "536870912"},
      {"/proc/sys/kernel/shmmni", "1024"},
      {"/proc/sys/kernel/msgmax", "8192"},
      {"/proc/sys/kernel/msgmni", "1024"},
      {"/proc/sys/kernel/msgmnb", "1024"},
      {"/proc/sys/kernel/sem", "1024 1048576 500 1024"},
  };
  unsigned i;
  for (i = 0; i < sizeof(sysctls) / sizeof(sysctls[0]); i++)
    write_file(sysctls[i].name, sysctls[i].value);
}

int wait_for_loop(int pid)
{
  if (pid < 0)
    exit(1);
  int status = 0;
  while (waitpid(-1, &status, __WALL) != pid) {
  }
  return WEXITSTATUS(status);
}

static int do_sandbox_none(void)
{
  if (unshare(CLONE_NEWPID)) {
  }
  int pid = fork();
  if (pid != 0)
    return wait_for_loop(pid);
  setup_common();
  sandbox_common();
  if (unshare(CLONE_NEWNET)) {
  }
  loop();
  exit(1);
}

#define SYZ_HAVE_CLOSE_FDS 1
static void close_fds()
{
  int fd;
  for (fd = 3; fd < 30; fd++)
    close(fd);
}

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

void loop(void)
{
  long res = 0;
  res = syscall(__NR_socket, 2, 2, 0x88);
  if (res != -1)
    r[0] = res;
  memcpy((void*)0x200000c0, "/dev/net/tun\000", 13);
  res = syscall(__NR_openat, 0xffffffffffffff9c, 0x200000c0, 0, 0);
  if (res != -1)
    r[1] = res;
  memcpy((void*)0x20000180,
         "nr0\001\000\000\000\000\000\000\000\000\000\000\000\000", 16);
  *(uint16_t*)0x20000190 = 2;
  syscall(__NR_ioctl, r[1], 0x400454ca, 0x20000180);
  syscall(__NR_ioctl, r[1], 0x400454cd, 0x800000030a);
  memcpy((void*)0x20000c40,
         "nr0\001\000`\241\236\371\322\306s\331\241W\034\2716\233\315a\357~Iy:"
         "\341\207\022\354\227\200\351\241S\f\307?\246\225I\v\177\273\323["
         "\027\f\020u\0359\256\266`\330c\344\233\214O;=\255H\220+[-"
         "l\375\n\2757,c\274\365\327\r\363\375M.\215D<"
         "\210\274\016V7\335\202\37445\276\324\336]i<\232x\034\206>"
         "\005\330\246\370h\232[\342\222\026\006\037?\365?\213k9fx\347\272\025^"
         "\371\025-~\377\377\000\000#"
         "1\353\216\261\355U\206\334\370\325\033\312\251\307["
         "\242\357\254M\263\260\271\2316\032D\377,\"\302\253\276\364-"
         "\322N\253\346r3F\246\344l\004\231\242\024B\330\320\r\313W\360\023\377"
         "u\225\355\320\377\ai0\3366u\323A\027\244N\260\344\3702\223m\244NW\344"
         ":>6\275H\322\250["
         "\364\375J\200N\203\362\363\3177\212CZ\365\342\207\324\342s7\264\255"
         "\241\033&!\2302\354k+8Dk;\225\3767q\351\364,"
         "\243\017\262\036\022\360\243\330\274-\205EJ\371\374\300#-"
         "\217\331\tD\213\001\364lY=1\352\034\222de\343ZA\231\a\234<\264\021("
         "\261|\260\037\277[R+\340\375\002\002*"
         "\3327\376\314\024\266\310\310\203\030\203\270Z\021\006\362\370g\002"
         "\004R\237\027\243P\362\r\323\277Q\251\214\375\247\f.68\244\203\257h_"
         "\234\221\301q_|L\021\003\224\300\t=\027\225P\327\315H\0348^"
         "ARL\233\037\366P\rSj\225\331o\003\324\205\226\340\b\277\n\002\213S"
         "\234\354yl\354\023\202Rk\234Az\253\rT\255LO\f\027Y\035g\020\343LL\037"
         "C\372\331\260\373\264\363[\337\320\326\202\366~"
         "0\270\364\260X\376w\275Y\n\326\0205\234\267\345F\301:"
         "9\270\302\205\b\375\222\260k\223\327\3040J\302\360=p\326\343\344W:"
         "\322\366\374\203\261\313\321K\271(\"9(~\364\364\224`"
         "\350\333\027\371\317#)T\315j^\2461\022\221 "
         "\327\222\300\320s\251\344\030:",
         547);
  syscall(__NR_ioctl, r[0], 0x8914, 0x20000c40);
  close_fds();
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  do_sandbox_none();
  return 0;
}