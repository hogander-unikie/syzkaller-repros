// KASAN: use-after-free Read in ccid_hc_tx_delete
// https://syzkaller.appspot.com/bug?id=3e769c60cb2d1cab692fd541dae957b1fd31bde4
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if_arp.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <linux/futex.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/tcp.h>

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

static void use_temporary_dir(void)
{
  char tmpdir_template[] = "./syzkaller.XXXXXX";
  char* tmpdir = mkdtemp(tmpdir_template);
  if (!tmpdir)
    exit(1);
  if (chmod(tmpdir, 0777))
    exit(1);
  if (chdir(tmpdir))
    exit(1);
}

static void thread_start(void* (*fn)(void*), void* arg)
{
  pthread_t th;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 128 << 10);
  if (pthread_create(&th, &attr, fn, arg))
    exit(1);
  pthread_attr_destroy(&attr);
}

#define BITMASK(bf_off, bf_len) (((1ull << (bf_len)) - 1) << (bf_off))
#define STORE_BY_BITMASK(type, htobe, addr, val, bf_off, bf_len)               \
  *(type*)(addr) =                                                             \
      htobe((htobe(*(type*)(addr)) & ~BITMASK((bf_off), (bf_len))) |           \
            (((type)(val) << (bf_off)) & BITMASK((bf_off), (bf_len))))

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

static void vsnprintf_check(char* str, size_t size, const char* format,
                            va_list args)
{
  int rv;
  rv = vsnprintf(str, size, format, args);
  if (rv < 0)
    exit(1);
  if ((size_t)rv >= size)
    exit(1);
}

#define COMMAND_MAX_LEN 128
#define PATH_PREFIX                                                            \
  "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin "
#define PATH_PREFIX_LEN (sizeof(PATH_PREFIX) - 1)

static void execute_command(bool panic, const char* format, ...)
{
  va_list args;
  char command[PATH_PREFIX_LEN + COMMAND_MAX_LEN];
  int rv;
  va_start(args, format);
  memcpy(command, PATH_PREFIX, PATH_PREFIX_LEN);
  vsnprintf_check(command + PATH_PREFIX_LEN, COMMAND_MAX_LEN, format, args);
  va_end(args);
  rv = system(command);
  if (rv) {
    if (panic)
      exit(1);
  }
}

#define DEV_IPV4 "172.20.20.%d"
#define DEV_IPV6 "fe80::%02hx"
#define DEV_MAC "aa:aa:aa:aa:aa:%02hx"

static void snprintf_check(char* str, size_t size, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf_check(str, size, format, args);
  va_end(args);
}
static void initialize_netdevices(void)
{
  unsigned i;
  const char* devtypes[] = {"ip6gretap", "bridge", "vcan", "bond", "team"};
  const char* devnames[] = {"lo",
                            "sit0",
                            "bridge0",
                            "vcan0",
                            "tunl0",
                            "gre0",
                            "gretap0",
                            "ip_vti0",
                            "ip6_vti0",
                            "ip6tnl0",
                            "ip6gre0",
                            "ip6gretap0",
                            "erspan0",
                            "bond0",
                            "veth0",
                            "veth1",
                            "team0",
                            "veth0_to_bridge",
                            "veth1_to_bridge",
                            "veth0_to_bond",
                            "veth1_to_bond",
                            "veth0_to_team",
                            "veth1_to_team"};
  const char* devmasters[] = {"bridge", "bond", "team"};
  for (i = 0; i < sizeof(devtypes) / (sizeof(devtypes[0])); i++)
    execute_command(0, "ip link add dev %s0 type %s", devtypes[i], devtypes[i]);
  execute_command(0, "ip link add type veth");
  for (i = 0; i < sizeof(devmasters) / (sizeof(devmasters[0])); i++) {
    execute_command(
        0, "ip link add name %s_slave_0 type veth peer name veth0_to_%s",
        devmasters[i], devmasters[i]);
    execute_command(
        0, "ip link add name %s_slave_1 type veth peer name veth1_to_%s",
        devmasters[i], devmasters[i]);
    execute_command(0, "ip link set %s_slave_0 master %s0", devmasters[i],
                    devmasters[i]);
    execute_command(0, "ip link set %s_slave_1 master %s0", devmasters[i],
                    devmasters[i]);
    execute_command(0, "ip link set veth0_to_%s up", devmasters[i]);
    execute_command(0, "ip link set veth1_to_%s up", devmasters[i]);
  }
  execute_command(0, "ip link set bridge_slave_0 up");
  execute_command(0, "ip link set bridge_slave_1 up");
  for (i = 0; i < sizeof(devnames) / (sizeof(devnames[0])); i++) {
    char addr[32];
    snprintf_check(addr, sizeof(addr), DEV_IPV4, i + 10);
    execute_command(0, "ip -4 addr add %s/24 dev %s", addr, devnames[i]);
    snprintf_check(addr, sizeof(addr), DEV_IPV6, i + 10);
    execute_command(0, "ip -6 addr add %s/120 dev %s", addr, devnames[i]);
    snprintf_check(addr, sizeof(addr), DEV_MAC, i + 10);
    execute_command(0, "ip link set dev %s address %s", devnames[i], addr);
    execute_command(0, "ip link set dev %s up", devnames[i]);
  }
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
  rlim.rlim_cur = rlim.rlim_max = 200 << 20;
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
  initialize_netdevices();
  loop();
  exit(1);
}

