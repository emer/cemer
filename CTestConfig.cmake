## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(Dart)
set(CTEST_PROJECT_NAME "Emergent")
set(CTEST_NIGHTLY_START_TIME "04:00:00 EST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "grey.colorado.edu")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=Emergent")
set(CTEST_DROP_SITE_CDASH TRUE)
