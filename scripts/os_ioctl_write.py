#!/usr/bin/env python
import sys
import os
import binascii
from array import array
from ctypes import *
import struct
from fcntl import ioctl

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

ION = 0xB1
OPT_WRITE = _IO(ION, 0)
OPT_WRITE_BLK = _IO(ION, 1)
OPT_READ = _IO(ION, 2)
OPT_READ_BLK = _IO(ION, 3)

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

containt = EEPROMCONTENT(0, 0, 0, ptr)
containt.eepromDevice = 0
containt.eepromAddress = 0
containt.eepromBulkData = ptr
containt.eepromBulkDataSize = datasize


pi_bytes_store_path = "/var/shared/pi_bytes"
filename = "/dev/ee2prom0"

def write_eeprom():
    fd = os.open(filename, os.O_WRONLY)
    ioctl(fd, OPT_WRITE_BLK, containt)
    os.close(fd)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Error: Invalid args")
        exit(1)
    inpath = sys.argv[1]
    inpath = os.path.abspath(inpath)

    opath = os.path.abspath(pi_bytes_store_path)
    indata = array('B')
    with open(inpath, 'rb') as inf:
        indata.fromfile(inf, 512)

    for i in range(0, 511):
        dataArray[i] = indata[i]

    write_eeprom()
    print("EEPROM Written");

    with open(opath, 'wb') as outf:
        outf.write(indata)
    print("\nA Copy of written values stored at " + pi_bytes_store_path)
