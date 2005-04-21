/*
 *
 * Copyright (C) 2005 Michael Droettboom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define VECTORCOREINTERNAL
#include "vectormodule.hpp"

extern "C" {
  static PyObject* segment_new(PyTypeObject* pytype, PyObject* args,
			    PyObject* kwds);
  static void segment_dealloc(PyObject* self);
  // get/set
  static PyObject* segment_get_start(PyObject* self);
  static PyObject* segment_get_end(PyObject* self);
  static PyObject* segment_get_c1(PyObject* self);
  static PyObject* segment_get_c2(PyObject* self);
  static PyObject* segment_richcompare(PyObject* a, PyObject* b, int op);
  // methods
  static PyObject* segment_is_line(PyObject* self, PyObject* args);
  static PyObject* segment_length(PyObject* self, PyObject* args);
  static PyObject* segment_transform(PyObject* self, PyObject* args);
  static PyObject* segment_convert_to_path(PyObject* self, PyObject* args);
  static PyObject* segment_repr(PyObject* self);
}

static PyTypeObject SegmentType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PyGetSetDef segment_getset[] = {
  { "start", (getter)segment_get_start, NULL, 
    "(point property)\n\nGet the start of the segment", 0},
  { "c1", (getter)segment_get_c1, NULL, 
    "(point property)\n\nGet the first control point, if a curve", 0},
  { "c2", (getter)segment_get_c2, NULL,
    "(point property)\n\nGet the second control point, if a curve", 0},
  { "end", (getter)segment_get_end, NULL,
    "(point property)\n\nGet the end of the segment", 0},
  { NULL }
};

static PyMethodDef segment_methods[] = {
  { "is_line", segment_is_line, METH_NOARGS,
    "**is_line** ()\n\nReturns ``True`` if the segment is a line."},
  { "length", segment_length, METH_VARARGS,
    "**length** (float *accuracy* = 0.1)\n\nReturns the length of the segment.  If the segment is a curve, the error is bounded by *accuracy*."},
  { "slope", segment_length, METH_NOARGS,
    "**slope** ()\n\nThe slope of the segment.  If the segment is a curve, the control points are ignored, and it is treated as a line segment between the two end points."},
  { "reverse", segment_length, METH_NOARGS,
    "**reverse** ()\n\nReturns a new segment which is the reverse of this one.  That is, the start and end points (and control points) are swapped."},
  { "transform", segment_transform, METH_O,
    "**transform** (Transformation *t*)\n\nReturns a new segment transformed through the given transformation."},
  { "convert_to_path", segment_convert_to_path, METH_VARARGS,
    "**convert_to_path** (float *accuracy* = 0.1)\n\nConverts the segment to a path.  If the segment is a line, the path will have a single segment equal to the line.  If the segment is a curve, the path will be a sequence of straight line segments that make up the curve."},
  { NULL }
};

PyTypeObject* get_SegmentType() {
  return &SegmentType;
}

static PyObject* segment_new(PyTypeObject* pytype, PyObject* args,
			     PyObject* kwds) {
  Segment* new_segment;
  int num_args = PyTuple_GET_SIZE(args);
  if (num_args == 2) {
    PyObject* py_start, *py_end;
    if (PyArg_ParseTuple(args, "OO", &py_start, &py_end) <= 0)
      return 0;
    try {
      FloatPoint start = coerce_FloatPoint(py_start);
      FloatPoint end = coerce_FloatPoint(py_end);
      new_segment = new Segment(start, end);
    } catch (std::exception e) {
      return 0;
    }
  } else if (num_args == 4) {
    PyObject* py_start, *py_c1, *py_c2, *py_end;
    if (PyArg_ParseTuple(args, "OOOO", &py_start, &py_c1, &py_c2, &py_end) <= 0)
      return 0;
    try {
      FloatPoint start = coerce_FloatPoint(py_start);
      FloatPoint c1 = coerce_FloatPoint(py_c1);
      FloatPoint c2 = coerce_FloatPoint(py_c2);
      FloatPoint end = coerce_FloatPoint(py_end);
      new_segment = new Segment(start, c1, c2, end);
    } catch (std::exception e) {
      return 0;
    }
  } else {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments to Segment constructor.");
    return 0;
  }
  SegmentObject* so;
  so = (SegmentObject*)pytype->tp_alloc(pytype, 0);
  so->m_x = new_segment;
  return (PyObject*)so;
}

static void segment_dealloc(PyObject* self) {
  SegmentObject* x = (SegmentObject*)self;
  delete x->m_x;
  self->ob_type->tp_free(self);
}

#define CREATE_GET_FUNC(name) static PyObject* segment_get_##name(PyObject* self) {\
  Segment* x = ((SegmentObject*)self)->m_x; \
  return create_FloatPointObject(x->name()); \
} \

CREATE_GET_FUNC(start)
CREATE_GET_FUNC(c1)
CREATE_GET_FUNC(c2)
CREATE_GET_FUNC(end)

static PyObject* segment_is_line(PyObject* self, PyObject* args) {
  Segment* x = ((SegmentObject*)self)->m_x;
  if (x->is_line()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* segment_length(PyObject* self, PyObject* args) {
  Segment* x = ((SegmentObject*)self)->m_x;
  double accuracy = 0.1;
  if (PyArg_ParseTuple(args, "|d", &accuracy) <= 0)
    return 0;
  return PyFloat_FromDouble(x->length(accuracy));
}

static PyObject* segment_slope(PyObject* self, PyObject* args) {
  Segment* x = ((SegmentObject*)self)->m_x;
  return PyFloat_FromDouble(x->slope());
}

static PyObject* segment_reverse(PyObject* self, PyObject* args) {
  Segment* x = ((SegmentObject*)self)->m_x;
  return create_SegmentObject(x->reverse());
}

static PyObject* segment_transform(PyObject* self, PyObject* transform) {
  Segment* x = ((SegmentObject*)self)->m_x;
  if (is_TransformationObject(transform)) {
    Transformation* t = ((TransformationObject*)transform)->m_x;
    return create_SegmentObject(x->transform(*t));
  } else {
    PyErr_SetString
      (PyExc_TypeError,
       "Argument to transform must be a transformation object.");
    return 0;
  }
}

static PyObject* segment_convert_to_path(PyObject* self, PyObject* args) {
  Segment* x = ((SegmentObject*)self)->m_x;
  double accuracy = 0.1;
  if (PyArg_ParseTuple(args, "|d", &accuracy) <= 0)
    return 0;
  return create_PathObject(x->convert_to_path(accuracy));
}

static PyObject* segment_richcompare(PyObject* a, PyObject* b, int op) {
  Segment* as = ((SegmentObject*)a)->m_x;
  Segment* bs = ((SegmentObject*)b)->m_x; 

  /*
    Only equality and inequality make sense.
  */
  bool cmp;
  switch (op) {
  case Py_LT:
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
  case Py_LE:
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
  case Py_EQ:
    cmp = *as == *bs;
    break;
  case Py_NE:
    cmp = *as != *bs;
    break;
  case Py_GT:
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
  case Py_GE:
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
  default:
    return 0; // cannot happen
  }
  if (cmp) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* segment_repr(PyObject* self) {
  // Why do they make this so hard?  PyString_FromFormat does
  // not support doubles
  Segment* x = ((SegmentObject*)self)->m_x;
  std::ostringstream ostr;
  ostr << *x;
  PyObject* result = PyString_FromStringAndSize(ostr.str().data(), ostr.str().size());
  return result;
}

