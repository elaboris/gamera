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
import _tracing

class potrace(PluginFunction):
    """Traces the image using the potrace_ algorithm by Peter Selinger.

Returns a PathList object as output.  Each Path in the PathList represents
a different connected component in the image.

The arguments are all passed along to the potrace_ algorithm.  See
the potrace_ documentation for more information.

*turd_size*

  suppress speckles of up to this size (default 2)

*turn_policy*

  Defines how to resolve ambiguities in path decomposition

  - black (0)

  - white (1)

  - left (2)

  - right (3)

  - minority (4) [default]

  - majority (5)

  - random (6)

*optimize_curve*

  When True, the number of curves used to outline the image
  will be minimized.

*optimize_tolerance*

  The amount of curve optimization error tolerance.

*alphamax*

  Corner threshold parameter.

.. _potrace: http://potrace.sf.net/
"""
    self_type = ImageType([ONEBIT])
    args = Args([Int("turd_size", default=2),
                 Choice("turn_policy",
                        "black white left right minority majority random".split(),
                        default=4),
                 Check("optimize_curve", default=True),
                 Float("optimize_tolerance", default=0.2),
                 Float("alphamax", default=1.0)])
    return_type = PathList("paths")

    def __call__(self, turd_size=2, turn_policy=4, optimize_curve=True,
                 optimize_tolerance=0.2, alphamax=1.0):
        return _tracing.potrace(self, turd_size, turn_policy, optimize_curve,
                         optimize_tolerance, alphamax)
    __call__ = staticmethod(__call__)

    def __doc_example1__(images):
        from gamera.toolkits.vector.plugins import transformation, render
        from gamera.core import Image, RGBPixel
        onebit = images[ONEBIT]
        paths = onebit.potrace()
        t = transformation.scale(3, 3)
        new_image = Image(0, 0, onebit.nrows*3, onebit.ncols*3, RGB)
        new_paths = paths.transform(t)
        for path in new_paths:
            new_image.draw_path_hollow(
                path, RGBPixel(255, 128, 128))
        return [onebit, new_image]
    doc_examples = [__doc_example1__]

class TraceModule(PluginModule):
    cpp_headers=["tracing.hpp"]
    cpp_namespace=["Gamera"]
    category = "Vector/Tracing"
    functions = [potrace]
    author = "Peter Selinger (wrapped for Gamera by Michael Droettboom)"
    PO_TRACE_DIR = os.path.join("src", "potrace-1.4", "src")
    cpp_include_dirs = [PO_TRACE_DIR]
    cpp_sources = [os.path.join(PO_TRACE_DIR, x) for x in
                   ['bitmap.c', 'curve.c', 'path.c']]
module = TraceModule()
