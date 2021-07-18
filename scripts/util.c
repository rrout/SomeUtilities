#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
//#include "ee2prom-tera12.h"

#define EEPROM_MAX_SIZE             512

#define EE2PROM_BRIDGE_BASE         0xB1
#define EE2PROM_WRITE               _IO(EE2PROM_BRIDGE_BASE, 0)
#define EE2PROM_WRITE_BULK          _IO(EE2PROM_BRIDGE_BASE, 1)
#define EE2PROM_READ                _IO(EE2PROM_BRIDGE_BASE, 2)
#define EE2PROM_READ_BULK           _IO(EE2PROM_BRIDGE_BASE, 3)
#define EE2PROM_STATUS_WRITE        _IO(EE2PROM_BRIDGE_BASE, 4)
#define EE2PROM_STATUS_READ         _IO(EE2PROM_BRIDGE_BASE, 5)
typedef struct eeprom_data_s {
    unsigned int  eepromDevice;
    unsigned int  eepromAddress;
    unsigned int  eepromData;
    unsigned char *eepromBulkData;
    unsigned int  eepromBulkDataSize;
    unsigned int  eepromBlock0Protect;
    unsigned int  eepromBlock1Protect;
    unsigned int  eepromStatusReg;
}eeprom_data_t;

typedef struct TEST {
    int (*test_function)(void);
    char test_name[64];
    int status;
}TEST_T;

void help() {
printf("\n\
*******************************************************\n\
           WELCOME TO EEPROM UTILITY                   \n\
Use this utility to initialize/read/write the EEPROM   \n\
Diagonistics can also be done on both EEPROM as well   \n\
*******************************************************\n\
Command Options\n\
-r          Read  EEPROM\n\
-w          Write EEPROM\n\
-d          Device number\n\
-a          Address in Hex\n\
-v          Value to be writen in Hex\n\
-s          Size of values to be read from base address\n\
-i          Initoalize the EEPROM to default content\n\
-t          EEPROM Diagnostic\n\
\n\
Example:\n\
\t READ         : ee2PromRW -r -d 0 -a 0x01 -s 255\n\
\t WRITE        : ee2PromRW -w -d 1 -a 0x01 -v 0x55\n\
\t INIT DEFAULT : ee2PromRW -i -d 0\n\
\t DIAGONSTICS  : ee2PromRW -t\n\
"\
);
}

unsigned char pi_eeporom_template[EEPROM_MAX_SIZE] = {
0xF9, 0xFC, 0x01, 0x20, 0x30, 0x31, 0x42, 0x44, 0x53, 0x31, 0x2D, 0x42, 0x45, 0x41, 0x31, 0x46,
0x43, 0x39, 0x35, 0x34, 0x33, 0x42, 0x45, 0x41, 0x31, 0x49, 0x4E, 0x4D, 0x41, 0x53, 0x30, 0x30,
0x45, 0x52, 0x41, 0x31, 0x35, 0x2E, 0x31, 0x32, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x34, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x20, 0x30, 0x38, 0x41, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x64, 0x5F, 0x7D,
0x64, 0x5F, 0x7D, 0x5A, 0x55, 0x7D, 0x60, 0x5B, 0x7D, 0x69, 0x64, 0x7D, 0x5A, 0x55, 0x96, 0x69,
0x64, 0x96, 0x55, 0x50, 0x7D, 0x5A, 0x55, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x00, 0x00, 0x01, 0x10, 0x0F, 0x05, 0x0F, 0x03, 0xC8, 0x03, 0x9D, 0x03, 0x38, 0x02, 0xF6, 0x00,
0x00, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x02, 0x14, 0x14, 0x14, 0x14, 0x1E, 0x03, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x38, 0x30, 0x30, 0x2E, 0x30, 0x38, 0x30, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDA, 0x09,

0xF9, 0xFC, 0x01, 0x20, 0x30, 0x31, 0x42, 0x44, 0x53, 0x31, 0x2D, 0x42, 0x45, 0x41, 0x31, 0x46,
0x43, 0x39, 0x35, 0x34, 0x33, 0x42, 0x45, 0x41, 0x31, 0x49, 0x4E, 0x4D, 0x41, 0x53, 0x30, 0x30,
0x45, 0x52, 0x41, 0x31, 0x35, 0x2E, 0x31, 0x32, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x34, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x20, 0x30, 0x38, 0x41, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x64, 0x5F, 0x7D,
0x64, 0x5F, 0x7D, 0x5A, 0x55, 0x7D, 0x60, 0x5B, 0x7D, 0x69, 0x64, 0x7D, 0x5A, 0x55, 0x96, 0x69,
0x64, 0x96, 0x55, 0x50, 0x7D, 0x5A, 0x55, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
0x00, 0x00, 0x01, 0x10, 0x0F, 0x05, 0x0F, 0x03, 0xC8, 0x03, 0x9D, 0x03, 0x38, 0x02, 0xF6, 0x00,
0x00, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x02, 0x14, 0x14, 0x14, 0x14, 0x1E, 0x03, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x38, 0x30, 0x30, 0x2E, 0x30, 0x38, 0x30, 0x30, 0x2E, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDA, 0x09
};

