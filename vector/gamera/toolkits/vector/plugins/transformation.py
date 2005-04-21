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

class _DummyPoint:
   def __init__(self, x, y):
      self.x = x
      self.y = y

def _point_or_2_doubles(core):
   def __call__(x, y=None):
      from gamera.toolkits.vector.plugins import _transformation
      from gamera.toolkits.vector.vectorcore import FloatPoint
      if not y is None:
         p = FloatPoint(x, y)
      else:
         p = x
      return getattr(_transformation, core)(p)
   return __call__

def _2_points_or_point_and_2_doubles(core):
   def __call__(p0, x, y=None):
      from gamera.toolkits.vector.plugins import _transformation
      from gamera.toolkits.vector.vectorcore import FloatPoint
      if not y is None:
         p = FloatPoint(x, y)
      else:
         p = x
      return getattr(_transformation, core)(p0, p)
   return __call__

def _gen_doc(trans, *args):
   def _doc_transformation(images):
      from gamera.toolkits.vector.plugins import transformation, render
      from gamera.core import Image, RGBPixel
      onebit = images[ONEBIT]
      paths = onebit.potrace()
      paths.transform(transformation.translate(onebit.ncols, onebit.nrows), True)
      new_image = Image(0, 0, onebit.nrows * 3, onebit.ncols * 3, RGB)
      for path in paths:
         new_image.draw_path_filled(path, RGBPixel(0,0,0))
      t = globals()[trans](*args)
      paths.transform(t, True)
      for path in paths:
         new_image.draw_path_filled(path, RGBPixel(255,128,128))
      return [new_image]
   return _doc_transformation

class translate(PluginFunction):
   """   or\n\n**translate** (float *x*, float *y*)\n\nCreates a new Transformation matrix that translates by the given amount."""
   self_type = None
   args = Args([FloatPoint("translate")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_point_or_2_doubles("translate"))
   doc_examples = [_gen_doc("translate", 15, 15)]

class transform_at(PluginFunction):
   """Creates a new transformation based on *t* that is relative to point *p* rather than the origin.\n\nEquivalent to::\n\n   translate(-p) * t * translate(p)"""
   self_type = None
   args = Args([FloatPoint("p"), Transformation("t")])
   return_type = Transformation("transformation")

class scale(PluginFunction):
   """   or\n\n**translate** (float *x*, float *y*)\n\nCreates a new Transformation matrix that scales by the given amount around the origin."""
   self_type = None
   args = Args([FloatPoint("scale")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_point_or_2_doubles("scale"))
   doc_examples = [_gen_doc("scale", 1.5, 1.5)]

class scale_at(PluginFunction):
   """   or\n\n**translate** (FloatPoint *p*, float *x*, float *y*)\n\nCreates a new Transformation matrix that scales by the given amount around point *p*."""
   self_type = None
   args = Args([FloatPoint("at"), FloatPoint("scale")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_2_points_or_point_and_2_doubles("scale_at"))
   doc_examples = [_gen_doc("scale_at", _DummyPoint(105, 150), 2, 2)]

class shear(PluginFunction):
   """  or \n\n**shear** (float *x*, float *y*)\n\nCreates a new Transformation matrix that shears by the given amount, in pixels."""
   self_type = None
   args = Args([FloatPoint("shear")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_point_or_2_doubles("shear"))
   doc_examples = [_gen_doc("shear", 0.8, 0)]

class shear_radians(PluginFunction):
   """  or \n\n**shear_radians** (float *x*, float *y*)\n\nCreates a new Transformation matrix that shears by the given amount, in radians."""
   self_type = None
   args = Args([FloatPoint("shear_radians")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_point_or_2_doubles("shear_radians"))
   doc_examples = [_gen_doc("shear_radians", 0.4, 0)]

class shear_degrees(PluginFunction):
   """  or \n\n**shear_degrees** (float *x*, float *y*)\n\nCreates a new Transformation matrix that shears by the given amount, in degrees."""
   self_type = None
   args = Args([FloatPoint("shear_degrees")])
   return_type = Transformation("transformation")
   __call__ = staticmethod(_point_or_2_doubles("shear_degrees"))
   doc_examples = [_gen_doc("shear_degrees", 5, 0)]

class rotate_radians(PluginFunction):
   """Creates a new Transformation matrix that rotates by the given number of radians around the origin."""
   self_type = None
   args = Args([Float("radians")])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("rotate_radians", 0.1)]

class rotate_degrees(PluginFunction):
   """Creates a new Transformation matrix that rotates by the given number of degrees around the origin."""
   self_type = None
   args = Args([Float("degrees")])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("rotate_degrees", 5)]

class rotate_radians_at(PluginFunction):
   """Creates a new Transformation matrix that rotates by the given number of radians at point *p*."""
   self_type = None
   args = Args([FloatPoint("p"), Float("radians")])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("rotate_radians_at", _DummyPoint(105, 150), -0.8)]

class rotate_degrees_at(PluginFunction):
   """Creates a new Transformation matrix that rotates by the given number of degrees at point *p*."""
   self_type = None
   args = Args([FloatPoint("p"), Float("degrees")])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("rotate_degrees_at", _DummyPoint(105, 150), 15)]

class reflect(PluginFunction):
   """Creates a new Transformation matrix that reflects across the *x* axis (when *x* is True) and/or the *y* axis (when *y* is True)."""
   self_type = None
   args = Args([Check("x"), Check("y")])
   return_type = Transformation("transformation")

class reflect_at(PluginFunction):
   """Creates a new Transformation matrix that reflects across the vertical line through *p.x* (when *x* is True) and/or the horizontal line through *p.y* (when *y* is True)."""
   self_type = None
   args = Args([FloatPoint("p"), Check("x"), Check("y")])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("reflect_at", _DummyPoint(105, 150), True, False)]

class identity(PluginFunction):
   """Returns the identity transform, which is simply a transformation that does nothing."""
   self_type = None
   args = Args([])
   return_type = Transformation("transformation")
   doc_examples = [_gen_doc("identity")]

class TransformationModule(PluginModule):
   cpp_headers = ["vectormodule.hpp", "transformation.hpp"]
   category = "Vector/Transformation"
   functions = [translate, transform_at, scale, scale_at, shear,
                shear_radians, shear_degrees, rotate_radians,
                rotate_degrees, rotate_radians_at,
                rotate_degrees_at, reflect, reflect_at,
                identity]
   author = "Michael Droettboom"
module = TransformationModule()

translate = translate()
transform_at = transform_at()
scale = scale()
scale_at = scale_at()
shear = shear()
shear_radians = shear_radians()
shear_degrees = shear_degrees()
rotate_radians = rotate_radians()
rotate_degrees = rotate_degrees()
rotate_radians_at = rotate_radians_at()
rotate_degrees_at = rotate_degrees_at()
reflect = reflect()
reflect_at = reflect_at()
identity = identity()

__all__ = "translate transform_at scale scale_at shear shear_radians shear_degrees rotate_radians rotate_degrees rotate_radians_at rotate_degrees_at reflect reflect_at identity".split()
