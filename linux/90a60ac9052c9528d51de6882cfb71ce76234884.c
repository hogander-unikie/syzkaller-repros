// WARNING: suspicious RCU usage in tipc_bearer_find
// https://syzkaller.appspot.com/bug?id=90a60ac9052c9528d51de6882cfb71ce76234884
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <endian.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/genetlink.h>
#include <linux/netlink.h>

static long syz_genetlink_get_family_id(volatile long name)
{
  char buf[512] = {0};
  struct nlmsghdr* hdr = (struct nlmsghdr*)buf;
  struct genlmsghdr* genlhdr = (struct genlmsghdr*)NLMSG_DATA(hdr);
  struct nlattr* attr = (struct nlattr*)(genlhdr + 1);
  hdr->nlmsg_len =
      sizeof(*hdr) + sizeof(*genlhdr) + sizeof(*attr) + GENL_NAMSIZ;
  hdr->nlmsg_type = GENL_ID_CTRL;
  hdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  genlhdr->cmd = CTRL_CMD_GETFAMILY;
  attr->nla_type = CTRL_ATTR_FAMILY_NAME;
  attr->nla_len = sizeof(*attr) + GENL_NAMSIZ;
  strncpy((char*)(attr + 1), (char*)name, GENL_NAMSIZ);
  struct iovec iov = {hdr, hdr->nlmsg_len};
  struct sockaddr_nl addr = {0};
  addr.nl_family = AF_NETLINK;
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (fd == -1) {
    return -1;
  }
  struct msghdr msg = {&addr, sizeof(addr), &iov, 1, NULL, 0, 0};
  if (sendmsg(fd, &msg, 0) == -1) {
    close(fd);
    return -1;
  }
  ssize_t n = recv(fd, buf, sizeof(buf), 0);
  close(fd);
  if (n <= 0) {
    return -1;
  }
  if (hdr->nlmsg_type != GENL_ID_CTRL) {
    return -1;
  }
  for (; (char*)attr < buf + n;
       attr = (struct nlattr*)((char*)attr + NLMSG_ALIGN(attr->nla_len))) {
    if (attr->nla_type == CTRL_ATTR_FAMILY_ID)
      return *(uint16_t*)(attr + 1);
  }
  return -1;
}

uint64_t r[2] = {0xffffffffffffffff, 0x0};

int main(void)
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  long res = 0;
  res = syscall(__NR_socket, 0x10, 3, 0x10);
  if (res != -1)
    r[0] = res;
  memcpy((void*)0x20000300, "TIPC\000", 5);
  res = syz_genetlink_get_family_id(0x20000300);
  if (res != -1)
    r[1] = res;
  *(uint64_t*)0x20000100 = 0;
  *(uint32_t*)0x20000108 = 0;
  *(uint64_t*)0x20000110 = 0x200000c0;
  *(uint64_t*)0x200000c0 = 0x20000040;
  *(uint32_t*)0x20000040 = 0x68;
  *(uint16_t*)0x20000044 = r[1];
  *(uint16_t*)0x20000046 = 0x405;
  *(uint32_t*)0x20000048 = 0;
  *(uint32_t*)0x2000004c = 0;
  *(uint8_t*)0x20000050 = 1;
  *(uint8_t*)0x20000051 = 0;
  *(uint16_t*)0x20000052 = 0;
  *(uint32_t*)0x20000054 = 0;
  *(uint16_t*)0x20000058 = 0x4109;
  *(uint16_t*)0x2000005a = 0;
  *(uint16_t*)0x2000005c = htobe16(0x4c);
  *(uint16_t*)0x2000005e = htobe16(0x18);
  *(uint32_t*)0x20000060 = htobe32(0);
  memcpy((void*)0x20000064, "broadcast-"
                            "link\000\000\000\000\000\000\000\000\000\000\000"
                            "\000\000\000\000\000\000\000\000\000\000\000\000"
                            "\000\000\000\000\000\000\000\000\000\000\000\000"
                            "\000\000\000\000\000\000\000\000\000\000\000\000"
                            "\000\000\000\000\000\000\000",
         68);
  *(uint64_t*)0x200000c8 = 0x68;
  *(uint64_t*)0x20000118 = 1;
  *(uint64_t*)0x20000120 = 0;
  *(uint64_t*)0x20000128 = 0;
  *(uint32_t*)0x20000130 = 0;
  syscall(__NR_sendmsg, r[0], 0x20000100, 0);
  return 0;
}
