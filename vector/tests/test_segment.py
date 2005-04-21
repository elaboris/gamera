from gamera.toolkits.vector.vectorcore import *
from random import random
import py.test

def raises_error():
   segment = Segment(1, 2, 3, 4)

def test_line():
   py.test.raises(TypeError, raises_error)

   t = Transformation(1, 0, 0, 1, 0, 0)
   for i in xrange(50):
      start = FloatPoint(random(), random())
      end = FloatPoint(random(), random())
      segment = Segment(start, end)
      assert segment.start == start
      assert segment.end == end
      segment.length()
      segment2 = segment.transform(t)
      assert segment2 == segment

def test_curve():
   t = Transformation(1, 0, 0, 1, 0, 0)
   for i in xrange(50):
      start = FloatPoint(random(), random())
      c1 = FloatPoint(random(), random())
      c2 = FloatPoint(random(), random())
      end = FloatPoint(random(), random())
      segment = Segment(start, c1, c2, end)
      assert segment.start == start
      assert segment.end == end
      assert segment.c1 == c1
      assert segment.c2 == c2
      segment.length()
      segment2 = segment.transform(t)
      assert segment2 == segment
   
