/*
* Copyright (C) 2015-2019 FrostwingCore Team <http://www.ArcEmu.org/>
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

#include "Setup/Setup.h"
#include "Dungeon_TheVioletHold.h"

enum WorldStateIds
{
	WORLD_STATE_VH_SHOW = 3816,
	WORLD_STATE_VH_PRISON_STATE = 3815,
	WORLD_STATE_VH_WAVE_COUNT = 3810,
};


struct Location EventPreGuardSP[] = //PortalGuard spwns
{
    { 1888.046265f, 761.654053f, 47.667f, 2.2332f }, // [0] left
    { 1928.545532f, 803.849731f, 52.411f, 3.1223f }, // [1] center
    { 1878.080933f, 844.850281f, 43.334f, 4.2376f }  // [2] right
};

struct Location EventPreGuardWP[] = //PortalGuard WPs
{
    { 1858.386353f, 812.804993f, 42.9995f, 4.2376f }, // [0] left
    { 1861.916382f, 803.873230f, 43.6728f, 3.1223f }, // [1] center
    { 1858.678101f, 796.081970f, 43.1944f, 2.2332f }  // [2] right
};

enum DataIndex
{
    TVH_PHASE_1 = 0, // main event
    TVH_PHASE_2 = 1, // 1. portal (0/18)
    TVH_PHASE_3 = 2, // 2. portal (1/18)
    TVH_PHASE_4 = 3, // 3. portal (2/18)
    TVH_PHASE_5 = 4, // 4. portal (3/18)
    TVH_PHASE_6 = 5, // 5. portal (4/18)
    TVH_PHASE_DONE = 6, // 6. portal (5/18)

	PORTAL_NUMBER = 7,

    TVH_END = 8
};

enum TVH_ENTRIES
{
    //Map stuff
    AREA_VIOLET_HOLD = 4415,

    //Main event
    CN_LIEUTNANT_SINCLARI = 30658,
    CN_VIOLET_HOLD_GUARD = 30659,
    CN_PORTAL_GUARDIAN = 30660, //enemies
    CN_PORTAL_INTRO = 30679, //portals, not a go its a creature ;)
    CN_CRYSTAL_SYSTEM = 30837, // NPC with spell arcane spher

    //Portal Guardians (Normal)
    CN_AZURE_INVADER = 30661,
    CN_AZURE_SPELLBREAKER = 30662,
    CN_AZURE_BINDER = 30663,
    CN_AZURE_MAGE_SLAYER = 30664,
    CN_AZURE_CAPTAIN = 30666,
    CN_AZURE_SORCEROR = 30667,
    CN_AZURE_RAIDER = 30668,
    CN_AZURE_STALKER = 32191,

    //Bosses
    CN_EREKEM = 29315,
    CN_MORAGG = 29316,
    CN_ICHORON = 29313,
    CN_XEVOZZ = 29266,
    CN_LAVANTHOR = 29312,
    CN_TURAMAT_THE_OBLITERATOR = 29314,
    CN_CYANIGOSA = 31134,

    //Spell Crytals
    SPELL_ARCANE_LIGHTNING = 57930,

    //Crystals
    GO_INTRO_ACTIVATION_CRYSTAL = 193615,
    GO_ACTIVATION_CRYSTAL = 193611,

    //Door
    GO_PRISON_SEAL = 191723,
    GO_XEVOZZ_DOOR = 191556,
    GO_LAVANTHOR_DOOR = 191566,
    GO_ICHORON_DOOR = 191722,
    GO_ZURAMAT_THE_OBLITERATOR_DOOR = 191565,
    GO_EREKEM_DOOR = 191564,
    GO_MORAGG_DOOR = 191606
};

MapMgr* mInstance;

///////////////////////////////////////////////////////
//TheVioletHold Instance
class TheVioletHoldScript : public MoonInstanceScript
{
    friend class SinclariGossip; // Friendship forever ;-)
	friend class triggervh1;

    private:

        int32 m_numPortal;
        uint32 m_phaseData[TVH_END];

    public:

        MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(TheVioletHoldScript, MoonInstanceScript);
        TheVioletHoldScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
        {
			m_numPortal = 0;

            for (uint8 i = 0; i < TVH_END; ++i)
                m_phaseData[i] = State_NotStarted;
        };

		void StartSetupWS()
		{
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH, State_InProgress);
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_WAVE_COUNT, 1);
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_PRISON_STATE, 100);
		}

		void SetPortalNumber(uint32 portalnumber)
		{
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_WAVE_COUNT, portalnumber);
		}

		void SetPrisonSealIntegrity(uint32 value)
		{
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_PRISON_STATE, value);
		}

		void InstanceFinished()
		{
			mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH, 0);
		}

        void SetData(uint32 pIndex, uint32 pData)
        {
            if (pIndex >= TVH_END)
                return;

            m_phaseData[pIndex] = pData;
        };

        uint32 GetData(uint32 pIndex)
        {
            // If Phase = End/finishes, reset the Phases to 0
            if (pIndex >= TVH_END)
                return 0;

            return m_phaseData[pIndex];
        };

        void SetInstanceData(uint32 pType, uint32 pIndex, uint32 pData)
        {
            if (pType != Data_EncounterState || pIndex == 0)
                return;

            EncounterMap::iterator Iter = mEncounters.find(pIndex);
            if (Iter == mEncounters.end())
                return;

            (*Iter).second.mState = (EncounterState)pData;
        };

        uint32 GetInstanceData(uint32 pType, uint32 pIndex)
        {
            if (pType != Data_EncounterState || pIndex == 0)
                return 0;

            EncounterMap::iterator Iter = mEncounters.find(pIndex);
            if (Iter == mEncounters.end())
                return 0;

            return (*Iter).second.mState;
        };

        void OnGameObjectActivate(GameObject* pGameObject, Player* pPlayer)
        {};

        void OnPlayerEnter(Player* pPlayer)
        {
            TheVioletHoldScript* pInstance = (TheVioletHoldScript*)pPlayer->GetMapMgr()->GetScript();
            if (!pInstance)
                return;

			pInstance->SetCellForcedStates(225, 230, 241, 247); // Force inactive cells due to fucked up maps.

			Creature *spawner = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 43283);

			if (spawner)
			{
				mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH, State_InProgress);
				mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_WAVE_COUNT, spawner->m_portalNumber);
				mInstance->GetWorldStatesHandler().SetWorldStateForZone(4415, 0, WORLDSTATE_VH_PRISON_STATE, 100);
			}

			// Needs world state handling if you die & renenter.

			if (pInstance->GetInstanceData(Data_EncounterState, MAP_VIOLET_HOLD) == State_NotStarted)
			{
				mEncounters.insert(EncounterMap::value_type(MAP_VIOLET_HOLD, State_NotStarted));
			}
        }
};

#define SINCLARI_SAY_1 "Prison guards, we are leaving! These adventurers are taking over! Go go go!"
#define SINCLARY_SAY_2 "I'm locking the door. Good luck, and thank you for doing this."

#define GO_TVH_PRISON_SEAL 191723

#define SINCLARI_MAX_WP 4

uint32 WaveCount = 0;

///////////////////////////////////////////////////////
//Lieutnant Sinclari StartEvent
class SinclariAI : public MoonScriptCreatureAI
{
    public:

        MOONSCRIPT_FACTORY_FUNCTION(SinclariAI, MoonScriptCreatureAI);
        SinclariAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            _unit->GetAIInterface()->setMoveType(MOVEMENTTYPE_WANTEDWP);
        }

        void OnReachWP(uint32 iWaypointId, bool bForwards)
        {
            switch (iWaypointId)
            {
                case 2:
                {
					TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
                    _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, SINCLARI_SAY_1);
                    _unit->GetAIInterface()->setMoveType(MOVEMENTTYPE_FORWARDTHENSTOP);
					pInstance->SetData(TVH_PHASE_1, 1);
                }
                break;

                case 4:
                {
					SpawnCreature(43283, 1883.76f, 804.73f, 38.39f, 3.2f);
                    _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, SINCLARY_SAY_2);
                    _unit->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
                    pInstance->SetInstanceData(Data_EncounterState, MAP_VIOLET_HOLD, State_InProgress);
					pInstance->SetData(PORTAL_NUMBER, 1);

                    GameObject* pVioletHoldDoor = pInstance->FindClosestGameObjectOnMap(GO_TVH_PRISON_SEAL, 1822.59f, 803.93f, 44.36f);
                    if (pVioletHoldDoor != NULL)
                        pVioletHoldDoor->SetState(GAMEOBJECT_STATE_CLOSED);
                }
                break;
            }
        }
};

enum eGossipTexts
{
    SINCLARI_ON_HELLO = 13853,
    SINCLARI_ON_FINISH = 13854,
    SINCLARI_OUTSIDE = 14271
};

enum GossipItem
{
    SINCLARI_ACTIVATE = 600,
    SINCLARI_GET_SAFETY = 601,
    SINCLARI_SEND_ME_IN = 602
};


///////////////////////////////////////////////////////
//Lieutnant Sinclari Gossip and init events
//Sinclari Gossip
class SinclariGossip : public GossipScript
{
    public:

        void GossipHello(Object* pObject, Player* pPlayer)
        {
            TheVioletHoldScript* pInstance = (TheVioletHoldScript*)pPlayer->GetMapMgr()->GetScript();
            if (!pInstance)
                return;

            GossipMenu* menu;

            //Page 1: Textid and first menu item
            if (pInstance->GetInstanceData(Data_EncounterState, MAP_VIOLET_HOLD) == State_NotStarted)
            {
                objmgr.CreateGossipMenuForPlayer(&menu, pObject->GetGUID(), SINCLARI_ON_HELLO, pPlayer);
                menu->AddItem(ICON_CHAT, pPlayer->GetSession()->LocalizedGossipOption(SINCLARI_ACTIVATE), 1);

                menu->SendTo(pPlayer);
            }

            //If VioletHold is started, Sinclari has this item for people who aould join.
            if (pInstance->GetInstanceData(Data_EncounterState, MAP_VIOLET_HOLD) == State_InProgress)
            {
                objmgr.CreateGossipMenuForPlayer(&menu, pObject->GetGUID(), SINCLARI_OUTSIDE, pPlayer);
                menu->AddItem(ICON_CHAT, pPlayer->GetSession()->LocalizedGossipOption(SINCLARI_SEND_ME_IN), 3);

                menu->SendTo(pPlayer);
            }
        };

        void GossipSelectOption(Object* pObject, Player*  pPlayer, uint32 Id, uint32 IntId, const char* Code)
        {
            TheVioletHoldScript* pInstance = (TheVioletHoldScript*)pPlayer->GetMapMgr()->GetScript();

            if (!pInstance)
                return;

            if (!pObject->IsCreature())
                return;

            Creature* pCreature = TO_CREATURE(pObject);

            switch (IntId)
            {
                case 0:
                    GossipHello(pObject, pPlayer);
                    break;

                case 1:
                {
                    GossipMenu* menu;
                    objmgr.CreateGossipMenuForPlayer(&menu, pObject->GetGUID(), SINCLARI_ON_FINISH, pPlayer);
                    menu->AddItem(ICON_CHAT, pPlayer->GetSession()->LocalizedGossipOption(SINCLARI_GET_SAFETY), 2);
                    menu->SendTo(pPlayer);

                    // New Encounter State included
                    pInstance->SetInstanceData(Data_EncounterState, MAP_VIOLET_HOLD, State_PreProgress);
                }break;

                case 2:
                {
                    TO_CREATURE(pObject)->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                    pCreature->GetAIInterface()->setMoveType(MOVEMENTTYPE_NONE);
                    //pCreature->MoveToWaypoint(1);
                    pCreature->GetAIInterface()->StopMovement(10);

                }break;

                case 3:
                {
                    Arcemu::Gossip::Menu::Complete(pPlayer);
                    pPlayer->SafeTeleport(pPlayer->GetInstanceID(), MAP_VIOLET_HOLD, 1830.531006f, 803.939758f, 44.340508f, 6.281611f);
                }break;
            }
        }
};

///////////////////////////////////////////////////////
//VH Guards
class VHGuardsAI : public MoonScriptCreatureAI
{
    public:

        MOONSCRIPT_FACTORY_FUNCTION(VHGuardsAI, MoonScriptCreatureAI);
        VHGuardsAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
           _unit->GetAIInterface()->setMoveType(MOVEMENTTYPE_WANTEDWP);
        }

		void OnLoad()
		{
			RegisterAIUpdateEvent(1000);
			ParentClass::OnLoad();
		}

		void AIUpdate()
		{
			Creature* Sinclari = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1830.94f, 799.46f, 44.4f, CN_LIEUTNANT_SINCLARI);
			if (Sinclari && Sinclari->GetPositionX() == 1830.949951f == false)
			{
				_unit->GetAIInterface()->MoveTo(1806.07f, 803.988f, 44.36f, 3.14f);
				RemoveAIUpdateEvent();
			}
			ParentClass::AIUpdate();
		}

};

//Violet Hold Handler
// Completed/Fixed Issues:
/*
#1: Needs randomized spawn points like blizzard has.
#3: Worldstates need to be added.
*/
//Ongoing Issues:
// #2: Mini-Bosses need scripted.
// #5: Waypoints from adds need to be added.
class VioletHoldSpawnHandler : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(VioletHoldSpawnHandler, MoonScriptBossAI);
	VioletHoldSpawnHandler(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		RegisterAIUpdateEvent(1000);
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(1000);
		SetPhase(1);
		ParentClass::OnLoad();
	}
	void StartInstance()
	{
		wavetimer = AddTimer(10000);
		SetPhase(2);
	}

	void RandomSpawnWavePortal_1()
	{
		switch (RandomUInt(1, 2))
		{
		case 1:
			SpawnCreature(CN_PORTAL_INTRO, 1891.005f, 752.290f, 47.667f, 1.93f); // Portal
			break;
		case 2:
			SpawnCreature(CN_PORTAL_INTRO, 1877.75f, 849.27f, 43.33f, 4.8f); // Portal
			break;
		}
	}

	void RandomSpawnWavePortal_2()
	{
		switch (RandomUInt(1, 3))
		{
		case 1:
			SpawnCreature(CN_PORTAL_INTRO, 1913.03f, 805.655f, 38.646f, 3.2f); // Portal
			break;
		case 2:
			SpawnCreature(CN_PORTAL_INTRO, 1932.83f, 771.32f, 51.93f, 3.86f); // Portal
			break;
		case 3:
			SpawnCreature(CN_PORTAL_INTRO, 1909.74f, 804.43f, 38.64f, 3.07f); // Portal
			break;
		}
	}

	void RandomSpawnWavePortal_3()
	{
		switch (RandomUInt(1, 3))
		{
		case 1:
			SpawnCreature(CN_PORTAL_INTRO, 1913.03f, 805.655f, 38.646f, 3.2f); // Portal
			break;
		case 2:
			SpawnCreature(CN_PORTAL_INTRO, 1877.75f, 849.27f, 43.33f, 4.8f); // Portal
			break;
		case 3:
			SpawnCreature(CN_PORTAL_INTRO, 1891.005f, 752.290f, 47.667f, 1.93f); // Portal
			break;
		}
	}

	void AIUpdate()
	{
		TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
		if (GetPhase() == 1)
		{
			StartInstance();
			pInstance->StartSetupWS();
			_unit->m_portalNumber = 1;
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 2) // 1/18
		{
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(3);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(3);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(3);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 3) // 2/18
		{
			pInstance->SetPortalNumber(2);
			_unit->m_portalNumber = 2;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(4);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(4);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(4);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 4) // 3/18
		{
			pInstance->SetPortalNumber(3);
			_unit->m_portalNumber = 3;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(5);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(5);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(5);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 5) // 4/18
		{
			pInstance->SetPortalNumber(4);
			_unit->m_portalNumber = 4;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(6);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(6);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(6);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 6) // 5/18
		{
			pInstance->SetPortalNumber(5);
			_unit->m_portalNumber = 5;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(7);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(7);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(7);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 7) // 6/18 first boss
		{
			pInstance->SetPortalNumber(6);
			_unit->m_portalNumber = 6;
			SpawnCreature(30663, 1886.16f, 806.61f, 38.36f, 3.14f); // Azure Binder that handles the spawns
			ResetTimer(wavetimer, 150000);
			SetPhase(8);
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 8) // 7/18
		{
			pInstance->SetPortalNumber(7);
			_unit->m_portalNumber = 7;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(9);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(9);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(9);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 9) // 8/18
		{
			pInstance->SetPortalNumber(8);
			_unit->m_portalNumber = 8;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(10);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(10);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(10);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 10) // 9/18
		{
			pInstance->SetPortalNumber(9);
			_unit->m_portalNumber = 9;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(11);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(11);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(11);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 11) // 10/18
		{
			pInstance->SetPortalNumber(10);
			_unit->m_portalNumber = 10;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(12);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(12);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(12);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 12) // 11/18
		{
			pInstance->SetPortalNumber(11);
			_unit->m_portalNumber = 11;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(13);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(13);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(13);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 13) // 12/18 second boss
		{
			pInstance->SetPortalNumber(12);
			_unit->m_portalNumber = 12;
			SpawnCreature(30663, 1886.16f, 806.61f, 38.36f, 3.14f); // Azure Binder that handles the spawns
			ResetTimer(wavetimer, 150000);
			SetPhase(14);
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 14) // 13/18
		{
			pInstance->SetPortalNumber(13);
			_unit->m_portalNumber = 13;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(15);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(15);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 50000);
				SetPhase(15);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 15) // 14/18
		{
			pInstance->SetPortalNumber(14);
			_unit->m_portalNumber = 14;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 50000);
				SetPhase(16);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 50000);
				SetPhase(16);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 50000);
				SetPhase(16);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 16) // 15/18
		{
			pInstance->SetPortalNumber(15);
			_unit->m_portalNumber = 15;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 50000);
				SetPhase(17);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 50000);
				SetPhase(17);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 50000);
				SetPhase(17);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 17) // 16/18
		{
			pInstance->SetPortalNumber(16);
			_unit->m_portalNumber = 16;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 50000);
				SetPhase(18);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 50000);
				SetPhase(18);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 50000);
				SetPhase(18);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 18) // 17/18
		{
			pInstance->SetPortalNumber(17);
			_unit->m_portalNumber = 17;
			switch (RandomUInt(0, 2))
			{
			case 0:
				RandomSpawnWavePortal_1();
				ResetTimer(wavetimer, 52000);
				SetPhase(19);
				break;
			case 1:
				RandomSpawnWavePortal_2();
				ResetTimer(wavetimer, 52000);
				SetPhase(19);
				break;
			case 2:
				RandomSpawnWavePortal_3();
				ResetTimer(wavetimer, 52000);
				SetPhase(19);
				break;
			}
		}
		if (IsTimerFinished(wavetimer) && GetPhase() == 19)
		{
			pInstance->SetPortalNumber(18);
			_unit->m_portalNumber = 18;
			SpawnCreature(CN_PORTAL_INTRO, 1925.23f, 800.14f, 52.40f, 3.04f); // Portal
			SpawnCreature(CN_CYANIGOSA, 1923.25f, 800.33f, 52.40f, 3.04f); // Cyanigosa
			SetPhase(20);
			_unit->Despawn(1000, 0);
		}
		ParentClass::AIUpdate();
	}
