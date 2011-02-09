########### get svn revision ###############

# http://stackoverflow.com/questions/3780667/use-cmake-to-get-build-time-svn-revision
FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${SOURCE_DIR} Project)
  file(WRITE svnrev.h.txt "#define SVN_REV ${Project_WC_REVISION}\n")
  # build avoidance: only update .h if changed
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    svnrev.h.txt svnrev.h)
ENDIF(Subversion_FOUND)