void init_SegmentType(PyObject* module_dict) {
  SegmentType.ob_type = &PyType_Type;
  SegmentType.tp_name = "vectorcore.Segment";
  SegmentType.tp_basicsize = sizeof(SegmentObject);
  SegmentType.tp_dealloc = segment_dealloc;
  SegmentType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  SegmentType.tp_new = segment_new;
  SegmentType.tp_getattro = PyObject_GenericGetAttr;
  SegmentType.tp_alloc = NULL; // PyType_GenericAlloc;
  SegmentType.tp_richcompare = segment_richcompare;
  SegmentType.tp_getset = segment_getset;
  SegmentType.tp_free = NULL; // _PyObject_Del;
  SegmentType.tp_methods = segment_methods;
  SegmentType.tp_repr = segment_repr;
  SegmentType.tp_doc = "Segment represents either a line segment or cubic bezier curve.  The segment can be transformed with a Transformation object, and bezier curves can be converted to a sequence of line segments approximating its path.\n\n**Segment** (POINT *start*, POINT *end*)\n\nCreates a new line segment from *start* to *end*.\n\n   *or*\n\n**Segment** (POINT *start*, POINT *c1*, POINT *c2*, POINT *end*)\n\nCreates a new cubic bezier curve from *start* to *end* through control points *c1* and *c2*.";
  PyType_Ready(&SegmentType);
  PyDict_SetItemString(module_dict, "Segment", (PyObject*)&SegmentType);
}