protected:
	int32 wavetimer;
	int32 changeflag;
	bool notice;
};

class AzureBinderAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(AzureBinderAI, MoonScriptBossAI);
	AzureBinderAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		if (_unit->GetMapId() == 608) // Make sure creature is on map 608 (Violet Hold)
		_unit->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2); //Set "unattackable"
		RandomBossSelection();
		RegisterAIUpdateEvent(1000);
		SetPhase(1);
		_unit->EnableAI();
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		Creature* erekem = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1871.45f, 871.03f, 43.41f, CN_EREKEM);
		Creature* erekem_guard_1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(1892.42f, 872.28f, 43.41f, 29395, 121950);
		Creature* erekem_guard_2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(1853.75f, 862.45f, 43.41f, 29395, 121951);
		GameObject* erekem_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1871.45f, 871.03f, 43.41f, GO_EREKEM_DOOR);
		Creature* zuramat = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1934.15f, 860.94f, 47.29f, CN_TURAMAT_THE_OBLITERATOR);
		GameObject* zuramat_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1934.15f, 860.94f, 47.29f, GO_ZURAMAT_THE_OBLITERATOR_DOOR);
		Creature* moragg = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1893.90f, 728.125f, 47.75f, CN_MORAGG);
		GameObject* moragg_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1893.90f, 728.125f, 47.75f, GO_MORAGG_DOOR);
		Creature* lavanthos = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1844.56f, 748.70f, 38.74f, CN_LAVANTHOR);
		GameObject* lavanthos_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1844.56f, 748.70f, 38.74f, GO_LAVANTHOR_DOOR);
		Creature* ichron = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1942.04f, 749.52f, 30.95f, CN_ICHORON);
		GameObject* ichron_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1942.04f, 749.52f, 30.95f, GO_ICHORON_DOOR);
		Creature* xevozz = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1908.42f, 845.84f, 38.71f, CN_XEVOZZ);
		GameObject* xevozz_go = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1908.42f, 845.84f, 38.71f, GO_XEVOZZ_DOOR);
		if (IsTimerFinished(ReleaseTimer_Erekem) && GetPhase() == 1)
		{
			if (erekem)
			{
				if (erekem_guard_1)
				{
					erekem_guard_1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
					erekem_guard_1->GetAIInterface()->MoveTo(1881.57f, 852.61f, 43.33f, 4.7f);
				}
				if (erekem_guard_2)
				{
					erekem_guard_2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
					erekem_guard_2->GetAIInterface()->MoveTo(1872.07f, 851.58f, 43.33f, 4.7f);
				}
				if (erekem_go)
				{
					erekem_go->Despawn(500, 0);
				}
				erekem->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				erekem->GetAIInterface()->MoveTo(1877.53f, 855.21f, 43.33f, 4.7f);
				erekem->SendChatMessage(Text_Yell, 0, "Free to—mm—fly now. Ra-aak... Not find us—ekh-ekh! Escape!");
				_unit->PlaySoundToSet(14218);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else
			{
				RemoveTimer(ReleaseTimer_Erekem);
				SelectNewBoss();
			}
		}
		if (IsTimerFinished(ReleaseTimer_Moragg) && GetPhase() == 1)
		{
			if (moragg)
			{
				if (moragg_go)
				{
					moragg_go->Despawn(500, 0);
				}
				moragg->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				moragg->GetAIInterface()->MoveTo(1891.08f, 748.36f, 47.66f, 1.8f);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else
			{
				RemoveTimer(ReleaseTimer_Moragg);
				SelectNewBoss();
			}
		}
		if (IsTimerFinished(ReleaseTimer_Ichoron) && GetPhase() == 1)
		{
			if (ichron)
			{
				if (ichron_go)
				{
					ichron_go->Despawn(500, 0);
				}
				ichron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				ichron->GetAIInterface()->MoveTo(1910.91f, 783.66f, 36.98f, 2.4f);
				ichron->SendChatMessage(Text_Yell, 0, "I... am fury... unrestrained!");
				_unit->PlaySoundToSet(14229);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else
			{
				RemoveTimer(ReleaseTimer_Ichoron);
				SelectNewBoss();
			}
		}
		if (IsTimerFinished(ReleaseTimer_Xevozz) && GetPhase() == 1)
		{
			if (xevozz)
			{
				if (xevozz_go)
				{
					xevozz_go->Despawn(500, 0);
				}
				xevozz->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				xevozz->GetAIInterface()->MoveTo(1905.52f, 840.64f, 38.66f, 4.14f);
				xevozz->SendChatMessage(Text_Yell, 0, "Back in business! Now to execute an exit strategy.");
				_unit->PlaySoundToSet(14498);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else
			{
				RemoveTimer(ReleaseTimer_Xevozz);
				SelectNewBoss();
			}
		}
		if (IsTimerFinished(ReleaseTimer_Lavanthos) && GetPhase() == 1)
		{
			if (lavanthos)
			{
				if (lavanthos_go)
				{
					lavanthos_go->Despawn(500, 0);
				}
				lavanthos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				lavanthos->GetAIInterface()->MoveTo(1858.97f, 765.651f, 38.65f, 0.88f);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else
			{
				RemoveTimer(ReleaseTimer_Lavanthos);
				SelectNewBoss();
			}
		}
		if (IsTimerFinished(ReleaseTimer_Zuramat) && GetPhase() == 1)
		{
			if (zuramat)
			{
				if (zuramat_go)
				{
					zuramat_go->Despawn(500, 0);
				}
				zuramat->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				zuramat->GetAIInterface()->MoveTo(1925.13f, 849.009f, 47.16f, 4.17f);
				zuramat->SendChatMessage(Text_Yell, 0, "I am... renewed.");
				_unit->PlaySoundToSet(13995);
				_unit->Despawn(2000, 0);
				SetPhase(2);
			}
			else // Boss was already selected and killed so go back to randombossselection
			{
				RemoveTimer(ReleaseTimer_Zuramat);
				SelectNewBoss();
			}
		}
		ParentClass::AIUpdate();
	}

	void SelectNewBoss()
	{
		switch (RandomUInt(0, 5)) // Add this to the azure binders script to make this script cleaner. 
		{
		case 0: //Erekem
			ReleaseTimer_Erekem = AddTimer(1000);
			break;
		case 1: //Moragg
			ReleaseTimer_Moragg = AddTimer(1000);
			break;
		case 2: // Ichoron
			ReleaseTimer_Ichoron = AddTimer(1000);
			break;
		case 3: // Xevozz
			ReleaseTimer_Xevozz = AddTimer(1000);
			break;
		case 4: // Lavanthos
			ReleaseTimer_Lavanthos = AddTimer(1000);
			break;
		case 5: // Zuramat
			ReleaseTimer_Zuramat = AddTimer(1000);
			break;
		}
	};

	void RandomBossSelection()
	{
		switch (RandomUInt(0, 5)) // Add this to the azure binders script to make this script cleaner. 
		{
		case 0: //Erekem
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1872.82f, 864.60f, 43.33f, 1.82f);
			ReleaseTimer_Erekem = AddTimer(10000);
			break;
		case 1: //Moragg
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1894.56f, 740.974f, 47.66f, 4.7f);
			ReleaseTimer_Moragg = AddTimer(10000);
			break;
		case 2: // Ichoron
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1912.23f, 782.13f, 36.66f, 5.5f);
			ReleaseTimer_Ichoron = AddTimer(10000);
			break;
		case 3: // Xevozz
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1906.29f, 842.44f, 38.63f, 0.97f);
			ReleaseTimer_Xevozz = AddTimer(10000);
			break;
		case 4: // Lavanthos
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1854.26f, 761.06f, 38.65f, 4.05f);
			ReleaseTimer_Lavanthos = AddTimer(10000);
			break;
		case 5: // Zuramat
			_unit->GetAIInterface()->SetRun();
			_unit->GetAIInterface()->MoveTo(1924.46f, 851.81f, 47.18f, 0.89f);
			ReleaseTimer_Zuramat = AddTimer(10000);
			break;
		}
	}