int ee2prom_read_val(int dev, int addr) {
    int fd;
    char device[100];
    eeprom_data_t eprom = {0};
    printf("Reading EEPROM #%d at address #0x%x\n", dev, addr);
    eprom.eepromDevice = dev;
    eprom.eepromAddress = addr;
    sprintf(device, "/dev/ee2prom%d", dev);
    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("Unable to open Device : %s\n", device);
        return -1;
    }

    if (ioctl(fd, EE2PROM_READ, &eprom) < 0) {
        printf("IOCTL failed on Device : %s\n", device);
        close(fd);
        return -1;
    } else {
        printf("0x%x\n", eprom.eepromData);
    }
    close(fd);
    return eprom.eepromData;
}

int ee2prom_read(int dev, int addr) {
    if (ee2prom_read_val(dev, addr) == -1) {
        return -1;
    }
    return 0;
}

int ee2prom_bulkread_val(int dev, int addr, unsigned char *data, int size) {
    int i, fd;
    char device[100];
    unsigned char dataarray[EEPROM_MAX_SIZE] = {0};
    eeprom_data_t eprom = {0};
    printf("Reading EEPROM #%d at address #0x%x size #%d\n", dev, addr, size);
    eprom.eepromDevice = dev;
    eprom.eepromAddress = addr;
    eprom.eepromBulkDataSize = size;
    eprom.eepromBulkData = dataarray;

    sprintf(device, "/dev/ee2prom%d", dev);
    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("Unable to open Device : %s\n", device);
        return -1;
    }

    if (ioctl(fd, EE2PROM_READ_BULK, &eprom) < 0) {
        printf("IOCTL failed on Device : %s\n", device);
        close(fd);
        return -1;
    } else {
        for(i = 0 ; i < size ; i++ ) {
            data[i] = dataarray[i];
        }
    }
    close(fd);
    return 0;
}

int ee2prom_bulkread(int dev, int addr, int size) {
    int i, rv = 0, count = 0;
    unsigned char *dataarray = (unsigned char *)malloc(size * sizeof(unsigned char));
    if (ee2prom_bulkread_val(dev, addr, dataarray, size) == -1) {
        rv = -1;
    } else {
        for(i = 0 ; i < size ; i++) {
            if (count%16 == 0) {
                printf("\n0x%03x : ", addr+i);
            }
            printf("%02x ", dataarray[i]);
            count++;
        }
        printf("\n");
        rv = 0;
    }
    free(dataarray);
    return rv;
}

int ee2prom_write(int dev, int addr, int data) {
    int fd;
    char device[100];
    eeprom_data_t eprom = {0};
    printf("Writing EEPROM #%d at address #0x%x data #0x%x\n", dev, addr, data);

    eprom.eepromDevice = dev;
    eprom.eepromAddress = addr;
    eprom.eepromData = data;

    sprintf(device, "/dev/ee2prom%d", dev);
    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("Unable to open Device : %s\n", device);
        return -1;
    }

    if (ioctl(fd, EE2PROM_WRITE, &eprom) < 0) {
        printf("IOCTL failed on Device : %s\n", device);
        close(fd);
        return -1;
    } else {
        printf("Successfully Written\n");
    }
    close(fd);
    return 0;
}

