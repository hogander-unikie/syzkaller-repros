// kernel BUG at fs/ntfs/aops.c:LINE!
// https://syzkaller.appspot.com/bug?id=86d590f642a6d325d2f9dc9571e6702bed1a77ee
// status:open
// autogenerated by syzkaller (http://github.com/google/syzkaller)

#define _GNU_SOURCE
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/loop.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

extern unsigned long long procid;

struct fs_image_segment {
  void* data;
  uintptr_t size;
  uintptr_t offset;
};

#define IMAGE_MAX_SEGMENTS 4096
#define IMAGE_MAX_SIZE (32 << 20)

#define SYZ_memfd_create 319

static uintptr_t syz_mount_image(uintptr_t fs, uintptr_t dir, uintptr_t size,
                                 uintptr_t nsegs, uintptr_t segments,
                                 uintptr_t flags, uintptr_t opts)
{
  char loopname[64];
  int loopfd, err = 0, res = -1;
  uintptr_t i;
  struct fs_image_segment* segs = (struct fs_image_segment*)segments;

  if (nsegs > IMAGE_MAX_SEGMENTS)
    nsegs = IMAGE_MAX_SEGMENTS;
  for (i = 0; i < nsegs; i++) {
    if (segs[i].size > IMAGE_MAX_SIZE)
      segs[i].size = IMAGE_MAX_SIZE;
    segs[i].offset %= IMAGE_MAX_SIZE;
    if (segs[i].offset > IMAGE_MAX_SIZE - segs[i].size)
      segs[i].offset = IMAGE_MAX_SIZE - segs[i].size;
    if (size < segs[i].offset + segs[i].offset)
      size = segs[i].offset + segs[i].offset;
  }
  if (size > IMAGE_MAX_SIZE)
    size = IMAGE_MAX_SIZE;
  int memfd = syscall(SYZ_memfd_create, "syz_mount_image", 0);
  if (memfd == -1) {
    err = errno;
    goto error;
  }
  if (ftruncate(memfd, size)) {
    err = errno;
    goto error_close_memfd;
  }
  for (i = 0; i < nsegs; i++) {
    if (pwrite(memfd, segs[i].data, segs[i].size, segs[i].offset) < 0) {
    }
  }
  snprintf(loopname, sizeof(loopname), "/dev/loop%llu", procid);
  loopfd = open(loopname, O_RDWR);
  if (loopfd == -1) {
    err = errno;
    goto error_close_memfd;
  }
  if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
    if (errno != EBUSY) {
      err = errno;
      goto error_close_loop;
    }
    ioctl(loopfd, LOOP_CLR_FD, 0);
    usleep(1000);
    if (ioctl(loopfd, LOOP_SET_FD, memfd)) {
      err = errno;
      goto error_close_loop;
    }
  }
  mkdir((char*)dir, 0777);
  if (strcmp((char*)fs, "iso9660") == 0)
    flags |= MS_RDONLY;
  if (mount(loopname, (char*)dir, (char*)fs, flags, (char*)opts)) {
    err = errno;
    goto error_clear_loop;
  }
  res = 0;
error_clear_loop:
  ioctl(loopfd, LOOP_CLR_FD, 0);
error_close_loop:
  close(loopfd);
error_close_memfd:
  close(memfd);
error:
  errno = err;
  return res;
}

