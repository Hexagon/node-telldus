import Options
from os import unlink, symlink, popen, sys
from os.path import exists 

srcdir = '.'
blddir = 'build'
VERSION = '0.0.2'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  
  print(sys.platform)  
  if sys.platform == 'darwin':
      conf.check_tool('osx')
      tc_framework = 'TelldusCore'
      conf.env.append_value("FRAMEWORK_TC", tc_framework)
      tc_frameworkpath = '/Library/Frameworks/TelldusCore.framework/'
      conf.env.append_value("FRAMEWORKPATH_TC", tc_frameworkpath)
      tc_lib = tc_frameworkpath + 'Headers/'
      conf.env.append_value("CPPPATH_TC", tc_lib)
  elif sys.platform == 'linux2':
      conf.env.LIB_TC = 'telldus-core'
      #conf.env.LIBPATH_TC = ['/usr/lib']
      #conf.env.CCFLAGS_TC = ['-O0']
      conf.env.CCDEFINES_TC = ['TC']
      #conf.env.LINKFLAGS_TC = ['-g']
  else:
      raise ValueError("Dose not support: %r" % sys.platform)

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'telldus'
  obj.source = 'telldus.cc'
  obj.uselib = "TC"
