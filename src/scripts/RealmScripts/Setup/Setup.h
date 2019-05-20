/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"
#include "Setup/EasyFunctions.h"
#include "Setup/Instance_Base.h"
#include "Setup/Base.h"

#ifndef _REALM_SCRIPTS_SETUP_H
#define _REALM_SCRIPTS_SETUP_H
//World PvP Scripts
void SetupPvPTerokkarForest(ScriptMgr* mgr);
void SetupPvPHellfirePeninsula(ScriptMgr* mgr);
void SetupPvPSilithus(ScriptMgr* mgr);
void SetupPvPZangarmarsh(ScriptMgr* mgr);

// Event Scripts
void SetupBrewfest(ScriptMgr* mgr);
void SetupDarkmoonFaire(ScriptMgr* mgr);
void SetupDayOfTheDead(ScriptMgr* mgr);
void SetupL70ETC(ScriptMgr* mgr);
void SetupWinterVeil(ScriptMgr* mgr);
void SetupHallowsEnd(ScriptMgr* mgr);
void SetupVayrieTest(ScriptMgr* mgr);

//Gossip Scripts
void SetupDalaranGossip(ScriptMgr* mgr);
void SetupInnkeepers(ScriptMgr* mgr);
void SetupGuardGossip(ScriptMgr* mgr);
void SetupTrainerScript(ScriptMgr* mgr);
void SetupMulgoreGossip(ScriptMgr* mgr);
void SetupShattrathGossip(ScriptMgr* mgr);
void SetupTanarisGossip(ScriptMgr* mgr);
void SetupMoongladeGossip(ScriptMgr* mgr);
void SetupStormwindGossip(ScriptMgr* mgr);
void SetupTheramoreGossip(ScriptMgr* mgr);
void SetupTeldrassilGossip(ScriptMgr* mgr);
void SetupBoreanTundraGossip(ScriptMgr* mgr);
void SetupWyrmrestTempleGossip(ScriptMgr* mgr);
void SetupXpEliminatorGossip(ScriptMgr* mgr);

//Misc Scripts
void SetupGoHandlers(ScriptMgr* mgr);
void SetupQDGoHandlers(ScriptMgr* mgr);
void SetupRandomScripts(ScriptMgr* mgr);
void InitializeGameObjectTeleportTable(ScriptMgr* mgr);

//Quest Scripts
void SetupDruid(ScriptMgr* mgr);
void SetupMage(ScriptMgr* mgr);
void SetupPaladin(ScriptMgr* mgr);
void SetupWarrior(ScriptMgr* mgr);
void SetupDeathKnight(ScriptMgr* mgr);
void SetupFirstAid(ScriptMgr* mgr);
void SetupArathiHighlands(ScriptMgr* mgr);
void SetupAzshara(ScriptMgr* mgr);
void SetupAzuremystIsle(ScriptMgr* mgr);
void SetupBladeEdgeMountains(ScriptMgr* mgr);
void SetupBlastedLands(ScriptMgr* mgr);
void SetupBloodmystIsle(ScriptMgr* mgr);
void SetupBurningSteppes(ScriptMgr* mgr);
//void SetupDarkshore(ScriptMgr * mgr)
void SetupDeathKnight(ScriptMgr* mgr);
void SetupDesolace(ScriptMgr* mgr);
void SetupDragonblight(ScriptMgr* mgr);
void SetupDuskwood(ScriptMgr* mgr);
void SetupDustwallowMarsh(ScriptMgr* mgr);
void SetupEasternPlaguelands(ScriptMgr* mgr);
void SetupEversongWoods(ScriptMgr* mgr);
void SetupGhostlands(ScriptMgr* mgr);
void SetupHellfirePeninsula(ScriptMgr* mgr);
void SetupHillsbradFoothills(ScriptMgr* mgr);
void SetupHowlingFjord(ScriptMgr* mgr);
void SetupIsleOfQuelDanas(ScriptMgr* mgr);
void SetupLochModan(ScriptMgr* mgr);
void SetupMulgore(ScriptMgr* mgr);
void SetupNagrand(ScriptMgr* mgr);
void SetupNetherstorm(ScriptMgr* mgr);
void SetupRedrigeMountains(ScriptMgr* mgr);
void SetupShadowmoon(ScriptMgr* mgr);
void SetupSilithus(ScriptMgr* mgr);
void SetupSilvermoonCity(ScriptMgr* mgr);
void SetupSilverpineForest(ScriptMgr* mgr);
void SetupStonetalonMountains(ScriptMgr* mgr);
void SetupStormwind(ScriptMgr* mgr);
void SetupStranglethornVale(ScriptMgr* mgr);
void SetupTanaris(ScriptMgr* mgr);
void SetupTeldrassil(ScriptMgr* mgr);
void SetupTerrokarForest(ScriptMgr* mgr);
void SetupTheStormPeaks(ScriptMgr* mgr);
void SetupThousandNeedles(ScriptMgr* mgr);
void SetupTirisfalGlades(ScriptMgr* mgr);
void SetupUndercity(ScriptMgr* mgr);
void SetupUnGoro(ScriptMgr* mgr);
void SetupWestfall(ScriptMgr* mgr);
void SetupWetlands(ScriptMgr *mgr);
void SetupWinterspring(ScriptMgr* mgr);
void SetupZangarmarsh(ScriptMgr* mgr);
void SetupBarrens(ScriptMgr* mgr);
void SetupBoreanTundra(ScriptMgr* mgr);
void SetupSholazarBasin(ScriptMgr* mgr);
void SetupQuestGossip(ScriptMgr* mgr);
void SetupQuestHooks(ScriptMgr* mgr);
void SetupUnsorted(ScriptMgr* mgr);

