WATCHCHANNEL_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
BASE:=$(dir $(WATCHCHANNEL_MAKEFILE))
V1190B_BASE=$(BASE)../

INCLUDE=$(V1190B_BASE)include $(BASE)
VPATH+=$(INCLUDE)

CC=g++
CPPFLAGS+=`root-config --cflags` $(addprefix -I,$(INCLUDE))

all: V1190B_tree

report:
	@echo Makefile: $(V1190B_CLIENT_MAKEFILE)
	@echo CPPFLAGS: $(CPPFLAGS) 

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\[ :]*,\1 $@ : ,g' > $@

%.cpp.d: %.cpp
	@$(DEPGEN)

ifeq ($(MAKECMDGOALS),clean)
NODEPS=1
endif
ifeq ($(MAKECMDGOALS),Debug)
NODEPS=1
endif
ifeq ($(MAKECMDGOALS),Release)
NODEPS=1
endif

libV1190B_client.a:
	$(MAKE) -f $(V1190B_BASE)Makefile $@
	
UNITS=main

OBJECTS=$(addsuffix .o, $(UNITS))
DEPS=$(addsuffix .cpp.d, $(UNITS))
	
V1190B_tree: $(OBJECTS) libV1190B_client.a 
	$(CC) -o $@ $^ $(LDFLAGS) `root-config --libs` -lGui

.PHONY: Debug Release clean

ifeq "$(filter clean,$(MAKECMDGOALS))" ""
	-include $(DEP)
endif

clean:
	$(MAKE) -f $(V1190B_BASE)Makefile clean
	@rm -f $(DEPS) $(OBJECTS) rootdict.cpp rootdict.h watchChannel
