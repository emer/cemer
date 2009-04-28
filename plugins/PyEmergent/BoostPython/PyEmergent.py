import os
from pyplusplus import module_builder
 
#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [r"ta_gendoc.h"]
                                      , gccxml_path=r"/usr/bin/gccxml" 
                                      , working_directory=r"/usr/local/include/Emergent"
                                      , include_paths=['/usr/local/include/Emergent', '/usr/include/qt4', '/usr/include/qt4/Qt', '/usr/include/qt4/QtCore', '/usr/local/include/Emergent']
                                      , define_symbols=[] )
 
 
#Well, don't you want to see what is going on?
# mb.print_declarations()
 
#Creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='PyEmergent' )
 
#Writing code to file.
mb.write_module( 'PyEmergent.cpp' )
