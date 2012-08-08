V1190B_CLIENT_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))

V1190B_BASE=$(dir $(V1190B_CLIENT_MAKEFILE))

INCLUDE=$(V1190B_BASE)include
VPATH+=$(V1190B_BASE)src $(INCLUDE)

CC=g++
CPPFLAGS+=`root-config --cflags` $(addprefix -I,$(INCLUDE))

all: V1190B_client online_spectrum watchChannel V1190B_tree V1190B_hist

report:
	@echo Makefile: $(V1190B_CLIENT_MAKEFILE)
	@echo CPPFLAGS: $(CPPFLAGS) 

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

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

ifndef NODEPS
-include V1190BClient.cpp.d
-include socketbuf.cpp.d
-include online_spectrum.cpp.d
-include V1190B_client.cpp.d
endif

libV1190B_client.a: V1190BClient.o socketbuf.o
	ar -r $@ $^
CLEAN+=V1190BClient.o socketbuf.o libV1190B_client.a V1190BClient.cpp.d socketbuf.cpp.d libV1190B_client.cpp.d

CLEAN+=online_spectrum.o online_spectrum.cpp.d online_spectrum
online_spectrum: online_spectrum.o libV1190B_client.a
	$(CC) -o $@ $^ $(LDFLAGS) `root-config --libs`

	
CLEAN+=watchChannel.o watchChannel.cpp.d watchChannel
watchChannel: libV1190B_client.a $(addprefix, watchChannel/, gui.o  HistogramManager.o  main.o  rootdict.o  settings.o)
	$(CC) -o $@ $^ $(LDFLAGS) `root-config --libs`
	
	
CLEAN+=V1190B_client.o V1190B_client.cpp.d V1190B_client
V1190B_client: V1190B_client.o libV1190B_client.a
	$(CC) -o $@ $^ $(LDFLAGS)


CLEAN+=V1190B_tree.o V1190B_tree.cpp.d V1190B_tree
V1190B_tree: V1190B_tree.o libV1190B_client.a
	$(CC) -o $@ $^ $(LDFLAGS) `root-config --libs`

CLEAN+=V1190B_hist.o V1190B_hist.cpp.d V1190B_hist
V1190B_hist: V1190B_hist.o libV1190B_client.a
	$(CC) -o $@ $^ $(LDFLAGS) `root-config --libs`

.PHONY: Debug Release clean

clean:
	@rm -f $(CLEAN) $(addprefix Debug/,$(CLEAN)) $(addprefix Release/,$(CLEAN))
	-@rmdir Debug Release


#Debug: export CPPFLAGS+=-O0 -g -fno-inline
#Release: export CPPFLAGS+=-O3 -DNDEBUG

#Debug Release: report
#	mkdir -p $@ && $(MAKE) -f $(abspath $(V1190B_CLIENT_MAKEFILE)) -C $@
