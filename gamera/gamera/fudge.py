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

FUDGE_AMOUNT = 3
FUDGE_AMOUNT_2 = 6

from gamera.core import Rect

# This is a factory function that looks like a constructor
def Fudge(o):
  # For rectangles, just return a new rectangle that is slightly larger
  if isinstance(o, Rect):
    return Rect(o.ul_y - FUDGE_AMOUNT, o.ul_x - FUDGE_AMOUNT,
                o.nrows + FUDGE_AMOUNT_2, o.ncols + FUDGE_AMOUNT_2)

  # For integers, return one of our "fudge number proxies"
  elif isinstance(o, int):
    return FudgeInt(o)
  elif isinstance(o, float):
    return FudgeFloat(o)
F = Fudge

class FudgeNumber(object):
  def __lt__(self, other):
    return self.below < other
  def __le__(self, other):
    return self.below <= other
  def __eq__(self, other):
    return self.below <= other and self.above >= other
  def __ne__(self, other):
    return other < self.below and other > self.above
  def __gt__(self, other):
    return self.above > other
  def __ge__(self, other):
    return self.above >= other

class FudgeInt(FudgeNumber, int):
  def __init__(self, value):
    int.__init__(self, value)
    self.below = int(value - FUDGE_AMOUNT)
    self.above = int(value + FUDGE_AMOUNT)

class FudgeFloat(FudgeNumber, float):
  def __init__(self, value):
    int.__init__(self, value)
    self.below = float(value - FUDGE_AMOUNT)
    self.above = float(value + FUDGE_AMOUNT)