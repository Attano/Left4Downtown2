# (C)2004-2008 SourceMod Development Team
# Makefile written by David "BAILOPAN" Anderson

SMSDK ?= ../../../sourcemod
SRCDS_BASE ?= ../../../srcds
HL2SDK_L4D2 ?= ../../../hl2sdk
MMSOURCE ?= ../../../mmsource

#####################################
### EDIT BELOW FOR OTHER PROJECTS ###
#####################################

PROJECT = left4downtown

OBJECTS = smsdk_ext.cpp extension.cpp natives.cpp vglobals.cpp l4d2sdk/l4d2calls.cpp util.cpp asm/asm.c \
			detours/detour.cpp detours/spawn_tank.cpp detours/spawn_witch.cpp detours/clear_team_scores.cpp \
			detours/set_campaign_scores.cpp detours/first_survivor_left_safe_area.cpp \
			detours/mob_rush_start.cpp detours/spawn_it_mob.cpp detours/spawn_mob.cpp detours/try_offering_tank_bot.cpp \
			detours/get_script_value_int.cpp detours/get_script_value_float.cpp detours/shoved_by_survivor.cpp detours/get_crouch_top_speed.cpp \
			detours/get_run_top_speed.cpp detours/get_walk_top_speed.cpp detours/has_configurable_difficulty.cpp \
			detours/get_survivor_set.cpp detours/fast_get_survivor_set.cpp codepatch/patchmanager.cpp \
			detours/get_mission_versus_boss_spawning.cpp detours/cthrow_activate_ability.cpp l4d2sdk/l4d2timers.cpp \
			timernatives.cpp detours/start_melee_swing.cpp weaponnatives.cpp meleeweaponnatives.cpp directornatives.cpp \
			detours/send_in_rescue_vehicle.cpp detours/change_finale_stage.cpp detours/get_script_value_string.cpp \
			detours/end_versus_mode_round.cpp detours/select_weighted_sequence.cpp detours/spawn_special.cpp \
			detours/spawn_witchbride.cpp detours/on_revived.cpp detours/use_healing_items.cpp detours/find_scavenge_item.cpp \
			detours/water_move.cpp detours/on_stagger.cpp detours/terror_weapon_hit.cpp detours/get_mission_info.cpp \
			detours/replace_tank.cpp detours/choose_victim.cpp \
			addons_disabler.cpp

ifeq "$(USE_PLAYERSLOTS)" "true"
	OBJECTS += player_slots.cpp detours/server_player_counts.cpp
endif

##############################################
### CONFIGURE ANY OTHER FLAGS/OPTIONS HERE ###
##############################################

C_OPT_FLAGS = -DNDEBUG -O3 -funroll-loops -pipe -fno-strict-aliasing
C_DEBUG_FLAGS = -D_DEBUG -DDEBUG -g -ggdb3
C_GCC4_FLAGS = -fvisibility=hidden
CPP_GCC4_FLAGS = -fvisibility-inlines-hidden
CPP = gcc

HL2PUB = $(HL2SDK_L4D2)/public
HL2LIB = $(HL2SDK_L4D2)/lib/linux
CFLAGS += -DSOURCE_ENGINE=9
METAMOD = $(MMSOURCE)/core
INCLUDE += -I$(HL2SDK_L4D2)/public/game/server -I$(HL2SDK_L4D2)/common -I$(HL2SDK_L4D2)/game/shared
SRCDS = $(SRCDS_BASE)/left4dead2
GAMEFIX = 2.l4d2

LINK += $(HL2LIB)/tier1_i486.a $(HL2LIB)/mathlib_i486.a libvstdlib_srv.so libtier0_srv.so

INCLUDE += -I. -I.. -Isdk -I$(HL2PUB) -I$(HL2PUB)/engine -I$(HL2PUB)/mathlib -I$(HL2PUB)/tier0 \
        -I$(HL2PUB)/tier1 -I$(METAMOD) -I$(METAMOD)/sourcehook -I$(SMSDK)/public -I$(SMSDK)/public/extensions \
        -I$(SMSDK)/sourcepawn/include

CFLAGS += -DSE_EPISODEONE=1 -DSE_DARKMESSIAH=2 -DSE_ORANGEBOX=3 -DSE_BLOODYGOODTIME=4 -DSE_EYE=5 \
	-DSE_CSS=6 -DSE_ORANGEBOXVALVE=7 -DSE_LEFT4DEAD=8 -DSE_LEFT4DEAD2=9 -DSE_ALIENSWARM=10 \
	-DSE_PORTAL2=11 -DSE_CSGO=12

LINK += -m32 -ldl -lm

CFLAGS += -D_LINUX -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp \
        -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -Wall -Werror -Wno-switch \
        -Wno-unused -mfpmath=sse -msse -DSOURCEMOD_BUILD -DHAVE_STDINT_H -m32 -DGNUC

CPPFLAGS += -Wno-non-virtual-dtor -fno-exceptions -fno-rtti -fno-threadsafe-statics -Wno-overloaded-virtual -std=c++11

################################################
### DO NOT EDIT BELOW HERE FOR MOST PROJECTS ###
################################################

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS += $(C_DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS += $(C_OPT_FLAGS)
endif

PLAYERSLOTS_BIN_DIR := $(BIN_DIR).playerslots

ifeq "$(USE_PLAYERSLOTS)" "true"
	CFLAGS += -DUSE_PLAYERSLOTS_PATCHES
	BIN_DIR := $(PLAYERSLOTS_BIN_DIR)
endif

OS := $(shell uname -s)
ifeq "$(OS)" "Darwin"
	LINK += -dynamiclib
	BINARY = $(PROJECT).ext.$(GAMEFIX).dylib
else
	LINK += -static-libgcc -shared
	BINARY = $(PROJECT).ext.$(GAMEFIX).so
endif

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)
ifeq "$(GCC_VERSION)" "4"
	CFLAGS += $(C_GCC4_FLAGS)
	CPPFLAGS += $(CPP_GCC4_FLAGS)
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)
	ln -sf $(SMSDK)/public/smsdk_ext.cpp
	mkdir -p $(BIN_DIR)/detours
	mkdir -p $(BIN_DIR)/codepatch
	mkdir -p $(BIN_DIR)/l4d2sdk
	cp $(SRCDS)/bin/libvstdlib_srv.so libvstdlib_srv.so;
	cp $(SRCDS)/bin/libtier0_srv.so libtier0_srv.so;
	$(MAKE) -f Makefile extension

playerslots:
	mkdir -p $(BIN_DIR)
	ln -sf $(SMSDK)/public/smsdk_ext.cpp
	mkdir -p $(PLAYERSLOTS_BIN_DIR)/detours
	mkdir -p $(PLAYERSLOTS_BIN_DIR)/codepatch
	mkdir -p $(PLAYERSLOTS_BIN_DIR)/l4d2sdk
	cp $(SRCDS)/bin/libvstdlib_srv.so libvstdlib_srv.so;
	cp $(SRCDS)/bin/libtier0_srv.so libtier0_srv.so;
	$(MAKE) -f Makefile extension USE_PLAYERSLOTS=true DEBUG=$(DEBUG)

extension: $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(OBJ_LINUX) $(LINK) -o $(BIN_DIR)/$(BINARY)

debug:
	$(MAKE) -f Makefile all DEBUG=true

playerslots-debug:
	$(MAKE) -f Makefile playerslots DEBUG=true

default: all

clean:
	rm -rf Debug/ Debug.playerslots/ Release/ Release.playerslots/