#define FS_IOC_SETFLAGS _IOW('f', 2, long)
static void remove_dir(const char* dir)
{
  DIR* dp;
  struct dirent* ep;
  int iter = 0;
retry:
  while (umount2(dir, MNT_DETACH) == 0) {
  }
  dp = opendir(dir);
  if (dp == NULL) {
    if (errno == EMFILE) {
      exit(1);
    }
    exit(1);
  }
  while ((ep = readdir(dp))) {
    if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
      continue;
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename), "%s/%s", dir, ep->d_name);
    while (umount2(filename, MNT_DETACH) == 0) {
    }
    struct stat st;
    if (lstat(filename, &st))
      exit(1);
    if (S_ISDIR(st.st_mode)) {
      remove_dir(filename);
      continue;
    }
    int i;
    for (i = 0;; i++) {
      if (unlink(filename) == 0)
        break;
      if (errno == EPERM) {
        int fd = open(filename, O_RDONLY);
        if (fd != -1) {
          long flags = 0;
          if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == 0)
            close(fd);
          continue;
        }
      }
      if (errno == EROFS) {
        break;
      }
      if (errno != EBUSY || i > 100)
        exit(1);
      if (umount2(filename, MNT_DETACH))
        exit(1);
    }
  }
  closedir(dp);
  int i;
  for (i = 0;; i++) {
    if (rmdir(dir) == 0)
      break;
    if (i < 100) {
      if (errno == EPERM) {
        int fd = open(dir, O_RDONLY);
        if (fd != -1) {
          long flags = 0;
          if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == 0)
            close(fd);
          continue;
        }
      }
      if (errno == EROFS) {
        break;
      }
      if (errno == EBUSY) {
        if (umount2(dir, MNT_DETACH))
          exit(1);
        continue;
      }
      if (errno == ENOTEMPTY) {
        if (iter < 100) {
          iter++;
          goto retry;
        }
      }
    }
    exit(1);
  }
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
  for (call = 0; call < 12; call++) {
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
    char cwdbuf[32];
    sprintf(cwdbuf, "./%d", iter);
    if (mkdir(cwdbuf, 0777))
      exit(1);
    int pid = fork();
    if (pid < 0)
      exit(1);
    if (pid == 0) {
      if (chdir(cwdbuf))
        exit(1);
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
    remove_dir(cwdbuf);
  }
}

uint64_t r[3] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

