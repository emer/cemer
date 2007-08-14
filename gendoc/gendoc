#!/usr/bin/python
# Copyright 2007, Brian Mingus
#
# This file is part of GenDoc.py, the Emergent Document Generator
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details. 

"""
Convert the mta_gendoc.cpp generated XML to HTML

This program puts the files in /usr/local/gendoc, and therefore
needs to be run as root
"""

# Standard library imports
import os, tempfile, sys
from optparse import OptionParser

# Third party imports
from BeautifulSoup import BeautifulSoup, BeautifulStoneSoup

class Popen3:
    """
    This is a deadlock-safe version of popen that returns
    an object with errorlevel, out (a string) and err (a string).
    (capturestderr may not work under windows.)
    Example: print Popen3('grep spam','\n\nhere spam\n\n').out
    """
    def __init__(self, command, input=None,capturestderr=True):
        outfile=tempfile.mktemp()
        command="( %s ) > %s" % (command,outfile)
        if input:
            infile=tempfile.mktemp()
            open(infile,"w").write(input)
            command=command+" <"+infile
        if capturestderr:
            errfile=tempfile.mktemp()
            command=command+" 2>"+errfile
        self.errorlevel=os.system(command) >> 8
        self.out=open(outfile,"r").read()
        os.remove(outfile)
        if input:
            os.remove(infile)
        if capturestderr:
            self.err=open(errfile,"r").read()
            os.remove(errfile)

