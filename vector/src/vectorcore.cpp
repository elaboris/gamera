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

/*
  These are prototyped here because this is the only file in which
  they will be used. They are defined in the .cpp files for each of
  the types.
*/

#define VECTORCOREINTERNAL
#include "vectormodule.hpp"
// #include "transformationobject.hpp"

void init_FloatPointType(PyObject* module_dict);
void init_TransformationType(PyObject* module_dict);
void init_SegmentType(PyObject* module_dict);
void init_PathType(PyObject* module_dict);
void init_PathListType(PyObject* module_dict);

extern "C" {
  DL_EXPORT(void) initvectorcore(void);
}

PyMethodDef vector_module_methods[] = {
  {NULL, NULL },
};

DL_EXPORT(void)
initvectorcore(void) {
  PyObject* m = Py_InitModule("vectorcore", vector_module_methods);
  PyObject* d = PyModule_GetDict(m);
  
  init_FloatPointType(d);
  init_TransformationType(d);
  init_SegmentType(d);
  init_PathType(d);
  init_PathListType(d);
}
