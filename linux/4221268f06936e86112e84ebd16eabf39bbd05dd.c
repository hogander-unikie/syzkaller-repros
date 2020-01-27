// KASAN: use-after-free Read in dev_queue_xmit_nit (2)
// https://syzkaller.appspot.com/bug?id=4221268f06936e86112e84ebd16eabf39bbd05dd
// status:invalid
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/if_arp.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/uio.h>
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
#include <setjmp.h>
#include <signal.h>
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

static __thread int skip_segv;
static __thread jmp_buf segv_env;

static void segv_handler(int sig, siginfo_t* info, void* uctx)
{
  uintptr_t addr = (uintptr_t)info->si_addr;
  const uintptr_t prog_start = 1 << 20;
  const uintptr_t prog_end = 100 << 20;
  if (__atomic_load_n(&skip_segv, __ATOMIC_RELAXED) &&
      (addr < prog_start || addr > prog_end)) {
    _longjmp(segv_env, 1);
  }
  doexit(sig);
  for (;;) {
  }
}

static void install_segv_handler()
{
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  syscall(SYS_rt_sigaction, 0x20, &sa, NULL, 8);
  syscall(SYS_rt_sigaction, 0x21, &sa, NULL, 8);

  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = segv_handler;
  sa.sa_flags = SA_NODEFER | SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
}

#define NONFAILING(...)                                                \
  {                                                                    \
    __atomic_fetch_add(&skip_segv, 1, __ATOMIC_SEQ_CST);               \
    if (_setjmp(segv_env) == 0) {                                      \
      __VA_ARGS__;                                                     \
    }                                                                  \
    __atomic_fetch_sub(&skip_segv, 1, __ATOMIC_SEQ_CST);               \
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

static void vsnprintf_check(char* str, size_t size, const char* format,
                            va_list args)
{
  int rv;

  rv = vsnprintf(str, size, format, args);
  if (rv < 0)
    fail("tun: snprintf failed");
  if ((size_t)rv >= size)
    fail("tun: string '%s...' doesn't fit into buffer", str);
}

static void snprintf_check(char* str, size_t size, const char* format,
                           ...)
{
  va_list args;

  va_start(args, format);
  vsnprintf_check(str, size, format, args);
  va_end(args);
}

#define COMMAND_MAX_LEN 128
#define PATH_PREFIX                                                    \
  "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin "
#define PATH_PREFIX_LEN (sizeof(PATH_PREFIX) - 1)

static void execute_command(const char* format, ...)
{
  va_list args;
  char command[PATH_PREFIX_LEN + COMMAND_MAX_LEN];
  int rv;

  va_start(args, format);
  memcpy(command, PATH_PREFIX, PATH_PREFIX_LEN);
  vsnprintf_check(command + PATH_PREFIX_LEN, COMMAND_MAX_LEN, format,
                  args);
  rv = system(command);
  if (rv != 0)
    fail("tun: command \"%s\" failed with code %d", &command[0], rv);

  va_end(args);
}

static int tunfd = -1;
static int tun_frags_enabled;

#define SYZ_TUN_MAX_PACKET_SIZE 1000

#define MAX_PIDS 32
#define ADDR_MAX_LEN 32

#define LOCAL_MAC "aa:aa:aa:aa:aa:%02hx"
#define REMOTE_MAC "bb:bb:bb:bb:bb:%02hx"

#define LOCAL_IPV4 "172.20.%d.170"
#define REMOTE_IPV4 "172.20.%d.187"

#define LOCAL_IPV6 "fe80::%02hxaa"
#define REMOTE_IPV6 "fe80::%02hxbb"

#define IFF_NAPI 0x0010
#define IFF_NAPI_FRAGS 0x0020

static void initialize_tun(uint64_t pid)
{
  if (pid >= MAX_PIDS)
    fail("tun: no more than %d executors", MAX_PIDS);
  int id = pid;

  tunfd = open("/dev/net/tun", O_RDWR | O_NONBLOCK);
  if (tunfd == -1) {
    printf(
        "tun: can't open /dev/net/tun: please enable CONFIG_TUN=y\n");
    printf("otherwise fuzzing or reproducing might not work as "
           "intended\n");
    return;
  }

  char iface[IFNAMSIZ];
  snprintf_check(iface, sizeof(iface), "syz%d", id);

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, iface, IFNAMSIZ);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_NAPI | IFF_NAPI_FRAGS;
  if (ioctl(tunfd, TUNSETIFF, (void*)&ifr) < 0) {
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (ioctl(tunfd, TUNSETIFF, (void*)&ifr) < 0)
      fail("tun: ioctl(TUNSETIFF) failed");
  }
  if (ioctl(tunfd, TUNGETIFF, (void*)&ifr) < 0)
    fail("tun: ioctl(TUNGETIFF) failed");
  tun_frags_enabled = (ifr.ifr_flags & IFF_NAPI_FRAGS) != 0;

  char local_mac[ADDR_MAX_LEN];
  snprintf_check(local_mac, sizeof(local_mac), LOCAL_MAC, id);
  char remote_mac[ADDR_MAX_LEN];
  snprintf_check(remote_mac, sizeof(remote_mac), REMOTE_MAC, id);

  char local_ipv4[ADDR_MAX_LEN];
  snprintf_check(local_ipv4, sizeof(local_ipv4), LOCAL_IPV4, id);
  char remote_ipv4[ADDR_MAX_LEN];
  snprintf_check(remote_ipv4, sizeof(remote_ipv4), REMOTE_IPV4, id);

  char local_ipv6[ADDR_MAX_LEN];
  snprintf_check(local_ipv6, sizeof(local_ipv6), LOCAL_IPV6, id);
  char remote_ipv6[ADDR_MAX_LEN];
  snprintf_check(remote_ipv6, sizeof(remote_ipv6), REMOTE_IPV6, id);

  execute_command("sysctl -w net.ipv6.conf.%s.accept_dad=0", iface);

  execute_command("sysctl -w net.ipv6.conf.%s.router_solicitations=0",
                  iface);

  execute_command("ip link set dev %s address %s", iface, local_mac);
  execute_command("ip addr add %s/24 dev %s", local_ipv4, iface);
  execute_command("ip -6 addr add %s/120 dev %s", local_ipv6, iface);
  execute_command("ip neigh add %s lladdr %s dev %s nud permanent",
                  remote_ipv4, remote_mac, iface);
  execute_command("ip -6 neigh add %s lladdr %s dev %s nud permanent",
                  remote_ipv6, remote_mac, iface);
  execute_command("ip link set dev %s up", iface);
}

