#!/usr/bin/env python
import os
from ctypes import *
import struct
from fcntl import ioctl
import struct

# constant for linux portability
_IOC_NRBITS = 8
_IOC_TYPEBITS = 8

# architecture specific
_IOC_SIZEBITS = 14
_IOC_DIRBITS = 2

_IOC_NRMASK = (1 << _IOC_NRBITS) - 1
_IOC_TYPEMASK = (1 << _IOC_TYPEBITS) - 1
_IOC_SIZEMASK = (1 << _IOC_SIZEBITS) - 1
_IOC_DIRMASK = (1 << _IOC_DIRBITS) - 1

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = _IOC_NRSHIFT + _IOC_NRBITS
_IOC_SIZESHIFT = _IOC_TYPESHIFT + _IOC_TYPEBITS
_IOC_DIRSHIFT = _IOC_SIZESHIFT + _IOC_SIZEBITS

_IOC_NONE = 0
_IOC_WRITE = 1
_IOC_READ = 2

def _IOC(dir, type, nr, size):
    if isinstance(size, str) or isinstance(size, unicode):
        size = struct.calcsize(size)
    return dir  << _IOC_DIRSHIFT  | \
           type << _IOC_TYPESHIFT | \
           nr   << _IOC_NRSHIFT   | \
           size << _IOC_SIZESHIFT

def _IO(type, nr): return _IOC(_IOC_NONE, type, nr, 0)
def _IOR(type, nr, size): return _IOC(_IOC_READ, type, nr, size)
def _IOW(type, nr, size): return _IOC(_IOC_WRITE, type, nr, size)
def _IOWR(type, nr, size): return _IOC(_IOC_READ | _IOC_WRITE, type, nr, size)


datasize = 512
dataArray = (c_ubyte * datasize)()
ptr = POINTER(c_ubyte)(dataArray)
ptr.contents

class EEPROMCONTENT(Structure):
    _fields_ = [("eepromDevice", c_uint),
            ("eepromAddress", c_uint),
            ("eepromData", c_uint),
            ("eepromBulkData", POINTER(c_ubyte)),
            ("eepromBulkDataSize", c_uint),
            ("eepromBlock0Protect", c_uint),
            ("eepromBlock1Protect", c_uint),
            ("eepromStatusReg", c_uint)]

containt = EEPROMCONTENT(10, 20, 30, ptr)

ION = 0xB1
OPT_WRITE = _IO(ION, 0)
OPT_WRITE_BLK = _IO(ION, 1)
OPT_READ = _IO(ION, 2)
OPT_READ_BLK = _IO(ION, 3)

containt.eepromDevice = 0
containt.eepromAddress = 0
containt.eepromBulkDataSize = datasize;

filename = "/dev/ee2prom0"

fd = os.open(filename, os.O_RDONLY)
ioctl(fd, OPT_READ_BLK, containt)
os.close(fd)

pistr = ""
for x in dataArray :
    print("%02x"%(x)),
    pistr = pistr + str(hex(x)[2:]).zfill(1)

print ("\nHex String Dump\n")
print pistr
