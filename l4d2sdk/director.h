/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2010 Michael "ProdigySim" Busby
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef _INCLUDE_DIRECTOR_H_
#define _INCLUDE_DIRECTOR_H_
#include "l4d2timers.h"
#include "constants.h"

// 1128 bytes
typedef struct _CDirectorItemManager {
	char unknown[1128];
} CDirectorItemManager;

// 92 bytes
typedef struct _CDirectorMusicBanks {
	char unknown[92];
} CDirectorMusicBanks;

// 76 bytes
typedef struct _CDirectorSessionManager {
	char unknown[76];
} CDirectorSessionManager;

// 156 bytes
// new size 340 bytes
// INDIVIDUAL OFFSETS HAVE NOT BEEN UPDATED/VERIFIED FOR v2125
typedef struct _CDirectorScriptedEventManager {
	char unknown[156];
} CDirectorScriptedEventManager;

// 92 bytes
typedef struct _CDirectorVersusMode {
	void *vtableptr; // 0x00
	bool m_bVersusRoundInProgress; // 0x04
	bool m_bFirstMap; // 0x05 idk
	bool m_bTransitioning; // 0x06 still making shit up
	char padding; // 0x07 mite b bool
	int m_iCampaignScores[2]; // 0x08
	char unknown[12]; // 0x10
	CountdownTimer VersusStartTimer; // 0x1c Timer controlling saferoom door open, allowing PZ spawning.
	float m_fTankSpawnFlowPercent[2]; // 0x28 Tank spawn flow percent for rounds 1 and 2
	float m_fWitchSpawnFlowPercent[2]; // 0x30 Witch spawn flow percent for rounds 1 and 2
	bool m_bTankThisRound[2]; // 0x38 One for each round!
	bool m_bWitchThisRound[2]; // 0x3a One for each round!
	float m_fFinaleTravelCompletionValue; // 0x3c 5660 in CFinaleTrigger
	int m_iFinaleTriggerVar2; // 0x40 5656 in CFinaleTrigger
	bool m_bInFinaleMap; // 0x44
	int m_iNumMarkersReached; // 0x48
	int m_iMarkersCount; // 0x4c
	CountdownTimer UpdateMarkersTimer; // 0x50 Timer controlling when to check if survivors have passed markers. Normally set to 1s duration
} CDirectorVersusMode;

// 160 bytes
typedef struct _CDirectorSurvivalMode {
	char unknown[160];
} CDirectorSurvivalMode;

// 104 bytes
typedef struct _CDirectorScavengeMode {
	bool m_bUnknown; 
	char padding[3]; 
	float m_fUnknown; // 0x04
	bool m_bScavengeRoundInProgress; // 0x08
	bool m_bUnk2; // 0x09
	bool m_bAreTeamsSwapped; // 0x0a
	bool m_bInOvertime; // 0x0b
	bool m_bInOvertime2; // 0x0c
	char padding2[3];
	CountdownTimer RoundSetupTimer; // 0x10 (see: scavenge_round_setup_time)
	CountdownTimer OvertimeGraceTimer; // 0x1c (see: scavenge_overtime_grace_time)
	char unknown[64];
} CDirectorScavengeMode;

// 8 bytes
// new size 412 bytes
// INDIVIDUAL OFFSETS HAVE NOT BEEN UPDATED/VERIFIED FOR v2125
typedef struct _CDirectorChallengeMode {
	char unknown[8];
} CDirectorChallengeMode;

// 1 byte
typedef struct _CDirectorTacticalServices {
	char unknown; // does this even exist? blah.
} CDirectorTacticalServices;

enum DirectorTempoState
{
	TEMPO_BUILDUP=0,
	TEMPO_SUSTAIN_PEAK=1,
	TEMPO_PEAK_FADE=2,
	TEMPO_RELAX=3
};
enum ScenarioRestartReason
{
	RESTART_FINALE_WON=3,
	RESTART_VERSUS_SOMETHING1=5,
	RESTART_VERSUS_FROMVOTE=7,
	RESTART_VERSUS_SOMETHING2=8,
	RESTART_SCAVENGE_ROUND=9, // Halftime or round end
	RESTART_SCAVENGE_ROUND_TIE=10, // exact tie (cans+time
	RESTART_VERSUS_CHANGELEVEL,
	RESTART_SCAVENGE_MATCH_FINISHED=12,
	RESTART_SCAVENGE_SOMETHING3=13,
	RESTART_SURVIVAL_ROUND1=14,
	RESTART_SURVIVAL_ROUND2=16,
	RESTART_MISSION_ABORTED=18,
};