protected:
	int32 ReleaseTimer_Erekem;
	int32 ReleaseTimer_Moragg;
	int32 ReleaseTimer_Ichoron;
	int32 ReleaseTimer_Xevozz;
	int32 ReleaseTimer_Lavanthos;
	int32 ReleaseTimer_Zuramat;
};

class POrtalAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(POrtalAI, MoonScriptBossAI);
	POrtalAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		_monsterSpawned = false;
		RegisterAIUpdateEvent(RandomUInt(6000, 9000));
		Despawn(26000, 0);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		if (_monsterSpawned == false)
		{
			if (_unit->GetPositionX() == 1891.005f && _unit->GetPositionY() == 752.290f && _unit->GetPositionZ() == 47.667f)
			{
				if (random <= 49)
				{
					_unit->SendChatMessage(42, 0, "A Portal Keeper appears to defend the portal!");
					SpawnCreature(30893, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				if (random >= 50)
				{
					_unit->SendChatMessage(42, 0, "A Portal Guardian appears to defend the portal!");
					SpawnCreature(CN_PORTAL_GUARDIAN, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				_monsterSpawned = true;
			}
			if (_unit->GetPositionX() == 1877.75f && _unit->GetPositionY() == 849.27f && _unit->GetPositionZ() == 43.33f)
			{
				if (random <= 49)
				{
					_unit->SendChatMessage(42, 0, "A Portal Keeper appears to defend the portal!");
					SpawnCreature(30893, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				if (random >= 50)
				{
					_unit->SendChatMessage(42, 0, "A Portal Guardian appears to defend the portal!");
					SpawnCreature(CN_PORTAL_GUARDIAN, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				_monsterSpawned = true;
			}
			if (_unit->GetPositionX() == 1913.03f && _unit->GetPositionY() == 805.655f && _unit->GetPositionZ() == 38.646f)
			{
				if (random <= 49)
				{
					_unit->SendChatMessage(42, 0, "A Portal Keeper appears to defend the portal!");
					SpawnCreature(30893, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				if (random >= 50)
				{
					_unit->SendChatMessage(42, 0, "An elite blue dragonflight squad appears out of the portal!");
					SpawnCreature(CN_AZURE_CAPTAIN, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
					SpawnCreature(CN_AZURE_RAIDER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
					SpawnCreature(CN_AZURE_STALKER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				}
				_monsterSpawned = true;
			}
			if (_unit->GetPositionX() == 1932.83f && _unit->GetPositionY() == 771.32f && _unit->GetPositionZ() == 51.93f)
			{
				_unit->SendChatMessage(42, 0, "An elite blue dragonflight squad appears out of the portal!");
				SpawnCreature(CN_AZURE_CAPTAIN, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				SpawnCreature(CN_AZURE_RAIDER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				SpawnCreature(CN_AZURE_STALKER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				_monsterSpawned = true;
			}
			if (_unit->GetPositionX() == 1909.74f && _unit->GetPositionY() == 804.43f && _unit->GetPositionZ() == 38.64f)
			{
				_unit->SendChatMessage(42, 0, "An elite blue dragonflight squad appears out of the portal!");
				SpawnCreature(CN_AZURE_CAPTAIN, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				SpawnCreature(CN_AZURE_RAIDER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				SpawnCreature(CN_AZURE_STALKER, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
				_monsterSpawned = true;
			}
		}
		ParentClass::AIUpdate();
	}
private:
	int32 random = RandomUInt(0, 100);
	bool _monsterSpawned;
};

///////////////////////////////////////////////////////
//Boss: Erekem
//class ErekemAI : public CreatureAIScript
enum ErekemSpells
{
	SPELL_BLOODLUST = 54516,
	SPELL_BREAK_BONDS = 59463,
	SPELL_CHAIN_HEAL_N = 54481,
	SPELL_CHAIN_HEAL_H = 59473,
	SPELL_EARTH_SHIELD_N = 54479,
	SPELL_EARTH_SHIELD_H = 59471,
	SPELL_EARTH_SHOCK = 54511,
	SPELL_LIGHTNING_BOLT = 53044,
	SPELL_STORMSTRIKE = 51876,
};

#define SPELL_CHAIN_HEAL						IsHeroic() ? SPELL_CHAIN_HEAL_H : SPELL_CHAIN_HEAL_N
#define SPELL_EARTH_SHIELD						IsHeroic() ? SPELL_EARTH_SHIELD_H : SPELL_EARTH_SHIELD_N

enum ErekemEvents
{
	EVENT_SPELL_BLOODLUST = 1,
	EVENT_SPELL_BREAK_BONDS,
	EVENT_SPELL_CHAIN_HEAL,
	EVENT_SPELL_EARTH_SHIELD,
	EVENT_SPELL_EARTH_SHOCK,
	EVENT_SPELL_LIGHTNING_BOLT,
	EVENT_SPELL_STORMSTRIKE,
};
class ErekemAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(ErekemAI, MoonScriptBossAI);
	ErekemAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		_addDeath = 0;
	}

	void OnCombatStart(Unit* mTarget)
	{
		RegisterAIUpdateEvent(1000);
		Emote("Not--caww--get in way of--rrak-rrak--flee!", Text_Yell, 14219);
		_unit->CastSpell(_unit, SPELL_EARTH_SHIELD, false);
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_BLOODLUST, 15000);
		events.RescheduleEvent(EVENT_SPELL_BREAK_BONDS, RandomUInt(9000, 14000));
		events.RescheduleEvent(EVENT_SPELL_CHAIN_HEAL, 0);
		events.RescheduleEvent(EVENT_SPELL_EARTH_SHIELD, 20000);
		events.RescheduleEvent(EVENT_SPELL_EARTH_SHOCK, RandomUInt(2000, 8000));
		events.RescheduleEvent(EVENT_SPELL_LIGHTNING_BOLT, RandomUInt(5000, 10000));
		if (IsHeroic())
			events.RescheduleEvent(EVENT_SPELL_STORMSTRIKE, 3000);
		ParentClass::OnCombatStart(mTarget);
	}

	void OnCombatStop(Unit* pTarget)
	{
		events.Reset();
		ParentClass::OnCombatStop(pTarget);
	}

	void OnTargetDied(Unit* pTarget)
	{
		switch (RandomUInt(0, 2))
		{
		case 0:
			Emote("Kaw-Kaw!", Text_Yell, 14222);
			break;
		case 1:
			Emote("Only strong, Aak, survive!", Text_Yell, 14224);
			break;
		case 2:
			Emote("Precious life, Awk, wasted!", Text_Yell, 14223);
			break;
		}
		ParentClass::OnTargetDied(pTarget);
	}

	void OnDied(Unit* pTarget)
	{
		Emote("No—kaw, kaw—flee...", Text_Yell, 14225);
		RemoveAIUpdateEvent();
		ParentClass::OnDied(pTarget);
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	Player* GetRandomPlayerTarget()
	{

		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
		{
			if ((*iter) && (TO< Player* >(*iter))->isAlive())
				possible_targets.push_back((uint32)(*iter)->GetGUID());
		}
		if (possible_targets.size() > 0)
		{
			uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
			return _unit->GetMapMgr()->GetPlayer(random_player);
		}
		return NULL;
	}


	uint64 GetChainHealTargetGUID()
	{
		if (_unit->GetHealthPct() <= 85)
			return _unit->GetGUID();

		Creature *pGuard1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121950);
		Creature *pGuard2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121951);

		if (Creature *c1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121950))
				if (c1->isAlive() && !c1->GetHealthPct() >= 75)
					return c1->GetGUID();

		if (Creature *c2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121951))
				if (c2->isAlive() && !c2->GetHealthPct() >= 75)
					return c2->GetGUID();

		return _unit->GetGUID();
	}

	void AIUpdate()
	{
		Creature *pGuard1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121950);
		Creature *pGuard2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121951);
		if (pGuard1 && pGuard1->IsDead() && _addDeath == 0)
		{
			Emote("My---raaak—favorite! Awk awk awk! Raa-kaa!", Text_Yell, 14220);
			_addDeath = 1;
		}
		if (pGuard2 && pGuard2->IsDead() && _addDeath == 1)
		{
			Emote("Nasty little...A-ak, kaw! Kill! Yes, kill you!", Text_Yell, 14221);
			_addDeath = 2;
		}
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case 0:
			break;
		case EVENT_SPELL_BLOODLUST:
			_unit->CastSpell((Unit*)NULL, SPELL_BLOODLUST, false);
			events.RepeatEvent(RandomUInt(35000, 45000));
			break;
		case EVENT_SPELL_BREAK_BONDS:
			_unit->CastSpell((Unit*)NULL, SPELL_BREAK_BONDS, false);
			events.RepeatEvent(RandomUInt(16000, 22000));
			break;
		case EVENT_SPELL_CHAIN_HEAL:
			if (uint64 TargetGUID = GetChainHealTargetGUID())
			{
					_unit->CastSpell(TargetGUID, SPELL_CHAIN_HEAL, false);

					Creature *pGuard1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121950);
					Creature *pGuard2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121951);
					if (pGuard1 && pGuard1->IsDead() || pGuard2 && pGuard2->IsDead())
					{
						events.RepeatEvent(RandomUInt(3000, 6000));
						break;
					}
			}
			events.RepeatEvent(RandomUInt(8000, 11000));
			break;
		case EVENT_SPELL_EARTH_SHIELD:
			_unit->CastSpell(_unit, SPELL_EARTH_SHIELD, false);
			events.RepeatEvent(20000);
			break;
		case EVENT_SPELL_EARTH_SHOCK:
			_unit->CastSpell(_unit->GetVictim(), SPELL_EARTH_SHOCK, false);
			events.RepeatEvent(RandomUInt(8000, 13000));
			break;
		case EVENT_SPELL_LIGHTNING_BOLT:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_LIGHTNING_BOLT, false);
			events.RepeatEvent(RandomUInt(15000, 25000));
			break;
		case EVENT_SPELL_STORMSTRIKE:
		{
			Creature *pGuard1 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121950);
			Creature *pGuard2 = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 29395, 121951);
			if (pGuard1 && pGuard1->IsDead() && pGuard2 && pGuard2->IsDead()) // both dead
				_unit->CastSpell(_unit->GetVictim(), SPELL_STORMSTRIKE, false);
			events.RepeatEvent(3000);
		}
		break;
		}
		ParentClass::AIUpdate();
	}
	protected:
		uint32 _addDeath;
};

///////////////////////////////////////////////////////
//Boss: Moragg
//class MoraggAI : public CreatureAIScript
enum eSpells
{
	SPELL_RAY_OF_SUFFERING_N = 54442,
	SPELL_RAY_OF_SUFFERING_H = 59524,
	//SPELL_RAY_OF_SUFFERING_TRIGGERED		= 54417,

	SPELL_RAY_OF_PAIN_N = 54438,
	SPELL_RAY_OF_PAIN_H = 59523,
	//SPELL_RAY_OF_PAIN_TRIGGERED_N			= 54416,
	//SPELL_RAY_OF_PAIN_TRIGGERED_H			= 59525,

	SPELL_CORROSIVE_SALIVA = 54527,
	SPELL_OPTIC_LINK = 54396,
};

#define SPELL_RAY_OF_SUFFERING				IsHeroic() ? SPELL_RAY_OF_SUFFERING_H : SPELL_RAY_OF_SUFFERING_N
#define SPELL_RAY_OF_PAIN					IsHeroic() ? SPELL_RAY_OF_PAIN_H : SPELL_RAY_OF_PAIN_N

enum MoraggEvents
{
	EVENT_SPELL_CORROSIVE_SALIVA = 1,
	EVENT_SPELL_OPTIC_LINK,
};
class MoraggAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(MoraggAI, MoonScriptBossAI);
	MoraggAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnCombatStop(Unit* pTarget)
	{
		events.Reset();
		ParentClass::OnCombatStop(pTarget);
	}

	void OnCombatStart(Unit* pTarget)
	{
		_unit->CastSpell(_unit, SPELL_RAY_OF_SUFFERING, true);
		_unit->CastSpell(_unit, SPELL_RAY_OF_PAIN, true);
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_CORROSIVE_SALIVA, RandomUInt(4000, 6000));
		events.RescheduleEvent(EVENT_SPELL_OPTIC_LINK, RandomUInt(10000, 11000));
		ParentClass::OnCombatStart(pTarget);
	}

	Player* GetRandomPlayerTarget()
	{

		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
		{
			if ((*iter) && (TO< Player* >(*iter))->isAlive())
				possible_targets.push_back((uint32)(*iter)->GetGUID());
		}
		if (possible_targets.size() > 0)
		{
			uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
			return _unit->GetMapMgr()->GetPlayer(random_player);
		}
		return NULL;
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case 0:
			break;
		case EVENT_SPELL_CORROSIVE_SALIVA:
			_unit->CastSpell(_unit->GetVictim(), SPELL_CORROSIVE_SALIVA, false);
			events.RepeatEvent(RandomUInt(8000, 10000));
			break;
		case EVENT_SPELL_OPTIC_LINK:
			if (Unit* target = GetRandomPlayerTarget())
			{
				_unit->CastSpell(target, SPELL_OPTIC_LINK, false);
				events.RepeatEvent(RandomUInt(18000, 21000));
			}
			else
				events.RepeatEvent(5000);
			break;
		}
		ParentClass::AIUpdate();
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}
};