//Instances
void SetupTheVioletHold(ScriptMgr* mgr);
void SetupCullingOfStratholme(ScriptMgr* mgr);
void SetupDrakTharonKeep(ScriptMgr* mgr);
void SetupAzjolNerub(ScriptMgr* mgr);

//Raids
void SetupICC(ScriptMgr* mgr);

struct ScriptSpell
{
    uint32 normal_spellid;
    uint32 heroic_spellid;
    uint32 timer;
    uint32 time;
    uint32 chance;
    uint32 target;
    uint32 phase;
};

enum SPELL_TARGETS
{
    SPELL_TARGET_SELF,
    SPELL_TARGET_CURRENT_ENEMY,
    SPELL_TARGET_RANDOM_PLAYER,
    SPELL_TARGET_SUMMONER,
    SPELL_TARGET_RANDOM_PLAYER_POSITION,
    SPELL_TARGET_GENERATE,                  // this will send null as target
    SPELL_TARGET_LOWEST_THREAT,
    SPELL_TARGET_CUSTOM
};

struct SP_AI_Spell
{
    SP_AI_Spell();
    SpellEntry* info;       // spell info
    char targettype;        // 0-self , 1-attaking target, ....
    bool instant;           // does it is instant or not?
    float perctrigger;      // % of the cast of this spell in a total of 100% of the attacks
    int attackstoptimer;    // stop the creature from attacking
    int soundid;            // sound id from DBC
    std::string speech;     // text displaied when spell was casted
    uint32 cooldown;        // spell cooldown
    uint32 casttime;        // "time" left to cast spell
    uint32 reqlvl;          // required level ? needed?
    float hpreqtocast;      // ? needed?
    float mindist2cast;     // min dist from caster to victim to perform cast (dist from caster >= mindist2cast)
    float maxdist2cast;     // max dist from caster to victim to perform cast (dist from caster <= maxdist2cast)
    int minhp2cast;         // min hp amount of victim to perform cast on it (health >= minhp2cast)
    int maxhp2cast;         // max hp amount of victim to perform cast on it (health <= maxhp2cast)
};

enum
{
    TARGET_SELF,
    TARGET_VARIOUS,
    TARGET_ATTACKING,
    TARGET_DESTINATION,
    TARGET_SOURCE,
    TARGET_RANDOM_FRIEND,    // doesn't work yet
    TARGET_RANDOM_SINGLE,
    TARGET_RANDOM_DESTINATION
};

///\todo create for all instance scripts for these maps... best example how encounter states work ->Raid_IceCrownCitadel.cpp
enum InstanceMaps
{
    MAP_SHADOWFANG_KEEP     = 33,   //Shadowfang Keep
    MAP_THE_STOCKADE        = 34,   //Stormwind Stockade
    MAP_DEADMINES           = 36,   //Deadmines
    MAP_WAILING_CAVERNS     = 43,   //Wailing Caverns
    MAP_BLACKFATHOM_DEEPS   = 48,   //Blackfathom Deeps
    MAP_ULDAMAN             = 70,   //Uldaman

    MAP_RAZORFEN_DOWNS      = 129,  //Razorfen Downs
    MAP_SCARLET_MONASTERY   = 189,  //Scarlet Monastery