void execute_call(int call)
{
  long res;
  switch (call) {
  case 0:
    syscall(__NR_openat, 0xffffffffffffff9c, 0, 0, 0);
    break;
  case 1:
    syscall(__NR_socket, 0x10, 3, 0);
    break;
  case 2:
    *(uint32_t*)0x2001d000 = 1;
    *(uint32_t*)0x2001d004 = 0x70;
    *(uint8_t*)0x2001d008 = 0;
    *(uint8_t*)0x2001d009 = 0;
    *(uint8_t*)0x2001d00a = 0;
    *(uint8_t*)0x2001d00b = 0;
    *(uint32_t*)0x2001d00c = 0;
    *(uint64_t*)0x2001d010 = 0x7f;
    *(uint64_t*)0x2001d018 = 0;
    *(uint64_t*)0x2001d020 = 0;
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 0, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 1, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 2, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 3, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 4, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0x81, 5, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 6, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 7, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 8, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 9, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 10, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 11, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 12, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 13, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 14, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 15, 2);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 17, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 18, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 19, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 20, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 21, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 22, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 23, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 24, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 25, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 26, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 27, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 28, 1);
    STORE_BY_BITMASK(uint64_t, , 0x2001d028, 0, 29, 35);
    *(uint32_t*)0x2001d030 = 0;
    *(uint32_t*)0x2001d034 = 0;
    *(uint64_t*)0x2001d038 = 0;
    *(uint64_t*)0x2001d040 = 0;
    *(uint64_t*)0x2001d048 = 0;
    *(uint64_t*)0x2001d050 = 0;
    *(uint32_t*)0x2001d058 = 0;
    *(uint32_t*)0x2001d05c = 0;
    *(uint64_t*)0x2001d060 = 0;
    *(uint32_t*)0x2001d068 = 0;
    *(uint16_t*)0x2001d06c = 0;
    *(uint16_t*)0x2001d06e = 0;
    res = syscall(__NR_perf_event_open, 0x2001d000, 0, -1, -1, 0);
    if (res != -1)
      r[0] = res;
    break;
  case 3:
    syscall(__NR_ioctl, r[0], 0x4004240b, 0);
    break;
  case 4:
    res = syscall(__NR_socket, 0xa, 0x40000080806, 0);
    if (res != -1)
      r[1] = res;
    break;
  case 5:
    *(uint16_t*)0x2047b000 = 0xa;
    *(uint16_t*)0x2047b002 = htobe16(0x4e20);
    *(uint32_t*)0x2047b004 = 0;
    *(uint8_t*)0x2047b008 = 0;
    *(uint8_t*)0x2047b009 = 0;
    *(uint8_t*)0x2047b00a = 0;
    *(uint8_t*)0x2047b00b = 0;
    *(uint8_t*)0x2047b00c = 0;
    *(uint8_t*)0x2047b00d = 0;
    *(uint8_t*)0x2047b00e = 0;
    *(uint8_t*)0x2047b00f = 0;
    *(uint8_t*)0x2047b010 = 0;
    *(uint8_t*)0x2047b011 = 0;
    *(uint8_t*)0x2047b012 = 0;
    *(uint8_t*)0x2047b013 = 0;
    *(uint8_t*)0x2047b014 = 0;
    *(uint8_t*)0x2047b015 = 0;
    *(uint8_t*)0x2047b016 = 0;
    *(uint8_t*)0x2047b017 = 0;
    *(uint32_t*)0x2047b018 = 0;
    syscall(__NR_bind, r[1], 0x2047b000, 0x1c);
    break;
  case 6:
    syscall(__NR_listen, r[1], 9);
    break;
  case 7:
    syscall(__NR_setsockopt, -1, 6, 0xd, 0, 0);
    break;
  case 8:
    res = syscall(__NR_socket, 0xa, 6, 0);
    if (res != -1)
      r[2] = res;
    break;
  case 9:
    *(uint16_t*)0x20419000 = 0xa;
    *(uint16_t*)0x20419002 = htobe16(0x4e20);
    *(uint32_t*)0x20419004 = 0;
    *(uint64_t*)0x20419008 = htobe64(0);
    *(uint64_t*)0x20419010 = htobe64(1);
    *(uint32_t*)0x20419018 = 0;
    syscall(__NR_connect, r[2], 0x20419000, 0x1c);
    break;
  case 10:
    syscall(__NR_sendmmsg, -1, 0, 0, 0);
    break;
  case 11:
    syscall(__NR_dup2, r[2], r[1]);
    break;
  }
}
int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  use_temporary_dir();
  do_sandbox_none();
  return 0;
}