enum GuardErekemSpells
{
	SPELL_GUSHING_WOUND = 39215,
	SPELL_HOWLING_SCREECH = 54462,
	SPELL_STRIKE = 14516
};

enum GuardErekemEvents
{
	EVENT_SPELL_GUSHING_WOUND = 1,
	EVENT_SPELL_HOWLING_SCREECH,
	EVENT_SPELL_STRIKE
};

class ErekemGuardAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(ErekemGuardAI, MoonScriptBossAI);
	ErekemGuardAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnCombatStop(Unit* pTarget)
	{
		events.Reset();
		ParentClass::OnCombatStop(pTarget);
	}

	void OnCombatStart(Unit* pTarget)
	{
		RegisterAIUpdateEvent(1000);
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_GUSHING_WOUND, RandomUInt(1000, 3000));
		events.RescheduleEvent(EVENT_SPELL_HOWLING_SCREECH, RandomUInt(8000, 13000));
		events.RescheduleEvent(EVENT_SPELL_STRIKE, RandomUInt(4000, 8000));
		ParentClass::OnCombatStart(pTarget);
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case 0:
			break;
		case EVENT_SPELL_GUSHING_WOUND:
			_unit->CastSpell(_unit->GetVictim(), SPELL_GUSHING_WOUND, false);
			events.RepeatEvent(RandomUInt(7000, 12000));
			break;
		case EVENT_SPELL_HOWLING_SCREECH:
			_unit->CastSpell(_unit->GetVictim(), SPELL_HOWLING_SCREECH, false);
			events.RepeatEvent(RandomUInt(8000, 13000));
			break;
		case EVENT_SPELL_STRIKE:
			_unit->CastSpell(_unit->GetVictim(), SPELL_STRIKE, false);
			events.RepeatEvent(RandomUInt(4000, 8000));
			break;
		}
		ParentClass::AIUpdate();
	}
};

