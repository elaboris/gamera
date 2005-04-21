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
#include "transformation.hpp"

extern "C" {
  static PyObject* transformation_new(PyTypeObject* pytype, PyObject* args,
				 PyObject* kwds);
  static void transformation_dealloc(PyObject* self);
  // methods
  static PyObject* transformation_repr(PyObject* self);
  static PyObject* transformation_transform(PyObject* self, PyObject* args);
  static PyObject* transformation_inverse_transform(PyObject* self, PyObject* args);
  static PyObject* transformation_get_inverse_transformation(PyObject* self, PyObject* args);
  static PyObject* transformation_is_identity(PyObject* self, PyObject* args);
  static PyObject* transformation_is_rectilinear(PyObject* self, PyObject* args);
  static PyObject* transformation_expansion_factor(PyObject* self, PyObject* args);
  // operators
  static PyObject* transformation_mul(PyObject* self, PyObject* other);
  // properties
  static PyObject* transformation_get_scale_x(PyObject* self);
  static PyObject* transformation_get_rotate0(PyObject* self);
  static PyObject* transformation_get_rotate1(PyObject* self);
  static PyObject* transformation_get_scale_y(PyObject* self);
  static PyObject* transformation_get_translate_x(PyObject* self);
  static PyObject* transformation_get_translate_y(PyObject* self);
}

// Type stuff

static PyTypeObject TransformationType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PyNumberMethods transformation_number_methods;

static PyGetSetDef transformation_getset[] = {
  { "scale_x", (getter)transformation_get_scale_x, NULL, "(float property)\n\nThe scale_x part of the transformation matrix.", 0},
  { "rotate0", (getter)transformation_get_rotate0, NULL, "(float property)\n\nThe rotate0 part of the transformation matrix.", 0},
  { "rotate1", (getter)transformation_get_rotate1, NULL, "(float property)\n\nThe rotate1 part of the transformation matrix.", 0},
  { "scale_y", (getter)transformation_get_scale_y, NULL, "(float property)\n\nThe scale_y part of the transformation matrix.", 0},
  { "translate_x", (getter)transformation_get_translate_x, NULL, "(float property)\n\nThe translate_x part of the transformation matrix.", 0},
  { "translate_y", (getter)transformation_get_translate_y, NULL, "(float property)\n\nThe translate_y part of the transformation matrix.", 0},
  { NULL }
};

static PyMethodDef transformation_methods[] = {
  { "transform", transformation_transform, METH_O,
    "**transform** (POINT *p*)\n\nApplies the transformation to the given point and returns the result."},
  { "inverse_transform", transformation_transform, METH_O,
    "**inverse_transform** (POINT *p*)\n\nApplies the inverse of the transformation to the given point and returns the result."},
  { "get_inverse_transformation", transformation_get_inverse_transformation, METH_O,
    "**get_inverse_transformation** ()\n\nCreates a new transformation that is the inverse of this one."},
  { "is_identity", transformation_is_identity, METH_NOARGS,
    "**is_identity** ()\n\nReturns ``True`` if the transformation is the identity transformation (i.e. a tranformation that does nothing)." },
  { "is_rectilinear", transformation_is_identity, METH_NOARGS,
    "**is_rectilinear** ()\n\nReturns ``True`` if the transformation is rectilinear, i.e. grid-aligned rectangles are transformed to other grid-aligned rectangles." },
  { "expansion_factor", transformation_expansion_factor, METH_NOARGS,
    "**expansion_factor** ()\n\nReturns the increase in size of the bounding box of an object after going through this transformation." },
  { NULL }
};

PyTypeObject* get_TransformationType() {
  return &TransformationType;
}

// Constructor / destructor

static PyObject* transformation_new(PyTypeObject* pytype, PyObject* args,
				    PyObject* kwds) {
  Transformation* new_point;
  double scale_x = 1.0;
  double rotate0 = 0.0;
  double rotate1 = 0.0;
  double scale_y = 1.0;
  double translate_x = 0.0;
  double translate_y = 0.0;
  
  static char* keywords[] = {"scale_x", "rotate0", "rotate1", "scale_y", 
			     "translate_x", "translate_y", NULL};
  if (PyArg_ParseTupleAndKeywords
      (args, kwds, "|dddddd", keywords, 
       &scale_x, &rotate0, &rotate1, &scale_y, &translate_x, &translate_y) <= 0)
    return 0;
  
  TransformationObject* so;
  so = (TransformationObject*)pytype->tp_alloc(pytype, 0);
  so->m_x = new Transformation(scale_x, rotate0, rotate1, scale_y, 
			       translate_x, translate_y);
  return (PyObject*)so;
}

static void transformation_dealloc(PyObject* self) {
  TransformationObject* x = (TransformationObject*)self;
  delete x->m_x;
  self->ob_type->tp_free(self);
}

