#!/bin/bash

# this script generates changelogs/ChangeLog_vers.html based on svn log info
# svn_gen is main function, called with revision range and version number
# just add new item at next highest line to add for a new revision

svn_gen() {
    revrange=$1
    vers=$2
    fname=changelogs/ChangeLog_${vers}.html
    svn log -v -r $revrange --xml . | xsltproc --stringparam versNum $vers --stringparam revRange $revrange ./svn_changelog_style.xsl - > $fname
    svn add $fname
}

svn_index_gen() {
    cd changelogs
    echo "<html>" > index.html
    echo "<head>" >> index.html
    echo "<title>Emergent ChangeLogs Generated from SVN Logs</title>" >> index.html
    echo "</head>" >> index.html
    echo "<h1>Emergent ChangeLogs Generated from SVN Logs</h1>" >> index.html
    echo "<ul>" >> index.html
    for file in ChangeLog*.html; do
	echo "<li><a href=$file>$file</a></li>" >> index.html
    done 
    echo "</ul>" >>index.html
    echo "</html>" >> index.html
}

#svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
# svn_gen "" ""
svn_gen "10824:11074" "8.2.0"
#svn_gen "10445:10823" "8.1.0"
#svn_gen "10019:10445" "8.0.4"
#svn_gen "10019:10272" "8.0.3"
#svn_gen "9984:10018" "8.0.1"
#svn_gen "7464:9983" "8.0.0"
#svn_gen "7375:7463" "7.0.1"
#svn_gen "6417:7374" "7.0.0"
#svn_gen "6301:6416" "6.3.2"
#svn_gen "6228:6300" "6.3.0"
#svn_gen "5753:6227" "6.2.0"
#svn_gen "5575:5752" "6.1.0"
#svn_gen "5230:5574" "6.0.0"
#svn_gen "4992:5229" "5.3.0"
#svn_gen "4476:4991" "5.2.0"
#svn_gen "4322:4475" "5.1.0"
#svn_gen "4099:4321" "5.0.2"
#svn_gen "4038:4098" "5.0.1"
#svn_gen "3509:4037" "5.0.0"
#svn_gen "3124:3508" "4.0.19"
#svn_gen "2990:3123" "4.0.17"
#svn_gen "2745:2989" "4.0.15"
#svn_gen "2688:2744" "4.0.13"
#svn_gen "2410:2687" "4.0.11"
#svn_gen "1:2409" "4.0.5"    # first public release!

svn_index_gen
