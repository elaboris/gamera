# Gamera Vector Toolkit
#
# Copyright (C) 2005 Michael Droettboom
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
from vector_args import *
import _deform

class affine_transform_image(PluginFunction):
    """Returns a new image which is the original image transformed
through the given Transformation matrix.

If *transform_bb* == ``True``, the bounding box of the new image will
be adjusted to reflect the transformation.  However, if the
transformation transforms pixels into negative coordinate space, the
image will be truncated.

When *transform_bb* == ``False``, the upper left corner of the resulting
image will always be (0, 0).

affine_transform_image is more general than rotate, scale etc. in the
Gamera core, however, in most cases is approximately twice as slow
when compared against single transformations.  However, for compound
transformations, affine_transformation_image can be faster.
"""
    self_type = ImageType([ONEBIT, GREYSCALE, GREY16, FLOAT, RGB])
    args = Args([Transformation("t"), Check("transform_bb", default=False)])
    return_type = ImageType(ALL)
    def __call__(self, t, transform_bb=False):
       return _deform.affine_transform_image(self, t, transform_bb)
    __call__ = staticmethod(__call__)

    def __doc_example1__(images):
       from gamera.toolkits.vector.plugins import transformation
       rgb = images[RGB]
       t = transformation.rotate_degrees(3) * transformation.scale(-2, 2)
       result = rgb.affine_transform_image(t, False)
       return [rgb, result]
    doc_examples = [__doc_example1__]

class DeformModule(PluginModule):
    cpp_headers=["deform.hpp"]
    cpp_namespace=["Gamera"]
    category = "Vector/Deform"
    functions = [affine_transform_image]
    author = "Michael Droettboom"
module = DeformModule()
