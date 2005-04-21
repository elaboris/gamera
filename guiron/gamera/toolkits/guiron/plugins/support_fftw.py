
# Copyright (C) 2005 Robert Ferguson
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from gamera.plugin import *
import sys
import glob
import _support_fftw
from gamera.plugins import _arithmetic       # Image multiplication
from gamera.plugins import _deformation      # Rotation
from gamera.plugins import _image_utilities  # Image padding

class dft(PluginFunction):
    """Calculates a discrete fourier transform on complex images.  The operation will be faster if executed on images with sizes in powers of two.

*image*

.. code:: Python

        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
"""
    self_type = ImageType([COMPLEX])
    return_type = ImageType([COMPLEX])

    def __doc_example1__(images):
        from gamera.core import load_image
        from gamera.plugins import _image_conversion
        import os
        path = os.path.join("src","images","a_square.png")
        image = load_image(path)
        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        return [image, dfted]
    doc_examples = [__doc_example1__]


class idft(PluginFunction):
    """Calculates the inverse discrete fourier transform of a complex image.

*image*

.. code:: Python

        cimage = _image_conversion.to_complex(image)
        cimage2 = cimage.pad_to_power_of_2()
        dfted = cimage.dft()
        ifted = dfted.idft()
"""
    self_type = ImageType([COMPLEX])
    return_type = ImageType([COMPLEX])

    def __doc_example1__(images):
        from gamera.core import load_image
        from gamera.plugins import _image_conversion
        import os
        path = os.path.join("src","images","a_square.png")
        image = load_image(path)
        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        idfted = dfted.idft()
        return [image, dfted, idfted]
    doc_examples = [__doc_example1__]

class move_dc_to_center(PluginFunction):
    """Moves the DC component of a fourier image to the center.  This operation is useful for visualization, but  will only work for images with even dimensions.

.. code:: Python

        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        moved = dfted.move_dc_to_center()
"""
    self_type = ImageType([COMPLEX])
    return_type = ImageType([COMPLEX])

    def __doc_example1__(images):
        from gamera.core import load_image
        from gamera.plugins import _image_conversion
        import os
        path = os.path.join("src","images","a_square.png")
        image = load_image(path)
        cimage = _image_conversion.to_complex(image)
        cimage2 = cimage.pad_to_power_of_2()
        dfted = cimage.dft()
        moved = dfted.move_dc_to_center()
        return [image, dfted, moved]
    doc_examples = [__doc_example1__]
    
class log_scale_pixels(PluginFunction):
    """Log scales the results of a fourier transform for display purposes.


.. code:: Python

        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        logged = dfted.log_scale_pixels()
"""
    self_type = ImageType([COMPLEX])
    return_type = ImageType([COMPLEX])

    def __doc_example1__(images):
        from gamera.core import load_image
        from gamera.plugins import _image_conversion
        import os
        path = os.path.join("src","images","a_square.png")
        image = load_image(path)
        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        logged = dfted.log_scale_pixels()
        return [image, dfted, logged]
    doc_examples = [__doc_example1__]
    
class pad_to_power_of_2(PluginFunction):
    """Returns an image padded to the nearest power of two."""
    self_type = ImageType(ALL)
    return_type = ImageType(ALL)
    pure_python = True
    def __call__(self):
        size = max(self.nrows, self.ncols)
        pow2 = 1
        while pow2 < size:
            pow2 <<= 1
        
        if size == pow2 and self.nrows == self.ncols:
            return _image_utilities.image_copy(self, 0)
        else:
            return _image_utilities.pad_image(
                self, 0, pow2 - self.ncols, pow2 - self.nrows, 0, 0)
    __call__ = staticmethod(__call__)

    def __doc_example1__(images):
        image = images[GREYSCALE]
        padded = image.pad_to_power_of_2()
        return [image, padded]
    doc_examples = [__doc_example1__]

class log_polar(PluginFunction):
    """Converts DFT results to log-polar coordinates.  Useful for phase correlation.

*image*

.. code:: Python

        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        poled = dfted.log_polar()
        logged = poled.log_scale_pixels()
        
"""
    self_type = ImageType([COMPLEX])
    return_type = ImageType([COMPLEX])

    def __doc_example1__(images):
        from gamera.core import load_image
        from gamera.plugins import _image_conversion
        import os
        path = os.path.join("src","images","a_square.png")
        image = load_image(path)
        cimage = _image_conversion.to_complex(image)
        dfted = cimage.dft()
        poled = dfted.log_polar()
        logged = poled.log_scale_pixels()
        return [image, dfted, poled, logged]
    doc_examples = [__doc_example1__]
    
class SupportFFTWModule(PluginModule):
    
    extra_libraries = ["fftw3"]
    category = "DFT"
    cpp_headers = ["support_fftw.hpp"]
    cpp_namespaces = ["Gamera"]
    functions = [dft, idft, move_dc_to_center, log_scale_pixels, pad_to_power_of_2, log_polar]
    author = "Robert Ferguson"
    url = "http://gamera.dkc.jhu.edu/"

module = SupportFFTWModule()
