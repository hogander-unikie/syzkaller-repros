// KASAN: global-out-of-bounds Read in fb_pad_aligned_buffer
// https://syzkaller.appspot.com/bug?id=43706ff83b09eb65e3176f37346ffb7754bd6d7a
// status:open
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <endian.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/genetlink.h>
#include <linux/if_addr.h>
#include <linux/if_link.h>
#include <linux/in6.h>
#include <linux/neighbour.h>
#include <linux/net.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/veth.h>

struct nlmsg {
  char* pos;
  int nesting;
  struct nlattr* nested[8];
  char buf[1024];
};

static struct nlmsg nlmsg;

static void netlink_init(struct nlmsg* nlmsg, int typ, int flags,
                         const void* data, int size)
{
  memset(nlmsg, 0, sizeof(*nlmsg));
  struct nlmsghdr* hdr = (struct nlmsghdr*)nlmsg->buf;
  hdr->nlmsg_type = typ;
  hdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
  memcpy(hdr + 1, data, size);
  nlmsg->pos = (char*)(hdr + 1) + NLMSG_ALIGN(size);
}

static void netlink_attr(struct nlmsg* nlmsg, int typ, const void* data,
                         int size)
{
  struct nlattr* attr = (struct nlattr*)nlmsg->pos;
  attr->nla_len = sizeof(*attr) + size;
  attr->nla_type = typ;
  memcpy(attr + 1, data, size);
  nlmsg->pos += NLMSG_ALIGN(attr->nla_len);
}

static int netlink_send_ext(struct nlmsg* nlmsg, int sock, uint16_t reply_type,
                            int* reply_len)
{
  if (nlmsg->pos > nlmsg->buf + sizeof(nlmsg->buf) || nlmsg->nesting)
    exit(1);
  struct nlmsghdr* hdr = (struct nlmsghdr*)nlmsg->buf;
  hdr->nlmsg_len = nlmsg->pos - nlmsg->buf;
  struct sockaddr_nl addr;
  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  unsigned n = sendto(sock, nlmsg->buf, hdr->nlmsg_len, 0,
                      (struct sockaddr*)&addr, sizeof(addr));
  if (n != hdr->nlmsg_len)
    exit(1);
  n = recv(sock, nlmsg->buf, sizeof(nlmsg->buf), 0);
  if (hdr->nlmsg_type == NLMSG_DONE) {
    *reply_len = 0;
    return 0;
  }
  if (n < sizeof(struct nlmsghdr))
    exit(1);
  if (reply_len && hdr->nlmsg_type == reply_type) {
    *reply_len = n;
    return 0;
  }
  if (n < sizeof(struct nlmsghdr) + sizeof(struct nlmsgerr))
    exit(1);
  if (hdr->nlmsg_type != NLMSG_ERROR)
    exit(1);
  return -((struct nlmsgerr*)(hdr + 1))->error;
}

static int netlink_send(struct nlmsg* nlmsg, int sock)
{
  return netlink_send_ext(nlmsg, sock, 0, NULL);
}

static int netlink_next_msg(struct nlmsg* nlmsg, unsigned int offset,
                            unsigned int total_len)
{
  struct nlmsghdr* hdr = (struct nlmsghdr*)(nlmsg->buf + offset);
  if (offset == total_len || offset + hdr->nlmsg_len > total_len)
    return -1;
  return hdr->nlmsg_len;
}

static void netlink_device_change(struct nlmsg* nlmsg, int sock,
                                  const char* name, bool up, const char* master,
                                  const void* mac, int macsize,
                                  const char* new_name)
{
  struct ifinfomsg hdr;
  memset(&hdr, 0, sizeof(hdr));
  if (up)
    hdr.ifi_flags = hdr.ifi_change = IFF_UP;
  hdr.ifi_index = if_nametoindex(name);
  netlink_init(nlmsg, RTM_NEWLINK, 0, &hdr, sizeof(hdr));
  if (new_name)
    netlink_attr(nlmsg, IFLA_IFNAME, new_name, strlen(new_name));
  if (master) {
    int ifindex = if_nametoindex(master);
    netlink_attr(nlmsg, IFLA_MASTER, &ifindex, sizeof(ifindex));
  }
  if (macsize)
    netlink_attr(nlmsg, IFLA_ADDRESS, mac, macsize);
  int err = netlink_send(nlmsg, sock);
  (void)err;
}