if __name__ == "__main__":
    """
    Command-line behavior
    """
    usage = "Usage: %prog --options[=value]"
    parser = OptionParser(usage)

    # TODO: Make the directories this accepts more general if others want to use it

    parser.add_option("-g",
                      "--gendoc",
                      action="store_true",
                      dest="gendoc",
                      default=0,
                      help="Convert the XML GenDoc format to HTML in `/usr/local/gendoc'")

    parser.add_option("-r",
                      "--run-gendoc",
                      action="store_true",
                      dest="make_gendoc",
                      default=0,
                      help="Run `css -gendoc' and `emergent -gendoc' in `pwd'")

    (options, args) = parser.parse_args()

    pwd = os.getcwd()
    gendoc_dir = '/usr/local/gendoc/'

    if options.make_gendoc:
        Popen3("emergent -gendoc")
        Popen3("css -gendoc")

    if options.gendoc:
        Popen3("rm -fr " + gendoc_dir) # Cleanup from last time
        # Create new directory structure

        # Output of emergent -gendoc writes emergent_TA_doc.xml
        os.makedirs(gendoc_dir + "emergent")

        # Output of css -gendoc writes css_TA_doc.xml
        os.makedirs(gendoc_dir + "temt")

        # CSS
        css = ".navcontainer ul li {align:left; list-style-type: none;margin: 0;display: block;float: left;font: 13px \"Lucida Grande\", verdana, sans-serif;text-align: center;}\n"
        css += "a {color: #000;text-decoration: none;display: block;border: 1px solid #A8B090;padding-left:6px;padding-right:6px;padding-top:4px;padding-bottom:4px;}\n"
        css += "a:hover  { background: url(\"http://grey.colorado.edu/emergent/images/c/c9/NavBarBgHover.gif\") repeat-x 20px; }\n"
        css += ".met_def {clear:both;word-wrap: break-word;margin-top:20px;margin-left:30px;border:1px;background-color:#f8f8f8;font-size:12px;}\n"
        css += "#td_opts {clear:both;}\n"
        css += ".metdefh3 {clear:both;}"

        css_file = open(gendoc_dir + "gendoc.css","w")
        css_file.write(css);css_file.close()

        emer_path = pwd + "/emergent_TA_doc.xml"
        temt_path = pwd + "/css_TA_doc.xml"

        emer = os.path.exists(emer_path)
        temt = os.path.exists(temt_path)

        if not emer and temt:
            print "Fatal error: Could not find emergent_TA_doc.xml AND css_TA_doc.xml in", pwd
            sys.exit(1)

        for project in ['emergent', 'css']:

            print "Scanning:", project

            if project == 'emergent':
                this_xml = emer_path
            if project == 'css':
                this_xml = temt_path


            f = open(this_xml)
            xml = f.read()
            f.close()

            xml = BeautifulStoneSoup(xml)
            typespace = xml.typespace
            typespacename = typespace.typename.string
            typedefs = typespace.findAllNext("typedef", recursive=False)
        
            # Populate list of typedef names
            typedef_names = []
            for typedef in typedefs:
                if len(typedef):
                    # Name (required)
                    if getattr(typedef, 'typedefname'):
                        typedefname = typedef('typedefname')[0].string
                        typedef_names.append(typedefname)
                    else:
                        # Require the typedef to have a name
                        continue

            typedef_names.sort()

            for typedef in typedefs:
                h_body = ""
                
                if len(typedef):
        
                    # Name (required)
                    if getattr(typedef, 'typedefname'):
                        typedefname = typedef('typedefname')[0].string
                        typedef_names.append(typedefname)

                        if project == 'emergent':
                            file_name = gendoc_dir + "emergent/" + typedefname + ".html"
                        if project == 'css':
                            file_name = gendoc_dir + "temt/" + typedefname + ".html"

                    else:
                        # Require the typedef to have a name
                        continue
        
                    # Type (optional) (legal values: enum, class)
                    if getattr(typedef, 'type'):
                        typetype = typedef('type')[0].string
                        if not typetype in ['enum', 'class']:
                            typetype = None
                    else:
                        typetype = None
        
                    # Description (optional)
                    if getattr(typedef, 'desc'):
                        typedesc = typedef('desc')[0].string
                        if len(typedesc):
                            h_body +=  "<div class=\"met_def\">" + typedesc + "</div>\n"
        
                    # Options (optional).
                    # typeoptions is list of options, or None
                    cats, opts = [], [] # Categories
                    if getattr(typedef, 'options'):
                        typeoptions = [option.string.replace(' ','')
                                       for option in typedef('options')[0].findAll('option')]

                        for option in typeoptions:
                            if len(option) > 3:
                                if option[:4] == "CAT_":
                                    cats.append(option[4:])
                                else:
                                    opts.append(option.strip())                                

                    # Parents (optional).
                    # typeparents is list of parents, or None
                    if getattr(typedef, 'parents'):
                        typeparents = [parent.string.replace(' ','')
                                       for parent in typedef('parents')[0].findAll('parent')]
        
                        typeparents.reverse()
                        h_body += "<h3>Parents</h3>\n"
                        h_body += "<div class=\"navcontainer\">\n"
                        h_body += "<ul>\n"
                        for parent in typeparents:
                            h_body += "<li><a href=\"" + parent  + ".html\">" + parent + "</a></li>\n" # Removed id=active
                        h_body += "</ul>\n"
                        h_body += "</div>\n" # Removed <br>

                    # Children (optional).
                    # typechildren is list of children, or None
                    if getattr(typedef, 'children'):
                        typechildren = [child.string.replace(' ','')
                                        for child in typedef('children')[0].findAll('child')]
                        h_body += "<h3>Children</h3>\n"
                        h_body += "<div class=\"navcontainer\">\n<ul>\n"
                        for child in typechildren:
                            h_body += "<li><a href=\"" + child  + ".html\">" + child + "</a></li>\n" # Removed id=active
                        h_body += "</ul>\n"
                        h_body += "</div>\n" # Removed br
                    
                    # Adds a listing of the options and categories a type is in
                    if opts:
                        h_body += "<h4 class=\"metdefh3\">Options</h4>\n"
                        h_body +=  "<div class=\"met_def\">" + ', '.join(opts) + "</div>\n"
                    if cats:
                        h_body += "<h4 class=\"metdefh3\">Categories</h4>\n"
                        h_body +=  "<div class=\"met_def\">" + ', '.join(cats) + "</div>\n"



                    this_file = open(file_name,"w")

                    h_head = "<!DOCTYPE html\n"
                    h_head += "    PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
                    h_head += "    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"

                    h_head += "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
                    h_head += "<head>\n"
                    h_head += "<title>The Emergent Document Generator</title>\n"
                    h_head += "<link href=\"../gendoc.css\" rel=\"stylesheet\" type=\"text/css\"/>\n"
                    h_head += "</head>\n"
                    h_head += "<body>\n"

                    h_jump = "<div class=\"navcontainer\">\n" # Begin navcontainer div
                    h_jump += "<ul>\n"
                    h_jump += "<li><a href=\"#TypeDef\" >TypeDef</a></li>\n" # Removed id=active

                    h_td = "<h1 id=\"TypeDef\" style=\"clear:both\">" + "typedef " + typedefname + "</h1>\n"

                    h_mem_def, h_mem_def_jump, h_mem_def_head = '', '', ''

                    # MemberSpace (optional)
                    if getattr(typedef, 'memberspace'):
                        memberspace = typedef.memberspace
        
                        # Memberspace name (optional)
                        if getattr(memberspace, 'memberspacename'):
                            memberspacename = memberspace.memberspacename.string
                        else:
                            memberspacename = None
        
                        # MemberDef (mandatory)
                        memberdefs = typedef.memberspace.findAll('memberdef', recursive=False)
        
                        if len(memberdefs):
                            h_mem_def = ""
                            membdef_names = []

                            h_jump += "<li><a href=\"#MemberDef\">MemberDef</a></li>\n" # removed id=active
                            h_mem_def_head = "<h2 id=\"MemberDef\" style=\"clear:both\">MemberDefs</h2>\n"
                            h_mem_def_jump = "<div class=\"navcontainer\">\n"
                            h_mem_def_jump += "<ul>\n"
                            h_mem_def += "<div style=\"clear:both;\">\n"

                            for memberdef in memberdefs:
        
                                # name (mandatory)
                                memberdefname = memberdef.memberdefname.string
                                membdef_names.append(memberdefname)
        
                                # type (mandatory)
                                memberdeftype = memberdef.type.string
        
                                # Options (optional).
                                # memberdefoptions is list of options, or None
                                mem_cats, mem_opts = [], []
                                if getattr(memberdef, 'options'):
                                    memberdefoptions = [memberdefoption.string.replace(' ','')
                                                        for memberdefoption in memberdef('options')[0].findAll('option')]

                                    for option in memberdefoptions:
                                        if len(option) > 3:
                                            if option[:4] == "CAT_":
                                                mem_cats.append(option[4:])
                                            else:
                                                mem_opts.append(option.strip())                                

                                h_mem_def += "<h3 id=\"" + memberdefname + "\">" + memberdefname + "</h3>\n"

                                # Adds a listing of the options and categories a type is in
                                if mem_opts:
                                    h_mem_def +=  "<div class=\"met_def\"><b>Options:</b> " + ', '.join(mem_opts) + "</div>\n"
                                if mem_cats:
                                    h_mem_def +=  "<div class=\"met_def\"><b>Categories:</b> " + ', '.join(mem_cats) + "</div>\n"

                            h_mem_def += "</div>\n"

                            # This comes outside of the main loop so we can have an alpha-sorted reference
                            # and a non-sorted listing

                            membdef_names.sort()
                            for memberdefname in membdef_names:
                                h_mem_def_jump += "<li><a href=\"#" + memberdefname + "\">" + memberdefname + "</a></li>\n" # removed id=active

                            h_mem_def_jump += "</ul>\n"
                            h_mem_def_jump += "</div>\n"

                    # MethodSpace (optional)

                    # These variables allow us to non-linearly insert
                    # information into the final document

                    h_met_def, h_met_def_jump, h_met_def_head = '', '', ''

                    if getattr(typedef, 'methodspace'):
                        methodspace = typedef.methodspace
                        # Get a sorted list of MethodDef names for the navigation bar
                        metdef_names = []
        
                        # MethodDefs (mandatory)
                        if len(methodspace):
                            methoddefs = methodspace.findAll('methoddef')
        
                            h_jump += "<li><a href=\"#MethodDef\" >MethodDef</a></li>\n" # removed id=active
                            h_met_def_head = "<h2 id=\"MethodDef\" style=\"clear:both\">MethodDefs</h2>\n"
                            h_met_def_jump = "<div class=\"navcontainer\">\n"
                            h_met_def_jump += "<ul>\n"

                            for methoddef in methoddefs:
                                # name (mandatory)
                                methoddefname = methoddef.methoddefname.string
                                metdef_names.append(methoddefname)

                                # desc (optional)
                                if getattr(methoddef, 'desc'):
                                    methoddefdesc = methoddef.desc.string
                                else:
                                    methoddefdesc = ""
        
                                # prototype (optional)
                                if getattr(methoddef, 'prototype'):
                                    methoddefprototype = methoddef.prototype.string
                                else:
                                    methoddefprototype = None
        
                                # Options (optional).
                                # methoddefoptions is list of options, or None
                                if getattr(methoddef, 'options'):
                                    methoddefoptions = [methoddefoption.string.replace(' ','')
                                                        for methoddefoption in methoddef('options')[0].findAll('option')]
                                else:
                                    methoddefoptions = None

                                h_met_def += "<h3 class=\"metdefh3\" id=\"" + methoddefname + "\">" + methoddefname + "</h3>\n"

                                h_met_def += "<div class=\"met_def\">"
                                h_met_def += "<b>Prototype: </b>" + methoddefprototype + "<br/><br/>"
                                h_met_def += "</div>\n"

                                if len(methoddefdesc):
                                    h_met_def += "<div class=\"met_def\">"
                                    h_met_def += "<b>Desc</b>: " + methoddefdesc
                                    h_met_def += "</div>\n"

                            metdef_names.sort()
                            for methoddefname in metdef_names:
                                h_met_def_jump += "<li><a href=\"#" + methoddefname + "\">" + methoddefname + "</a></li>\n"


                    # EnumSpace (optional)
                    h_enum_def, h_enum_def_jump, h_enum_def_head = '', '', ''

                    if getattr(typedef, 'enumspace'):
                        enumspace = typedef.enumspace
                        # Get a sorted list of EnumDef names for the navigation bar        
                        enumdef_names = []

                        # EnumDef (mandatory)
                        if len(enumspace):
                            enumdefs = enumspace.findAll('enumdef')
        
                            h_jump += "<li><a href=\"#EnumDef\">EnumDef</a></li>\n" # removed id=active
                            h_enum_def_head = "<h2 id=\"EnumDef\" style=\"clear:both\">EnumDefs</h2>\n"
                            h_enum_def_jump = "<div class=\"navcontainer\">\n"
                            h_enum_def_jump += "<ul>\n"

                            for enumdef in enumdefs:
        
                                # name (mandatory)
                                enumdefname = enumdef.enumdefname.string
                                enumdef_names.append(enumdefname)
                                h_enum_def_jump += "<li><a href=\"#" + enumdefname + "\">" + enumdefname + "</a></li>\n" # removed id=active

                                # desc (optional)
                                if getattr(enumdef, 'desc'):
                                    enumdefdesc = enumdef.desc.string
                                else:
                                    enumdefdesc = None
        
                                # value (mandatory)
                                enumdefvalue = enumdef.value.string

                                h_enum_def += "<h3 class=\"metdefh3\" id=\"" + methoddefname + "\">" + enumdefname + " = " + enumdefvalue + "</h3>\n"

                                if enumdefdesc:
                                    h_enum_def += "<div class=\"met_def\">"
                                    h_enum_def += "<b>Desc</b>: " + enumdefdesc
                                    h_enum_def += "</div>\n"

                            enumdef_names.sort()
                            h_enum_def_jump += "</ul>\n"
                            h_enum_def_jump += "</div>\n"


                    # This should come at the end of each typedef,
                    # nested on the same level as memberdefs, methoddefs, etc...

                    h_jump += "</ul>\n" # End navlist ul
                    h_jump += "</div>\n" # End navcontainer div

                    h_met_def_jump += "</ul>\n"
                    h_met_def_jump += "</div>\n"

                    h_foot = "</body>\n"
                    h_foot += "</html>\n"


                    ##################################
                    #        HTML AGGREGATION        #
                    ##################################

                    # Up to this point, we've just been collecting bits of HTML
                    # Now lets put them together

                    # This is all header bits, including the types parents/children/options
                    html = h_head + h_td + h_jump + h_body

                    # Add MethodDefs
                    if getattr(typedef, 'methodspace'):
                        html += h_met_def_head + h_jump + "<p><br/></p> " + h_met_def_jump + h_met_def

                    # Add MemberDefs
                    if getattr(typedef, 'memberspace'):
                        html += h_mem_def_head + h_jump + "<p><br/></p> " + h_mem_def_jump + h_mem_def

                    # Add EnumDefs
                    if getattr(typedef, 'enumspace'):
                        html += h_enum_def_head + h_jump + "<p><br/></p> " + h_enum_def_jump + h_enum_def

                    # Add SubTypes/EnumTypes
                    #if getattr(typedef, 'subtypes'):
                    #    html += h_sub_def_head + "<p><br/></p> " + h_jump + h_sub_def

                    html += h_foot

                    pretty_html = BeautifulSoup(html)
                    pretty_html = pretty_html.prettify()

                    print file_name
                    this_file.write(pretty_html)
                    this_file.close()