int ee2prom_bulkwrite(int dev, int addr, unsigned char *data, int size) {
    int fd;
    char device[100];
    eeprom_data_t eprom = {0};
    printf("Writing EEPROM #%d at address #0x%x bulksize #%d\n", dev, addr, size);

    eprom.eepromDevice = dev;
    eprom.eepromAddress = addr;
    eprom.eepromBulkDataSize = size;
    eprom.eepromBulkData = data;

    sprintf(device, "/dev/ee2prom%d", dev);
    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("Unable to open Device : %s\n", device);
        return -1;
    }

    if (ioctl(fd, EE2PROM_WRITE_BULK, &eprom) < 0) {
        printf("IOCTL failed on Device : %s\n", device);
        close(fd);
        return -1;
    } else {
        printf("Successfully Written\n");
    }
    close(fd);
    return 0;
}

int ee2prom_init_default(int dev) {
    printf("\033[1;31m");
    printf("\n*******************************************************");
    printf("\n*         WELCOME TO EEPROM INITIALIZATION            *");
    printf("\n*This will initialize the EEPROM#%d to default content *", dev);
    printf("\n*******************************************************");
    printf("\033[0m\n");
    extern int eeprom_ask_user();
    if (eeprom_ask_user() != 0) {
        printf("Thank You!!!\n");
        return 0;
    }
    printf("Initializating EEPROM#%d to Factory Default\n", dev);
    return ee2prom_bulkwrite(dev, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE);
}

/* TEST Utilities */

