-include Makefile.local
TARGET=
VERBOSE=
Q=@
LOCAL_CONFIG_DIR=~/.local/share/engine
BUILDDIR?=build

MAKE_PID := $$PPID
JOB_FLAG := $(filter -j%, $(subst -j ,-j,$(shell ps T | grep "^\s*$(MAKE_PID).*$(MAKE)")))

all: build

.PHONY: build
build:
	$(Q)mkdir -p $(BUILDDIR); cd $(BUILDDIR); cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_INSTALL_PREFIX=./linux $(CURDIR); make $(JOB_FLAG); make install

release:
	$(Q)mkdir -p $(BUILDDIR); cd $(BUILDDIR); cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_INSTALL_PREFIX=./linux -DCMAKE_BUILD_TYPE=Release $(CURDIR); make $(JOB_FLAG); make install

clean:
	$(Q)rm -rf $(BUILDDIR)

clean-local-config:
	$(Q)rm -rf $(LOCAL_CONFIG_DIR)

edit-local-config:
	$(Q)$(EDITOR) $(LOCAL_CONFIG_DIR)/shapetool/shapetool.vars

server:
	$(Q)cd $(BUILDDIR); make $@ $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); ./$@ $(ARGS)

client:
	$(Q)cd $(BUILDDIR); make $@ $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); ./$@ $(ARGS)

debugclient:
	$(Q)cd $(BUILDDIR); make client $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); gdb --args ./client $(ARGS)

shapetool:
	$(Q)cd $(BUILDDIR); make $@ $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); ./$@ -set voxel-plainterrain false $(ARGS)

run: shapetool

runfast:
	$(Q)cd $(BUILDDIR); make shapetool $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); ./shapetool -set voxel-plainterrain true $(ARGS)

tests:
	$(Q)cd $(BUILDDIR); make $@ $(JOB_FLAG)
	$(Q)cd $(BUILDDIR); ./$@ -- $(ARGS)

remotery:
	$(Q)xdg-open file://$(CURDIR)/tools/remotery/index.html

.PHONY: tags
tags:
	$(Q)ctags -R src

shapetool2: clean-local-config
	$(Q)cd $(BUILDDIR); make $(JOB_FLAG) shapetool copy-data-shapetool copy-data-shared && ./shapetool -set cl_debug_geometry false

material-color: build
	$(Q)cd $(BUILDDIR); ./tests -- --gtest_filter=MaterialTest* $(ARGS)
	$(Q)xdg-open build/material.png

test-ambient-occlusion: build
	$(Q)cd $(BUILDDIR); ./tests -- --gtest_filter=AmbientOcclusionTest* $(ARGS)