#                     # SubTypes (optional)
#                     # SubTypes contain EnumTypes contain EnumDefs

#                     h_sub_def, h_sub_def_jump, h_sub_def_head, h_sub_sub_def_head = '', '', '', ''

#                     if getattr(typedef, 'subtypes'):
#                         subtypes = typedef.subtypes
#                         h_jump += "<li><a href=\"#EnumTypes\" >EnumType</a></li>\n"
#                         h_sub_def_head += "<h2 id=\"EnumTypes\" style=\"clear:both\">EnumType</h2>\n"
#                         h_sub_sub_def_head += "<h3 id=\"EnumDefs\" style=\"clear:both\">EnumDefs</h2>\n"

#                         # Get a sorted list of EnumDefnames for the navigation bar

#                         if getattr(typedef, 'enumtype'):
#                             enumtypes = subtypes.findAll('enumtype', recursive=False)
#                             if len(enumtypes):
#                                 for enumtype in enumtypes:
#                                     enumtypename = enumtype.enumtypename.string
#                                     enumdefs = enumtype.findAll('enumdef', recursive=False)

#                                     # EnumDefs (optional)
#                                     for enumdef in enumdefs:
#                                         enumdefname = enumdef.enumdefname.string
#                                         enumdef_names = []

#                                         if len(enumdefs):

