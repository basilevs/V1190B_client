WATCHCHANNEL_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
BASE:=$(dir $(WATCHCHANNEL_MAKEFILE))
V1190B_BASE=$(BASE)../

SOCKETBUF?=../../../common/socketbuf

INCLUDE=$(V1190B_BASE)include $(BASE) $(SOCKETBUF)
VPATH+=$(INCLUDE) 

CC=g++
CPPFLAGS+=$(addprefix -I,$(INCLUDE))

all: V1190B_dump

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
	
UNITS=V1190B_dump

OBJECTS=$(addsuffix .o, $(UNITS))
DEPS=$(addsuffix .cpp.d, $(UNITS))
	
V1190B_dump: $(OBJECTS) libV1190B_client.a 
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: Debug Release clean

ifeq "$(filter clean,$(MAKECMDGOALS))" ""
	-include $(DEPS)
endif

clean:
	$(MAKE) -f $(V1190B_BASE)Makefile clean
	rm -f $(OBJECTS)
	rm V1190B_dump
