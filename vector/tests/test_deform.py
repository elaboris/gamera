from gamera.core import *
init_gamera()

from gamera.toolkits.vector.plugins import deform, transformation

def test_deform():
   image = load_image("data/example.png")
   image = image.to_greyscale()
   t = transformation.rotate_degrees(3)
   image1 = image.affine_transform_image(t, True)
   assert image1.nrows == 207 and image.ncols == 337
   image2 = image.affine_transform_image(t, False)
   assert image2.nrows == 207 and image.ncols == 347
   
