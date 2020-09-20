
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <linux/usb/functionfs.h>

#define PROGMEM
#include "../../arduino/dsremap/DS4HID.h"

#include "utils.h"
#include "config.h"

#define CONFIGFS_MP "/mnt/configfs"
#define FUNCTIONFS_MP "/mnt/functionfs"
#define GADGET_PATH "usb_gadget/dualshock"

static const struct {
  struct usb_functionfs_descs_head_v2 header;
  __le32 fs_count;
  __le32 hs_count;
  uint8_t fs_descs[32];
  uint8_t hs_descs[32];
} __attribute__((packed)) descriptor = {
  .header = {
    .magic = FUNCTIONFS_DESCRIPTORS_MAGIC_V2,
    .length = sizeof(descriptor),
    .flags = FUNCTIONFS_HAS_HS_DESC|FUNCTIONFS_HAS_FS_DESC,
  },
  .fs_count = 4,
  .hs_count = 4,
  .fs_descs = {
    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x02,        // bNumEndpoints 2
    0x03,        // bInterfaceClass
    0x00,        // bInterfaceSubClass
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x21,        // bDescriptorType (HID)
    0x11, 0x01,  // bcdHID 1.11
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    0x22,        // bDescriptorType[0] (HID)
    0xFB, 0x01,  // wDescriptorLength[0] 507

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x81,        // bEndpointAddress (IN/D2H)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x02,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)
  },
  .hs_descs = {
    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x02,        // bNumEndpoints 2
    0x03,        // bInterfaceClass
    0x00,        // bInterfaceSubClass
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x21,        // bDescriptorType (HID)
    0x11, 0x01,  // bcdHID 1.11
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    0x22,        // bDescriptorType[0] (HID)
    0xFB, 0x01,  // wDescriptorLength[0] 507

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x81,        // bEndpointAddress (IN/D2H)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x02,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)
  },
};

static const struct {
  struct usb_functionfs_strings_head header;
} __attribute__((packed)) strings = {
  .header = {
    .magic = FUNCTIONFS_STRINGS_MAGIC,
    .length = sizeof(strings),
    .str_count = 0,
    .lang_count = 0,
  },
};

int handle_setup(int fd, const struct usb_ctrlrequest* req, options_t* opt) {
  switch (req->bRequestType & 0x80) {
    case USB_DIR_IN:
      switch (req->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_STANDARD:
          switch (req->bRequestType & USB_RECIP_MASK) {
            case USB_RECIP_INTERFACE:
              switch (req->bRequest) {
                case 0x06: // Get_Descriptor
                  if (req->wValue == 0x2200) {
                    if (write(fd, usbHidReportDescriptor, sizeof(usbHidReportDescriptor)) < 0) {
                      perror("write report descriptor");
                      return -1;
                    }
                  }
                  break;
              }
              break;
          }
          break;
        case USB_TYPE_CLASS:
          switch (req->bRequestType & USB_RECIP_MASK) {
            case USB_RECIP_INTERFACE:
              switch (req->bRequest) {
                case 0x01:
                {
                  int reportType = req->wValue >> 8;
                  int reportId = req->wValue & 0xFF;
                  switch (reportType) {
                    case 0x01:
                      break;
                    case 0x03:
                      switch (reportId) {
                        case 0x02:
                          if (write(fd, opt->report02, sizeof(opt->report02)) < 0) {
                            perror("write report 02");
                            return -1;
                          }
                          break;
                        case 0xa3:
                          if (write(fd, opt->reporta3, sizeof(opt->reporta3)) < 0) {
                            perror("write report a3");
                            return -1;
                          }
                          break;
                        case 0x12:
                          if (write(fd, opt->report12, sizeof(opt->report12)) < 0) {
                            perror("write report 12");
                            return -1;
                          }
                          break;
                      }
                      break;
                    default:
                      break;
                  }
                }
                break;
              }
              break;
          }
          break;
      }
      break;
    case USB_DIR_OUT:
      switch (req->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_CLASS:
          switch (req->bRequestType & USB_RECIP_MASK) {
            case USB_RECIP_INTERFACE:
              switch (req->bRequest) {
                case 0x09:
                {
                  int reportType = req->wValue >> 8;
                  int reportId = req->wValue & 0xFF;
                  switch (reportType) {
                    case 0x03:
                      switch (reportId) {
                        case 0x13:
                        {
                          uint8_t data[23];
                          if (read(fd, data, 23) < 0) {
                            perror("read pairing data");
                            return -1;
                          } else {
                            printf("!! PS4: ");
                            for (int i = 0; i < 6; ++i) {
                              if (i != 0)
                                printf(":");
                              printf("%02X", data[6 - i]);
                            }
                            printf("\n");

                            printf("!! KEY: ");
                            for (int i = 0; i < 16; ++i)
                              printf("%02X", data[7 + i]);
                            printf("\n");

                            return 0;
                          }
                        }
                        break;
                      }
                      break;
                  }
                }
                break;
              }
              break;
          }
          break;
      }
      break;
  }

  return 1;
}

