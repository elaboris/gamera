#
# Copyright (C) 2001, 2002 Ichiro Fujinaga, Michael Droettboom,
#                          and Karl MacMillan
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

from __future__ import generators

import string, UserList, sys, re   ## Python standard
from math import pow
from gamera.enums import *
from gamera import config

# determine if an object is a sequence
def is_sequence(obj):
  return type(obj) in (type([]), type(()))

def make_sequence(obj):
  if not type(obj) in (type([]), type(())):
    return (obj,)
  return obj

# replaces the prefix a with b
def replace_prefix(s, a, b):
  return b + s[len(a):]

def fast_cmp(x, y):
  return cmp(x.sort_cache, y.sort_cache)

# determines if an object is a C/C++ object
def is_cpp_instance(obj, name):
  return name == string.split(repr(obj))[0][1:]

# Returns true if the distance between a and b is
# within range
def rangeeq(a, b, range, mod = 0):
  """Returns true if the difference of *a* and *b* is within a given *range*.
If *mod* is given, than the difference is always considered less than *mod*."""
  if mod == 0:
    return abs(b - a) <= range
  else:
    return min((b - a) % mod,
               (a - b) % mod) <= range

def constains_instance(gl, klass):
  """Returns true if list *gl* contains an instance of *klass*"""
  for g in gl:
    if isinstance(g, klass):
      return 1
  return 0  

# Converts any string into a valid Python identifier
def string2identifier(str):
  """Defines how illegal variable names are converted to legal ones."""
  if len(str):
    name = re.sub('\-|/|\.|\ ', '_', str, 0)
    if name[0] in string.digits:
      name = "_" + name
    return name
  else:
    return "DEFAULT"

# Returns the sign of a number
def sign(i):
  if i < 0:
    return -1
  return 1

# A container with all unique elements
class Set(UserList.UserList):
    def append(self, item):
      if item not in self.data:
        self.data.append(item)
    def insert(self, i, item):
      if item not in self.data:
        self.data.insert(i, item)
    def extend(self, other):
      for item in other:
        self.append(item)

_byte_steps = (('Gb', float(1 << 30), float(1 << 30) * 1.1),
               ('Mb', float(1 << 20), float(1 << 20) * 1.1),
               ('kb', float(1 << 10), float(1 << 10) * 1.1),
               ('bytes', 1, -1))
def pretty_print_bytes(bytes):
  for step in _byte_steps:
    if bytes > step[2]:
      return "%.2f %s" % (float(bytes) / step[1], step[0])
  return 'error (negative!)'

# This is a back port of Python 2.3's enumerate function.
# This will make our loops look a lot cleaner
if float(sys.version[0:3]) < 2.3:
  def enumerate(collection):
    i = 0
    it = iter(collection)
    while 1:
      yield(i, it.next())
      i += 1
  __builtins__['enumerate'] = enumerate
  __builtins__['True'] = 1
  __builtins__['False'] = 0
else:
  enumerate = __builtins__.enumerate

_pixel_type_names = {ONEBIT:     "OneBit",
                     GREYSCALE:  "GreyScale",
                     GREY16:     "Grey16",
                     RGB:        "RGB",
                     FLOAT:      "Float"}
def get_pixel_type_name(type_):
    return _pixel_type_names[type_]

def group_list(list, group_size):
  groups = []
  for i in range(0, len(list), group_size):
    groups.append(list[i:min(i+group_size, len(list))])
  return groups

def word_wrap(stream, l, indent=0, width=78):
  indent *= 2
  width -= indent
  indent_spaces = ' ' * (indent)
  if is_sequence(l):
    l = ' '.join([str(x) for x in l])
  i = 0
  p = 0
  while i != -1:
    stream.write(indent_spaces)
    if len(l) - p < width:
      stream.write(l[p:].strip())
      stream.write('\n')
      break
    else:
      i = l.rfind(' ', p, p + width)
      if i == -1:
        stream.write(l[p:].strip())
      stream.write(l[p:i].strip())
    stream.write('\n')
    p = i + 1

def encode_binary(s):
  import zlib, binascii
  return binascii.b2a_base64(zlib.compress(s))
  
def decode_binary(s):
  import zlib, binascii
  return zlib.decompress(binascii.a2b_base64(s))

class ProgressText:
  width = 70
  
  def __init__(self, message):
    self._message = message
    self._starting = 1
    self._done = 0
    self._last_amount = 0
  
  def update(self, num, den):
    if self._starting:
      sys.stdout.write(self._message)
      sys.stdout.write("\n")
      self._starting = 0
    if not self._done:
      progress = int((float(num) / float(den)) * self.width)
      if progress != self._last_amount:
        self._last_amount = progress
        left = self.width - progress
        sys.stdout.write("|")
        sys.stdout.write("#" * progress)
        sys.stdout.write("=" * left)
        sys.stdout.write("|\r")
        sys.stdout.flush()
      if num >= den:
        self._done = 1
        sys.stdout.write("\n")
        sys.stdout.flush()

def ProgressFactory(message):
  gui = config.get_option('__gui')
  if gui:
    return gui.ProgressBox(message)
  else:
    return ProgressText(message)
  