static void setup_tun(uint64_t pid, bool enable_tun)
{
  if (enable_tun)
    initialize_tun(pid);
}

static uintptr_t syz_open_dev(uintptr_t a0, uintptr_t a1, uintptr_t a2)
{
  if (a0 == 0xc || a0 == 0xb) {
    char buf[128];
    sprintf(buf, "/dev/%s/%d:%d", a0 == 0xc ? "char" : "block",
            (uint8_t)a1, (uint8_t)a2);
    return open(buf, O_RDWR, 0);
  } else {
    char buf[1024];
    char* hash;
    NONFAILING(strncpy(buf, (char*)a0, sizeof(buf)));
    buf[sizeof(buf) - 1] = 0;
    while ((hash = strchr(buf, '#'))) {
      *hash = '0' + (char)(a1 % 10);
      a1 /= 10;
    }
    return open(buf, a2, 0);
  }
}

static void loop();

static void sandbox_common()
{
  prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);
  setpgrp();
  setsid();

  struct rlimit rlim;
  rlim.rlim_cur = rlim.rlim_max = 128 << 20;
  setrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 8 << 20;
  setrlimit(RLIMIT_MEMLOCK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_FSIZE, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 1 << 20;
  setrlimit(RLIMIT_STACK, &rlim);
  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);

  unshare(CLONE_NEWNS);
  unshare(CLONE_NEWIPC);
  unshare(CLONE_IO);
}

static int do_sandbox_none(int executor_pid, bool enable_tun)
{
  int pid = fork();
  if (pid)
    return pid;

  sandbox_common();
  setup_tun(executor_pid, enable_tun);

  loop();
  doexit(1);
}

static void test();

void loop()
{
  while (1) {
    test();
  }
}

