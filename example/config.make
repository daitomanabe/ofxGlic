################################################################################
# CONFIGURE PROJECT MAKEFILE (optional)
################################################################################

# OF_ROOT allows to move projects outside apps/* directory
OF_ROOT = ../../..

# make sure the path to the parrent addon is added
PROJECT_ADDON_PATHS = ../

################################################################################
# PROJECT LINKER FLAGS
################################################################################

PROJECT_LDFLAGS = -std=c++17
