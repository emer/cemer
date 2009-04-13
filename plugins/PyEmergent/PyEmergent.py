# In order to overcome limitations of the plugin implementation it may
# be necessary to first wrap some objects in simpler python


def HelloPython():
    f=open("HelloPython","w")
    f.write("HelloPython")
    f.close()

# Get a list of modules from twisted

from time import time
from twisted.python.modules import walkModules, getModule

modules = []
start = time()
for modinfo in walkModules():
    modules.append(modinfo.name)
print time() - start
print len(modules)