#                                             h_sub_def_jump = "<div class=\"navcontainer\">\n"
#                                             h_sub_def_jump += "<ul>\n"

#                                             for enumdef in enumdefs:
#                                                 # EnumDefName (mandatory)
#                                                 enumdefname = enumdef.enumdefname.string
#                                                 enumdef_names.append(enumdefname)

#                                                 # EnumDef description (optional)
#                                                 if getattr(enumdef, 'desc'):
#                                                     enumdefdesc = enumdef.desc.string
#                                                 else:
#                                                     enumdefdesc = None

#                                                 # EnumDef value (mandatory)
#                                                 if getattr(enumdef, 'value'):
#                                                     enumdefvalue = enumdef.value.string
#                                                 else:
#                                                     enumdefvalue = None

#                                                 h_sub_def += "<h4 class=\"metdefh3\" id=\"" + enumdefname + "\">" + enumdefname + " = " + enumdefvalue + "</h3>\n"

#                                                 if enumdefdesc:
#                                                     h_sub_def += "<div class=\"met_def\">"
#                                                     h_sub_def += "<b>Desc: </b>" + enumdefdesc + "<br/><br/>"
#                                                     h_sub_def += "</div>\n"

#                                             enumdef_names.sort()
#                                             for enumdefname in enumdef_names:
#                                                 h_sub_def_jump += "<li><a href=\"#" + enumdefname + "\">" + enumdefname + "</a></li>\n"

#                                             h_mem_def_jump += "</ul>\n"
#                                             h_mem_def_jump += "</div>\n"
