CONFIG += debug
CONFIG += 
CONFIG += 

# the tacss/pdp version string, for adding to libs and folder (Unix only)
# the pdp system folder (Unix); (just the app folder on Win)
win32 {
  LIB_VER = 
  SYS_PLUGIN_DIR = $$(PDP4DIR)/plugins
  CONFIG_PRI = $$(PDP4DIR)/plugins/config.pri
} else {
  LIB_VER = 4.0.2b1
  #note: following should have been sub'ed by DATADIR but we couldn't
  # find a way to get that to resolve at configure time
  SYS_PLUGIN_DIR = /usr/local/share/pdp++-4.0.2b1/plugins
  CONFIG_PRI = /home/baisa/pdp4.0/plugins/config.pri
}

