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
#pragma warning( disable : 4018)

#include "Setup/Setup.h"
#define SKIP_ALLOCATOR_SHARING 1
#include <ScriptSetup.h>

extern "C" SCRIPT_DECL uint32 _exp_get_script_type()
{
    return SCRIPT_TYPE_MISC;
}

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)    // Comment any script to disable it
{
    //World PvP Scripts
    SetupPvPTerokkarForest(mgr);
    SetupPvPHellfirePeninsula(mgr);
    SetupPvPSilithus(mgr);
    SetupPvPZangarmarsh(mgr);

    //Event Scripts
    SetupBrewfest(mgr);
    SetupDarkmoonFaire(mgr);
    SetupDayOfTheDead(mgr);
    SetupL70ETC(mgr);
    SetupWinterVeil(mgr);
    SetupHallowsEnd(mgr);
    SetupVayrieTest(mgr);

    //Gossip Scripts
    SetupDalaranGossip(mgr);
    SetupInnkeepers(mgr);
    SetupGuardGossip(mgr);
    SetupTrainerScript(mgr);
    SetupMulgoreGossip(mgr);
    SetupShattrathGossip(mgr);
    SetupTanarisGossip(mgr);
    SetupMoongladeGossip(mgr);
    SetupStormwindGossip(mgr);
    SetupTheramoreGossip(mgr);
    SetupTeldrassilGossip(mgr);
    SetupBoreanTundraGossip(mgr);
    SetupWyrmrestTempleGossip(mgr);
    SetupXpEliminatorGossip(mgr);

    // Misc Scripts
    SetupGoHandlers(mgr);
    SetupRandomScripts(mgr);
    // Sets up gossip scripts for gameobjects in the (optional)
    // gameobject_teleports table. If the table doesn't exist the
    // initialization will quietly fail.
    InitializeGameObjectTeleportTable(mgr);

    // Quest Scripts
    SetupDruid(mgr);
    SetupMage(mgr);
    SetupPaladin(mgr);
    SetupWarrior(mgr);
    SetupDeathKnight(mgr);
    SetupFirstAid(mgr);
    SetupArathiHighlands(mgr);
    SetupAzshara(mgr);
    SetupAzuremystIsle(mgr);
    SetupBladeEdgeMountains(mgr);
    SetupBlastedLands(mgr);
    SetupBloodmystIsle(mgr);
    SetupBurningSteppes(mgr);
    //SetupDarkshore(mgr);
    SetupDeathKnight(mgr);
    SetupDesolace(mgr);
    SetupDragonblight(mgr);
    SetupDuskwood(mgr);
    SetupDustwallowMarsh(mgr);
    SetupEasternPlaguelands(mgr);
    SetupEversongWoods(mgr);
    SetupGhostlands(mgr);
    SetupHellfirePeninsula(mgr);
    SetupHillsbradFoothills(mgr);
    SetupHowlingFjord(mgr);
    SetupIsleOfQuelDanas(mgr);
    SetupLochModan(mgr);
    SetupMulgore(mgr);
    SetupNagrand(mgr);
    SetupNetherstorm(mgr);
    SetupRedrigeMountains(mgr);
    SetupShadowmoon(mgr);
    SetupSilithus(mgr);
    SetupSilvermoonCity(mgr);
    SetupSilverpineForest(mgr);
    SetupStonetalonMountains(mgr);
    SetupStormwind(mgr);
    SetupStranglethornVale(mgr);
    SetupTanaris(mgr);
    SetupTeldrassil(mgr);
    SetupTerrokarForest(mgr);
    SetupTheStormPeaks(mgr);
    SetupThousandNeedles(mgr);
    SetupTirisfalGlades(mgr);
    SetupUndercity(mgr);
    SetupUnGoro(mgr);
    SetupWestfall(mgr);
    SetupWetlands(mgr);
    SetupWinterspring(mgr);
    SetupZangarmarsh(mgr);
    SetupBarrens(mgr);
    SetupBoreanTundra(mgr);
    SetupSholazarBasin(mgr);
    SetupQuestGossip(mgr);
    SetupQuestHooks(mgr);
    SetupUnsorted(mgr);

    //Instances
    SetupTheVioletHold(mgr);
    SetupCullingOfStratholme(mgr);
    SetupDrakTharonKeep(mgr);
    SetupAzjolNerub(mgr);

    //Raids
    SetupICC(mgr);
}

#ifdef WIN32

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

#endif

SP_AI_Spell::SP_AI_Spell()
{
    info                = NULL;
    targettype          = 0;
    instant             = false;
    perctrigger         = 0.0f;
    attackstoptimer     = 0;
    soundid             = 0;
    cooldown            = 0;
    casttime            = 0;
    reqlvl              = 0;
    hpreqtocast         = 0.0f;
    mindist2cast        = 0.0f;
    maxdist2cast        = 0.0f;
    minhp2cast          = 0;
    maxhp2cast          = 0;
}