const int kInitNetNsFd = 239;

#define DEVLINK_FAMILY_NAME "devlink"

#define DEVLINK_CMD_PORT_GET 5
#define DEVLINK_CMD_RELOAD 37
#define DEVLINK_ATTR_BUS_NAME 1
#define DEVLINK_ATTR_DEV_NAME 2
#define DEVLINK_ATTR_NETDEV_NAME 7
#define DEVLINK_ATTR_NETNS_FD 138

static int netlink_devlink_id_get(struct nlmsg* nlmsg, int sock)
{
  struct genlmsghdr genlhdr;
  struct nlattr* attr;
  int err, n;
  uint16_t id = 0;
  memset(&genlhdr, 0, sizeof(genlhdr));
  genlhdr.cmd = CTRL_CMD_GETFAMILY;
  netlink_init(nlmsg, GENL_ID_CTRL, 0, &genlhdr, sizeof(genlhdr));
  netlink_attr(nlmsg, CTRL_ATTR_FAMILY_NAME, DEVLINK_FAMILY_NAME,
               strlen(DEVLINK_FAMILY_NAME) + 1);
  err = netlink_send_ext(nlmsg, sock, GENL_ID_CTRL, &n);
  if (err) {
    return -1;
  }
  attr = (struct nlattr*)(nlmsg->buf + NLMSG_HDRLEN +
                          NLMSG_ALIGN(sizeof(genlhdr)));
  for (; (char*)attr < nlmsg->buf + n;
       attr = (struct nlattr*)((char*)attr + NLMSG_ALIGN(attr->nla_len))) {
    if (attr->nla_type == CTRL_ATTR_FAMILY_ID) {
      id = *(uint16_t*)(attr + 1);
      break;
    }
  }
  if (!id) {
    return -1;
  }
  recv(sock, nlmsg->buf, sizeof(nlmsg->buf), 0); /* recv ack */
  return id;
}

static void netlink_devlink_netns_move(const char* bus_name,
                                       const char* dev_name, int netns_fd)
{
  struct genlmsghdr genlhdr;
  int sock;
  int id, err;
  sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (sock == -1)
    exit(1);
  id = netlink_devlink_id_get(&nlmsg, sock);
  if (id == -1)
    goto error;
  memset(&genlhdr, 0, sizeof(genlhdr));
  genlhdr.cmd = DEVLINK_CMD_RELOAD;
  netlink_init(&nlmsg, id, 0, &genlhdr, sizeof(genlhdr));
  netlink_attr(&nlmsg, DEVLINK_ATTR_BUS_NAME, bus_name, strlen(bus_name) + 1);
  netlink_attr(&nlmsg, DEVLINK_ATTR_DEV_NAME, dev_name, strlen(dev_name) + 1);
  netlink_attr(&nlmsg, DEVLINK_ATTR_NETNS_FD, &netns_fd, sizeof(netns_fd));
  err = netlink_send(&nlmsg, sock);
  if (err) {
  }
error:
  close(sock);
}

static struct nlmsg nlmsg2;

static void initialize_devlink_ports(const char* bus_name, const char* dev_name,
                                     const char* netdev_prefix)
{
  struct genlmsghdr genlhdr;
  int len, total_len, id, err, offset;
  uint16_t netdev_index;
  int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
  if (sock == -1)
    exit(1);
  int rtsock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (rtsock == -1)
    exit(1);
  id = netlink_devlink_id_get(&nlmsg, sock);
  if (id == -1)
    goto error;
  memset(&genlhdr, 0, sizeof(genlhdr));
  genlhdr.cmd = DEVLINK_CMD_PORT_GET;
  netlink_init(&nlmsg, id, NLM_F_DUMP, &genlhdr, sizeof(genlhdr));
  netlink_attr(&nlmsg, DEVLINK_ATTR_BUS_NAME, bus_name, strlen(bus_name) + 1);
  netlink_attr(&nlmsg, DEVLINK_ATTR_DEV_NAME, dev_name, strlen(dev_name) + 1);
  err = netlink_send_ext(&nlmsg, sock, id, &total_len);
  if (err) {
    goto error;
  }
  offset = 0;
  netdev_index = 0;
  while ((len = netlink_next_msg(&nlmsg, offset, total_len)) != -1) {
    struct nlattr* attr = (struct nlattr*)(nlmsg.buf + offset + NLMSG_HDRLEN +
                                           NLMSG_ALIGN(sizeof(genlhdr)));
    for (; (char*)attr < nlmsg.buf + offset + len;
         attr = (struct nlattr*)((char*)attr + NLMSG_ALIGN(attr->nla_len))) {
      if (attr->nla_type == DEVLINK_ATTR_NETDEV_NAME) {
        char* port_name;
        char netdev_name[IFNAMSIZ];
        port_name = (char*)(attr + 1);
        snprintf(netdev_name, sizeof(netdev_name), "%s%d", netdev_prefix,
                 netdev_index);
        netlink_device_change(&nlmsg2, rtsock, port_name, true, 0, 0, 0,
                              netdev_name);
        break;
      }
    }
    offset += len;
    netdev_index++;
  }
error:
  close(rtsock);
  close(sock);
}

