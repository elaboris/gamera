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
import _render

class draw_path_filled(PluginFunction):
    """Renders a Path object to an Image.

*path*

  The Path object to draw

*value*

  The pixel value to use to fill

*accuracy*

  The error threshold for the accuracy of the bezier curve drawing
"""
    self_type = ImageType(ALL)
    args = Args([Path("path"), Pixel("value"), Float("accuracy")])
    def __call__(image, path, value, accuracy=1.0):
        return _render.draw_path_filled(image, path, value, accuracy)
    __call__ = staticmethod(__call__)

class draw_path_hollow(PluginFunction):
    """Renders a Path object to an Image.

*path*

  The Path object to draw

*value*

  The pixel value to use to fill

*accuracy*

  The error threshold for the accuracy of the bezier curve drawing
"""
    self_type = ImageType(ALL)
    args = Args([Path("path"), Pixel("value"), Float("accuracy")])
    def __call__(image, path, value, accuracy=1.0):
        return _render.draw_path_hollow(image, path, value, accuracy)
    __call__ = staticmethod(__call__)

class RenderModule(PluginModule):
    cpp_headers=["render.hpp"]
    cpp_namespace=["Gamera"]
    category = "Vector/Draw"
    functions = [draw_path_filled, draw_path_hollow]
    author = "Michael Droettboom"
module = RenderModule()
