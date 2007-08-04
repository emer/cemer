#!/usr/bin/env python
# Copyright 2007, Brian Mingus
#
# This file is part of GenDoc.py, the PDP++ Document Generator
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

if __name__ == "__main__":
    """
    Command-line behavior
    """
    usage = "Usage: %prog --options[=value]"
    parser = OptionParser(usage)

    parser.add_option("-g",
                      "--gendoc",
                      action="store_true",
                      dest="gendoc",
                      default=0,
                      help="Run `maketa -gendoc', convert the XML GenDoc format to HTML and update the wiki. Depends on the PDP4DIR environment variable to know where to find maketa and the xml files.")

    (options, args) = parser.parse_args()

    if options.gendoc:
        """
        TODO: Put all of the TA/CSS stuff in that category, and all of the PDP stuff in that category
        """
        valid_tags = ['typespace','typedef','desc','options','parent','children','subtypes','enumtype','enumdef','memberspace','memberdef','type','methodspace','methoddef','prototype','desc','option','enumspace','enumdef']

        if os.environ.has_key("PDP4DIR"):
            pwd = os.environ["PDP4DIR"]
        else:
            print "Error: PDP4DIR not set"
            sys.exit(1)

        # TODO: Uncomment
        #tmp = Popen3("cd " + pwd).out
        #tmp += Popen3("make gendoc").out

        xml = ""
        gendoc_dir = '/usr/local/gendoc/'
        Popen3("rm -fr " + gendoc_dir) # Cleanup from last time

        dirs = ["ta","pdp","bp","leabra","so","cs"]
        print "Scanning",dirs
        for ddir in dirs:
            # Scan in XML files
            abs_xml_path = pwd + "/src/" + ddir + "/" + ddir + "_TA_doc.xml"
            f=open(abs_xml_path);xml+=f.read();f.close()

            # Create new directory structure
            os.makedirs(gendoc_dir + ddir)

        # Write css file
        css = "<style>\n<!--\n" + HtmlFormatter().get_style_defs('.highlight') + "\n#navcontainer ul li {align:left;list-style-type: none;padding: 0;margin: 0;display: block;float: left;font: 10px/20px \"Lucida Grande\", verdana, sans-serif;text-align: center;}\n#navcontainer a {color: #000;text-decoration: none;display: block;width: 115px;border: 1px solid #A8B090;}\n#navcontainer a:hover { background: url(\"http://grey.colorado.edu/pdp/images/c/c9/NavBarBgHover.gif\") repeat-x 20px; }\n#met_def {word-wrap: break-word;margin-top:20px;margin-left:30px;border:1px;background-color:#f8f8f8;font-size:12px;}\n#td_opts {margin-top:5%;}\n-->\n</style>"

        css_file = open(gendoc_dir + "gendoc.css","w")
        css_file.write(css);css_file.close()

        xml = unescape(xml)
        soup = BeautifulStoneSoup(xml)

        ####################
        #     TypeSpace    #
        ####################
        TypeSpaces = soup.findAll("typespace")
        for TypeSpace in TypeSpaces:
            space_name = TypeSpace.next.next.next
            this_dir = gendoc_dir + space_name + "/"

            ####################
            #     TypeDef      #
            ####################
            # TODO: Global css include
            # TODO: Write a file for each TypeDef. Put them in folders according
            # to the name of the TypeSpace

            TypeDefs = TypeSpace.findAllNext("typedef")

            TypeDef_names = []
            for TypeDef in TypeDefs:
                TypeDef_names.append(TypeDef.next.next.next)

            TypeDef_names.sort()

            for TypeDef in TypeDefs:
                def_name = TypeDef.next.next.next

                h_body = ""

                try:
                    td_desc = TypeDef.desc
                    if len(td_desc) > 4:
                        h_body += td_desc + "\n"
                except:
                    None

                try:
                    opts = []
                    cats = []
                    td_options = TypeDef.options
                    for td_option in td_options:
                        if len(td_option.string) > 3:
                            if td_option.string[:4] == "CAT_":
                                cats.append(td_option.string[4:])
                            else:
                                opts.append(td_option.string.strip())
                except:
                    cats = None
                    opts = None

                # Parents
                try:
                    parents = []
                    td_parents = TypeDef.parents
                    for td_parent in td_parents:
                        if len(td_parent.string) > 2:
                            parents.append(td_parent.string)
                except:
                    parents = None

                if parents:
                    parents.reverse()
                    h_body += "<h3>Parents</h3>"
                    h_body += "<div id=\"navcontainer\">\n<ul id=\"navlist\">"
                    for parent in parents:
                        h_body += "<li><a href=\"" + parent  + ".html\" id=\"active\">" + parent + "</a></li>"
                    h_body += "</ul></div>\n<br/>"
                # Children
                try:
                    children = []
                    td_children = TypeDef.children
                    for td_child in td_children:
                        if len(td_child.string) > 2:
                            children.append(td_child.string)
                except:
                    children = None

                if children:
                    #children.sort()
                    h_body += "<h3>Children</h3>"
                    h_body += "<div id=\"navcontainer\">\n<ul id=\"navlist\">"
                    for child in children:
                        h_body += "<li><a href=\"" + child  + ".html\" id=\"active\">" + child + "</a></li>"
                    h_body += "</ul></div>\n<br/>"

                if opts:
                    h_body += "<span style=\"margin-top: 5px;\"><h4 id=\"td_opts\">Options</h4></span>"
                    h_body +=  "<div id=\"met_def\">" + ', '.join(opts) + "</div>"
                if cats:
                    h_body += "<h4 id=\"td_cats\">Categories</h4>"
                    h_body +=  "<div id=\"met_def\">" + ', '.join(cats) + "</div>"

                file_name = this_dir + def_name + ".html"
                this_file = open(file_name,"w")

                h_head = "<html><head><link href=\"../gendoc.css\" rel=\"stylesheet\" type=\"text/css\"/></head><body>"
                h_jump = "<div id=\"navcontainer\">\n<ul id=\"navlist\">"
                h_jump += "<li><a href=\"#TypeDef\" id=\"active\">TypeDef</a></li>"
                h_td = "<h1 id=\"TypeDef\" style=\"clear:both\">" + highlight("typedef " + def_name, CppLexer(), HtmlFormatter())  + "</h1>"

                ####################
                #     MethodDef    #
                ####################
                MethodDefs = TypeDef.findAll("methoddef")
                total = len(MethodDefs)
                h_met_def,h_met_def_jump,h_met_def_head = '','',''
                if total:
                    h_met_def = ""
                    # Get a sorted list of MethodDef names for the navigation bar
                    MetDef_names = []
                    for MethodDef in MethodDefs:
                        MetDef_names.append(MethodDef.next.next.next)
                    MetDef_names.sort()

                    h_met_def_head = "<h2 id=\"MethodDef\" style=\"clear:both\">MethodDef's</h2>"
                    h_met_def_jump = "<div id=\"navcontainer\">\n<ul id=\"navlist\">\n"

                    for name in MetDef_names:
                        h_met_def_jump += "<li><a href=\"#" + name + "\" id=\"active\">" + name + "</a></li>"

                    h_jump += "<li id=\"active\"><a href=\"#MethodDef\" id=\"active\">MethodDef</a></li>"

                    h_met_def += "<div style=\"clear:both;margin-top:10%;\">" # Start: Head room

                    for MethodDef in MethodDefs:
                        m_def_name = MethodDef.next.next.next
                        h_met_def += "<span style=\"margin-top: 5px;\"><h3 id=\"" + m_def_name + "\">" + m_def_name + "</b></span>\n"
                        try:
                            if MethodDef.desc.string:
                                h_met_desc = MethodDef.desc.string
                        except:
                            h_met_desc = ""

                        h_met_def += "<div id=\"met_def\">" + highlight(MethodDef.prototype.string, CppLexer(), HtmlFormatter()) + "\n" + h_met_desc + "</div>"

                    h_met_def += "</div>" # End: Head room

                h_foot = "</body></html>"
                h_jump += "</ul></div>\n<br/>"
                h_met_def_jump += "</ul></div><br/>\n"
                html = h_head + h_td + h_jump + h_body + h_met_def_head + h_jump + h_met_def_jump + h_met_def + h_foot

                print "Writing" + file_name
                this_file.write(html);this_file.close()