long r[82];
void* thr(void* arg)
{
  switch ((long)arg) {
  case 0:
    r[0] = syscall(__NR_mmap, 0x20000000ul, 0xfff000ul, 0x3ul, 0x32ul,
                   0xfffffffffffffffful, 0x0ul);
    break;
  case 1:
    r[1] = syscall(__NR_socket, 0x2ul, 0x1ul, 0x0ul);
    break;
  case 2:
    NONFAILING(memcpy(
        (void*)0x20c55000,
        "\x2f\x64\x65\x76\x2f\x6e\x65\x74\x2f\x74\x75\x6e\x00", 13));
    r[3] = syz_open_dev(0x20c55000ul, 0x0ul, 0x100000aul);
    break;
  case 3:
    r[4] = syscall(__NR_socket, 0x2000000011ul, 0x3ul, 0x0ul);
    break;
  case 4:
    NONFAILING(*(uint8_t*)0x20927fd8 = (uint8_t)0x73);
    NONFAILING(*(uint8_t*)0x20927fd9 = (uint8_t)0x79);
    NONFAILING(*(uint8_t*)0x20927fda = (uint8_t)0x7a);
    NONFAILING(*(uint8_t*)0x20927fdb = (uint8_t)0x30);
    NONFAILING(*(uint8_t*)0x20927fdc = (uint8_t)0x0);
    NONFAILING(*(uint32_t*)0x20927fe8 = (uint32_t)0x5);
    NONFAILING(*(uint32_t*)0x20927fec = (uint32_t)0x0);
    NONFAILING(*(uint64_t*)0x20927ff0 = (uint64_t)0x201a3ff4);
    NONFAILING(*(uint32_t*)0x201a3ff4 = (uint32_t)0x0);
    NONFAILING(*(uint32_t*)0x201a3ff8 = (uint32_t)0x0);
    NONFAILING(*(uint16_t*)0x201a3ffc = (uint16_t)0x0);
    r[16] = syscall(__NR_ioctl, r[3], 0x400454caul, 0x20927fd8ul);
    break;
  case 5:
    NONFAILING(*(uint8_t*)0x2062ffe0 = (uint8_t)0x73);
    NONFAILING(*(uint8_t*)0x2062ffe1 = (uint8_t)0x79);
    NONFAILING(*(uint8_t*)0x2062ffe2 = (uint8_t)0x7a);
    NONFAILING(*(uint8_t*)0x2062ffe3 = (uint8_t)0x30);
    NONFAILING(*(uint8_t*)0x2062ffe4 = (uint8_t)0x0);
    NONFAILING(*(uint16_t*)0x2062fff0 = (uint16_t)0x20301);
    r[23] = syscall(__NR_ioctl, r[1], 0x8914ul, 0x2062ffe0ul);
    break;
  case 6:
    NONFAILING(*(uint8_t*)0x20b13000 = (uint8_t)0x73);
    NONFAILING(*(uint8_t*)0x20b13001 = (uint8_t)0x79);
    NONFAILING(*(uint8_t*)0x20b13002 = (uint8_t)0x7a);
    NONFAILING(*(uint8_t*)0x20b13003 = (uint8_t)0x30);
    NONFAILING(*(uint8_t*)0x20b13004 = (uint8_t)0x0);
    NONFAILING(*(uint32_t*)0x20b13010 = (uint32_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13014 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13015 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13016 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13017 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13018 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13019 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301a = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301b = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301c = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301d = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301e = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b1301f = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13020 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13021 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13022 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13023 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13024 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13025 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13026 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20b13027 = (uint8_t)0x0);
    r[50] = syscall(__NR_ioctl, r[4], 0x8933ul, 0x20b13000ul);
    if (r[50] != -1)
      NONFAILING(r[51] = *(uint32_t*)0x20b13010);
    break;
  case 7:
    NONFAILING(*(uint16_t*)0x20c85000 = (uint16_t)0x11);
    NONFAILING(*(uint16_t*)0x20c85002 = (uint16_t)0x300);
    NONFAILING(*(uint32_t*)0x20c85004 = r[51]);
    NONFAILING(*(uint16_t*)0x20c85008 = (uint16_t)0x1);
    NONFAILING(*(uint8_t*)0x20c8500a = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20c8500b = (uint8_t)0x6);
    NONFAILING(*(uint8_t*)0x20c8500c = (uint8_t)0xbb);
    NONFAILING(*(uint8_t*)0x20c8500d = (uint8_t)0xbb);
    NONFAILING(*(uint8_t*)0x20c8500e = (uint8_t)0xbb);
    NONFAILING(*(uint8_t*)0x20c8500f = (uint8_t)0xbb);
    NONFAILING(*(uint8_t*)0x20c85010 = (uint8_t)0xbb);
    NONFAILING(*(uint8_t*)0x20c85011 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20c85012 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20c85013 = (uint8_t)0x0);
    r[66] = syscall(__NR_bind, r[4], 0x20c85000ul, 0x14ul);
    break;
  case 8:
    NONFAILING(*(uint16_t*)0x20041000 = (uint16_t)0x11);
    NONFAILING(*(uint16_t*)0x20041002 = (uint16_t)0x1700);
    NONFAILING(*(uint32_t*)0x20041004 = r[51]);
    NONFAILING(*(uint16_t*)0x20041008 = (uint16_t)0x1);
    NONFAILING(*(uint8_t*)0x2004100a = (uint8_t)0x7);
    NONFAILING(*(uint8_t*)0x2004100b = (uint8_t)0x6);
    NONFAILING(*(uint8_t*)0x2004100c = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x2004100d = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x2004100e = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x2004100f = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20041010 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20041011 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20041012 = (uint8_t)0x0);
    NONFAILING(*(uint8_t*)0x20041013 = (uint8_t)0x0);
    r[81] = syscall(__NR_bind, r[4], 0x20041000ul, 0x14ul);
    break;
  }
  return 0;
}

void test()
{
  long i;
  pthread_t th[18];

  memset(r, -1, sizeof(r));
  for (i = 0; i < 9; i++) {
    pthread_create(&th[i], 0, thr, (void*)i);
    usleep(rand() % 10000);
  }
  usleep(rand() % 100000);
}

int main()
{
  int i;
  for (i = 0; i < 8; i++) {
    if (fork() == 0) {
      install_segv_handler();
      use_temporary_dir();
      int pid = do_sandbox_none(i, true);
      int status = 0;
      while (waitpid(pid, &status, __WALL) != pid) {
      }
      return 0;
    }
  }
  sleep(1000000);
  return 0;
}