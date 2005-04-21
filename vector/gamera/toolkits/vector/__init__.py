# Gamera Vector Toolkit
#
# Copyright (C) 2005 Michael Droettboom
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

"""
Toolkit setup

This file is run on importing anything within this directory.
Its purpose is only to help with the Gamera GUI shell,
and may be omitted if you are not concerned with that.
"""

from gamera import toolkit
# from gamera.toolkits.vector import main

# Let's import all our plugins here so that when this toolkit
# is imported using the "Toolkit" menu in the Gamera GUI
# everything works.

from gamera.toolkits.vector.plugins import *

## # You can inherit from toolkit.CustomMenu to create a menu
## # for your toolkit.  Create a list of menu option in the
## # member _items, and a series of callback functions that
## # correspond to them.  The name of the callback function
## # should be the same as the menu item, prefixed by '_On'
## # and with all spaces converted to underscores.
## class VectorMenu(toolkit.CustomMenu):
##     _items = ["Vector Toolkit",
##               "Vector Toolkit 2"]
##     def _OnVector_Toolkit(self, event):
##         wxMessageDialog(None, "You clicked on Vector Toolkit!").ShowModal()
##         main.main()
##     def _OnVector_Toolkit_2(self, event):
##         wxMessageDialog(None, "You clicked on Vector Toolkit 2!").ShowModal()
##         main.main()
## vector_menu = VectorMenu()