// Win32: 1420 bytes
// Lin: 1420 bytes
typedef struct _CDirector {
	void * vptr; // 0x0
	char unknown4[156]; // 0x04
	bool m_bHasSurvivorLeftSafeArea; // 160
	bool m_bUknown161; // 161 set to 0in CDirector::Reset()
	char padding162[2]; // 161
	int m_iFinaleEscapeState; // 164
	char unknown168[4]; // 168
	CountdownTimer TankProhibitionTimer; // 172 see director_tank_checkpoint_interval, director_tank_min/max_interval
	CountdownTimer TankLotteryEntryTimer; // 184
	CountdownTimer TankLotterySelectionTimer; // 196
	char unknown184[4]; // 208
	DirectorTempoState m_iTempoState; // 212
	CountdownTimer m_ChangeTempoTimer; // 216 Set to various values depending on current state, see CDirector::UpdateTempo()
	float m_flEndFadeFlowDistance; // 228 Highest survivor flow stored here on transition to PEAK_FADE, goes to to RELAX after director_relax_max_flow_travel distance made
	char unknown232[20]; // 232
	int m_iWitchCount; // 252
	int m_iTankCount; // 256
	float m_fTankFlowDistance; // 0x104
	char unknown264[8]; // 264
	CountdownTimer ZombieGhostSpawnTimer; // 272 Started with z_ghost_spawn_interval whenever SI dies during normal gameplay
	char unknown284[104]; // 284
	float m_fAvgSurvivorSpan; // 388
	float m_fAvgSurvivorSpeed; // 392
	float m_fFurthestSurvivorFlow; // 396
	char unknown400[24]; // 400
	CountdownTimer RestartScenarioTimer; // 424 Counts down to a scenario restart (UpdateScenarioState)
	CountdownTimer EndScenarioTimer; // 436 Counts down to a scenario end (UpdateScenarioState)
	bool m_bUnknown448;
	bool m_bUnknown449;
	bool m_bUnknown450;
	ScenarioRestartReason m_iScenarioExitReason; // 452 Set in EndScenario(), read in various
	char unknown456[8]; // 456
	bool m_bInIntro; // 464
	bool m_bUnknown465; // 465 set to 0 in CDirector::Reset()
	// should be padded
	CountdownTimer MobSpawnTimer; // 468
	CountdownTimer unknownCTimer480; // 480
	IntervalTimer m_ClassDeathTimers[7]; // 492 ZC_NONE=0 through ZC_CHARGER=6
	IntervalTimer unknownITimer548; // 548
	IntervalTimer unknownITimer556; // 556
	CountdownTimer SmokerSpawnTimer; // 0x234
	CountdownTimer BoomerSpawnTimer;
	CountdownTimer HunterSpawnTimer;
	CountdownTimer SpitterSpawnTimer;
	CountdownTimer JockeySpawnTimer;
	CountdownTimer ChargerSpawnTimer;
	//CUTLVECTORFIXEDGROWABLE(CountdownTimer,3) m_pzSpawnTimers; // 636
	// 3*12 + 4 + 20 = 60
	char m_pzSpawnTimersFake[60];
	char unknown696[12]; // 696
	float m_fMobSpawnSize; // 708
	char unknown712[4];
	float m_fMobSpawnInterval; // 716 I think that's what this is
	float m_fTankProhibitionInterval; // 720
	int m_iUnknown724; // 724 some kind of userid
	int m_iSelectedTankPlayerId; // 728
	int m_iNumReservedWanderers; // 732
	int m_iScavengeItemsRemaining; // 736 For scavenge mode/finales I assume
	char unknown740[4];
	IntervalTimer ElapsedMissionTimer; // 744 Should be timing playtime on this map
	float m_fCumulativeMissionTime; // 752 Add this to timer duration from above to get TotalElapsedMissionTime
	char m_sCurrentMap[0x20]; // 756
	CountdownTimer m_rescueCheckTimer; // 788 see rescue_interval cvar
	char unknown800[52];
	void * m_kvPopulationData; // 852 KeyValues *
	char unknown856[8];
	int m_iMapNumber; // 864, should be 0 indexed into current campaign
	int m_iSessionStartMapNumber; // 868
	char unknown872[24];
	int m_iMissionWipes; // 896 number of wipes on this mission (coop)
	ZombieClassType m_zThreatRoster[3]; // 900 Threat roster for coop
	int m_iNextThreatIdx; // 912 Which threat will be used next?
	char unknown916[20];
	CountdownTimer unknownCTimer936;
	char unknown948[96]; // "forbidden targets" starts here 
	CountdownTimer unknownCTimer1044;
	float m_flUnknown1056; // 1056
	bool m_bUnknown1160;
	bool m_bIsFirstRoundFinished; // 0x425 Fuck naming this var
	bool m_bIsSecondRoundFinished; // This one too
	bool m_bUnknownJunk; // probably related
	CountdownTimer TransitionTimeoutTimer; // 0x428
	char unknown1076[308]; // 0x434
	CDirectorTacticalServices * TacticalServicesPtr;
	CDirectorItemManager * ItemManagerPtr; // 0x56c
	CDirectorMusicBanks * MusicBanksPtr; 
	CDirectorSessionManager * SessionManagerPtr; 
	CDirectorScriptedEventManager * ScriptedEventManagerPtr; 
	CDirectorVersusMode * VersusModePtr; 
	CDirectorSurvivalMode * SurvivalModePtr;
	CDirectorScavengeMode * ScavengeModePtr; 
	CDirectorChallengeMode * ChallengeModePtr;
} CDirector;

#endif //_INCLUDE_DIRECTOR_H_
