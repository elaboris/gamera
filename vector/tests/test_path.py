from gamera.toolkits.vector.vectorcore import *
from random import random

def test_path():
   path = Path()

   start = FloatPoint(random(), random())
   path.add_segment(start)
   for i in xrange(100):
      path.add_segment(FloatPoint(random(), random()))
      path.add_segment(FloatPoint(random(), random()),
                       FloatPoint(random(), random()),
                       FloatPoint(random(), random()))

   assert not path.is_closed()
   path.add_segment(start)
   assert path.is_closed()

   a = path.area()
   assert len(path.holes) == 0

   hole = Path()
   path.holes.append(hole)
   start = FloatPoint(random(), random())
   hole.add_segment(start)
   for i in xrange(20):
      hole.add_segment(FloatPoint(random(), random()))
      hole.add_segment(FloatPoint(random(), random()),
                       FloatPoint(random(), random()),
                       FloatPoint(random(), random()))
   hole.add_segment(start)

   assert path.area() == a - hole.area()
   assert len(path) < len(path.convert_to_lines())
                       
   
