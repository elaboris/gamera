
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
from gamera.plugins import _deformation
from gamera.plugins import _image_utilities
from gamera.plugins import _arithmetic

class template_correlation(PluginFunction):
    """This technique compares a template of the desired image (the correlation kernel) with the actual source image.  This generates a new image (the correlation image) that indicates where the template matches.

*image*
   The source image

*image*
   The template you wish to correlate
"""
    self_type = ImageType([COMPLEX])
    args = Args([ImageType([COMPLEX], "template_image")])
    return_type = ImageType([COMPLEX]);
    pure_python = True
    def __call__(self, convolution_image):
    	if ((self.height <= convolution_image.height) or (self.width <= convolution_image.width)):
	   print "The template may not be larger than the source image!"
    	else:
            # Comment here...
	   padded = _image_utilities.pad_image(convolution_image, 0, self.width-convolution_image.width, self.height-convolution_image.height, 0, 0)
	   convoluded = _arithmetic.multiply_images(_support_fftw.fft(_deformation.rotate(self, 90.0, 0.0)), _support_fftw.fft(padded), 0)
	   return convoluded
    __call__ = staticmethod(__call__)

class cross_spectrum(PluginFunction):
    """Cross correlation is a method of estimating the correlation of two complex functions (basically by taking the complex conjugate). The DFT of the cross-correlation is called the cross spectrum and it is the more pertinent function for use in image processing.

*image*
   The source image

*image*
   The template to correlate
"""
    self_type = ImageType([COMPLEX])
    args = Args([ImageType([COMPLEX], "template_image")])
    return_type = ImageType([COMPLEX]);
    pure_python = True
    def __call__(self, convolution_image):
    	if ((self.height <= convolution_image.height) or (self.width <= convolution_image.width)):
	   print "The template may not be larger than the source image!"
    	else:
	   padded = _image_utilities.pad_image(convolution_image, 0, self.width-convolution_image.width, self.height-convolution_image.height, 0, 0)
	   cross_spectrum = _arithmetic.multiply_images(_support_fftw.complex_fft(conjugate(self)), _support_fftw.complex_fft(padded), 0)
	   return cross_spectrum
    __call__ = staticmethod(__call__)        


class MatchedFilteringModule(PluginModule):
    
    category = "Matched_Filtering"
    cpp_namespaces = ["Gamera"]
    functions = [template_correlation, cross_spectrum]
    author = "Robert Ferguson"
    url = "http://gamera.dkc.jhu.edu/"

module = MatchedFilteringModule()
