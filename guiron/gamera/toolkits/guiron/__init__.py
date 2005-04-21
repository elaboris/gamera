"""
Toolkit setup

This file is run on importing anything within this directory.
Its purpose is only to help with the Gamera GUI shell,
and may be omitted if you are not concerned with that.
"""

from gamera import toolkit
from gamera.toolkits.guiron import main

from gamera.toolkits.guiron.plugins import support_fftw
from gamera.toolkits.guiron.plugins import matched_filtering
