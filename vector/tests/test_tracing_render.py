from gamera.core import *
init_gamera()
from gamera.toolkits.vector.vectorcore import *
from gamera.toolkits.vector.plugins import *
from gamera.toolkits.vector.plugins import transformation
from random import randint

def test_tracing():
   image = load_image("data/example.png")
   paths = image.potrace()
   assert paths.area() == 6998.0292934033632

   render = Image(0, 0, image.nrows, image.ncols, RGB)
   for path in paths:
      render.draw_path_filled(path, RGBPixel(randint(0, 255), randint(0, 255), randint(0, 255)), 0.1)

   render2 = Image(0, 0, image.nrows * 4, image.ncols * 4, RGB)
   t = transformation.scale(FloatPoint(4, 4))
   paths.transform(t, True)
   t = transformation.rotate_degrees(1)
   for i in xrange(5):
      paths.transform(t, True)
      for path in paths:
         render2.draw_path_filled(path, RGBPixel(randint(0, 255), randint(0, 255), randint(0, 255)), 0.1)
         render2.draw_path_hollow(path, RGBPixel(randint(0, 255), randint(0, 255), randint(0, 255)), 0.1)
                 
