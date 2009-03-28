# In order to overcome limitations of the plugin implementation it may
# be necessary to first wrap some objects in simpler python

def HelloPython():
    f=open("HelloPython","w")
    f.write("HelloPython")
    f.close()
