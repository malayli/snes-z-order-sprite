ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

# include ./snes_rules
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := zorder

TARGET := ZORDER
GTITLE := -ht"$(TARGET)"

all: $(ROMNAME).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(ROMNAME).sfc
	cp $(ROMNAME).sfc $(ROMNAME)_ntsc.sfc
	$(SNTOOLS) -hi! -hS! $(GTITLE) -hc03 $(ROMNAME).sfc
	mv $(ROMNAME).sfc $(ROMNAME)_pal.sfc

cleanRom:
	@echo clean rom
	@rm -f $(ROMNAME).sfc $(ROMNAME).sym $(ROMNAME)_ntsc.sfc $(ROMNAME)_pal.sfc

clean: cleanBuildRes cleanRom cleanGfx cleanAudio
