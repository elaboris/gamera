# vi:set tabsize=3:

from distutils.core import setup, Extension
from distutils.util import get_platform
from distutils.sysconfig import get_python_lib
import sys, os, time, locale, string
import glob

# If gamera.generate is imported gamera.__init__.py will
# also be imported, which won't work until the build is
# finished. To get around this, the gamera directory is
# added to the path and generate is imported directly
sys.path.append("gamera")
import generate, gamera_setup

########################################
# Check that this is at least Python 2.2
gamera_setup.check_python_version()

##########################################
# generate the plugins

plugin_extensions = []

# we grab all of the plugins except __init__.py - of course
# to exclude this we have to go throug all sorts of crap...
if not '--help' in sys.argv and not '--help-commands' in sys.argv:
   plugins = gamera_setup.get_plugin_filenames('gamera/plugins/')

   # Create the list of modules to ignore at import - because
   # we are in the middle of the build process a lot of C++
   # plugins don't yet exist. By preventing the import of
   # the core of gamera and all of the plugins we allow the
   # plugins to be imported for the build process to examine
   # them.
   ignore = ["core", "gamera.core", "gameracore"]
   for x in plugins:
      plug_path, filename = os.path.split(x)
      module_name = "_" + filename.split('.')[0]
      ignore.append(module_name)
   generate.magic_import_setup(ignore)

   plugin_extensions = gamera_setup.generate_plugins(plugins)

   generate.restore_import()


   ########################################
   # Generate the command line scripts based on the running version of Python
   command_line_utils = {
       'gamera_gui':
       """#!%(executable)s\n"""
       """%(header)s\n"""
       """print "Loading GAMERA..."\n"""
       """from gamera.gui import gui\n"""
       """gui.run()""",

       'gamera_cl':
       """#!/usr/bin/env sh\n"""
       """%(header)s\n"""
       """%(executable)s -i -c "from gamera.core import *; init_gamera()"\n""",

       'gamera_test':
       """#!%(executable)s\n"""
       """from gamera import testing\n"""
       """testing.main()"""
       }

   #if os.name == 'posix':
   info = {'executable': sys.executable,
           'header'    :
           """# This file was automatically generated by the\n"""
           """# Gamera setup script on %s.\n""" } #%
      #time.strftime(locale.nl_langinfo(locale.D_FMT))}
   for file, content in command_line_utils.items():
      fd = open(file, 'w')
      fd.write(content % info)
      fd.close()
   os.chmod(file, 0700)

########################################
# Distutils setup

ga_files = glob.glob("src/ga/*.C")
ga_files.append("src/knncoremodule.cpp")

extensions = [Extension("gamera.gameracore",
                        ["src/gameramodule.cpp",
                         "src/sizeobject.cpp",
                         "src/pointobject.cpp",
                         "src/dimensionsobject.cpp",
                         "src/rectobject.cpp",
                         "src/regionobject.cpp",
                         "src/regionmapobject.cpp",
                         "src/rgbpixelobject.cpp",
                         "src/imagedataobject.cpp",
                         "src/imageobject.cpp",
                         "src/imageinfoobject.cpp"
                         ],
                        include_dirs=["include"],
                        # FIXME
                        libraries=["stdc++"]),
              Extension("gamera.knncore", ga_files,
                        include_dirs=["include", "src/ga", "src"],
                        libraries=["stdc++"]),
              Extension("gamera.graph", ["src/graphmodule.cpp"],
                        include_dirs=["include", "src"],
                        libraries=["stdc++"])]
extensions.extend(plugin_extensions)

setup(name = "gamera", version="1.1",
      ext_modules = extensions,
      packages = ['gamera', 'gamera.gui', 'gamera.plugins', 'gamera.toolkits',
                  'gamera.toolkits.omr'],
      data_files=[('include/gamera', glob.glob("include/*.hpp")),
                  ('include/gamera/plugins', glob.glob("include/plugins/*.hpp")),
                  ('gamera/doc', glob.glob("gamera/doc/*.html")),
                  ('gamera/doc/classes', glob.glob("gamera/doc/classes/*.html")),
                  ('gamera/doc/plugins', glob.glob("gamera/doc/plugins/*.html"))]
      )

##########################################
# generate the non-plugin help pages
# This needs to be done last since it requires a built system
if not '--help' in sys.argv and not '--help-commands' in sys.argv:
   # This is where things just got built
   sys.path.append("build/lib.%s-%s/gamera" % (get_platform(), sys.version[0:3]))
   import generate_help
   generate_help.generate_help()
