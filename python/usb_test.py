import usb.core
import usb.util
import time
import array
import numpy as np
import cv2


def usb_ctrl(dev, bReq, wIndex, wVal):
    ret = dev.ctrl_transfer(0xC0, bReq, wVal, wIndex, 4)
    return ret


def sensor_write(dev, reg, value):
    dev.ctrl_transfer(0xC0, 1, value, reg, 4)


def sensor_read(dev, reg):
    ret = dev.ctrl_transfer(0xC0, 0, 0, reg, 4)
    return int.from_bytes(ret[2:4], 'little')


while True:
    try:
        # find our device
        dev = usb.core.find(idVendor=0x070a, idProduct=0x4020)

        # was it found?
        if dev is None:
            raise ValueError('Device not found')

        # set the active configuration. With no arguments, the first
        # configuration will be the active one
        dev.set_configuration()

        print(dev)

        # get an endpoint instance
        cfg = dev.get_active_configuration()
        intf = cfg[(0, 0)]

        ep_out = usb.util.find_descriptor(
            intf,
            # match the first OUT endpoint
            custom_match=lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_OUT)

        assert ep_out is not None

        ep_in = usb.util.find_descriptor(
            intf,
            # match the first IN endpoint
            custom_match=lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_IN)

        assert ep_in is not None

        timeout = 1000000

        ret = usb_ctrl(dev, 2, 0, 0)  # start capturing

        senddata=bytearray([0x12, 0x34, 0x56, 0x78,0xaa])
        while True:
            ep_out.write(senddata, timeout)
            data = ep_in.read(len(senddata), timeout)
            print('length:',len(data))
    except:
        time.sleep(1)
        print('restart')