static void initialize_devlink_pci(void)
{
  int netns = open("/proc/self/ns/net", O_RDONLY);
  if (netns == -1)
    exit(1);
  int ret = setns(kInitNetNsFd, 0);
  if (ret == -1)
    exit(1);
  netlink_devlink_netns_move("pci", "0000:00:10.0", netns);
  ret = setns(netns, 0);
  if (ret == -1)
    exit(1);
  close(netns);
  initialize_devlink_ports("pci", "0000:00:10.0", "netpci");
}

static long syz_open_dev(volatile long a0, volatile long a1, volatile long a2)
{
  if (a0 == 0xc || a0 == 0xb) {
    char buf[128];
    sprintf(buf, "/dev/%s/%d:%d", a0 == 0xc ? "char" : "block", (uint8_t)a1,
            (uint8_t)a2);
    return open(buf, O_RDWR, 0);
  } else {
    char buf[1024];
    char* hash;
    strncpy(buf, (char*)a0, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    while ((hash = strchr(buf, '#'))) {
      *hash = '0' + (char)(a1 % 10);
      a1 /= 10;
    }
    return open(buf, a2, 0);
  }
}

uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

int main(void)
{
  syscall(__NR_mmap, 0x20000000ul, 0x1000000ul, 3ul, 0x32ul, -1, 0);
  intptr_t res = 0;
  res = syz_open_dev(0xc, 4, 1);
  if (res != -1)
    r[0] = res;
  *(uint32_t*)0x20000040 = 0;
  *(uint32_t*)0x20000044 = 0;
  *(uint32_t*)0x20000048 = 0x20;
  *(uint32_t*)0x2000004c = 0x13;
  *(uint32_t*)0x20000050 = 0x100;
  *(uint64_t*)0x20000058 = 0x200003c0;
  memcpy(
      (void*)0x200003c0,
      "\xcb\xb6\x2c\x7c\x7e\x12\x72\x7d\x60\xc5\xdb\x26\x5c\x1d\x8b\x3f\x75\x25"
      "\x50\xd4\xea\xc7\xaf\xf1\x02\x13\x26\xe1\x5d\xfd\xad\x91\x1d\x57\xa8\x53"
      "\x41\x4b\x19\x47\x02\xa4\x1a\x75\x60\x56\x8b\xe1\x11\x1d\xac\x9f\xf2\x58"
      "\x6d\x67\xdc\x8e\xf3\xe9\xd3\x66\x94\xc5\xea\x96\xd8\x98\x5a\x99\x53\xe8"
      "\x77\x5c\xbf\x7a\xe1\x51\xea\x1f\xd6\x08\x63\xac\xd9\xe7\x60\x31\x83\x3d"
      "\xff\xb4\x47\xc4\x25\xc9\x74\x9c\x7d\xe7\xfb\xe2\xe5\x65\xb4\xa2\x5f\x7b"
      "\x2a\xc0\x13\x92\xce\x04\xec\x8d\x6d\x21\x0e\x0a\x23\xae\x3a\x5f\xd8\x1c"
      "\x3c\x89\xc9\x28\x74\x70\x9f\xcd\xd4\xd4\xc3\xff\xe7\xfa\x05\x2a\xc6\x8f"
      "\xb6\x01\x7e\x48\x2e\x24\xbe\x6a\xc8\x19\x10\x39\xb4\x34\xaa\x0e\x9f\x97"
      "\x39\xae\xdf\x30\xc0\xfa\x9c\x42\xcd\xa4\x11\x02\x5c\xa2\x0c\x42\xbf\x88"
      "\x68\x56\x03\x99\x87\x3f\x4d\x1b\x28\xed\x71\x3e\x41\x1b\x34\xec\x02\x63"
      "\x21\xc8\xcd\x9a\xd1\x7c\xbc\x95\x70\x4c\x14\x2d\x7c\x71\x06\x3b\x4a\x0b"
      "\x52\xe2\x20\x73\xf8\x59\xd6\x32\x89\x92\x12\x6e\x5f\x50\x57\xad\x1d\x02"
      "\x29\xba\xde\xc1\xe0\x88\x0f\x47\x6f\x18\xce\x68\x55\x60\x78\xa0\x06\x7d"
      "\x8e\x90\x05\x7f\x56\x0b\x51\x51\xc4\x50\xe6\x55\x89\x81\x2f\x77\x9b\x50"
      "\xe4\xde\xd6\x60\xee\x4a\xa6\x1e\x7f\x84\xa9\xb4\xb7\x91\x83\x71\x14\x09"
      "\x55\x39\xfb\x55\x73\xe4\x7c\xce\x26\xdc\x38\x31\x19\x13\x99\x51\x81\xe7"
      "\x28\xfd\x4d\x6d\x33\x4e\x62\xfd\x0a\x82\x2f\x93\x71\xbc\xdc\x43\xc8\xa6"
      "\x96\x88\xa1\x1f\xc9\xf4\x3a\xfe\x64\x2c\x30\xdd\xf3\x94\x65\xb1\x3c\xf1"
      "\xdd\xad\xd7\x89\x70\x77\x18\x27\xf4\x45\xf0\xd4\x56\x1e\x8e\x2b\xb6\xcf"
      "\x22\x44\x63\x4b\xcd\x72\xd1\x24\xf3\xca\xa1\x52\xc6\xfd\x6d\x47\xbb\x00"
      "\x4b\x97\x97\x5d\x4a\x46\xb2\xee\x12\x9b\x88\x9d\xc5\x9c\x1c\x71\x62\xff"
      "\xf2\x2c\x42\x95\x54\x52\x95\xea\x72\xfe\x14\xc6\xb2\xba\xb3\x5a\xb5\x45"
      "\x51\x73\x98\xb0\x03\xb6\xd3\xb8\x9c\x5c\x64\xb4\x49\xf7\xd5\x60\xdd\xe2"
      "\x0f\xc1\x3b\x3e\xc8\x8d\xc0\x52\x42\xa1\x60\x07\x6c\xcb\xff\x3e\x32\xb8"
      "\x78\x0b\xd1\x91\xa0\xb1\x2a\x6f\x99\x00\xa9\x37\xa1\x2d\x9f\xb5\xed\x34"
      "\x88\xd5\xcd\x0a\xa3\x29\x58\x3f\x2c\x47\xc4\xfd\x47\x26\x46\x9c\x98\xe0"
      "\xb6\x6d\x74\xc3\x1d\x0a\x1f\x7f\xc1\x0c\xc1\x94\x4d\x25\x26\x7e\xff\x7a"
      "\x74\x0c\xc4\x13\x06\x9b\x58\x47\x9c\xa6\x96\x00\x63\x6a\x0f\x22\xa4\x70"
      "\xa6\x9b\x46\xc1\xcc\x1b\x95\xcf\x32\xdd\x7b\x73\xad\x67\x41\x61\xca\x39"
      "\x25\x83\xaa\x83\x4a\xaa\xde\x88\xa4\x39\x33\x89\xb7\x6d\x58\x8a\x60\x81"
      "\x91\xdb\x7c\xa7\x5d\xd0\x09\x38\x2a\x45\x83\xe8\x78\x1e\xb0\x9f\x48\xa4"
      "\x4d\x06\xb4\x17\x56\xeb\x3f\x1b\x7e\xea\x18\xad\xf2\x97\x83\x22\xd0\xb3"
      "\x84\xdf\x03\x07\x98\x91\xe1\x0f\x09\x2c\x57\x33\x20\xfb\x94\x1f\x10\xe3"
      "\xc3\xaf\x6f\x3e\x72\xb2\xaf\xcd\x95\xa3\x66\x6a\x6a\x40\xa8\x40\xe6\x3d"
      "\x1c\xb9\xe2\x49\x25\xc0\x49\x7a\x04\xef\x7b\x9b\x1e\xe3\x9d\xe0\x45\x6f"
      "\xec\xd6\x76\xac\xee\x23\x3f\x5b\x64\x5e\x8d\x05\xf3\xf5\x05\xa1\x2a\x0f"
      "\x5b\x35\x65\x86\xf2\x2c\x2c\x2c\xeb\xef\x83\xc5\xe6\x8c\x22\x72\x34\x95"
      "\xab\xa0\x64\x27\x92\x55\xde\x48\x9e\x33\xe9\x20\x9a\x63\x68\x90\xbc\xfb"
      "\x2a\x52\x7e\x8d\xa8\x36\x21\xae\xf3\x74\xf5\xa1\x1d\x8a\xc7\x7f\x5d\xcd"
      "\x42\x1f\x45\xe5\x2d\xdf\x9b\xc3\x5f\x64\xcf\x72\x1c\xe6\xc9\x6b\x56\x13"
      "\x26\x8c\x02\x5c\xa9\x91\x0a\x8a\x23\x5e\x6a\xf3\xd6\x7a\xdd\xc6\x6e\x75"
      "\xc1\x56\x4a\x7f\x86\x84\xfc\xd2\x3e\xc9\x8d\x61\xa4\xfa\x42\x2c\x55\xe6"
      "\x78\x9c\xbc\x42\xb1\xd5\x32\x63\x3e\x65\x8a\x5c\x2f\xfe\x57\x67\x03\x97"
      "\x62\xf1\x8c\xce\x89\xb8\x00\x0e\x37\xca\xbc\x35\x40\x9d\x3c\x7a\x02\x13"
      "\xde\x87\xce\x95\xcf\xbf\xb0\xc7\x1b\xe4\xc4\xef\x44\x86\x6b\xd8\xe0\x50"
      "\xe3\x36\xb3\x1a\x6e\x8f\x15\xbd\x21\x60\xef\x48\x48\xad\xa2\x56\xdf\x90"
      "\x27\x58\xeb\xbc\x1e\x83\x7f\xab\x4c\xb8\xbb\x84\x67\xa7\x3d\xf7\x17\xd9"
      "\x2e\x22\x04\x48\x2f\xe9\x5c\x21\x12\x11\x86\xa5\x7c\x3b\x12\xf7\x5c\x83"
      "\x6a\xb7\x02\x43\x34\x18\xf3\x29\x44\x32\x3e\xaa\xad\xbc\xa3\x66\x96\x1c"
      "\x25\x62\x0c\x43\x95\x1a\x7a\x3f\xd3\xe3\xcc\x85\xd6\xa3\x99\x6b\x50\xd1"
      "\x2e\xa7\x6d\xf6\x58\xa6\x40\x5f\xdf\x1c\x9f\x2c\x05\x24\x8d\x95\x15\xce"
      "\x96\x65\x0e\xbf\x62\x96\x64\xe6\xc1\x92\xdd\x28\xe1\x61\x72\xbe\x15\x93"
      "\xe9\x4d\xbc\xa0\xe5\x54\xfc\xe9\x73\x6c\x73\x54\x03\x0c\x64\xfd\xaf\x35"
      "\x03\x8e\x1f\x1e\x1d\x20\xe0\x6f\x34\x8e\xf5\xae\x75\x41\x39\xb4\xe8\x12"
      "\xc6\x0f\xbc\x55\xba\xce\x83\xf2\x7b\xb8\x35\xc1\x64\xc2\x9f\x5f\x7c\x1d"
      "\xbe\xeb\xfd\x98\x8a\x16\x23\x5c\x4b\xbe\xe4\xd9\xa3\xbc\x66\x6f",
      1024);
  syscall(__NR_ioctl, r[0], 0x4b72ul, 0x20000040ul);
  res = syz_open_dev(0xc, 4, 1);
  if (res != -1)
    r[1] = res;
  *(uint32_t*)0x20000000 = 3;
  *(uint32_t*)0x20000004 = 1;
  *(uint32_t*)0x20000008 = 0xfffffffe;
  *(uint32_t*)0x2000000c = 5;
  *(uint32_t*)0x20000010 = 0x40193;
  *(uint64_t*)0x20000018 = 0;
  syscall(__NR_ioctl, r[1], 0x4b72ul, 0x20000000ul);
  return 0;
}