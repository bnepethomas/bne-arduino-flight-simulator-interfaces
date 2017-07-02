#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2017 Richard Hull and contributors
# See LICENSE.rst for details.

import re
import time
import argparse

from random import randint

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT


def demo(n, block_orientation):
    print "Attribute n:", n
    print"Block Orientation:", block_orientation
    # create matrix device
    serial = spi(port=0, device=0, gpio=noop())
    device = max7219(serial, cascaded=n or 1, block_orientation=block_orientation)
    print("Created device")

    print("Drawing on Canvas 0")
    time.sleep(1)
    for x in range(8):
        with canvas(device) as draw:
            draw.point((x, x ), 1)
            time.sleep(0.1)
            
    print("Drawing on Canvas stage 1")
    time.sleep(1)
    with canvas(device) as draw:
        for x in range(8):
            draw.point((x, x ), 1)
            #draw.setposition()
            time.sleep(0.1)
            
    print("Drawing on Canvas stage 2")
    time.sleep(1)
    #with canvas(device) as draw:
    for abc in range(100):
        with canvas(device) as draw:
            for y in range(8):
                for x in range(8):
                    #print("Point " + str(x) + " " + str(y))
                    draw.point((x, y ), randint(0,1))
                
        time.sleep(0.1)
                
    print("Finished Drawing on Canvas stage 2")        
    time.sleep(5)        

    

    time.sleep(1)
    with canvas(device) as draw:
        text(draw, (0, 0), "A", fill="white")

    time.sleep(1)
    for _ in range(2):
        for intensity in range(16):
            device.contrast(intensity * 16)
            time.sleep(0.1)

    device.contrast(0x80)
    time.sleep(1)




if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='matrix_demo arguments',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--cascaded', '-n', type=int, default=1, help='Number of cascaded MAX7219 LED matrices')
    parser.add_argument('--block-orientation', type=int, default=0, choices=[0, 90, -90], help='Corrects block orientation when wired vertically')

    args = parser.parse_args()

    try:
        demo(args.cascaded, args.block_orientation)
    except KeyboardInterrupt:
        pass
