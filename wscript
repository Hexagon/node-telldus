import Options
from os import unlink, symlink, popen
from os.path import exists 

srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')
  opt.tool_options('osx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  #conf.check_tool('osx')

  #tc_framework = 'TelldusCore'
  #conf.env.append_value("FRAMEWORK_TC", tc_framework)
  #tc_frameworkpath = '/Library/Frameworks/TelldusCore.framework/'
  #conf.env.append_value("FRAMEWORKPATH_TC", tc_frameworkpath)
  #tc_lib = tc_frameworkpath + 'Headers/'
  #conf.env.append_value("CPPPATH_TC", tc_lib)

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'telldus'
  obj.source = 'telldus.cc'
  #obj.uselib = "TC"