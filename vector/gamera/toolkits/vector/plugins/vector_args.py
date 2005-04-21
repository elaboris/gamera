from gamera import args
from gamera.gui import has_gui
if has_gui.has_gui:
   import wx
   from gamera.gui import args_gui
from new import instancemethod

class FloatPoint(args.Arg):
   c_type = "FloatPoint"
   delete_cpp = False
   convert_from_PyObject = True
   def __init__(self, name=None, default=None):
      args.Arg.__init__(self, name)
      if default is None:
         self.has_default = False
         self.default = None
      else:
         self.has_default = True
         self.default = default
   
   # For generating C++ wrappers
   def from_python(self):
      return """
      try {
         %(symbol)s = coerce_FloatPoint(%(pysymbol)s);
      } catch (std::exception e) {
         PyErr_SetString(PyExc_TypeError, "Argument '%(name)s' must be a FloatPoint object, or convertible to a FloatPoint object");
         return 0;
      }
      """ % self

   def to_python(self):
      return """%(pysymbol)s = create_FloatPointObject(%(symbol)s);""" % self

   # No GUI stuff here, yet
   if has_gui.has_gui:
      def get_control(self, parent, locals=None):
         if self.default == None:
            from gamera.toolkits.vector.vectorcore import FloatPoint
            self.default = FloatPoint(0, 0)
         self.control = wx.BoxSizer(wx.HORIZONTAL)
         self.control.Add(wx.StaticText(parent, -1, "x:"))
         self.control_x = wx.TextCtrl(
            parent, -1, str(self.default.x),
            validator = args_gui._RealValidator(name=self.name))
         self.control.Add(self.control_x, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 5)
         self.control.Add(wx.StaticText(parent, -1, "y:"))
         self.control_y = wx.TextCtrl(
            parent, -1, str(self.default.y),
            validator = args_gui._RealValidator(name=self.name))
         self.control.Add(self.control_y, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 5)
         return self

      def get(self):
         from gamera.toolkits.vector.vectorcore import FloatPoint
         return FloatPoint(float(self.control_x.GetValue()),
                           float(self.control_y.GetValue()))

      def get_string(self):
         return str(self.get())
      
class Transformation(args.Arg):
   c_type = "Transformation"
   delete_cpp = False
   convert_from_PyObject = True

   # For generating C++ wrappers
   def from_python(self):
      return """
      if (!is_TransformationObject(%(pysymbol)s)) {
         PyErr_SetString(PyExc_TypeError, "Argument '%(name)s' must be a Transformation object");
         return 0;
      }
      %(symbol)s = *(((TransformationObject*)%(pysymbol)s)->m_x);
      """ % self

   def to_python(self):
      return "%(pysymbol)s = create_TransformationObject(%(symbol)s);" % self

   # Default to the standard "Class" GUI

class Segment(args.Arg):
   c_type = "Segment"
   delete_cpp = False
   convert_from_PyObject = True

   def __init__(self, name=None, list_of=False):
      from gamera.toolkits.vector.vectorcore import Path
      args.Class.__init__(self, name, Path, list_of)

   def from_python(self):
      return """
      if (!is_SegmentObject(%(pysymbol)s)) {
         PyErr_SetString(PyExc_TypeError, "Argument '%(name)s' must be a Segment object");
         return 0;
      }
      %(symbol)s = ((SegmentObject*)%(pysymbol)s)->m_x;
      """ % self

   def to_python(self):
      return "%(pysymbol)s = create_SegmentObject(%(symbol)s);" % self

class Path(args.Class):
   c_type = "boost::shared_ptr<Path>"
   delete_cpp = False
   convert_from_PyObject = True

   def __init__(self, name=None, list_of=False):
      from gamera.toolkits.vector.vectorcore import Path
      args.Class.__init__(self, name, Path, list_of)

   def from_python(self):
      return """
      if (!is_PathObject(%(pysymbol)s)) {
         PyErr_SetString(PyExc_TypeError, "Argument '%(name)s' must be a Path object");
         return 0;
      }
      %(symbol)s = ((PathObject*)%(pysymbol)s)->m_x;
      """ % self

   def to_python(self):
      return "%(pysymbol)s = create_PathObject(%(symbol)s);" % self

class PathList(args.Class):
   c_type = "boost::shared_ptr<PathList>"
   delete_cpp = False
   convert_from_PyObject = True

   def __init__(self, name=None, list_of=False):
      from gamera.toolkits.vector.vectorcore import PathList
      args.Class.__init__(self, name, PathList, list_of)

   def from_python(self):
      return """
      if (!is_PathListObject(%(pysymbol)s)) {
         PyErr_SetString(PyExc_TypeError, "Argument '%(name)s' must be a Path object");
         return 0;
      }
      %(symbol)s = ((PathListObject*)%(pysymbol)s)->m_x;
      """ % self

   def to_python(self):
      return "%(pysymbol)s = create_PathListObject(%(symbol)s);" % self
