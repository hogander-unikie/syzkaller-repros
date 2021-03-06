// WARNING in sk_stream_kill_queues (2)
// https://syzkaller.appspot.com/bug?id=5001bc551dea02e50692c43989f472c8ba9c7691
// status:dup
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

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

struct csum_inet {
  uint32_t acc;
};

static void csum_inet_init(struct csum_inet* csum)
{
  csum->acc = 0;
}

static void csum_inet_update(struct csum_inet* csum, const uint8_t* data,
                             size_t length)
{
  if (length == 0)
    return;

  size_t i;
  for (i = 0; i < length - 1; i += 2)
    csum->acc += *(uint16_t*)&data[i];

  if (length & 1)
    csum->acc += (uint16_t)data[length - 1];

  while (csum->acc > 0xffff)
    csum->acc = (csum->acc & 0xffff) + (csum->acc >> 16);
}

static uint16_t csum_inet_digest(struct csum_inet* csum)
{
  return ~csum->acc;
}

long r[2];
void loop()
{
  memset(r, -1, sizeof(r));
  syscall(__NR_mmap, 0x20000000, 0xfff000, 3, 0x32, -1, 0);
  r[0] = syscall(__NR_socket, 0xa, 1, 0);
  r[1] = syscall(__NR_socket, 0xa, 1, 0);
  *(uint16_t*)0x205ca000 = 0xa;
  *(uint16_t*)0x205ca002 = htobe16(0x4e22);
  *(uint32_t*)0x205ca004 = 0;
  *(uint8_t*)0x205ca008 = 0;
  *(uint8_t*)0x205ca009 = 0;
  *(uint8_t*)0x205ca00a = 0;
  *(uint8_t*)0x205ca00b = 0;
  *(uint8_t*)0x205ca00c = 0;
  *(uint8_t*)0x205ca00d = 0;
  *(uint8_t*)0x205ca00e = 0;
  *(uint8_t*)0x205ca00f = 0;
  *(uint8_t*)0x205ca010 = 0;
  *(uint8_t*)0x205ca011 = 0;
  *(uint8_t*)0x205ca012 = 0;
  *(uint8_t*)0x205ca013 = 0;
  *(uint8_t*)0x205ca014 = 0;
  *(uint8_t*)0x205ca015 = 0;
  *(uint8_t*)0x205ca016 = 0;
  *(uint8_t*)0x205ca017 = 0;
  *(uint32_t*)0x205ca018 = 0;
  syscall(__NR_bind, r[1], 0x205ca000, 0x1c);
  syscall(__NR_listen, r[1], 0);
  *(uint32_t*)0x20e56ffc = 0x71;
  syscall(__NR_setsockopt, r[0], 0x29, 0xb, 0x20e56ffc, 4);
  *(uint16_t*)0x20425fe4 = 0xa;
  *(uint16_t*)0x20425fe6 = htobe16(0x4e22);
  *(uint32_t*)0x20425fe8 = 0;
  *(uint8_t*)0x20425fec = 0;
  *(uint8_t*)0x20425fed = 0;
  *(uint8_t*)0x20425fee = 0;
  *(uint8_t*)0x20425fef = 0;
  *(uint8_t*)0x20425ff0 = 0;
  *(uint8_t*)0x20425ff1 = 0;
  *(uint8_t*)0x20425ff2 = 0;
  *(uint8_t*)0x20425ff3 = 0;
  *(uint8_t*)0x20425ff4 = 0;
  *(uint8_t*)0x20425ff5 = 0;
  *(uint8_t*)0x20425ff6 = 0;
  *(uint8_t*)0x20425ff7 = 0;
  *(uint8_t*)0x20425ff8 = 0;
  *(uint8_t*)0x20425ff9 = 0;
  *(uint8_t*)0x20425ffa = 0;
  *(uint8_t*)0x20425ffb = 0;
  *(uint32_t*)0x20425ffc = 0;
  syscall(__NR_sendto, r[0], 0x20f6f000, 0xfffffffffffffecf, 0x20000004,
          0x20425fe4, 0x1c);
  memcpy((void*)0x20b0cffc, "tls", 4);
  syscall(__NR_setsockopt, r[0], 6, 0x1f, 0x20b0cffc, 4);
  *(uint8_t*)0x2072afb1 = 1;
  *(uint8_t*)0x2072afb2 = 0x80;
  *(uint8_t*)0x2072afb3 = 0xc2;
  *(uint8_t*)0x2072afb4 = 0;
  *(uint8_t*)0x2072afb5 = 0;
  *(uint8_t*)0x2072afb6 = 0;
  memcpy((void*)0x2072afb7, "\xbd\x75\x86\x96\x12\xdb", 6);
  *(uint16_t*)0x2072afbd = htobe16(0x86dd);
  STORE_BY_BITMASK(uint8_t, 0x2072afbf, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x2072afbf, 6, 4, 4);
  memcpy((void*)0x2072afc0, "\x06\xf5\x26", 3);
  *(uint16_t*)0x2072afc3 = htobe16(0x30);
  *(uint8_t*)0x2072afc5 = 0x3a;
  *(uint8_t*)0x2072afc6 = 0;
  *(uint8_t*)0x2072afc7 = 0xfe;
  *(uint8_t*)0x2072afc8 = 0x80;
  *(uint8_t*)0x2072afc9 = 0;
  *(uint8_t*)0x2072afca = 0;
  *(uint8_t*)0x2072afcb = 0;
  *(uint8_t*)0x2072afcc = 0;
  *(uint8_t*)0x2072afcd = 0;
  *(uint8_t*)0x2072afce = 0;
  *(uint8_t*)0x2072afcf = 0;
  *(uint8_t*)0x2072afd0 = 0;
  *(uint8_t*)0x2072afd1 = 0;
  *(uint8_t*)0x2072afd2 = 0;
  *(uint8_t*)0x2072afd3 = 0;
  *(uint8_t*)0x2072afd4 = 0;
  *(uint8_t*)0x2072afd5 = 0;
  *(uint8_t*)0x2072afd6 = 0xbb;
  *(uint8_t*)0x2072afd7 = -1;
  *(uint8_t*)0x2072afd8 = 2;
  *(uint8_t*)0x2072afd9 = 0;
  *(uint8_t*)0x2072afda = 0;
  *(uint8_t*)0x2072afdb = 0;
  *(uint8_t*)0x2072afdc = 0;
  *(uint8_t*)0x2072afdd = 0;
  *(uint8_t*)0x2072afde = 0;
  *(uint8_t*)0x2072afdf = 0;
  *(uint8_t*)0x2072afe0 = 0;
  *(uint8_t*)0x2072afe1 = 0;
  *(uint8_t*)0x2072afe2 = 0;
  *(uint8_t*)0x2072afe3 = 0;
  *(uint8_t*)0x2072afe4 = 0;
  *(uint8_t*)0x2072afe5 = 0;
  *(uint8_t*)0x2072afe6 = 1;
  *(uint8_t*)0x2072afe7 = 2;
  *(uint8_t*)0x2072afe8 = 0;
  *(uint16_t*)0x2072afe9 = 0;
  *(uint8_t*)0x2072afeb = 0;
  *(uint8_t*)0x2072afec = 0;
  *(uint8_t*)0x2072afed = 0;
  *(uint8_t*)0x2072afee = 0;
  STORE_BY_BITMASK(uint8_t, 0x2072afef, 0, 0, 4);
  STORE_BY_BITMASK(uint8_t, 0x2072afef, 6, 4, 4);
  memcpy((void*)0x2072aff0, "\xc7\xa9\x3c", 3);
  *(uint16_t*)0x2072aff3 = htobe16(0);
  *(uint8_t*)0x2072aff5 = 0x29;
  *(uint8_t*)0x2072aff6 = 0;
  *(uint8_t*)0x2072aff7 = -1;
  *(uint8_t*)0x2072aff8 = 2;
  *(uint8_t*)0x2072aff9 = 0;
  *(uint8_t*)0x2072affa = 0;
  *(uint8_t*)0x2072affb = 0;
  *(uint8_t*)0x2072affc = 0;
  *(uint8_t*)0x2072affd = 0;
  *(uint8_t*)0x2072affe = 0;
  *(uint8_t*)0x2072afff = 0;
  *(uint8_t*)0x2072b000 = 0;
  *(uint8_t*)0x2072b001 = 0;
  *(uint8_t*)0x2072b002 = 0;
  *(uint8_t*)0x2072b003 = 0;
  *(uint8_t*)0x2072b004 = 0;
  *(uint8_t*)0x2072b005 = 0;
  *(uint8_t*)0x2072b006 = 1;
  *(uint8_t*)0x2072b007 = 0xfe;
  *(uint8_t*)0x2072b008 = 0x80;
  *(uint8_t*)0x2072b009 = 0;
  *(uint8_t*)0x2072b00a = 0;
  *(uint8_t*)0x2072b00b = 0;
  *(uint8_t*)0x2072b00c = 0;
  *(uint8_t*)0x2072b00d = 0;
  *(uint8_t*)0x2072b00e = 0;
  *(uint8_t*)0x2072b00f = 0;
  *(uint8_t*)0x2072b010 = 0;
  *(uint8_t*)0x2072b011 = 0;
  *(uint8_t*)0x2072b012 = 0;
  *(uint8_t*)0x2072b013 = 0;
  *(uint8_t*)0x2072b014 = 0;
  *(uint8_t*)0x2072b015 = 0;
  *(uint8_t*)0x2072b016 = 0xbb;
  *(uint32_t*)0x20ce9000 = 0;
  *(uint32_t*)0x20ce9004 = 1;
  *(uint32_t*)0x20ce9008 = 0x3c3;
  struct csum_inet csum_1;
  csum_inet_init(&csum_1);
  csum_inet_update(&csum_1, (const uint8_t*)0x2072afc7, 16);
  csum_inet_update(&csum_1, (const uint8_t*)0x2072afd7, 16);
  uint32_t csum_1_chunk_2 = 0x30000000;
  csum_inet_update(&csum_1, (const uint8_t*)&csum_1_chunk_2, 4);
  uint32_t csum_1_chunk_3 = 0x3a000000;
  csum_inet_update(&csum_1, (const uint8_t*)&csum_1_chunk_3, 4);
  csum_inet_update(&csum_1, (const uint8_t*)0x2072afe7, 48);
  *(uint16_t*)0x2072afe9 = csum_inet_digest(&csum_1);
}

int main()
{
  loop();
  return 0;
}