///////////////////////////////////////////////////////
//Boss: Ichoron
//class IchoronAI : public CreatureAIScript
class IchoronAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(IchoronAI, MoonScriptBossAI);
	IchoronAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}
};

///////////////////////////////////////////////////////
//Boss: Xevozz
//class XevozzAI : public CreatureAIScript
enum XevozzSpells
{
	SPELL_ARCANE_BARRAGE_VOLLEY = 54202,
	SPELL_ARCANE_BUFFET = 54226,
	SPELL_SUMMON_ETHEREAL_SPHERE_1 = 54102,
	SPELL_SUMMON_ETHEREAL_SPHERE_2 = 61337,
	SPELL_SUMMON_ETHEREAL_SPHERE_3 = 54138,
	SPELL_ARCANE_POWER = 54160,
	H_SPELL_ARCANE_POWER = 59474,
	SPELL_SUMMON_PLAYERS = 54164,
	SPELL_POWER_BALL_VISUAL = 54141,
	SPELL_POWER_BALL_DAMAGE_TRIGGER = 54207
};
enum NPCs
{
	NPC_ETHEREAL_SPHERE = 29271,
	NPC_ETHEREAL_SPHERE2 = 32582
};
enum XevozzEvents
{
	EVENT_ARCANE_BARRAGE = 1,
	EVENT_ARCANE_BUFFET,
	EVENT_SUMMON_SPHERE,
	EVENT_SUMMON_SPHERE_2,
	EVENT_RANGE_CHECK,
	EVENT_SUMMON_PLAYERS,
	EVENT_DESPAWN_SPHERE
};
class XevozzAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(XevozzAI, MoonScriptBossAI);
	XevozzAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnCombatStop(Unit* pTarget)
	{
		events.Reset();
		ParentClass::OnCombatStop(pTarget);
	}

	void OnDied(Unit* mKiller)
	{
		Emote("This is an... unrecoverable... loss.", Text_Yell, 14507);
		ParentClass::OnDied(mKiller);
	}

	void OnCombatStart(Unit* pTarget)
	{
		Emote("It seems my freedom must be bought with blood...", Text_Yell, 14499);
		events.ScheduleEvent(EVENT_SUMMON_SPHERE, 5000);
		events.ScheduleEvent(EVENT_ARCANE_BARRAGE, RandomUInt(8000, 10000));
		events.ScheduleEvent(EVENT_ARCANE_BUFFET, RandomUInt(10000, 11000));
		ParentClass::OnCombatStart(pTarget);
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	Player* GetRandomPlayerTarget()
	{

		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
		{
			if ((*iter) && (TO< Player* >(*iter))->isAlive())
				possible_targets.push_back((uint32)(*iter)->GetGUID());
		}
		if (possible_targets.size() > 0)
		{
			uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
			return _unit->GetMapMgr()->GetPlayer(random_player);
		}
		return NULL;
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_ARCANE_BARRAGE:
			_unit->CastSpell(_unit, SPELL_ARCANE_BARRAGE_VOLLEY, false);
			events.ScheduleEvent(EVENT_ARCANE_BARRAGE, RandomUInt(8000, 10000));
			break;
		case EVENT_ARCANE_BUFFET:
			if (Unit* target = GetRandomPlayerTarget())
			_unit->CastSpell(target, SPELL_ARCANE_BUFFET, false);
			events.ScheduleEvent(EVENT_ARCANE_BUFFET, RandomUInt(15000, 20000));
			break;
		case EVENT_SUMMON_SPHERE:
			Emote("Intriguing... a high quantity of arcane energy is near. Time for some prospecting...", Text_Yell, 14500);
			_unit->CastSpell(_unit, SPELL_SUMMON_ETHEREAL_SPHERE_1, false);
			if (IsHeroic())
			events.ScheduleEvent(EVENT_SUMMON_SPHERE_2, 2500);
			events.ScheduleEvent(EVENT_SUMMON_PLAYERS, RandomUInt(33000, 35000));
			events.ScheduleEvent(EVENT_SUMMON_SPHERE, RandomUInt(45000, 47000));
			break;
		case EVENT_SUMMON_SPHERE_2:
			Emote("The air teems with latent energy... quite the harvest!", Text_Yell, 14501);
			_unit->CastSpell(_unit, SPELL_SUMMON_ETHEREAL_SPHERE_2, false);
			break;
		case EVENT_SUMMON_PLAYERS:
		{
			Creature* sphere = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), NPC_ETHEREAL_SPHERE);
			if (!sphere)
				sphere = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), NPC_ETHEREAL_SPHERE2);
			if (sphere)
				sphere->CastSpell(sphere, SPELL_SUMMON_PLAYERS, true);
			break;
		}
		default:
			break;
		}
		ParentClass::AIUpdate();
	}
};

class EtherealSphereAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(EtherealSphereAI, MoonScriptBossAI);
	EtherealSphereAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnCombatStart(Unit* pTarget)
	{
		events.Reset();
		_unit->CastSpell(_unit, SPELL_POWER_BALL_VISUAL, true);
		_unit->CastSpell(_unit, SPELL_POWER_BALL_DAMAGE_TRIGGER, false);
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		_unit->SetFaction(16);
		events.ScheduleEvent(EVENT_DESPAWN_SPHERE, 40000);
		events.ScheduleEvent(EVENT_RANGE_CHECK, 1000);
		ParentClass::OnCombatStart(pTarget);
	}

	void OnLoad()
	{
		arcanePower = false;
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		events.Update(1000);

		if (_unit->IsCasting())
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_RANGE_CHECK:
			if (Creature* xevozz = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), CN_XEVOZZ))
			{
				if (_unit->GetDistance2dSq(xevozz) <= 3.0f && !arcanePower)
				{
					_unit->CastSpell(_unit, SPELL_ARCANE_POWER, false);
					arcanePower = true;
					events.ScheduleEvent(EVENT_DESPAWN_SPHERE, 8000);
				}
			}
			events.ScheduleEvent(EVENT_RANGE_CHECK, 1000);
			break;
		case EVENT_DESPAWN_SPHERE:
			_unit->Despawn(1000, 0);
			break;
		}
		ParentClass::AIUpdate();
	}