    MAP_ZUL_FARAK           = 209,  //Zul'Farrak
    MAP_BLACKROCK_SPIRE     = 229,  //Blackrock Spire
    MAP_BLACKROCK_DEPTHS    = 230,  //Blackrock Depths
    MAP_COT_BLACK_MORASS    = 269,  // rename this Opening of the Dark Portal -> Caverns of Time: Black Morass
    MAP_SCHOLOMANCE         = 289,  //Scholomance
    MAP_MARAUDON            = 349,  //Maraudon

    MAP_RAGEFIRE_CHASM      = 389,  //Ragefire Chasm

    MAP_DIRE_MAUL           = 429,  //Dire Maul
    MAP_NAXXRAMAS           = 533,  //Naxxramas
    MAP_HYJALPAST           = 534,  //The Battle for Mount Hyjal

    MAP_HC_SHATTERED_HALLS  = 540,  //Hellfire Citadel: The Shattered Halls
    MAP_HC_BLOOD_FURNANCE   = 542,  //Hellfire Citadel: The Blood Furnace
    MAP_HC_RAMPARTS         = 543,  //Hellfire Citadel: Ramparts

    MAP_CF_STEAMVAULT       = 545,  //Coilfang: The Steamvault
    MAP_CF_THE_UNDERBOG     = 546,  //Coilfang: The Underbog
    MAP_CF_SLAVE_PENS       = 547,  //Coilfang: The Slave Pens
    MAP_CF_SERPENTSHRINE_CA = 548,  //Coilfang: Serpentshrine Cavern

    MAP_TK_THE_ARCATRAZ     = 552,  //Tempest Keep: The Arcatraz
    MAP_TK_THE_BOTANICA     = 553,  //Tempest Keep: The Botanica
    MAP_TK_THE_MECHANAR     = 554,  //Tempest Keep: The Mechanar

    MAP_AUCHENAI_SHADOWLAB  = 555,  //Auchindoun: Shadow Labyrinth
    MAP_AUCHENAI_SETHEKK    = 556,  //Auchindoun: Sethekk Halls
    MAP_AUCHENAI_MANA_TOMBS = 557,  //Auchindoun: Mana-Tombs
    MAP_AUCHENAI_CRYPT      = 558,  //Auchindoun: Auchenai Crypts

    MAP_BLACK_TEMPLE        = 564,  //Black Temple
    MAP_NEXUS               = 576,  //Nexus
    MAP_MAGISTERS_TERRACE   = 585,  //Magister's Terrace
    MAP_HALLS_OF_STONE      = 599,  //Halls of Stone
    MAP_DRAK_THARON_KEEP    = 600,  //Drak'Tharon Keep
    MAP_AZJOL_NERUB         = 601,  //Azjol-Nerub
    MAP_HALLS_OF_LIGHTNING  = 602,  //Halls of Lightning
    MAP_GUNDRAK             = 604,  //Gundrak
    MAP_VIOLET_HOLD         = 608,  //The Violet Hold
    MAP_OS                  = 615,  //ObsidianSanctum
    MAP_ICECROWNCITADEL     = 631,  //Icecrown Citadel
    MAP_HALLSOFREFLECTION   = 668,  //Halls of Reflection

    /*
    30	Alterac Valley
    44	<unused> Monastery
    47	Razorfen Kraul
    90	Gnomeregan

    109	Sunken Temple
    169	Emerald Dream

    249	Onyxia's Lair

    309	Zul'Gurub
    329	Stratholme

    409	Molten Core
    469	Blackwing Lair
    489	Warsong Gulch

    509	Ruins of Ahn'Qiraj
    529	Arathi Basin
    531	Ahn'Qiraj Temple
    532	Karazhan
    544	Magtheridon's Lair
    548	Coilfang: Serpentshrine Cavern
    550	Tempest Keep
    552	Tempest Keep: The Arcatraz
    559	Nagrand Arena
    560	The Escape From Durnholde
    562	Blade's Edge Arena
    565	Gruul's Lair
    566	Eye of the Storm
    568	Zul'Aman
    572	Ruins of Lordaeron
    574	Utgarde Keep
    575	Utgarde Pinnacle
    578	The Oculus
    580	The Sunwell
    595	The Culling of Stratholme
    598	Sunwell Fix (Unused)

    603	Ulduar
    607	Strand of the Ancients
    615	The Obsidian Sanctum
    616	The Eye of Eternity
    617	Dalaran Sewers
    618	The Ring of Valor
    619	Ahn'kahet: The Old Kingdom
    624	Vault of Archavon
    628	Isle of Conquest
    632	The Forge of Souls
    649	Trial of the Crusader
    650	Trial of the Champion
    658	Pit of Saron
    724	The Ruby Sanctum*/

};

#endif      // _REALM_SCRIPTS_SETUP_H