static PyObject* transformation_richcompare(PyObject* a, PyObject* b, int op) {
  Transformation* at = ((TransformationObject*)a)->m_x;
  Transformation* bt = ((TransformationObject*)b)->m_x;

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
    cmp = *at == *bt;
    break;
  case Py_NE:
    cmp = *at != *bt;
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

// Methods

static PyObject* transformation_transform(PyObject* self, PyObject* point) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  try {
    FloatPoint fp = coerce_FloatPoint(point);
    FloatPoint result = x->transform(fp);
    return create_FloatPointObject(result);
  } catch (std::exception e) {
    return 0;
  }
}

static PyObject* transformation_call(PyObject* self, PyObject* args, PyObject* kwargs) {
  PyObject* other;
  if (PyArg_ParseTuple(args, "O", &other) <= 0)
    return 0;
  return transformation_transform(self, other);
}

static PyObject* transformation_inverse_transform(PyObject* self, PyObject* point) {
  Transformation* x = ((TransformationObject*)self)->m_x;
    
  try {
    FloatPoint fp = coerce_FloatPoint(point);
    FloatPoint result = x->inverse_transform(fp);
    return create_FloatPointObject(result);
  } catch (std::exception e) {
    return 0;
  }
}

static PyObject* transformation_get_inverse_transformation(PyObject* self, PyObject* point) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  return create_TransformationObject(x->get_inverse_transformation());
}

static PyObject* transformation_is_identity(PyObject* self, PyObject* _) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  if (x->is_identity()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* transformation_is_rectilinear(PyObject* self, PyObject* _) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  if (x->is_rectilinear()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* transformation_expansion_factor(PyObject* self, PyObject* _) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  return PyFloat_FromDouble(x->expansion_factor());
}

static PyObject* transformation_repr(PyObject* self) {
  // Why do they make this so hard?  PyString_FromFormat does
  // not support doubles
  Transformation* x = ((TransformationObject*)self)->m_x;
  std::ostringstream ostr;
  ostr << *x;
  PyObject* result = PyString_FromStringAndSize(ostr.str().data(), ostr.str().size());
  return result;
}

// Operators

static PyObject* transformation_mul(PyObject* self, PyObject* other) {
  Transformation* x = ((TransformationObject*)self)->m_x;
  if (is_TransformationObject(other)) {
    Transformation* y = ((TransformationObject*)other)->m_x;
    return create_TransformationObject((*x) * (*y));
  } else {
    PyErr_SetString(PyExc_TypeError,
		    "Transformation objects can only be multiplied with other transformation objects");
    return 0;
  }
}

// Properties

#define CREATE_GET_FUNC(name) static PyObject* transformation_get_##name(PyObject* self) {\
  Transformation* x = ((TransformationObject*)self)->m_x; \
  return PyFloat_FromDouble((int)x->name()); \
}

CREATE_GET_FUNC(scale_x);
CREATE_GET_FUNC(rotate0);
CREATE_GET_FUNC(rotate1);
CREATE_GET_FUNC(scale_y);
CREATE_GET_FUNC(translate_x);
CREATE_GET_FUNC(translate_y);

void init_TransformationType(PyObject* module_dict) {
  transformation_number_methods.nb_multiply = transformation_mul;
  
  TransformationType.ob_type = &PyType_Type;
  TransformationType.tp_name = "vectorcore.Transformation";
  TransformationType.tp_basicsize = sizeof(TransformationObject);
  TransformationType.tp_dealloc = transformation_dealloc;
  TransformationType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  TransformationType.tp_new = transformation_new;
  TransformationType.tp_getattro = PyObject_GenericGetAttr;
  TransformationType.tp_alloc = NULL; // PyType_GenericAlloc;
  TransformationType.tp_richcompare = transformation_richcompare;
  TransformationType.tp_free = NULL; // _PyObject_Del;
  TransformationType.tp_methods = transformation_methods;
  TransformationType.tp_getset = transformation_getset;
  TransformationType.tp_repr = transformation_repr;
  TransformationType.tp_doc = "Transformation objects hold a transformation matrix that can be used to apply affine transformations to 2D points.  These transformations include scale, translation, rotation and shearing.\n\nSetting the values of the transformation matrix by hand can be tricky.  To assist in this, convenience functions for creating transformations are available in the transformations plugin module.\n\nTransformations can be composed using the multiplication (*) operator.  For instance::\n\n  t = rotate_degrees(45) * scale(2)\n\nwill create a transformation that is equivalent to rotating by 45 degrees *then* scaling by 2.  Note that the order of transformations reads left-to-right, which is technically the opposite of a matrix multiplication, but easier to read and understand for this application.\n\nFor more information on affine transformation matrices, see the Wikipedia.";
  TransformationType.tp_as_number = &transformation_number_methods; 
  TransformationType.tp_call = transformation_call;
  PyType_Ready(&TransformationType);
  PyDict_SetItemString(module_dict, "Transformation", (PyObject*)&TransformationType);
}
