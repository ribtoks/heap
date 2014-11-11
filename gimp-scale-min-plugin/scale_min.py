#!/usr/bin/env python

import math
import os
from gimpfu import *

def process_image(timg, minsize, savecopy):
    currentWidth = timg.width
    currentHeight = timg.height

    newWidth = currentWidth
    newHeight = currentHeight

    r = math.sqrt(currentWidth*currentHeight / float(minsize))

    newWidth = float(math.ceil(currentWidth / r))
    newHeight = float(math.ceil(currentHeight / r))

    pdb.gimp_image_scale(timg, newWidth, newHeight)

    if savecopy:
        directory, filename = os.path.split(timg.filename)
        basefilename, _ = os.path.splitext(filename)
        new_filename = os.path.join(directory, basefilename + "_s.jpg")

        tdrawable = pdb.gimp_image_get_active_layer(timg)
        pdb.file_jpeg_save(timg, tdrawable, new_filename, new_filename,
                           1.0, 0, 0, 0, "", 0, 0, 0, 0)

def plugin_main(minsize=6000000, savecopy=TRUE, processdir=TRUE, dirname=""):
    if processdir:
        for filename in os.listdir(dirname):
            try:
                if filename.lower().split(".")[-1] in ("png", "jpg"):
                    #import pdb as debug; debug.set_trace()
                    fname = os.path.join(dirname, filename)
                    img = pdb.gimp_file_load(fname, fname)
                    process_image(img, minsize, savecopy)
                    pdb.gimp_image_delete(img)
            except Exception, error:
                print error
    else:
        process_image(img, minsize, savecopy)

register(
    "python_fu_resize_max",
    "Scales the image to fit minimal size in megapixels",
    "Scales the image to fit minimal size in megapixels",
    "Taras Kushnir",
    "Taras Kushnir",
    "2014",
    "<Toolbox>/Tools/Resize images to min size...",
    "",
    [
        (PF_INT, "min_size", "Minimum image size", 6000000),
        (PF_BOOL, "copy", "Make a JPEG copy", TRUE),
        (PF_BOOL, "process_dir", "Process a directory", TRUE),
        (PF_DIRNAME, "path", "Directory to Open", "./"),
    ],
    [],
    plugin_main)

main()
