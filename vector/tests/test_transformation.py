from gamera.core import *
from gamera.toolkits.vector.vectorcore import *
from gamera.toolkits.vector.plugins.transformation import *
from random import random

def test_identity_transformation():
   t = Transformation()
   assert t.is_identity()
   assert t == identity()
   assert t.is_rectilinear()
   assert t.scale_x == 1
   assert t.rotate0 == 0
   assert t.rotate1 == 0
   assert t.scale_y == 1
   assert t.translate_x == 0
   assert t.translate_y == 0
   for i in xrange(100):
      f = FloatPoint(random(), random())
      assert f == t(f)
   assert repr(t) == "Transformation(1, 0, 0, 1, 0, 0)"

def test_translate():
   for i in xrange(10):
      p1 = FloatPoint(random(), random())
      t = translate(p1)
      t2 = translate(p1.x, p1.y)
      for j in xrange(10):
         p2 = FloatPoint(random(), random())
         assert t(p2) == p2 + p1
         assert t2(p2) == p2 + p1
         assert t(p2) == t2(p2)

def test_scale():
   for i in xrange(10):
      p1 = FloatPoint(random(), random())
      t = scale(p1)
      t2 = scale(p1.x, p1.y)
      for j in xrange(10):
         p2 = FloatPoint(random(), random())
         assert t(p2) == p2 * p1
         assert t2(p2) == p2 * p1
         assert t(p2) == t2(p2)

def test_shear():
   for i in xrange(100):
      p1 = FloatPoint(random(), random())
      t = shear(random(), random())
      p2 = t(p1)
      t = shear_radians(random(), random())
      p2 = t(p1)
      t = shear_degrees(random(), random())
      p2 = t(p1)

def test_rotate():
   c = FloatPoint(50, 50)
   p = FloatPoint(40, 50)
   for i in xrange(360):
      t = rotate_degrees_at(c, i)
      assert abs(t(p).distance(c) - 10) < 1e-6

def test_reflect():
   assert reflect_at(FloatPoint(50, 50), True, 0)(FloatPoint(40, 40)) == FloatPoint(60, 40)

def test_composite_transformation():
   a = rotate_degrees_at(FloatPoint(50, 50), 5)
   b = transform_at(FloatPoint(50, 50), rotate_degrees(5))
   c = rotate_degrees(5)
   d = translate(-50, -50)
   e = translate(50, 50)
   for i in xrange(100):
      f = FloatPoint(random(), random())
      r0 = a(f)
      r1 = b(f)
      r2 = e(c(d(f)))
      assert r0 == r1
      diff = abs(r1 - r2)
      assert diff.x < 1e-6
      assert diff.y < 1e-6
      