int do_functions(const char* udc, options_t *opt)
{
  int ret = -1;

  int fd = open(FUNCTIONFS_MP "/ep0", O_RDWR);
  if (fd < 0) {
    perror("open");
    goto failed0;
  }

  if (write(fd, &descriptor, sizeof(descriptor)) < 0) {
    perror("write descriptor");
    goto failed1;
  }

  if (write(fd, &strings, sizeof(strings)) < 0) {
    perror("write strings");
    goto failed1;
  }

  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/UDC", udc) < 0)
    goto failed1;

  int cont = 1;
  while (cont) {
    unsigned char bf[4096];

    int count = read(fd, (void*)bf, sizeof(bf));
    if (count < 0) {
      perror("read");
      break;
    }

    struct usb_functionfs_event *evt = (struct usb_functionfs_event*)bf;
    ret = 1;
    for (int i = count / sizeof(*evt); i && cont; --i, ++evt) {
      if (evt->type == FUNCTIONFS_SETUP) {
        int result = handle_setup(fd, &evt->u.setup, opt);
        if (result < 0) { // Error
          cont = 0;
          ret = -1;
          break;
        }
        else if (result == 0) { // Finished
          cont = 0;
          ret = 0;
          break;
        }
      }
    }
  }

failed1:
  close(fd);

failed0:
  return ret;
}

int main(int argc, char *argv[]) {
  int ret = 1;
  options_t opt;

  if (parse_options(argc, argv, &opt) != 0)
    goto fail_0;

  if (geteuid() != 0) {
    fprintf(stderr, "Must be run as root\n");
    goto fail_0;
  }

  if (test_file(CONFIGFS_MP) == ENOENT) {
    if (mkdir(CONFIGFS_MP, 0700) < 0) {
      perror("mkdir " CONFIGFS_MP);
      goto fail_0;
    }
  }

  if (test_file(FUNCTIONFS_MP) == ENOENT) {
    if (mkdir(FUNCTIONFS_MP, 0700) < 0) {
      perror("mkdir " FUNCTIONFS_MP);
      goto fail_0;
    }
  }

  if (launch_command("/bin/mount -t configfs none " CONFIGFS_MP) < 0)
    goto fail_0;

  if (mkdir(CONFIGFS_MP "/" GADGET_PATH, 0700) < 0)
    goto fail_1;

  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/bDeviceClass", "0x00") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/bDeviceSubClass", "0x00") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/bDeviceProtocol", "0x00") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/bMaxPacketSize0", "0x40") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/idVendor", "0x054c") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/idProduct", "0x09cc") < 0)
    goto fail_2;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/bcdDevice", "0x100") < 0)
    goto fail_2;

  if (mkdir(CONFIGFS_MP "/" GADGET_PATH "/configs/c.1", 0700) < 0)
    goto fail_2;

  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/configs/c.1/bmAttributes", "0xc0") < 0)
    goto fail_3;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/configs/c.1/MaxPower", "500") < 0)
    goto fail_3;

  if (mkdir(CONFIGFS_MP "/" GADGET_PATH "/functions/ffs.dualshock", 0700) < 0)
    goto fail_3;

  if (mkdir(CONFIGFS_MP "/" GADGET_PATH "/strings/0x409", 0700) < 0)
    goto fail_4;

  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/strings/0x409/manufacturer", "Sony Interactive Entertainment") < 0)
    goto fail_5;
  if (echo_to(CONFIGFS_MP "/" GADGET_PATH "/strings/0x409/product", "Wireless Controller") < 0)
    goto fail_5;

  if (symlink(CONFIGFS_MP "/" GADGET_PATH "/functions/ffs.dualshock", CONFIGFS_MP "/" GADGET_PATH "/configs/c.1/ffs.dualshock") < 0)
    goto fail_5;

  if (launch_command("/bin/mount -t functionfs dualshock " FUNCTIONFS_MP) < 0)
    goto fail_6;

  DIR* dir = opendir("/sys/class/udc");
  struct dirent* rd;
  int found = 0;
  while ((rd = readdir(dir))) {
    if (!strcmp(rd->d_name, "."))
      continue;
    if (!strcmp(rd->d_name, ".."))
      continue;

    found = 1;
    ret = do_functions(rd->d_name, &opt);
    break;
  }
  closedir(dir);

  if (!found)
    fprintf(stderr, "No UDC\n");

  echo_to(CONFIGFS_MP "/" GADGET_PATH "/UDC", "");
  launch_command("/bin/umount " FUNCTIONFS_MP);

fail_6:
  unlink(CONFIGFS_MP "/" GADGET_PATH "/configs/c.1/ffs.dualshock");

fail_5:
  rmdir(CONFIGFS_MP "/" GADGET_PATH "/strings/0x409");

fail_4:
  rmdir(CONFIGFS_MP "/" GADGET_PATH "/functions/ffs.dualshock");

fail_3:
  rmdir(CONFIGFS_MP "/" GADGET_PATH "/configs/c.1");

fail_2:
  rmdir(CONFIGFS_MP "/" GADGET_PATH);

fail_1:
  launch_command("/bin/umount " CONFIGFS_MP);

fail_0:
  return ret;
}