private:
	bool arcanePower;
};
///////////////////////////////////////////////////////
//Boss: Lavanthos
//class LavanthosAI : public CreatureAIScript
enum LavanthosSpells
{
	SPELL_CAUTERIZING_FLAMES = 59466,
	SPELL_FIREBOLT_N = 54235,
	SPELL_FIREBOLT_H = 59468,
	SPELL_FLAME_BREATH_N = 54282,
	SPELL_FLAME_BREATH_H = 59469,
	SPELL_LAVA_BURN_N = 54249,
	SPELL_LAVA_BURN_H = 59594,
};
enum LavanthosEvents
{
	EVENT_SPELL_FIREBOLT = 1,
	EVENT_SPELL_FLAME_BREATH,
	EVENT_SPELL_LAVA_BURN,
	EVENT_SPELL_CAUTERIZING_FLAMES,
};
class LavanthosAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(LavanthosAI, MoonScriptBossAI);
	LavanthosAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{}


	void OnCombatStop(Unit* pAttacker)
	{
		events.Reset();
		ParentClass::OnCombatStop(pAttacker);
	}

	void OnCombatStart(Unit* pAttacker)
	{
		RegisterAIUpdateEvent(1000);
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_FIREBOLT, 1000);
		events.RescheduleEvent(EVENT_SPELL_FLAME_BREATH, 5000);
		events.RescheduleEvent(EVENT_SPELL_LAVA_BURN, 10000);
		if (IsHeroic())
			events.RescheduleEvent(EVENT_SPELL_CAUTERIZING_FLAMES, 3000);
		ParentClass::OnCombatStart(pAttacker);
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case 0:
			break;
		case EVENT_SPELL_FIREBOLT:
			if (IsHeroic())
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_FIREBOLT_H, false);
				events.RepeatEvent(RandomUInt(5000, 13000));
			}
			else
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_FIREBOLT_N, false);
				events.RepeatEvent(RandomUInt(5000, 13000));
			}
			break;
		case EVENT_SPELL_FLAME_BREATH:
			if (IsHeroic())
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_FLAME_BREATH_H, false);
				events.RepeatEvent(RandomUInt(10000, 15000));
			}
			else
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_FLAME_BREATH_N, false);
				events.RepeatEvent(RandomUInt(10000, 15000));
			}
			break;
		case EVENT_SPELL_LAVA_BURN:
			if (IsHeroic())
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_LAVA_BURN_H, false);
				events.RepeatEvent(RandomUInt(14000, 20000));
			}
			else
			{
				_unit->CastSpell(_unit->GetVictim(), SPELL_LAVA_BURN_N, false);
				events.RepeatEvent(RandomUInt(14000, 20000));
			}
			break;
		case EVENT_SPELL_CAUTERIZING_FLAMES:
			if (IsHeroic())
			{
				_unit->CastSpell((Unit*)NULL, SPELL_FLAME_BREATH_H, false);
				events.RepeatEvent(RandomUInt(10000, 16000));
			}
			else
			{
				_unit->CastSpell((Unit*)NULL, SPELL_FLAME_BREATH_N, false);
				events.RepeatEvent(RandomUInt(10000, 16000));
			}
			break;
		}
	}
};

///////////////////////////////////////////////////////
//Boss: Zuramat the Obliterator
//class ZuramatTheObliteratorAI : public CreatureAIScript

enum ZuramatSpells
{
	SPELL_SHROUD_OF_DARKNESS_N = 54524,
	SPELL_SHROUD_OF_DARKNESS_H = 59745,
	SPELL_VOID_SHIFT_N = 54361,
	SPELL_VOID_SHIFT_H = 59743,
	SPELL_SUMMON_VOID_SENTRY = 54369,
	SPELL_SUMMON_VOID_SENTRY_BALL = 58650,

	//SPELL_ZURAMAT_ADD_2_N							= 54342,
	//SPELL_ZURAMAT_ADD_2_H							= 59747,
};

#define NPC_VOID_SENTRY_BALL						29365

enum ZuramatEvents
{
	EVENT_SPELL_SHROUD_OF_DARKNESS = 1,
	EVENT_SPELL_VOID_SHIFT,
	EVENT_SPELL_SUMMON_VOID_SENTRY,
};

class ZuramatTheObliteratorAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(ZuramatTheObliteratorAI, MoonScriptBossAI);
	ZuramatTheObliteratorAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		AddEmote(Event_OnCombatStart, "Eradicate!", Text_Yell, 13996);
		AddEmote(Event_OnTargetDied, "More.... energy.", Text_Yell, 13999);
		AddEmote(Event_OnTargetDied, "Relinquish.", Text_Yell, 14000);
		AddEmote(Event_OnDied, "Disperse....", Text_Yell, 14002);
	}

	void OnCombatStart(Unit* pTarget)
	{
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_SHROUD_OF_DARKNESS, RandomUInt(5000, 7000));
		events.RescheduleEvent(EVENT_SPELL_VOID_SHIFT, RandomUInt(23000, 25000));
		events.RescheduleEvent(EVENT_SPELL_SUMMON_VOID_SENTRY, 10000);
		ParentClass::OnCombatStart(pTarget);
	}

	void OnCombatStop(Unit* pTarget)
	{
		events.Reset();
		ParentClass::OnCombatStop(pTarget);
	}

	Player* GetRandomPlayerTarget()
	{

		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
		{
			if ((*iter) && (TO< Player* >(*iter))->isAlive())
				possible_targets.push_back((uint32)(*iter)->GetGUID());
		}
		if (possible_targets.size() > 0)
		{
			uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
			return _unit->GetMapMgr()->GetPlayer(random_player);
		}
		return NULL;
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case 0:
			break;
		case EVENT_SPELL_SHROUD_OF_DARKNESS:
			if (IsHeroic())
			{
				_unit->CastSpell(_unit, SPELL_SHROUD_OF_DARKNESS_H, false);
				Emote("Know... my... pain.", Text_Yell, 13997);
				events.RepeatEvent(20000);
			}
			else
			{
				_unit->CastSpell(_unit, SPELL_SHROUD_OF_DARKNESS_N, false);
				Emote("Know... my... pain.", Text_Yell, 13997);
				events.RepeatEvent(20000);
			}
			break;
		case EVENT_SPELL_VOID_SHIFT:
			if (Unit* target = GetRandomPlayerTarget())
			{
				if (IsHeroic())
				{
					_unit->CastSpell(target, SPELL_VOID_SHIFT_H, false);
					_unit->SendChatMessage(15, 0, "Gaze... into the void.");
				}
				else
				{
					_unit->CastSpell(target, SPELL_VOID_SHIFT_N, false);
					_unit->SendChatMessage(15, 0, "Gaze... into the void.");
				}
			}
			events.RepeatEvent(RandomUInt(18000, 22000));
			break;
		case EVENT_SPELL_SUMMON_VOID_SENTRY:
			_unit->CastSpell((Unit*)NULL, SPELL_SUMMON_VOID_SENTRY, false);
			events.RepeatEvent(12000);
			break;
		}
		ParentClass::AIUpdate();
	}
};

class PrisonSealDoorAI : public MoonScriptBossAI // Npc Id: 30896
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(PrisonSealDoorAI, MoonScriptBossAI);
	PrisonSealDoorAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		_sealOpened = false;

		RegisterAIUpdateEvent(1000);
		_unit->SetMaxHealth(200000);
		_unit->SetHealth(200000);
		_unit->SetDisplayId(11686);
		_unit->m_canRegenerateHP = false; // Set health to not regenerate.
		_unit->SetFlag(UNIT_FIELD_FLAGS, 0);
		_unit->SetFaction(35);
		ParentClass::OnLoad();
	}

	void InstanceFailedSealBroken()
	{
		Creature* PortalSpawner = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1883.76f, 804.73f, 38.39f, 43283);
		if (PortalSpawner)
		{
			PortalSpawner->Despawn(1000, 0); // Despawn the portal spawner since we failed.
		}
		Creature* Sinclari = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), CN_LIEUTNANT_SINCLARI);
		if (Sinclari)
		{
			Sinclari->Despawn(1000, 1000); // Despawn the portal spawner since we failed.
		}
		TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
		if (pInstance)
		{
			pInstance->InstanceFinished(); // Instance is finished because we failed so reset everything
		}
		GameObject* PrisonSeal = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1822.57f, 804.03f, 44.36f, 191723);
		if (PrisonSeal)
		{
			PrisonSeal->SetState(GAMEOBJECT_STATE_OPEN);
		}
		_unit->Despawn(1000, 1000);
	}

	void AIUpdate()
	{
			TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
			if (pInstance)
				pInstance->SetPrisonSealIntegrity(_unit->GetHealthPct());

			if (_unit->GetHealthPct() <= 2 && _sealOpened == false)
			{
				InstanceFailedSealBroken();
				_sealOpened = true;
			}
			ParentClass::AIUpdate();
	}
private:
	bool _sealOpened;
};

class AzureCaptainAI : public MoonScriptCreatureAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(AzureCaptainAI, MoonScriptCreatureAI);
	AzureCaptainAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{
		attackTimer = AddTimer(2000);
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(1000);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		Creature* PrisonSeal = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1822.54f, 803.74f, 44.36f, 30896);
		if (IsTimerFinished(attackTimer))
		{
			if (PrisonSeal)
			_unit->GetAIInterface()->AttackReaction(PrisonSeal, 1, 0);
		}
		ParentClass::AIUpdate();
	}
