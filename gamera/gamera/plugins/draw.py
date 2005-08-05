#
#
# Copyright (C) 2001-2005 Ichiro Fujinaga, Michael Droettboom, and Karl MacMillan
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
from gamera.util import warn_deprecated
import _draw
try:
  from gamera.core import RGBPixel
except:
  def RGBPixel(*args):
    pass

class draw_marker(PluginFunction):
  """Draws a marker at a given point.

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a marker at (5, 5) will be in the upper left hand corner
of the image.

The coordinates can be specified either by two floats or one FloatPoint:

  *a*:
    The position of the marker.

**or**

  *y1*:
    Starting *y* coordinate.
  *x1*:
    Starting *x* coordinate.

.. warning::

  The (*y1*, *x1*, ...) form of draw_marker is deprecated.

  Reason: (x, y) coordinate consistency

*size*
  The size of the marker (number of pixels)

*style*
  PLUS + (0)

  X + (1)

  HOLLOW_SQUARE (2)

  FILLED_SQUARE (3)

*value*:
  The pixel value to set for the line.
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("location"), Int("size", default=5),
               Choice("style", "+ x hollow_square filled_square".split(),
                      default=0),
               Pixel("value")])
  authors = "Michael Droettboom"

  def __call__(self, *args):
    if len(args) == 4:
      return _draw.draw_marker(self, *args)
    elif len(args) == 5:
      try:
        y, x, size, style, value = args
        result =  _draw.draw_marker(self, (x, y), size, style, value)
        warn_deprecated("""draw_marker(y1, x1, size, style, value) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_marker((x1, y1), size, style, value) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to draw_marker are incorrect.")
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    points = [(randint(0, 100), randint(0, 100)) for x in range(4)]
    image.draw_bezier(*tuple(list(points) + [RGBPixel(255, 0, 0), 0.1]))
    image.draw_marker(points[0], 7, 0, RGBPixel(0, 0, 255))
    image.draw_marker(points[1], 7, 1, RGBPixel(0, 255, 0))
    image.draw_marker(points[2], 7, 1, RGBPixel(0, 255, 0))
    image.draw_marker(points[3], 7, 0, RGBPixel(0, 0, 255))
    return image
  doc_examples = [__doc_example1__]