unsigned long long procid;
void loop()
{
  memcpy((void*)0x20000000, "ntfs", 5);
  memcpy((void*)0x20000100, "./file0", 8);
  *(uint64_t*)0x20000200 = 0x20010000;
  memcpy((void*)0x20010000, "\xeb\x52\x90\x4e\x54\x46\x53\x20\x20\x20\x20\x00"
                            "\x04\x02\x00\x00\x00\x00\x00\x00\x00\xf8\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x80\x00\x80\x00\xff\x07\x00\x00\x00\x00\x00\x00"
                            "\x08\x00\x00\x00\x00\x00\x00\x00\xff\x01\x00\x00"
                            "\x00\x00\x00\x00\xf6\x00\x00\x00\x02",
         69);
  *(uint64_t*)0x20000208 = 0x45;
  *(uint64_t*)0x20000210 = 0;
  *(uint64_t*)0x20000218 = 0x20010300;
  memcpy((void*)0x20010300,
         "\x46\x49\x4c\x45\x30\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
         "\x00\x01\x00\x38\x00\x01\x00\x98\x01\x00\x00\x00\x04\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00"
         "\x00\x00\x00\x00\x00\x10\x00\x00\x00\x60\x00\x00\x00\x00\x00\x18\x00"
         "\x00\x00\x00\x00\x48\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x30"
         "\x00\x00\x00\x68\x00\x00\x00\x00\x00\x18\x00\x00\x00\x02\x00\x4a\x00"
         "\x00\x00\x18\x00\x01\x00\x05\x00\x00\x00\x00\x00\x05\x00\x00\xba\x92"
         "\x18\x34\xc8\xd3\x01\x00\xba\x92\x18\x34\xc8\xd3\x01\x00\xba\x92\x18"
         "\x34\xc8\xd3\x01\x00\xba\x92\x18\x34\xc8\xd3\x01\x00\x70\x00\x00\x00"
         "\x00\x00\x00\x00\x6c\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00"
         "\x00\x00\x04\x03\x24\x00\x4d\x00\x46\x00\x54\x00\x00\x00\x00\x00\x00"
         "\x00\x80\x00\x00\x00\x48\x00\x00\x00\x01\x00\x40\x00\x00\x00\x01\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x25\x00\x00\x00\x00\x00\x00\x00\x40"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x30\x01\x00\x00\x00\x00\x00\x00\x24"
         "\x01\x00\x00\x00\x00\x00",
         312);
  *(uint64_t*)0x20000220 = 0x138;
  *(uint64_t*)0x20000228 = 0x4000;
  *(uint64_t*)0x20000230 = 0x20010500;
  memcpy((void*)0x20010500, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                            "\x00\x00\x00\x00\x00\x00\x0b\x00",
         32);
  *(uint64_t*)0x20000238 = 0x20;
  *(uint64_t*)0x20000240 = 0x41e0;
  *(uint64_t*)0x20000248 = 0x20010600;
  memcpy((void*)0x20010600,
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x46\x49"
         "\x4c\x45\x30\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x01"
         "\x00\x38\x00\x01\x00\x58\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x00"
         "\x00\x00\x00\x10\x00\x00\x00\x60\x00\x00\x00\x00\x00\x18\x00\x00\x00"
         "\x00\x00\x48\x00\x00\x00\x18\x00\x00\x00\x00\xba\x92\x18\x34\xc8\xd3"
         "\x01\x00\xba\x92\x18\x34\xc8\xd3\x01\x00\xba\x92\x18\x34\xc8\xd3\x01"
         "\x00\xba\x92\x18\x34\xc8\xd3\x01\x06\x00\x00\x00\x00\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x30\x00\x00"
         "\x00\x70\x00\x00\x00\x00\x00\x18\x00\x00\x00\x02\x00\x52\x00\x00\x00"
         "\x18\x00\x01\x00\x05\x00\x00\x00\x00\x00\x05\x00\x00\xba\x92\x18\x34"
         "\xc8\xd3\x01\x00\xba\x92\x18\x34\xc8\xd3\x01\x00\xba\x92\x18\x34\xc8"
         "\xd3\x01\x00\xba\x92\x18\x34\xc8\xd3\x01\x00\x10\x00\x00\x00\x00\x00"
         "\x00\x00\x10\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00"
         "\x08\x03\x24\x00\x4d\x00\x46\x00\x54\x00\x4d\x00\x69\x00\x72\x00\x72"
         "\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x48\x00\x00\x00\x01\x00"
         "\x40\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00"
         "\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00"
         "\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00"
         "\x00\x00\x00\x21\x02\xff\x01\x00\x00\x00\x00\xff\xff\xff\xff\x00\x00"
         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
         384);
  *(uint64_t*)0x20000250 = 0x180;
  *(uint64_t*)0x20000258 = 0x43e0;
  *(uint64_t*)0x20000260 = 0x20010800;
  *(uint64_t*)0x20000268 = 0;
  *(uint64_t*)0x20000270 = 0x45e0;
  *(uint64_t*)0x20000278 = 0x20010900;
  *(uint64_t*)0x20000280 = 0;
  *(uint64_t*)0x20000288 = 0x47e0;
  *(uint64_t*)0x20000290 = 0x20010b00;
  *(uint64_t*)0x20000298 = 0;
  *(uint64_t*)0x200002a0 = 0x49e0;
  *(uint64_t*)0x200002a8 = 0x20010c00;
  *(uint64_t*)0x200002b0 = 0;
  *(uint64_t*)0x200002b8 = 0x4be0;
  *(uint64_t*)0x200002c0 = 0x20010f00;
  *(uint64_t*)0x200002c8 = 0;
  *(uint64_t*)0x200002d0 = 0x4fe0;
  *(uint64_t*)0x200002d8 = 0x20011100;
  *(uint64_t*)0x200002e0 = 0;
  *(uint64_t*)0x200002e8 = 0x51e0;
  *(uint64_t*)0x200002f0 = 0x20011200;
  *(uint64_t*)0x200002f8 = 0;
  *(uint64_t*)0x20000300 = 0x53e0;
  *(uint64_t*)0x20000308 = 0x20011500;
  *(uint64_t*)0x20000310 = 0;
  *(uint64_t*)0x20000318 = 0x57e0;
  *(uint64_t*)0x20000320 = 0x20011700;
  *(uint64_t*)0x20000328 = 0;
  *(uint64_t*)0x20000330 = 0x59e0;
  *(uint64_t*)0x20000338 = 0x20011800;
  *(uint64_t*)0x20000340 = 0;
  *(uint64_t*)0x20000348 = 0x5be0;
  *(uint64_t*)0x20000350 = 0x20011a00;
  *(uint64_t*)0x20000358 = 0;
  *(uint64_t*)0x20000360 = 0x5de0;
  *(uint64_t*)0x20000368 = 0x20011b00;
  *(uint64_t*)0x20000370 = 0;
  *(uint64_t*)0x20000378 = 0x5fe0;
  *(uint64_t*)0x20000380 = 0x20011d00;
  *(uint64_t*)0x20000388 = 0;
  *(uint64_t*)0x20000390 = 0x61e0;
  *(uint64_t*)0x20000398 = 0x20011e00;
  *(uint64_t*)0x200003a0 = 0;
  *(uint64_t*)0x200003a8 = 0x63e0;
  *(uint64_t*)0x200003b0 = 0x20012100;
  *(uint64_t*)0x200003b8 = 0;
  *(uint64_t*)0x200003c0 = 0x67e0;
  *(uint64_t*)0x200003c8 = 0x20012300;
  *(uint64_t*)0x200003d0 = 0;
  *(uint64_t*)0x200003d8 = 0x69e0;
  *(uint64_t*)0x200003e0 = 0x20012400;
  *(uint64_t*)0x200003e8 = 0;
  *(uint64_t*)0x200003f0 = 0x6be0;
  *(uint64_t*)0x200003f8 = 0x20012700;
  *(uint64_t*)0x20000400 = 0;
  *(uint64_t*)0x20000408 = 0x6fe0;
  *(uint64_t*)0x20000410 = 0x20012900;
  *(uint64_t*)0x20000418 = 0;
  *(uint64_t*)0x20000420 = 0x71e0;
  *(uint64_t*)0x20000428 = 0x20012a00;
  *(uint64_t*)0x20000430 = 0;
  *(uint64_t*)0x20000438 = 0x73e0;
  *(uint64_t*)0x20000440 = 0x20012c00;
  *(uint64_t*)0x20000448 = 0;
  *(uint64_t*)0x20000450 = 0x75e0;
  *(uint64_t*)0x20000458 = 0x20012d00;
  *(uint64_t*)0x20000460 = 0;
  *(uint64_t*)0x20000468 = 0x77e0;
  *(uint64_t*)0x20000470 = 0x20012f00;
  *(uint64_t*)0x20000478 = 0;
  *(uint64_t*)0x20000480 = 0x79e0;
  *(uint64_t*)0x20000488 = 0x20013000;
  *(uint64_t*)0x20000490 = 0;
  *(uint64_t*)0x20000498 = 0x7be0;
  *(uint64_t*)0x200004a0 = 0x20013200;
  *(uint64_t*)0x200004a8 = 0;
  *(uint64_t*)0x200004b0 = 0x7de0;
  *(uint64_t*)0x200004b8 = 0x20013300;
  *(uint64_t*)0x200004c0 = 0;
  *(uint64_t*)0x200004c8 = 0x7fe0;
  *(uint64_t*)0x200004d0 = 0x20013400;
  *(uint64_t*)0x200004d8 = 0;
  *(uint64_t*)0x200004e0 = 0x81e0;
  *(uint64_t*)0x200004e8 = 0x20013500;
  *(uint64_t*)0x200004f0 = 0;
  *(uint64_t*)0x200004f8 = 0x83e0;
  *(uint64_t*)0x20000500 = 0x20013600;
  *(uint64_t*)0x20000508 = 0;
  *(uint64_t*)0x20000510 = 0x85e0;
  *(uint64_t*)0x20000518 = 0x20013700;
  *(uint64_t*)0x20000520 = 0;
  *(uint64_t*)0x20000528 = 0x87e0;
  *(uint64_t*)0x20000530 = 0x20013800;
  *(uint64_t*)0x20000538 = 0;
  *(uint64_t*)0x20000540 = 0x89e0;
  *(uint64_t*)0x20000548 = 0x20013900;
  *(uint64_t*)0x20000550 = 0;
  *(uint64_t*)0x20000558 = 0x8be0;
  *(uint64_t*)0x20000560 = 0x20013a00;
  *(uint64_t*)0x20000568 = 0;
  *(uint64_t*)0x20000570 = 0x8de0;
  *(uint64_t*)0x20000578 = 0x20013b00;
  *(uint64_t*)0x20000580 = 0;
  *(uint64_t*)0x20000588 = 0x8fe0;
  *(uint64_t*)0x20000590 = 0x20013c00;
  *(uint64_t*)0x20000598 = 0;
  *(uint64_t*)0x200005a0 = 0x91e0;
  *(uint64_t*)0x200005a8 = 0x20013d00;
  *(uint64_t*)0x200005b0 = 0;
  *(uint64_t*)0x200005b8 = 0x93e0;
  *(uint64_t*)0x200005c0 = 0x20013e00;
  *(uint64_t*)0x200005c8 = 0;
  *(uint64_t*)0x200005d0 = 0x95e0;
  *(uint64_t*)0x200005d8 = 0x20013f00;
  *(uint64_t*)0x200005e0 = 0;
  *(uint64_t*)0x200005e8 = 0x97e0;
  *(uint64_t*)0x200005f0 = 0x20014000;
  *(uint64_t*)0x200005f8 = 0;
  *(uint64_t*)0x20000600 = 0x99e0;
  *(uint64_t*)0x20000608 = 0x20014100;
  *(uint64_t*)0x20000610 = 0;
  *(uint64_t*)0x20000618 = 0x9be0;
  *(uint64_t*)0x20000620 = 0x20014200;
  *(uint64_t*)0x20000628 = 0;
  *(uint64_t*)0x20000630 = 0x9de0;
  *(uint64_t*)0x20000638 = 0x20014300;
  *(uint64_t*)0x20000640 = 0;
  *(uint64_t*)0x20000648 = 0x9fe0;
  *(uint64_t*)0x20000650 = 0x20014600;
  *(uint64_t*)0x20000658 = 0;
  *(uint64_t*)0x20000660 = 0xa3e0;
  *(uint64_t*)0x20000668 = 0x20014800;
  *(uint64_t*)0x20000670 = 0;
  *(uint64_t*)0x20000678 = 0xa5e0;
  *(uint64_t*)0x20000680 = 0x20014900;
  *(uint64_t*)0x20000688 = 0;
  *(uint64_t*)0x20000690 = 0xa7e0;
  *(uint64_t*)0x20000698 = 0x20014b00;
  *(uint64_t*)0x200006a0 = 0;
  *(uint64_t*)0x200006a8 = 0xa9e0;
  *(uint64_t*)0x200006b0 = 0x20014c00;
  *(uint64_t*)0x200006b8 = 0;
  *(uint64_t*)0x200006c0 = 0xabe0;
  *(uint64_t*)0x200006c8 = 0x20014d00;
  *(uint64_t*)0x200006d0 = 0;
  *(uint64_t*)0x200006d8 = 0xade0;
  *(uint64_t*)0x200006e0 = 0x20014e00;
  *(uint64_t*)0x200006e8 = 0;
  *(uint64_t*)0x200006f0 = 0xafe0;
  *(uint64_t*)0x200006f8 = 0x20014f00;
  *(uint64_t*)0x20000700 = 0;
  *(uint64_t*)0x20000708 = 0xb1e0;
  *(uint64_t*)0x20000710 = 0x20015000;
  *(uint64_t*)0x20000718 = 0;
  *(uint64_t*)0x20000720 = 0xb3e0;
  *(uint64_t*)0x20000728 = 0x20015100;
  *(uint64_t*)0x20000730 = 0;
  *(uint64_t*)0x20000738 = 0xb5e0;
  *(uint64_t*)0x20000740 = 0x20015200;
  *(uint64_t*)0x20000748 = 0;
  *(uint64_t*)0x20000750 = 0xb7e0;
  *(uint64_t*)0x20000758 = 0x20015300;
  *(uint64_t*)0x20000760 = 0;
  *(uint64_t*)0x20000768 = 0xb9e0;
  *(uint64_t*)0x20000770 = 0x20015400;
  *(uint64_t*)0x20000778 = 0;
  *(uint64_t*)0x20000780 = 0xbbe0;
  *(uint64_t*)0x20000788 = 0x20015500;
  *(uint64_t*)0x20000790 = 0;
  *(uint64_t*)0x20000798 = 0xbde0;
  *(uint64_t*)0x200007a0 = 0x20015600;
  *(uint64_t*)0x200007a8 = 0;
  *(uint64_t*)0x200007b0 = 0xbfe0;
  *(uint64_t*)0x200007b8 = 0x20015700;
  *(uint64_t*)0x200007c0 = 0;
  *(uint64_t*)0x200007c8 = 0xc1e0;
  *(uint64_t*)0x200007d0 = 0x20015800;
  *(uint64_t*)0x200007d8 = 0;
  *(uint64_t*)0x200007e0 = 0xc3e0;
  *(uint64_t*)0x200007e8 = 0x20015900;
  *(uint64_t*)0x200007f0 = 0;
  *(uint64_t*)0x200007f8 = 0xc5e0;
  *(uint64_t*)0x20000800 = 0x20015a00;
  *(uint64_t*)0x20000808 = 0;
  *(uint64_t*)0x20000810 = 0xc7e0;
  *(uint64_t*)0x20000818 = 0x20015b00;
  *(uint64_t*)0x20000820 = 0;
  *(uint64_t*)0x20000828 = 0xc9e0;
  *(uint64_t*)0x20000830 = 0x20015c00;
  *(uint64_t*)0x20000838 = 0;
  *(uint64_t*)0x20000840 = 0xcbe0;
  *(uint64_t*)0x20000848 = 0x20015d00;
  *(uint64_t*)0x20000850 = 0;
  *(uint64_t*)0x20000858 = 0xcde0;
  *(uint64_t*)0x20000860 = 0x20015e00;
  *(uint64_t*)0x20000868 = 0;
  *(uint64_t*)0x20000870 = 0xcfe0;
  *(uint64_t*)0x20000878 = 0x20015f00;
  *(uint64_t*)0x20000880 = 0;
  *(uint64_t*)0x20000888 = 0xd1e0;
  *(uint64_t*)0x20000890 = 0x20016000;
  *(uint64_t*)0x20000898 = 0;
  *(uint64_t*)0x200008a0 = 0xd3e0;
  *(uint64_t*)0x200008a8 = 0x20016100;
  *(uint64_t*)0x200008b0 = 0;
  *(uint64_t*)0x200008b8 = 0xd5e0;
  *(uint64_t*)0x200008c0 = 0x20016200;
  *(uint64_t*)0x200008c8 = 0;
  *(uint64_t*)0x200008d0 = 0xd7e0;
  *(uint64_t*)0x200008d8 = 0x20016300;
  *(uint64_t*)0x200008e0 = 0;
  *(uint64_t*)0x200008e8 = 0xd9e0;
  *(uint64_t*)0x200008f0 = 0x20016400;
  *(uint64_t*)0x200008f8 = 0;
  *(uint64_t*)0x20000900 = 0xdbe0;
  *(uint64_t*)0x20000908 = 0x20016500;
  *(uint64_t*)0x20000910 = 0;
  *(uint64_t*)0x20000918 = 0xdde0;
  *(uint64_t*)0x20000920 = 0x20016600;
  *(uint64_t*)0x20000928 = 0;
  *(uint64_t*)0x20000930 = 0xdfe0;
  *(uint64_t*)0x20000938 = 0x20016700;
  *(uint64_t*)0x20000940 = 0;
  *(uint64_t*)0x20000948 = 0xe1e0;
  *(uint64_t*)0x20000950 = 0x20016800;
  *(uint64_t*)0x20000958 = 0;
  *(uint64_t*)0x20000960 = 0xe3e0;
  *(uint64_t*)0x20000968 = 0x20016900;
  *(uint64_t*)0x20000970 = 0;
  *(uint64_t*)0x20000978 = 0xe5e0;
  *(uint64_t*)0x20000980 = 0x20016a00;
  *(uint64_t*)0x20000988 = 0;
  *(uint64_t*)0x20000990 = 0xe7e0;
  *(uint64_t*)0x20000998 = 0x20016b00;
  *(uint64_t*)0x200009a0 = 0;
  *(uint64_t*)0x200009a8 = 0xe9e0;
  *(uint64_t*)0x200009b0 = 0x20016c00;
  *(uint64_t*)0x200009b8 = 0;
  *(uint64_t*)0x200009c0 = 0xebe0;
  *(uint64_t*)0x200009c8 = 0x20016d00;
  *(uint64_t*)0x200009d0 = 0;
  *(uint64_t*)0x200009d8 = 0xede0;
  *(uint64_t*)0x200009e0 = 0x20016e00;
  *(uint64_t*)0x200009e8 = 0;
  *(uint64_t*)0x200009f0 = 0xefe0;
  *(uint64_t*)0x200009f8 = 0x20016f00;
  *(uint64_t*)0x20000a00 = 0;
  *(uint64_t*)0x20000a08 = 0xf1e0;
  *(uint64_t*)0x20000a10 = 0x20017000;
  *(uint64_t*)0x20000a18 = 0;
  *(uint64_t*)0x20000a20 = 0xf3e0;
  *(uint64_t*)0x20000a28 = 0x20017100;
  *(uint64_t*)0x20000a30 = 0;
  *(uint64_t*)0x20000a38 = 0xf5e0;
  *(uint64_t*)0x20000a40 = 0x20017200;
  *(uint64_t*)0x20000a48 = 0;
  *(uint64_t*)0x20000a50 = 0xf7e0;
  *(uint64_t*)0x20000a58 = 0x20017300;
  *(uint64_t*)0x20000a60 = 0;
  *(uint64_t*)0x20000a68 = 0xf9e0;
  *(uint64_t*)0x20000a70 = 0x20017400;
  *(uint64_t*)0x20000a78 = 0;
  *(uint64_t*)0x20000a80 = 0xfbe0;
  *(uint64_t*)0x20000a88 = 0x20017500;
  *(uint64_t*)0x20000a90 = 0;
  *(uint64_t*)0x20000a98 = 0xfde0;
  *(uint64_t*)0x20000aa0 = 0x20017600;
  *(uint64_t*)0x20000aa8 = 0;
  *(uint64_t*)0x20000ab0 = 0xffe0;
  *(uint64_t*)0x20000ab8 = 0x20017700;
  *(uint64_t*)0x20000ac0 = 0;
  *(uint64_t*)0x20000ac8 = 0x101e0;
  *(uint64_t*)0x20000ad0 = 0x20017800;
  *(uint64_t*)0x20000ad8 = 0;
  *(uint64_t*)0x20000ae0 = 0x103e0;
  *(uint64_t*)0x20000ae8 = 0x20017900;
  *(uint64_t*)0x20000af0 = 0;
  *(uint64_t*)0x20000af8 = 0x105e0;
  *(uint64_t*)0x20000b00 = 0x20017a00;
  *(uint64_t*)0x20000b08 = 0;
  *(uint64_t*)0x20000b10 = 0x107e0;
  *(uint64_t*)0x20000b18 = 0x20017b00;
  *(uint64_t*)0x20000b20 = 0;
  *(uint64_t*)0x20000b28 = 0x109e0;
  *(uint64_t*)0x20000b30 = 0x20017c00;
  *(uint64_t*)0x20000b38 = 0;
  *(uint64_t*)0x20000b40 = 0x10be0;
  *(uint64_t*)0x20000b48 = 0x20017d00;
  *(uint64_t*)0x20000b50 = 0;
  *(uint64_t*)0x20000b58 = 0x10de0;
  *(uint64_t*)0x20000b60 = 0x20017e00;
  *(uint64_t*)0x20000b68 = 0;
  *(uint64_t*)0x20000b70 = 0x10fe0;
  *(uint64_t*)0x20000b78 = 0x20017f00;
  *(uint64_t*)0x20000b80 = 0;
  *(uint64_t*)0x20000b88 = 0x111e0;
  *(uint64_t*)0x20000b90 = 0x20018000;
  *(uint64_t*)0x20000b98 = 0;
  *(uint64_t*)0x20000ba0 = 0x113e0;
  *(uint64_t*)0x20000ba8 = 0x20018100;
  *(uint64_t*)0x20000bb0 = 0;
  *(uint64_t*)0x20000bb8 = 0x115e0;
  *(uint64_t*)0x20000bc0 = 0x20018200;
  *(uint64_t*)0x20000bc8 = 0;
  *(uint64_t*)0x20000bd0 = 0x117e0;
  *(uint64_t*)0x20000bd8 = 0x20018300;
  *(uint64_t*)0x20000be0 = 0;
  *(uint64_t*)0x20000be8 = 0x119e0;
  *(uint64_t*)0x20000bf0 = 0x20018400;
  *(uint64_t*)0x20000bf8 = 0;
  *(uint64_t*)0x20000c00 = 0x11be0;
  *(uint64_t*)0x20000c08 = 0x20018500;
  *(uint64_t*)0x20000c10 = 0;
  *(uint64_t*)0x20000c18 = 0x11de0;
  *(uint64_t*)0x20000c20 = 0x20018600;
  *(uint64_t*)0x20000c28 = 0;
  *(uint64_t*)0x20000c30 = 0x11fe0;
  *(uint64_t*)0x20000c38 = 0x20018700;
  *(uint64_t*)0x20000c40 = 0;
  *(uint64_t*)0x20000c48 = 0x121e0;
  *(uint64_t*)0x20000c50 = 0x20018800;
  *(uint64_t*)0x20000c58 = 0;
  *(uint64_t*)0x20000c60 = 0x123e0;
  *(uint64_t*)0x20000c68 = 0x20018900;
  *(uint64_t*)0x20000c70 = 0;
  *(uint64_t*)0x20000c78 = 0x125e0;
  *(uint64_t*)0x20000c80 = 0x20018a00;
  *(uint64_t*)0x20000c88 = 0;
  *(uint64_t*)0x20000c90 = 0x127e0;
  *(uint64_t*)0x20000c98 = 0x20018b00;
  *(uint64_t*)0x20000ca0 = 0;
  *(uint64_t*)0x20000ca8 = 0x129e0;
  *(uint64_t*)0x20000cb0 = 0x20018c00;
  *(uint64_t*)0x20000cb8 = 0;
  *(uint64_t*)0x20000cc0 = 0x12be0;
  *(uint64_t*)0x20000cc8 = 0x20018d00;
  *(uint64_t*)0x20000cd0 = 0;
  *(uint64_t*)0x20000cd8 = 0x12de0;
  *(uint64_t*)0x20000ce0 = 0x20018e00;
  *(uint64_t*)0x20000ce8 = 0;
  *(uint64_t*)0x20000cf0 = 0x12fe0;
  *(uint64_t*)0x20000cf8 = 0x20018f00;
  *(uint64_t*)0x20000d00 = 0;
  *(uint64_t*)0x20000d08 = 0x131e0;
  *(uint64_t*)0x20000d10 = 0x20019000;
  *(uint64_t*)0x20000d18 = 0;
  *(uint64_t*)0x20000d20 = 0x133e0;
  *(uint64_t*)0x20000d28 = 0x20019100;
  *(uint64_t*)0x20000d30 = 0;
  *(uint64_t*)0x20000d38 = 0x135e0;
  *(uint64_t*)0x20000d40 = 0x20019200;
  *(uint64_t*)0x20000d48 = 0;
  *(uint64_t*)0x20000d50 = 0x137e0;
  *(uint64_t*)0x20000d58 = 0x20019300;
  *(uint64_t*)0x20000d60 = 0;
  *(uint64_t*)0x20000d68 = 0x139e0;
  *(uint64_t*)0x20000d70 = 0x20019400;
  *(uint64_t*)0x20000d78 = 0;
  *(uint64_t*)0x20000d80 = 0x13be0;
  *(uint64_t*)0x20000d88 = 0x20019500;
  *(uint64_t*)0x20000d90 = 0;
  *(uint64_t*)0x20000d98 = 0x13de0;
  *(uint64_t*)0x20000da0 = 0x20019600;
  *(uint64_t*)0x20000da8 = 0;
  *(uint64_t*)0x20000db0 = 0x13fe0;
  *(uint64_t*)0x20000db8 = 0x20019800;
  *(uint64_t*)0x20000dc0 = 0;
  *(uint64_t*)0x20000dc8 = 0x141e0;
  *(uint64_t*)0x20000dd0 = 0x20019900;
  *(uint64_t*)0x20000dd8 = 0;
  *(uint64_t*)0x20000de0 = 0x143e0;
  *(uint64_t*)0x20000de8 = 0x20019c00;
  *(uint64_t*)0x20000df0 = 0;
  *(uint64_t*)0x20000df8 = 0x147e0;
  *(uint64_t*)0x20000e00 = 0x20019e00;
  *(uint64_t*)0x20000e08 = 0;
  *(uint64_t*)0x20000e10 = 0x149e0;
  *(uint64_t*)0x20000e18 = 0x20019f00;
  *(uint64_t*)0x20000e20 = 0;
  *(uint64_t*)0x20000e28 = 0x14be0;
  *(uint64_t*)0x20000e30 = 0x2001a100;
  *(uint64_t*)0x20000e38 = 0;
  *(uint64_t*)0x20000e40 = 0x14de0;
  *(uint64_t*)0x20000e48 = 0x2001a200;
  *(uint64_t*)0x20000e50 = 0;
  *(uint64_t*)0x20000e58 = 0x14fe0;
  *(uint64_t*)0x20000e60 = 0x2001a400;
  *(uint64_t*)0x20000e68 = 0;
  *(uint64_t*)0x20000e70 = 0x151e0;
  *(uint64_t*)0x20000e78 = 0x2001a500;
  *(uint64_t*)0x20000e80 = 0;
  *(uint64_t*)0x20000e88 = 0x153e0;
  *(uint64_t*)0x20000e90 = 0x2001a700;
  *(uint64_t*)0x20000e98 = 0;
  *(uint64_t*)0x20000ea0 = 0x155e0;
  *(uint64_t*)0x20000ea8 = 0x2001a800;
  *(uint64_t*)0x20000eb0 = 0;
  *(uint64_t*)0x20000eb8 = 0x157e0;
  *(uint64_t*)0x20000ec0 = 0x2001ab00;
  *(uint64_t*)0x20000ec8 = 0;
  *(uint64_t*)0x20000ed0 = 0x15be0;
  *(uint64_t*)0x20000ed8 = 0x2001ad00;
  *(uint64_t*)0x20000ee0 = 0;
  *(uint64_t*)0x20000ee8 = 0x15de0;
  *(uint64_t*)0x20000ef0 = 0x2001ae00;
  *(uint64_t*)0x20000ef8 = 0;
  *(uint64_t*)0x20000f00 = 0x15fe0;
  *(uint64_t*)0x20000f08 = 0x2001b000;
  *(uint64_t*)0x20000f10 = 0;
  *(uint64_t*)0x20000f18 = 0x161e0;
  *(uint64_t*)0x20000f20 = 0x2001b100;
  *(uint64_t*)0x20000f28 = 0;
  *(uint64_t*)0x20000f30 = 0x163e0;
  *(uint64_t*)0x20000f38 = 0x2001b200;
  *(uint64_t*)0x20000f40 = 0;
  *(uint64_t*)0x20000f48 = 0x43800;
  *(uint64_t*)0x20000f50 = 0x2001b300;
  *(uint64_t*)0x20000f58 = 0;
  *(uint64_t*)0x20000f60 = 0x44800;
  *(uint64_t*)0x20000f68 = 0x2001b400;
  *(uint64_t*)0x20000f70 = 0;
  *(uint64_t*)0x20000f78 = 0x45000;
  *(uint64_t*)0x20000f80 = 0x2001bc00;
  *(uint64_t*)0x20000f88 = 0;
  *(uint64_t*)0x20000f90 = 0x457e0;
  *(uint64_t*)0x20000f98 = 0x2001bd00;
  *(uint64_t*)0x20000fa0 = 0;
  *(uint64_t*)0x20000fa8 = 0x459e0;
  *(uint64_t*)0x20000fb0 = 0x2001be00;
  *(uint64_t*)0x20000fb8 = 0;
  *(uint64_t*)0x20000fc0 = 0x45be0;
  *(uint64_t*)0x20000fc8 = 0x2001bf00;
  *(uint64_t*)0x20000fd0 = 0;
  *(uint64_t*)0x20000fd8 = 0x45de0;
  *(uint64_t*)0x20000fe0 = 0x2001c000;
  *(uint64_t*)0x20000fe8 = 0;
  *(uint64_t*)0x20000ff0 = 0x45fe0;
  *(uint64_t*)0x20000ff8 = 0x2001c100;
  *(uint64_t*)0x20001000 = 0;
  *(uint64_t*)0x20001008 = 0x46080;
  *(uint64_t*)0x20001010 = 0x2001c200;
  *(uint64_t*)0x20001018 = 0;
  *(uint64_t*)0x20001020 = 0x46120;
  *(uint64_t*)0x20001028 = 0x2001c300;
  *(uint64_t*)0x20001030 = 0;
  *(uint64_t*)0x20001038 = 0x461c0;
  *(uint64_t*)0x20001040 = 0x2001c400;
  *(uint64_t*)0x20001048 = 0;
  *(uint64_t*)0x20001050 = 0x46260;
  *(uint64_t*)0x20001058 = 0x2001c500;
  *(uint64_t*)0x20001060 = 0;
  *(uint64_t*)0x20001068 = 0x46300;
  *(uint64_t*)0x20001070 = 0x2001c600;
  *(uint64_t*)0x20001078 = 0;
  *(uint64_t*)0x20001080 = 0x463a0;
  *(uint64_t*)0x20001088 = 0x2001c700;
  *(uint64_t*)0x20001090 = 0;
  *(uint64_t*)0x20001098 = 0x46440;
  *(uint64_t*)0x200010a0 = 0x2001c800;
  *(uint64_t*)0x200010a8 = 0;
  *(uint64_t*)0x200010b0 = 0x464e0;
  *(uint64_t*)0x200010b8 = 0x2001c900;
  *(uint64_t*)0x200010c0 = 0;
  *(uint64_t*)0x200010c8 = 0x46580;
  *(uint64_t*)0x200010d0 = 0x2001ca00;
  *(uint64_t*)0x200010d8 = 0;
  *(uint64_t*)0x200010e0 = 0x46620;
  *(uint64_t*)0x200010e8 = 0x2001cb00;
  *(uint64_t*)0x200010f0 = 0;
  *(uint64_t*)0x200010f8 = 0x466c0;
  *(uint64_t*)0x20001100 = 0x2001cc00;
  *(uint64_t*)0x20001108 = 0;
  *(uint64_t*)0x20001110 = 0x46760;
  *(uint64_t*)0x20001118 = 0x2001cd00;
  *(uint64_t*)0x20001120 = 0;
  *(uint64_t*)0x20001128 = 0x46800;
  *(uint64_t*)0x20001130 = 0x2001ce00;
  *(uint64_t*)0x20001138 = 0;
  *(uint64_t*)0x20001140 = 0x468a0;
  *(uint64_t*)0x20001148 = 0x2001cf00;
  *(uint64_t*)0x20001150 = 0;
  *(uint64_t*)0x20001158 = 0x46940;
  *(uint64_t*)0x20001160 = 0x2001d000;
  *(uint64_t*)0x20001168 = 0;
  *(uint64_t*)0x20001170 = 0x47000;
  *(uint64_t*)0x20001178 = 0x2001d100;
  *(uint64_t*)0x20001180 = 0;
  *(uint64_t*)0x20001188 = 0x47800;
  *(uint64_t*)0x20001190 = 0x2001d200;
  *(uint64_t*)0x20001198 = 0;
  *(uint64_t*)0x200011a0 = 0x87800;
  *(uint64_t*)0x200011a8 = 0x2001d300;
  *(uint64_t*)0x200011b0 = 0;
  *(uint64_t*)0x200011b8 = 0x88000;
  *(uint64_t*)0x200011c0 = 0x2003d300;
  *(uint64_t*)0x200011c8 = 0;
  *(uint64_t*)0x200011d0 = 0xff800;
  *(uint64_t*)0x200011d8 = 0x2003d500;
  *(uint64_t*)0x200011e0 = 0;
  *(uint64_t*)0x200011e8 = 0xff9e0;
  *(uint64_t*)0x200011f0 = 0x2003d600;
  *(uint64_t*)0x200011f8 = 0;
  *(uint64_t*)0x20001200 = 0xffbe0;
  *(uint64_t*)0x20001208 = 0x2003d800;
  *(uint64_t*)0x20001210 = 0;
  *(uint64_t*)0x20001218 = 0xffde0;
  *(uint64_t*)0x20001220 = 0x2003d900;
  *(uint64_t*)0x20001228 = 0;
  *(uint64_t*)0x20001230 = 0xfffe0;
  *(uint64_t*)0x20001238 = 0x2003db00;
  *(uint64_t*)0x20001240 = 0;
  *(uint64_t*)0x20001248 = 0x1001e0;
  *(uint64_t*)0x20001250 = 0x2003dc00;
  *(uint64_t*)0x20001258 = 0;
  *(uint64_t*)0x20001260 = 0x1003e0;
  *(uint64_t*)0x20001268 = 0x2003df00;
  *(uint64_t*)0x20001270 = 0;
  *(uint64_t*)0x20001278 = 0x1007e0;
  *(uint64_t*)0x20001280 = 0x2007e000;
  *(uint64_t*)0x20001288 = 0;
  *(uint64_t*)0x20001290 = 0x1ffc00;
  *(uint64_t*)0x20001298 = 0x2007e100;
  *(uint64_t*)0x200012a0 = 0;
  *(uint64_t*)0x200012a8 = 0x1ffde0;
  *(uint8_t*)0x2007e200 = 0;
  syz_mount_image(0x20000000, 0x20000100, 0, 0xb2, 0x20000200, 0, 0x2007e200);
}

int main()
{
  syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
  loop();
  return 0;
}