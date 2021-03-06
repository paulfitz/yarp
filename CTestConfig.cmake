# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(Dart)
set(CTEST_PROJECT_NAME "YARP")
set(CTEST_NIGHTLY_START_TIME "00:00:00 CEST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "dashboard.icub.org")
set(CTEST_DROP_LOCATION "/submit.php?project=YARP")
set(CTEST_DROP_SITE_CDASH TRUE)

