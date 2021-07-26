###############################################################################
#                                                                             #
#                        General Makefile for C/C++                           #
#           Copyright (C) 2015 Grant Ayers <ayers@cs.stanford.edu>            #
#           Hosted at GitHub: https://github.com/grantea/makefiles            #
#                                                                             #
# This file is free software distributed under the BSD license. See LICENSE   #
# for more information.                                                       #
#                                                                             #
# This is a single-target, general-purpose Makefile for C/C++ projects. It is #
# desgined for use with GNU Make and GCC, but may work with other software    #
# with little or no modification.                                             #
#                                                                             #
# Set the program name, source root, and any desired compiler options.        #
# All dependencies (including header file changes) will be                    #
# handled automatically.                                                      #
#                                                                             #
###############################################################################

#modified my Rami Abueshsheikh 1/18/2018 to resolve bugs

#---------- Basic settings  ----------#
PROGRAM_NAME := chess
SOURCE_BASE  := .

#---------- Compilation and linking ----------#
CXX        := gcc
SRC_EXTS   := .c
CXX_LANG   := -Wall -Wextra -Wfatal-errors
OPT_BASE   := -march=native

# can pass in paramter debug=yes
ifeq ($(debug),yes)
    CXX_OPT    := -ggdb -O0 $(OPT_BASE)
else
    CXX_OPT    := -O3 $(OPT_BASE)
endif

# can pass parameter prod=yes, removes asserts from code
ifeq ($(prod),yes)
    CXX_OPT    := -NDEBUG $(CXX_OPT)
endif


INC_DIRS   := -I$(SOURCE_BASE)
LINK_FLAGS := -lm


#---------- No need to modify below ----------#
DIRS := $(shell find $(SOURCE_BASE) -type d -print)
SRCS := $(foreach DIR,$(DIRS),$(foreach EXT,$(SRC_EXTS),$(wildcard $(DIR)/*$(EXT))))
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)
OPTS := $(CXX_LANG) $(CXX_OPT)
APP  := $(PROGRAM_NAME)

.PHONY: clean all

all: $(APP)
	@echo $(SRCS)

$(APP) : $(OBJS)
	@echo [LD] $@
	@$(CXX) $(OPTS) $(OBJS) $(LINK_FLAGS) -o $(APP)

$(SRC_EXTS:=.o) :
	@echo [CC] $@
	@$(CXX) $(OPTS) $(INC_DIRS) -MD -MP -c -o $@ $<

clean:
	rm -f $(OBJS) $(DEPS)

-include $(DEPS)