int ee2prom_TEST_F_read_byte() {
    int rv = 0;
    if(ee2prom_read(0, 0x00) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_read(1, 0x00) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_write_byte() {
    int rv = 0;
    if(ee2prom_write(0, 0x00, 0x55) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    sleep(2);
    if(ee2prom_write(1, 0x00, 0x57) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_byte_val() {
    int rv = 0;

    ee2prom_write(0, 0x00, 0x75);
    sleep(2);
    ee2prom_write(1, 0x00, 0x77);

    if(ee2prom_read_val(0, 0x00) != 0x75) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_read_val(1, 0x00) != 0x77) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_bulk() {
    int rv = 0;
    if(ee2prom_bulkread(0, 0x00, 256) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_bulkread(1, 0x00, 256) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_write_bulk() {
    int rv = 0;
    if(ee2prom_bulkread(0, 0x00, 256) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_bulkread(1, 0x00, 256) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }

    sleep(1);
    if(ee2prom_bulkwrite(0, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }

    sleep(2);
    if(ee2prom_bulkwrite(1, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE) != 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }

    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_write_read_bulk_check() {
    int i, rv = 0;
    unsigned char data[EEPROM_MAX_SIZE] = {0};

    sleep(1);
    if(ee2prom_bulkwrite(0, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE) != 0) {
         printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
         rv  = -1;
    }

    sleep(2);
    if(ee2prom_bulkwrite(1, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE) != 0) {
         printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
         rv  = -1;
    }

    sleep(1);
    if(ee2prom_bulkread_val(0, 0x00, data, 256) != 0) {
         printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
         rv  = -1;
    }

    for( i = 0 ; i < 256 ; i++ ) {
        if (data[i] != pi_eeporom_template[i]) {
            printf("%20s:%5d at i=%d  %x  vs %x : Failed\n",__FUNCTION__, __LINE__,i , data[i],  pi_eeporom_template[i]);
            rv  = -1;
        }
    }

    sleep(2);
    if(ee2prom_bulkread_val(1, 0x00, data, 256) != 0) {
         printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
         rv  = -1;
    }

    for( i = 0 ; i < 256 ; i++ ) {
        if (data[i] != pi_eeporom_template[i]) {
            printf("%20s:%5d at i=%d  %x  vs %x : Failed\n",__FUNCTION__, __LINE__,i , data[i],  pi_eeporom_template[i]);
            rv  = -1;
        }
    }

    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_invalid_device() {
    int rv = 0;
    if(ee2prom_read(-1, 0x00) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_read(3, 0x00) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_invalid_address() {
    int rv = 0;
    if(ee2prom_read(0, 0x2FF) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_read(1, 0x2FF) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_invalid_range() {
    int rv = 0;
    if(ee2prom_bulkread(0, 0x20, 511) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if(ee2prom_bulkread(1, 0x20, 511) == 0) {
        printf("%20s:%5d : Failed\n",__FUNCTION__, __LINE__);
        rv  = -1;
    }
    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

int ee2prom_TEST_F_read_invalid_pointer() {
    int rv, fd;
    char device[100];
    eeprom_data_t eprom = {0};
    eprom.eepromDevice = 0;
    eprom.eepromAddress = 0x00;
    eprom.eepromBulkDataSize = 256;
    eprom.eepromBulkData = NULL;

    sprintf(device, "/dev/ee2prom0");
    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("Unable to open Device : %s\n", device);
        return -1;
    }

    if (ioctl(fd, EE2PROM_READ_BULK, &eprom) < 0) {
        printf("IOCTL failed on Device : %s\n", device);
        rv  = 0;
    } else {
        rv = -1;
    }
    close(fd);

    if (rv != 0) {
        printf("%20s: ----- \033[1;31mTEST FAIL\033[0m -------\n", __FUNCTION__);
    } else {
        printf("%20s: ----- \033[0;32mTEST PASS\033[0m -------\n", __FUNCTION__);
    }
    return rv;
}

TEST_T tests[] = {
                    {ee2prom_TEST_F_read_byte,"ee2prom_TEST_F_read_byte", 0 },
                    {ee2prom_TEST_F_write_byte, "ee2prom_TEST_F_write_byte", 0 },
                    {ee2prom_TEST_F_read_byte_val, "ee2prom_TEST_F_read_byte_val", 0 },
                    {ee2prom_TEST_F_read_bulk, "ee2prom_TEST_F_read_bulk", 0 },
                    {ee2prom_TEST_F_write_bulk, "ee2prom_TEST_F_write_bulk", 0 },
                    {ee2prom_TEST_F_write_read_bulk_check, "ee2prom_TEST_F_write_read_bulk_check", 0 },
                    {ee2prom_TEST_F_read_invalid_device , "ee2prom_TEST_F_read_invalid_device", 0 },
                    {ee2prom_TEST_F_read_invalid_address, "ee2prom_TEST_F_read_invalid_address", 0 },
                    {ee2prom_TEST_F_read_invalid_range, "ee2prom_TEST_F_read_invalid_range", 0},
                    {ee2prom_TEST_F_read_invalid_pointer, "ee2prom_TEST_F_read_invalid_pointer", 0},
                };

int ee2prom_run_tests() {
    int i, pcount = 0, fcount = 0;
    int num_testcases = sizeof(tests)/sizeof(tests[0]);
    printf("\033[1;31m");
    printf("\n***************************************************");
    printf("\n*         WELCOME TO EEPROM DIAG TEST             *");
    printf("\n*This test will alter the current content of both *");
    printf("\n*EEPROM and will initialize the both EEPROMs to   *");
    printf("\n*default content after the test execution         *");
    printf("\n***************************************************");
    printf("\033[0m\n");
    extern int eeprom_ask_user();
    if (eeprom_ask_user() != 0) {
        printf("Thank You!!!\n");
        return 0;
    }
    printf("\n\033[0;35mEEPROM DIAGNOSTIC\033[0m\n");
    printf("------------------------------------\n");

    for( i = 0 ; i < num_testcases ; i++ ) {
        printf("\033[1;33mRunning Test\033[0m : %s\n", tests[i].test_name);
        if (tests[i].test_function() != 0) {
            tests[i].status = -1;
        } else {
            tests[i].status = 0;
        }
    }

    ee2prom_bulkwrite(0, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE);
    ee2prom_bulkwrite(1, 0x00, pi_eeporom_template, EEPROM_MAX_SIZE);

    printf("\n\033[0;35mTEST RESULTS\033[0m\n");
    printf("%-5s %s\n", "Status", "Test case");
    printf("----- --------------------------------------------\n");
    for( i = 0 ; i < num_testcases ; i++ ) {
        if (tests[i].status == 0) {
            printf("%-5s %s\n", "\033[0;32mPASS\033[0m", tests[i].test_name);
            pcount++;
        } else {
            printf("%-5s %s\n", "\033[1;31mFAIL\033[0m", tests[i].test_name);
            fcount++;
        }
    }

    printf("\n\033[0;35mTEST SUMMERY\033[0m\n");
    printf("Total Test Cases: %d\n", num_testcases);
    printf("Passed          : %d\n", pcount);
    printf("Failed          : %d\n", fcount);
    printf("\033[0;35m-------------------\033[0m\n");

    if (fcount) {
        printf("\033[1;31mTest Fail \033[0m \n");
        return -1;
    } else {
        printf("\033[0;32mTest Pass\033[0m\n");
    }

    return 0;
}

int eeprom_ask_user() {
    int count = 0;
    char answer;
    printf("Would you like to proceed? Enter Y or N: ");
    while (scanf(" %c", &answer) == 1 &&
           printf("Answer is %c\n", answer) > 0 &&
           (answer == 'Y' || answer == 'y')) {
        count++;
        if(count > 1)
            break;
        printf("Are Sure! You want to proceed? Y or N: ");
    }

    if (answer == 'Y' || answer == 'y') {
        return 0;
    }
    return -1;
}

int main (int argc, char *argv[]) {
    int opt;
    int dev = -1, addr = -1, data = -1, size = -1;
    int readopt = 0, writeopt = 0, initopt = 0, testopt = 0;
    while ((opt = getopt(argc, argv, "rwit-:d:a:v:s:")) != -1) {
        switch (opt) {
            case 'r':
                readopt = 1;
                //printf("Option r has arg: %s\n", optarg);
            break;
            case 'w':
                writeopt = 1;
                //printf("Option w has arg: %s\n", optarg);
            break;
            case 'd':
                dev = strtol(optarg, NULL, 10);
                //printf("Option dd has arg: %d\n", dev);
            break;
            case 'a':
                addr = strtol(optarg, NULL, 16);
                //printf("Option a has arg: %x\n", addr);
            break;
            case 's':
                size = strtol(optarg, NULL, 10);
                //printf("Option s has arg: %d\n", size);
            break;
            case 'v':
                data = strtol(optarg, NULL, 16);
                //printf("Option v has arg: %d\n", data);
            break;
            case 'i':
                initopt = 1;
            break;
            case 't':
                testopt = 1;
            break;
            case ':':
                //printf("Missing arg for %c\n", optopt);
            break;
        }
    }

    if (readopt && writeopt) {
        printf("Option read(-r) and write(-w) are mutually exclusive\n");
        exit(1);
    }
    if (testopt) {
        return ee2prom_run_tests();
    } else if (initopt) {
        if (dev == -1) {
            printf("A valid device must be provided\n");
            exit(1);
        }
        return ee2prom_init_default(dev);
    } else if (readopt) {
        if (dev == -1) {
            printf("A valid device must be provided\n");
            exit(1);
        }
        if (size > 256) {
            printf("Read size must be between 0-255\n");
            exit(1);
        }
        if (addr < 0 || addr > 0x1FF) {
            printf("A valid device address must be provided\n");
            exit(1);
        }
        if (size > 0) {
            return ee2prom_bulkread(dev, addr, size);
        } else {
            return ee2prom_read(dev, addr);
        }
    } else if (writeopt) {
        if (dev == -1) {
            printf("A valid device must be provided\n");
            exit(1);
        }
        if (addr < 0 || addr > 0x1FF) {
            printf("A valid device address must be provided\n");
            exit(1);
        }
        if (data == -1) {
            printf("A valid value must be provided\n");
            exit(1);
        }
        return ee2prom_write(dev, addr, data);
    } else {
        help();
        exit(1);
    }
    return 0;
}