private:
	int32 attackTimer;
};

class AzureRaiderAI : public MoonScriptCreatureAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(AzureRaiderAI, MoonScriptCreatureAI);
	AzureRaiderAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{
		attackTimer = AddTimer(2000);
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(1000);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		Creature* PrisonSeal = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1822.54f, 803.74f, 44.36f, 30896);
		if (IsTimerFinished(attackTimer))
		{
			if (PrisonSeal)
				_unit->GetAIInterface()->AttackReaction(PrisonSeal, 1, 0);
		}
		ParentClass::AIUpdate();
	}
private:
	int32 attackTimer;
};

class AzureStalkerAI : public MoonScriptCreatureAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(AzureStalkerAI, MoonScriptCreatureAI);
	AzureStalkerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{
		attackTimer = AddTimer(2000);
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(1000);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		Creature* PrisonSeal = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1822.54f, 803.74f, 44.36f, 30896);
		if (IsTimerFinished(attackTimer))
		{
			if (PrisonSeal)
				_unit->GetAIInterface()->AttackReaction(PrisonSeal, 1, 0);
		}
		ParentClass::AIUpdate();
	}
private:
	int32 attackTimer;
};

///////////////////////////////////////////////////////
//Final Boss: Cyanigosa
//class CyanigosaAI : public CreatureAIScript

enum SpellsCyanigosa
{
	SPELL_ARCANE_VACUUM = 58694,
	SPELL_BLIZZARD = 58693,
	SPELL_MANA_DESTRUCTION = 59374,
	SPELL_TAIL_SWEEP = 58690,
	SPELL_UNCONTROLLABLE_ENERGY = 58688,
};
class CyanigosaAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(CyanigosaAI, MoonScriptBossAI);
	CyanigosaAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		timerjump = 0;
		attackable = 0;

		if (IsHeroic() == false)
		{
			AddSpell(SPELL_ARCANE_VACUUM, Target_RandomPlayer, 70, 0, 5);
			AddSpell(SPELL_BLIZZARD, Target_RandomPlayer, 70, 0, 8);
			AddSpell(SPELL_TAIL_SWEEP, Target_RandomPlayer, 70, 0, 10);
			AddSpell(SPELL_UNCONTROLLABLE_ENERGY, Target_Current, 70, 2, 13, 0.0f, 0.0f, false, "Shiver and die!", Text_Yell, 13948);

			AddEmote(Event_OnTargetDied, "I will end the Kirin Tor!", Text_Yell, 13952);
			AddEmote(Event_OnTargetDied, "Dalaran will fall!", Text_Yell, 13953);
		}
		if (IsHeroic())
		{
			AddSpell(SPELL_ARCANE_VACUUM, Target_RandomPlayer, 70, 0, 5);
			AddSpell(SPELL_BLIZZARD, Target_RandomPlayer, 70, 0, 8);
			AddSpell(SPELL_TAIL_SWEEP, Target_RandomPlayer, 70, 0, 10);
			AddSpell(SPELL_UNCONTROLLABLE_ENERGY, Target_Current, 70, 2, 13, 0.0f, 0.0f, false, "Shiver and die!", Text_Yell, 13948);
			AddSpell(SPELL_MANA_DESTRUCTION, Target_RandomPlayer, 70, 0, 15, 0.0f, 0.0f, false, "Am I interrupting?", Text_Yell, 13951);

			AddEmote(Event_OnTargetDied, "I will end the Kirin Tor!", Text_Yell, 13952);
			AddEmote(Event_OnTargetDied, "Dalaran will fall!", Text_Yell, 13953);
		}
	}

	void OnCombatStart(Unit* mTarget)
	{
		SetPhase(1);
		Emote("We finish this now, champions of Kirin Tor!", Text_Yell, 13947);
		ParentClass::OnCombatStart(mTarget);
	}

	void OnDied(Unit* mKiller)
	{
		TheVioletHoldScript* pInstance = (TheVioletHoldScript*)_unit->GetMapMgr()->GetScript();
		pInstance->InstanceFinished();
		GameObject* PrisonSeal = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(1822.57f, 804.03f, 44.36f, 191723);
		if (PrisonSeal)
		{
			Emote("Perhaps... we have... underestimated... you.", Text_Yell, 13955);
			PrisonSeal->SetState(GAMEOBJECT_STATE_OPEN);
		}
        //Achivement: Defenseless
        // If prison seal intergrity is at 100% and crystals aren't used (NYI) then give achievement.
        Creature* PrisonSeal_Npc = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(1822.54f, 803.74f, 44.36f, 30896);
        if (_unit->GetMapMgr()->iInstanceMode == DUNGEON_HEROIC)
        {
            if (PrisonSeal_Npc && PrisonSeal_Npc->GetHealthPct() == 100)
            {
                _unit->GetMapMgr()->SendAchievementToAllPlayers(ACHIEVEMENT_DEFENSELESS);
            }
        }
		ParentClass::OnDied(mKiller);
	}

	void OnLoad()
	{
			Emote("A valiant defense, but this city must be razed. I will fulfill Malygos's wishes myself!", Text_Yell, 13946);
			_unit->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT); //Unattackable
			_unit->CastSpell(_unit, 49411, true);
			RegisterAIUpdateEvent(1000);
			timerjump = AddTimer(6000);
			SetPhase(10);
	}

	void AIUpdate()
	{
		if (IsTimerFinished(timerjump) && GetPhase() == 10)
		{
			_unit->GetAIInterface()->MoveJump(1892.40f, 804.39f, 38.42f, 3.1f, true);
			attackable = AddTimer(6000);
			SetPhase(11);
		}
		if (IsTimerFinished(attackable) && GetPhase() == 11)
		{
			_unit->CastSpell(_unit, 58538, true);
			_unit->SetDisplayId(29549); // dragon id
			_unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
			_unit->SetFaction(14);
			_unit->RemoveAura(49411);
			SetPhase(12);
		}
		ParentClass::AIUpdate();
	}
protected:
	int32 timerjump;
	int32 attackable;
};


void SetupTheVioletHold(ScriptMgr* mgr)
{
    //Instance
    mgr->register_instance_script(MAP_VIOLET_HOLD, &TheVioletHoldScript::Create);

    //Sinclari and Guards
    mgr->register_creature_script(CN_LIEUTNANT_SINCLARI, &SinclariAI::Create);
    mgr->register_creature_script(CN_VIOLET_HOLD_GUARD, &VHGuardsAI::Create);

	//Trash
	mgr->register_creature_script(29395, &ErekemGuardAI::Create);
	mgr->register_creature_script(NPC_ETHEREAL_SPHERE, &EtherealSphereAI::Create);
	mgr->register_creature_script(NPC_ETHEREAL_SPHERE2, &EtherealSphereAI::Create);
	mgr->register_creature_script(CN_AZURE_CAPTAIN, &AzureCaptainAI::Create);
	mgr->register_creature_script(CN_AZURE_STALKER, &AzureStalkerAI::Create);
	mgr->register_creature_script(CN_AZURE_RAIDER, &AzureRaiderAI::Create);
	mgr->register_creature_script(30896, &PrisonSealDoorAI::Create);
	mgr->register_creature_script(CN_AZURE_BINDER, &AzureBinderAI::Create);

    //Bosses
	mgr->register_creature_script(43283, &VioletHoldSpawnHandler::Create);
	mgr->register_creature_script(CN_PORTAL_INTRO, &POrtalAI::Create);
    mgr->register_creature_script(CN_EREKEM, &ErekemAI::Create);
    mgr->register_creature_script(CN_MORAGG, &MoraggAI::Create);
    mgr->register_creature_script(CN_ICHORON, &IchoronAI::Create);
    mgr->register_creature_script(CN_XEVOZZ, &XevozzAI::Create);
	mgr->register_creature_script(CN_LAVANTHOR, &LavanthosAI::Create);
    mgr->register_creature_script(CN_TURAMAT_THE_OBLITERATOR, &ZuramatTheObliteratorAI::Create);
    mgr->register_creature_script(CN_CYANIGOSA, &CyanigosaAI::Create);

    GossipScript* GSinclari = new SinclariGossip;
    mgr->register_gossip_script(CN_LIEUTNANT_SINCLARI, GSinclari);


}