class draw_line(PluginFunction):
  """Draws a straight line between two points.

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a line at (5, 5) will be in the upper left hand corner
of the image.

The coordinates can be specified either by four floats or two FloatPoints:

  *a*:
    The start ``FloatPoint``.
  *b*:
    The end ``FloatPoint``.

**or**

  *y1*:
    Starting *y* coordinate.
  *x1*:
    Starting *x* coordinate.
  *y2*:
    Ending *y* coordinate.
  *x2*:
    Ending *x* coordinate.

.. warning::

  The (*y1*, *x1*, *y2*, *x2* ...) form of draw_line is deprecated.

  Reason: (x, y) coordinate consistency

*value*:
  The pixel value to set for the line.

*thickness* = 1.0:
  The thickness of the line (in pixels)

Based on Po-Han Lin's "Extremely Fast Line Algorithm", which is based
on the classical Breshenham's algorithm.

Freely useable in non-commercial applications as long as credits to
Po-Han Lin and link to http://www.edepot.com is provided in source
code and can been seen in compiled executable.
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("start"), FloatPoint("end"), Pixel("value"), Float("thickness")])
  authors = "Michael Droettboom based on Po-Han Lin's Extremely Fast Line Algorithm"

  def __call__(self, *args):
    if len(args) == 4:
      return _draw.draw_line(self, *args)
    elif len(args) == 3:
      return _draw.draw_line(self, *tuple(list(args) + [1.0]))
    elif len(args) == 5:
      try:
        y1, x1, y2, x2, value = args
        result = _draw.draw_line(self, (x1, y1), (x2, y2), value, 1.0)
        warn_deprecated("""draw_line(y1, x1, y2, x2, value) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_line((x1, y1), (x2, y2), value) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    elif len(args) == 6:
      try:
        y1, x1, y2, x2, value, thickness = args
        result = _draw.draw_line(self, (x1, y1), (x2, y2), value, thickness)
        warn_deprecated("""draw_line(y1, x1, y2, x2, value, thickness) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_line((x1, y1), (x2, y2), value, thickness) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to draw_line are incorrect.")
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    for i in range(10):
      image.draw_line((randint(0, 100), randint(0, 100)),
                      (randint(0, 100), randint(0, 100)),
                      RGBPixel(randint(0, 255), randint(0,255), randint(0, 255)),
                      float(randint(0, 4)))
    return image
  doc_examples = [__doc_example1__]

class draw_hollow_rect(PluginFunction):
  """Draws a hollow rectangle.

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a rectangle at (5, 5) will be in the upper left hand corner
of the image.

The coordinates can be specified either by four integers, two FloatPoints, or one Rect:

  **draw_hollow_rect** (FloatPoint(*x1*, *y1*), FloatPoint(*x2*, *y2*), *value*)

  **draw_hollow_rect** (Rect *rect*, *value*)

  **draw_hollow_rect** (*y1*, *x1*, *y2*, *x2*, *value*)

*value*:
  The pixel value to set for the lines.

*thickness*:
  The thickness of the outline

.. warning::

  The (*y1*, *x1*, *y2*, *x2*, *value*, *thickness*) form of draw_hollow_rect is deprecated.

  Reason: (x, y) coordinate consistency.
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("ul"), FloatPoint("lr"), Pixel("value"), Float("thickness")])

  def __call__(self, *args):
    if len(args) == 4:
      return _draw.draw_hollow_rect(self, *args)
    elif len(args) == 3:
      return _draw.draw_hollow_rect(self, *tuple(list(args) + [1.0]))
    elif len(args) == 2:
      try:
        a, value = args
        return _draw.draw_hollow_rect(self, a.ul, a.lr, value)
      except KeyError, AttributeError:
        pass
    elif len(args) == 5:
      try:
        y1, x1, y2, x2, value = args
        result = _draw.draw_hollow_rect(self, a.y, a.x, b.y, b.x, value)
        warn_deprecated("""draw_hollow_rect(y1, x1, y2, x2, value) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_hollow_rect((x1, y1), (x2, y2), value) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    elif len(args) == 6:
      try:
        y1, x1, y2, x2, value, thickness = args
        result = _draw.draw_hollow_rect(self, a.y, a.x, b.y, b.x, value, thickness)
        warn_deprecated("""draw_hollow_rect(y1, x1, y2, x2, value, thickness) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_hollow_rect((x1, y1), (x2, y2), value, thickness) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to draw_hollow_rect are incorrect.")
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    for i in range(10):
      image.draw_hollow_rect((randint(0, 100), randint(0, 100)),
                             (randint(0, 100), randint(0, 100)),
                             RGBPixel(randint(0, 255), randint(0,255), randint(0, 255)))
    return image
  doc_examples = [__doc_example1__]

class draw_filled_rect(PluginFunction):
  """Draws a filled rectangle.

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a rectangle at (5, 5) will be in the upper left hand corner
of the image.

The coordinates can be specified either by four integers, two FloatPoints, or one Rect:

  **draw_filled_rect** (FloatPoint(*x1*, *y1*), FloatPoint(*x2*, *y2*), *value*)

  **draw_filled_rect** (Rect *rect*, *value*)

  **draw_filled_rect** (*y1*, *x1*, *y2*, *x2*, *value*)

*value*:
  The pixel value to set for the rectangle.

.. warning::

  The (*y1*, *x1*, *y2*, *x2*, *value*) form of draw_filled_rect is deprecated.

  Reason: (x, y) coordinate consistency.
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("ul"), FloatPoint("lr"), Pixel("value")])

  def __call__(self, *args):
    if len(args) == 3:
      return _draw.draw_filled_rect(self, *args)
    elif len(args) == 2:
      try:
        a, value = args
        return _draw.draw_filled_rect(self, a.ul, a.lr, value)
      except KeyError, AttributeError:
        pass
    elif len(args) == 5:
      try:
        y1, x1, y2, x1, value = args
        result = _draw.draw_filled_rect(self, (x1, y1), (x2, y2), value)
        warn_deprecated("""draw_filled_rect(y1, x1, y2, x2, value) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_filled_rect((x1, y1), (x2, y2), value) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to draw_filled_rect are incorrect.")
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    for i in range(10):
      image.draw_filled_rect((randint(0, 100), randint(0, 100)),
                             (randint(0, 100), randint(0, 100)),
                             RGBPixel(randint(0, 255), randint(0,255), randint(0, 255)))
    return image
  doc_examples = [__doc_example1__]

class draw_bezier(PluginFunction):
  """Draws a cubic bezier curve

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a curve at (5, 5) will be in the upper left hand corner
of the image.

The coordinates can be specified either by eight floats or four FloatPoints:

  *start*:
    The start ``FloatPoint``.
  *c1*:
    The control point associated with the start point.
  *c2*
    The control point associated with the end point.
  *end*
    The end ``FloatPoint``.

**or**

  *start_y*:
    Starting *y* coordinate.
  *start_x*:
    Starting *x* coordinate.
  *c1_y*:
    Control point 1 *y* coordinate.
  *c1_x*:
    Control point 1 *x* coordinate.
  *c2_y*
    Control point 2 *y* coordinate.
  *c2_x*
    Control point 2 *x* coordinate.
  *end_y*
    Ending *y* coordinate.
  *end_x*
    Ending *x* coordinate.

.. warning::

  The "eight floats" form of draw_bezier is deprecated.

  Reason: (x, y) coordinate consistency.

*value*:
  The pixel value to set for the curve.

*accuracy* = ``0.1``:
  The rendering accuracy (in pixels)
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("start"), FloatPoint("c1"), FloatPoint("c2"), FloatPoint("end"),
               Pixel("value"), Float("accuracy", default=0.1)])

  def __call__(self, *args):
    if len(args) in (5, 6):
      if len(args) == 5:
        args = tuple(list(args) + [0.1])
      return _draw.draw_bezier(self, *args)
    elif len(args) in (9, 10):
      try:
        if len(args) == 9:
          start_y, start_x, c1_y, c1_x, c2_y, c2_x, end_y, end_x, value = args
          accuracy = 0.1
        else:
          start_y, start_x, c1_y, c1_x, c2_y, c2_x, end_y, end_x, value, accuracy = args
        result = _draw.draw_bezier(self, (start_x, start_y),
                                   (c1_x, c1_y), (c2_x, c2_y),
                                   (end_x, end_y), value, accuracy)
        warn_deprecated("""draw_bezier(start_y, start_x, c1_y, c1_x, c2_y, c2_x, end_y,
end_x, value, accuracy) is deprecated.

Reason: (x, y) coordinate consistency.

Use draw_bezier((start_x, start_y), (c1_x, c1_y), (c2_x, c2_y),
(end_x, end_y), value, accuracy) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to draw_bezier are incorrect.")
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    for i in range(10):
      image.draw_bezier((randint(0, 100), randint(0, 100)),
                        (randint(0, 100), randint(0, 100)),
                        (randint(0, 100), randint(0, 100)),
                        (randint(0, 100), randint(0, 100)),
                        RGBPixel(randint(0, 255), randint(0,255), randint(0, 255)),
                        0.1)
    return image
  doc_examples = [__doc_example1__]

class draw_circle(PluginFunction):
  """Draws a center centered at *c* with radius *r*.

Coordinates are relative to the offset of the image.  Therefore, if the image
offset is (5, 5), a line at (5, 5) will be in the upper left hand corner
of the image.

*c*:
  The center of the circle

*r*:
  The radius of the circle

*value*:
  The pixel value to set for the line.

*thickness* = 1.0:
  The thickness of the circle (in pixels)

*accuracy* = 0.1:
  The accuracy of the circle drawing

Based on the "approximating a circle with Bezier curves" approach.
http://www.whizkidtech.redprince.net/bezier/circle/
"""
  self_type = ImageType(ALL)
  args = Args([FloatPoint("c"), Float("r"), Pixel("value"), Float("thickness"), Float("accuracy")])
  authors = "Michael Droettboom with the help of Christoph Dalitz"

  def __call__(self, c, r, value, thickness = 1.0, accuracy = 0.1):
    return _draw.draw_circle(self, c, r, value, thickness, accuracy)
  __call__ = staticmethod(__call__)

  def __doc_example1__(images):
    from random import randint
    from gamera.core import Image, Dim
    image = Image((0, 0), Dim(100, 100), RGB, DENSE)
    for i in range(10):
      image.draw_circle((randint(0, 100), randint(0, 100)),
                        randint(0, 100),
                        RGBPixel(randint(0, 255), randint(0,255), randint(0, 255)),
                        1.0, 0.1)
    return image
  doc_examples = [__doc_example1__]

class flood_fill(PluginFunction):
  """Flood fills from the given point using the given color.  This is similar
to the "bucket" tool found in many paint programs.

The coordinates can be specified either by two integers or one Point:


  *a*:
    The start ``Point``.

**or**

  *y*:
    Starting *y* coordinate.
  *x*:
    Starting *x* coordinate.

*color*:
  The pixel value to set for the rectangle.

.. warning::

  The (*y*, *x*, ...) form of flood_fill is deprecated.

  Reason: (x, y) coordinate consistency.
"""
  self_type = ImageType([GREYSCALE, FLOAT, ONEBIT, RGB])
  args = Args([Point("start"), Pixel("color")])
  doc_examples = [(ONEBIT, (10, 58), 0)]

  def __call__(self, *args):
    if len(args) == 2:
      return _draw.flood_fill(self, *args)
    elif len(args) == 3:
      try:
        y1, x1, value = args
        result = _draw.flood_fill(self, (x1, y1), value)
        warn_deprecated("""flood_fill(y1, x1, color) is deprecated.

Reason: (x, y) coordinate consistency.

Use flood_fill((x1, y1), color) instead.""")
        return result
      except KeyError, AttributeError:
        pass
    raise ValueError("Arguments to flood_fill are incorrect.")
  __call__ = staticmethod(__call__)

class remove_border(PluginFunction):
  """This is a special case of the flood_fill algorithm that is designed to
remove dark borders produced by photocopiers or flatbed scanners around the
border of the image."""
  self_type = ImageType([ONEBIT])

class highlight(PluginFunction):
  """Highlights a connected component on a given image using the given color.
Self must be an RGB image (usually the original image.)

*cc*
   A one-bit connected component from the image

*color*
   An RGBPixel color value used to color the *cc*."""
  self_type = ImageType([RGB])
  args = Args([ImageType([ONEBIT], "cc"), Pixel("color")])

  def __doc_example1__(images):
    image = images[ONEBIT]
    ccs = image.cc_analysis()
    rgb = image.to_rgb()
    rgb.highlight(ccs[0], RGBPixel(255, 0, 128))
    return rgb
  doc_examples = [__doc_example1__]

class DrawModule(PluginModule):
  cpp_headers = ["draw.hpp"]
  category = "Draw"
  functions = [draw_line, draw_bezier, draw_marker,
               draw_hollow_rect, draw_filled_rect, flood_fill,
               remove_border, highlight, draw_circle]
  author = "Michael Droettboom"
  url = "http://gamera.dkc.jhu.edu/"

module = DrawModule()
