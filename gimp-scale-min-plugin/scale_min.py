#!/usr/bin/env python

import math
from gimpfu import *

def plugin_main(timg, tdrawable, minsize=6000000, savecopy=TRUE):
    currentWidth = tdrawable.width
    currentHeight = tdrawable.height
    
    newWidth = currentWidth
    newHeight = currentHeight
            
    r = math.sqrt(currentWidth*currentHeight / float(minsize))

    newWidth = float(math.ceil(currentWidth / r))
    newHeight = float(math.ceil(currentHeight / r))
    
    pdb.gimp_image_scale(timg, newWidth, newHeight)

    if savecopy:
        pdb.file_jpeg_save(timg, tdrawable, timg.name+"_s.jpg", timg.name+"_s.jpg", 
                           1.0, 0, 0, 0, "", 0, 0, 0, 0)

register(
    "python_fu_resize_max",
    "Scales the image to fit minimal size in megapixels",
    "Scales the image to fit minimal size in megapixels",
    "Taras Kushnir",
    "Taras Kushnir",
    "2014",
    "<Image>/Image/Resize to min size...",
    "RGB*, GRAY*",
    [
        (PF_INT, "min_size", "Minimum image size", 6000000),
        (PF_BOOL, "copy", "Make a JPEG copy", TRUE),
    ],
    [],
    plugin_main)

main()
