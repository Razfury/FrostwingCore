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
#include "Dungeon_UtgardeKeep.h"

//////////////////////////////////////////////////////////////////////////
//////// Utgarde Keep Instance script

enum UtgardeData
{
    UTGARDE_FORGE_MASTER,

    // Bosses
    UTGARDE_KELESETH,
    UTGARDE_SKARVALD_AND_DALRONN,
    UTGARDE_INGVAR,

    UTGARDE_DATA_END
};

struct ForgeMasterData
{
    ForgeMasterData(uint32 pB = 0, uint32 pF = 0, uint32 pA = 0)
    {
        mBellow = pB;
        mFire = pF;
        mAnvil = pA;
    }

    uint32 mBellow;
    uint32 mFire;
    uint32 mAnvil;
};

class UtgardeKeepScript : public MoonInstanceScript
{
    public:
        uint32        mKelesethGUID;
        uint32        mSkarvaldGUID;
        uint32        mDalronnGUID;
        uint32        mIngvarGUID;

        ForgeMasterData m_fmData[3];
        uint32        mDalronnDoorsGUID;
        uint32        mIngvarDoors[2];

        uint8        mUtgardeData[UTGARDE_DATA_END];

        MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(UtgardeKeepScript, MoonInstanceScript);
        UtgardeKeepScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
        {
            mKelesethGUID = 0;
            mSkarvaldGUID = 0;
            mDalronnGUID = 0;
            mIngvarGUID = 0;

            for (uint8 i = 0; i < 3; ++i)
                m_fmData[i] = ForgeMasterData();

            mDalronnDoorsGUID = 0;

            for (uint8 i = 0; i < 2; ++i)
                mIngvarDoors[i] = 0;

            for (uint8 i = 0; i < UTGARDE_DATA_END; ++i)
                mUtgardeData[i] = 0;
        };

        void OnCreaturePushToWorld(Creature* pCreature)
        {
            switch (pCreature->GetEntry())
            {
                case CN_PRINCE_KELESETH:
                    mKelesethGUID = pCreature->GetLowGUID();
                    break;
                case CN_SKARVALD:
                    mSkarvaldGUID = pCreature->GetLowGUID();
                    break;
                case CN_DALRONN:
                    mDalronnGUID = pCreature->GetLowGUID();
                    break;
                case CN_INGVAR:
                    mIngvarGUID = pCreature->GetLowGUID();
                    break;
            }
        };

        void OnGameObjectPushToWorld(GameObject* pGameObject)
        {
            switch (pGameObject->GetEntry())
            {
                case BELLOW_1:
                    m_fmData[0].mBellow = pGameObject->GetLowGUID();
                    break;
                case BELLOW_2:
                    m_fmData[1].mBellow = pGameObject->GetLowGUID();
                    break;
                case BELLOW_3:
                    m_fmData[2].mBellow = pGameObject->GetLowGUID();
                    break;
                case FORGEFIRE_1:
                    m_fmData[0].mFire = pGameObject->GetLowGUID();
                    break;
                case FORGEFIRE_2:
                    m_fmData[1].mFire = pGameObject->GetLowGUID();
                    break;
                case FORGEFIRE_3:
                    m_fmData[2].mFire = pGameObject->GetLowGUID();
                    break;
                case GLOWING_ANVIL_1:
                    m_fmData[0].mAnvil = pGameObject->GetLowGUID();
                    break;
                case GLOWING_ANVIL_2:
                    m_fmData[1].mAnvil = pGameObject->GetLowGUID();
                    break;
                case GLOWING_ANVIL_3:
                    m_fmData[2].mAnvil = pGameObject->GetLowGUID();
                    break;
                case DALRONN_DOORS:
                    mDalronnDoorsGUID = pGameObject->GetLowGUID();
                    break;
                case INGVAR_DOORS_1:
                    mIngvarDoors[0] = pGameObject->GetLowGUID();
                    break;
                case INGVAR_DOORS_2:
                    mIngvarDoors[1] = pGameObject->GetLowGUID();
                    break;
            }
        };

        void SetInstanceData(uint32 pType, uint32 pIndex, uint32 pData)
        {
            switch (pIndex)
            {
                case UTGARDE_FORGE_MASTER:
                    {
                        mUtgardeData[UTGARDE_FORGE_MASTER]++;
                        HandleForge();
                    };
                    break;
                case UTGARDE_INGVAR:
                    {
                        mUtgardeData[UTGARDE_INGVAR] = pData;

                        if (pData == State_Finished)
                        {
                            GameObject* pGO = NULL;
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                pGO = GetGameObjectByGuid(mIngvarDoors[i]);
                                if (pGO)
                                    pGO->SetState(pGO->GetState() == 1 ? 0 : 1);
                            };
                        };
                    }
                    break;
            }


        };

        void HandleForge()
        {
            GameObject* pGO = NULL;
            pGO = GetGameObjectByGuid(m_fmData[mUtgardeData[UTGARDE_FORGE_MASTER] - 1].mBellow);
            if (pGO)
                pGO->SetState(pGO->GetState() == 1 ? 0 : 1);

            pGO = GetGameObjectByGuid(m_fmData[mUtgardeData[UTGARDE_FORGE_MASTER] - 1].mFire);
            if (pGO)
                pGO->SetState(pGO->GetState() == 1 ? 0 : 1);

            pGO = GetGameObjectByGuid(m_fmData[mUtgardeData[UTGARDE_FORGE_MASTER] - 1].mAnvil);
            if (pGO)
                pGO->SetState(pGO->GetState() == 1 ? 0 : 1);
        };
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Forge Master
class DragonflayerForgeMasterAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerForgeMasterAI, HybridCreatureScriptAI);
        DragonflayerForgeMasterAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            mInstance = GetInstanceScript();

            AddSpell(DRAGONFLAYER_FORGE_MASTER_BURNING_BRAND, Target_Current, 8, 0, 40, 0, 30);
        }

		void OnCombatStart(Unit* pTarget)
		{
			if (mInstance)
				mInstance->SetInstanceData(Data_UnspecifiedType, UTGARDE_FORGE_MASTER, 0);

			ParentClass::OnCombatStart(pTarget);
		};

        MoonInstanceScript* mInstance;
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer HeartSplitter
class DragonflayerHeartSplitterAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerHeartSplitterAI, HybridCreatureScriptAI);
        DragonflayerHeartSplitterAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_HEARTSPLITTER_PIERCING_JAB, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_HEARTSPLITTER_THROW, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_HEARTSPLITTER_WING_CLIP, Target_Current, 8, 0, 40, 0, 30);
        }
};


//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Ironhelm
class DragonflayerIronhelmAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerIronhelmAI, HybridCreatureScriptAI);
        DragonflayerIronhelmAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_IRONHELM_HEROIC_STRIKE, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_IRONHELM_RINGING_SLAP, Target_Current, 8, 0, 40, 0, 30);
        }
};


//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Metalworker
class DragonflayerMetalworkerAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerMetalworkerAI, HybridCreatureScriptAI);
        DragonflayerMetalworkerAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_METALWORKER_SUNDER_ARMOR, Target_Current, 8, 0, 40, 0, 30);
            mDfEnrage = AddSpell(DRAGONFLAYER_METALWORKER_ENRAGE, Target_Self, 0, 0, 0);
            Enrage = true;
        }

        void AIUpdate()
        {
            if (GetHealthPercent() <= 20 && Enrage)
            {
                CastSpell(mDfEnrage);
                Enrage = false;
            }

            ParentClass::AIUpdate();
        }

        bool Enrage;
        SpellDesc* mDfEnrage;
};

//////////////////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Overseer
class DragonflayerOverseerAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerOverseerAI, HybridCreatureScriptAI);
        DragonflayerOverseerAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_OVERSEER_BATTLE_SHOUT, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_OVERSEER_CHARGE, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_OVERSEER_DEMORALIZING_SHOUT, Target_Current, 8, 0, 40, 0, 30);
        }
};

//////////////////////////////////////////////////////////////////////////////////////
//////// Tunneling Ghoul
class TunnelingGhoulAI : public HybridCreatureScriptAI
{
    HYBRIDSCRIPT_FUNCTION(TunnelingGhoulAI, HybridCreatureScriptAI);
    TunnelingGhoulAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {
        AddSpell(TUNNELING_GHOUL_DECREPIFY, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(TUNNELING_GHOUL_STRIKE, Target_Current, 8, 0, 40, 0, 30);
    }

	void OnLoad()
	{
		_unit->SetEmoteState(EMOTE_ONESHOT_ATTACK1H);
		ParentClass::OnLoad();
	}
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Runecaster
class DragonflayerRunecasterAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerRunecasterAI, HybridCreatureScriptAI);
        DragonflayerRunecasterAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_RUNECASTER_BOLTHORNS_RUNE_OF_FLAME, Target_Self , 100, 0, 0);
            AddSpell(DRAGONFLAYER_RUNECASTER_NJORDS_RUNE_OF_PROTECTION, Target_Self , 100, 0, 0);
        }
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Spiritualist
class DragonflayerSpiritualistAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerSpiritualistAI, HybridCreatureScriptAI);
        DragonflayerSpiritualistAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_SPIRITUALIST_FLAME_SHOCK, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_SPIRITUALIST_LIGHTNING_BOLT, Target_RandomPlayerNotCurrent, 8, 0, 40, 0, 30);
            mHealDf = AddSpell(DRAGONFLAYER_SPIRITUALIST_HEALING_WAVE, Target_Self, 0, 0, 2);
            Heal = true;
        }

        void AIUpdate()
        {
            if (GetHealthPercent() <= 42 && Heal == true)
            {
                CastSpell(mHealDf);
                Heal = false;
            }

            ParentClass::AIUpdate();
        }

        bool Heal;
        SpellDesc* mHealDf;
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Strategist
class DragonflayerStrategistAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DragonflayerStrategistAI, HybridCreatureScriptAI);
        DragonflayerStrategistAI(Creature*  pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(DRAGONFLAYER_STRATEGIST_BLIND, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_STRATEGIST_HURL_DAGGER, Target_Current, 8, 0, 40, 0, 30);
            AddSpell(DRAGONFLAYER_STRATEGIST_TICKING_BOMB, Target_Current, 8, 0, 40, 0, 30);
        }
};

//////////////////////////////////////////////////////////////////////////
//////// Proto-Drake Handler
class ProtoDrake_HandlerAI : public HybridCreatureScriptAI
{
    HYBRIDSCRIPT_FUNCTION(ProtoDrake_HandlerAI, HybridCreatureScriptAI);
    ProtoDrake_HandlerAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {
        AddSpell(PROTO_DRAKE_HANDLER_DEBILITATING_STRIKE, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(PROTO_DRAKE_HANDLER_THROW, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(PROTO_DRAKE_HANDLER_UNHOLY_RAGE, Target_Current, 8, 0, 40, 0, 30);
    }
};

//////////////////////////////////////////////////////////////////////////
//////// Frenzied Geist
class FrenziedGeistAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(FrenziedGeistAI, HybridCreatureScriptAI);
        FrenziedGeistAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(FRENZIED_GEIST_FIXATE, Target_Current, 8, 0, 40, 0, 30);
        }
};

//////////////////////////////////////////////////////////////////////////
//////// Savage Worg
class SavageWorgAI : public HybridCreatureScriptAI
{
    HYBRIDSCRIPT_FUNCTION(SavageWorgAI, HybridCreatureScriptAI);
    SavageWorgAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {
        AddSpell(SAVAGE_WORG_ENRAGE, Target_Self, 8, 0, 40, 0, 30);
        AddSpell(SAVAGE_WORG_POUNCE, Target_Current, 8, 0, 40, 0, 30);
    }
};

//////////////////////////////////////////////////////////////////////////
//////// Dragonflayer Bonecrusher
class DragonflayerBonecrusherAI : public HybridCreatureScriptAI
{
    HYBRIDSCRIPT_FUNCTION(DragonflayerBonecrusherAI, HybridCreatureScriptAI);
    DragonflayerBonecrusherAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {
        AddSpell(DRAGONFLAYER_BONECRUSHER_HEAD_CRACK, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(DRAGONFLAYER_BONECRUSHER_KNOCKDOWNSPIN, Target_Self, 8, 0, 40, 0, 30);
    }
};

//////////////////////////////////////////////////////////////////////////
//////// Proto-Drake Rider
class ProtoDrake_RiderAI : public HybridCreatureScriptAI
{
    HYBRIDSCRIPT_FUNCTION(ProtoDrake_RiderAI, HybridCreatureScriptAI);
    ProtoDrake_RiderAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {
        AddSpell(PROTO_DRAKE_RIDER_PIERCING_JAB, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(PROTO_DRAKE_RIDER_THROW, Target_Current, 8, 0, 40, 0, 30);
        AddSpell(PROTO_DRAKE_RIDER_WING_CLIP, Target_Current, 8, 0, 40, 0, 30);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Skarvald the Constructor  & Dalronn the Controller  ///////////////////
//////////////////////////////////////////////////////////////////////////////////////////
enum SkarDalronnTexts
{
	//signed for 24200, but used by 24200, 27390
	YELL_SKARVALD_AGGRO = 4471,
	YELL_SKARVALD_DAL_DIED = 4473,
	YELL_SKARVALD_SKA_DIEDFIRST = 4472,
	YELL_SKARVALD_KILL = 4474,
	YELL_SKARVALD_DAL_DIEDFIRST = 4475,

	//signed for 24201, but used by 24201, 27389
	YELL_DALRONN_AGGRO = 4481,
	YELL_DALRONN_SKA_DIED = 4483,
	YELL_DALRONN_DAL_DIEDFIRST = 4482,
	YELL_DALRONN_KILL = 4484,
	YELL_DALRONN_SKA_DIEDFIRST = 4485,
};

enum SkarDalronnSpells
{
	// Skarvald:
	SKARVALD_SPELL_CHARGE = 43651,
	SKARVALD_SPELL_STONE_STRIKE = 48583,
	SKARVALD_SPELL_SUMMON_SKARVALD_GHOST = 48613,

	// Dalronn:
	DALR_SPELL_SHADOW_BOLT_N = 43649,
	DALR_SPELL_SHADOW_BOLT_H = 59575,
	DALR_SPELL_DEBILITATE = 43650,
	DALR_SPELL_SUMMON_SKELETONS = 52611,
	DALR_SPELL_SUMMON_DALRONN_GHOST = 48612,
};

enum SkarDalronnEvents
{
	SKARDAL_EVENT_SPELL_CHARGE = 1,
	SKARDAL_EVENT_SPELL_STONE_STRIKE,

	SKARDAL_EVENT_SPELL_SHADOW_BOLT,
	SKARDAL_EVENT_SPELL_DEBILITATE,
	SKARDAL_EVENT_SPELL_SUMMON_SKELETONS,

	SKARDAL_EVENT_YELL_DALRONN_AGGRO,
	SKARDAL_EVENT_MATE_DIED,
};
// Skarvald the Constructor
class SkarvaldTheConstructorAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(SkarvaldTheConstructorAI, HybridCreatureScriptAI);
        SkarvaldTheConstructorAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            
        };

		void OnCombatStop(Unit* pTarget)
		{
			events.Reset();
			ParentClass::OnCombatStop(pTarget);
		}

		void DoAction()
		{
			events.RescheduleEvent(SKARDAL_EVENT_MATE_DIED, 3500);
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

		void OnTargetDied(Unit* pTarget)
		{
			_unit->SendScriptTextChatMessage(YELL_SKARVALD_KILL);
			ParentClass::OnTargetDied(pTarget);
		};

		void OnDied(Unit* pTarget)
		{
			if (ForceCreatureFind(CN_DALRONN) && ForceCreatureFind(CN_DALRONN)->IsDead()) // If dalronn is dead
			{
				_unit->SendScriptTextChatMessage(YELL_SKARVALD_SKA_DIEDFIRST);
				return;
			}
			else // Dalronn is not dead
			{
				_unit->SendScriptTextChatMessage(YELL_SKARVALD_DAL_DIEDFIRST);
				if (ForceCreatureFind(CN_DALRONN))
					ForceCreatureFind(CN_DALRONN)->SendChatMessage(14, 0, "Skarvald, you incompetent slug! Return and make yourself useful!");
					_unit->PlaySoundToSet(13203);
			}
			_unit->CastSpell((Unit*)NULL, SKARVALD_SPELL_SUMMON_SKARVALD_GHOST, true);
			ParentClass::OnDied(pTarget);
		}

        void OnCombatStart(Unit* pTarget)
        {
			events.Reset();
			events.RescheduleEvent(SKARDAL_EVENT_SPELL_CHARGE, 5000);
			events.RescheduleEvent(SKARDAL_EVENT_SPELL_STONE_STRIKE, 10000);
			_unit->SendScriptTextChatMessage(YELL_SKARVALD_AGGRO);

            ParentClass::OnCombatStart(pTarget);
        };

        void AIUpdate()
        {
			events.Update(1000);

			if (_unit->IsCasting())
				return;

			switch (events.GetEvent())
			{
			case 0:
				break;
			case SKARDAL_EVENT_MATE_DIED:
				_unit->SendScriptTextChatMessage(YELL_SKARVALD_DAL_DIEDFIRST);
				events.PopEvent();
				break;
			case SKARDAL_EVENT_SPELL_CHARGE:
				if (Unit* target = GetRandomPlayerTarget())
				{
					_unit->CastSpell(target, SKARVALD_SPELL_CHARGE, false);
				}
				events.RepeatEvent(RandomUInt(5000, 10000));
				break;
			case SKARDAL_EVENT_SPELL_STONE_STRIKE:
				if (_unit->GetVictim())
				{
					_unit->CastSpell(_unit->GetVictim(), SKARVALD_SPELL_STONE_STRIKE, false);
					events.RepeatEvent(RandomUInt(5000, 10000));
				}
				else
					events.RepeatEvent(3000);
				break;
			}
            ParentClass::AIUpdate();
        };
};

// Dalronn the Controller
class DalronnTheControllerAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DalronnTheControllerAI, HybridCreatureScriptAI);
        DalronnTheControllerAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {};

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

        void OnCombatStart(Unit* pTarget)
        {
			events.Reset();
			events.RescheduleEvent(SKARDAL_EVENT_SPELL_SHADOW_BOLT, 1000);
			events.RescheduleEvent(SKARDAL_EVENT_SPELL_DEBILITATE, 5000);
			events.RescheduleEvent(SKARDAL_EVENT_YELL_DALRONN_AGGRO, 4999);
			if (IsHeroic())
				events.RescheduleEvent(SKARDAL_EVENT_SPELL_SUMMON_SKELETONS, 10000);
            ParentClass::OnCombatStart(pTarget);
        };

		void OnCombatStop(Unit* pTarget)
		{
			events.Reset();
			if (ForceCreatureFind(CN_DALRONN_GHOST))
			{
				TO_CREATURE(ForceCreatureFind(CN_DALRONN_GHOST))->Despawn(1000, 0);
			}
			ParentClass::OnCombatStop(pTarget);
		};

		void OnDied(Unit* pKiller)
		{
			if (ForceCreatureFind(CN_SKARVALD) && ForceCreatureFind(CN_SKARVALD)->IsDead()) // If skarvald is dead
			{
				_unit->SendScriptTextChatMessage(YELL_DALRONN_DAL_DIEDFIRST);
				return;
			}
			else // skarvald is not dead
			{
				_unit->SendScriptTextChatMessage(YELL_DALRONN_DAL_DIEDFIRST);
				if (ForceCreatureFind(CN_SKARVALD))
					ForceCreatureFind(CN_SKARVALD)->SendChatMessage(14, 0, "Pfagh! What sort of necromancer lets death stop him? I knew you were worthless!");
				_unit->PlaySoundToSet(13233);
			}
			_unit->CastSpell((Unit*)NULL, DALR_SPELL_SUMMON_DALRONN_GHOST, true);
			ParentClass::OnDied(pKiller);
		};

        void AIUpdate()
        {
			events.Update(1000);

			if (_unit->IsCasting())
				return;

			switch (events.GetEvent())
			{
			case 0:
				break;
			case SKARDAL_EVENT_YELL_DALRONN_AGGRO:
				_unit->SendScriptTextChatMessage(YELL_DALRONN_AGGRO);
				events.PopEvent();
				break;
			case SKARDAL_EVENT_MATE_DIED:
				_unit->SendScriptTextChatMessage(YELL_DALRONN_SKA_DIEDFIRST);
				events.PopEvent();
				break;
			case SKARDAL_EVENT_SPELL_SHADOW_BOLT:
				if (Unit* target = GetRandomPlayerTarget())
					if (IsHeroic())
					{
						_unit->CastSpell(target, DALR_SPELL_SHADOW_BOLT_H, false);
					}
					else
					{
						_unit->CastSpell(target, DALR_SPELL_SHADOW_BOLT_N, false);
					}
				events.RepeatEvent(2500);
				break;
			case SKARDAL_EVENT_SPELL_DEBILITATE:
				if (Unit* target = GetRandomPlayerTarget())
				{
					_unit->CastSpell(target, DALR_SPELL_DEBILITATE, false);
					events.RepeatEvent(RandomUInt(5000, 10000));
				}
				else
					events.RepeatEvent(3000);
				break;
			case SKARDAL_EVENT_SPELL_SUMMON_SKELETONS:
				_unit->CastSpell((Unit*)NULL, DALR_SPELL_SUMMON_SKELETONS, false);
				events.RepeatEvent(RandomUInt(20000, 30000));
				break;
			}

            ParentClass::AIUpdate();
		};
};

class SkarvaldTheConstructorGhostAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(SkarvaldTheConstructorGhostAI, HybridCreatureScriptAI);
        SkarvaldTheConstructorGhostAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            AddSpell(SKARVALD_CHARGE, Target_RandomPlayerNotCurrent, 35, 0, 8);
            AddSpell(STONE_STRIKE, Target_ClosestPlayer, 25, 0, 10);
        };

        void OnLoad()
        {
            _unit->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

            Player* pTarget = GetNearestPlayer();
            if (pTarget != NULL)
                _unit->GetAIInterface()->AttackReaction(pTarget, 50, 0);

            ParentClass::OnLoad();
        };

		void AIUpdate()
		{
			if (ForceCreatureFind(CN_DALRONN) == NULL) // Cannot find Dalronn so just despawn.
			{
				TO_CREATURE(ForceCreatureFind(CN_DALRONN))->Despawn(1000, 0);
			}
			if (ForceCreatureFind(CN_DALRONN) && ForceCreatureFind(CN_DALRONN)->CombatStatus.IsInCombat() == false) // Found Dalronn but is NOT in combat. Assume death and despawn.
			{
				TO_CREATURE(ForceCreatureFind(CN_SKARVALD))->Despawn(1000, 0);
			}
				ParentClass::AIUpdate();
		}
};

class DalronnTheControllerGhostAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(DalronnTheControllerGhostAI, HybridCreatureScriptAI);
        DalronnTheControllerGhostAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
            if (IsHeroic())
            {
                AddSpell(SHADOW_BOLT_HC, Target_RandomPlayer, 85, 2, 3);
                AddSpell(DEBILITATE, Target_RandomPlayer, 25, 0, 12);
            }
            else
            {
                AddSpell(SHADOW_BOLT, Target_RandomPlayer, 35, 2, 8);
                AddSpell(DEBILITATE, Target_RandomPlayer, 25, 0, 12);
            }
        };

        void OnLoad()
        {
            _unit->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

            Player* pTarget = GetNearestPlayer();
            if (pTarget != NULL)
                _unit->GetAIInterface()->AttackReaction(pTarget, 50, 0);

            ParentClass::OnLoad();
        };

		void AIUpdate()
		{
			if (ForceCreatureFind(CN_SKARVALD) == NULL) // Cannot find skarvald so just despawn.
			{
				TO_CREATURE(ForceCreatureFind(CN_SKARVALD))->Despawn(1000, 0);
			}
			if (ForceCreatureFind(CN_SKARVALD) && ForceCreatureFind(CN_SKARVALD)->CombatStatus.IsInCombat() == false) // Found skarvald but is NOT in combat.
			{
				TO_CREATURE(ForceCreatureFind(CN_SKARVALD))->Despawn(1000, 0);
			}
			ParentClass::AIUpdate();
		}

};

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Prince Keleseth ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
enum KelesethTexts
{
	KELESETH_SAY_START_COMBAT = 500,
	KELESETH_SAY_FROST_TOMB = 502,
	KELESETH_SAY_SUMMON_SKELETONS = 501,
	KELESETH_SAY_FROST_TOMB_EMOTE = 503,
	KELESETH_SAY_DEATH = 504,
};

enum eNPCs
{
	KELESETH_NPC_FROST_TOMB = 23965,
	KELESETH_NPC_SKELETON = 23970,
};

enum eSpells
{
	KELESETH_SPELL_FROST_TOMB = 42672,
	KELESETH_SPELL_FROST_TOMB_SUMMON = 42714,
	KELESETH_SPELL_FROST_TOMB_AURA = 48400,

	KELESETH_SPELL_SHADOWBOLT_N = 43667,
	KELESETH_SPELL_SHADOWBOLT_H = 59389,
};
enum KelesethEvents
{
	KELESETH_EVENT_SPELL_SHADOWBOLT = 1,
	KELESETH_EVENT_FROST_TOMB,
	KELESETH_EVENT_SUMMON_SKELETONS,
};

class PrinceKelesethAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(PrinceKelesethAI, HybridCreatureScriptAI);
        PrinceKelesethAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {}

		void OnLoad()
		{			
			ParentClass::OnLoad();
		}

		void OnCombatStop(Unit* pTarget)
		{
			events.Reset();
			ParentClass::OnCombatStop(pTarget);
		}

        void OnCombatStart(Unit* pTarget)
        {
			events.Reset();
			events.RescheduleEvent(KELESETH_EVENT_SPELL_SHADOWBOLT, 0);
			events.RescheduleEvent(KELESETH_EVENT_FROST_TOMB, 28000);
			events.RescheduleEvent(KELESETH_EVENT_SUMMON_SKELETONS, 4000);
			_unit->SendScriptTextChatMessage(KELESETH_SAY_START_COMBAT);
            ParentClass::OnCombatStart(pTarget);
        }

        void OnDied(Unit* pTarget)
        {
            _unit->SendScriptTextChatMessage(KELESETH_SAY_DEATH);      // I join... the night.
			ParentClass::OnDied(pTarget);
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
			case KELESETH_EVENT_SPELL_SHADOWBOLT:
				if (IsHeroic())
				{
					_unit->CastSpell(_unit->GetVictim(), KELESETH_SPELL_SHADOWBOLT_H, false);
					events.RepeatEvent(RandomUInt(4000, 5000));
				}
				else
				{
					_unit->CastSpell(_unit->GetVictim(), KELESETH_SPELL_SHADOWBOLT_N, false);
					events.RepeatEvent(RandomUInt(4000, 5000));
				}
				break;
			case KELESETH_EVENT_FROST_TOMB:
				if (Unit* target = GetRandomPlayerTarget())
					if (!target->HasAura(KELESETH_SPELL_FROST_TOMB_AURA))
					{
						string msg2 = "Prince Keleseth casts Frost Tomb on ";
						msg2 += TO_PLAYER(target)->GetName();
						Announce(msg2.c_str());
						_unit->SendScriptTextChatMessage(KELESETH_SAY_FROST_TOMB);
						_unit->CastSpell(target, KELESETH_SPELL_FROST_TOMB, false);
						_unit->CastSpell(target, KELESETH_SPELL_FROST_TOMB_AURA, true);
						_unit->GetMapMgr()->GetInterface()->SpawnCreature(CN_FROST_TOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, true, false, 0, 0);
						events.RepeatEvent(15000);
						break;
					}
				events.RepeatEvent(1000);
				break;
			case KELESETH_EVENT_SUMMON_SKELETONS:
				_unit->SendScriptTextChatMessage(KELESETH_SAY_SUMMON_SKELETONS);
				for (uint8 i = 0; i < 5; ++i)
				{
					const int arrayNum[11] = { '0.0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0' };
					int RandIndex = rand() % 11;
					cout << arrayNum[RandIndex];
					float dist = RandIndex * 4 + 3.0f;
					float angle = RandIndex * 2 * M_PI;
					if (Creature* c = _unit->GetMapMgr()->GetInterface()->SpawnCreature(KELESETH_NPC_SKELETON, 156.2f + cos(angle)*dist, 259.1f + sin(angle)*dist, 42.9f, 0, true, false, 0, 0))
						if (Unit* target = GetRandomPlayerTarget())
						{
							c->GetAIInterface()->AttackReaction(target, 1, 0);
						}
				}
				events.PopEvent();
				break;
			}
			ParentClass::AIUpdate();
		}
};

// FrostTombAI
class FrostTombAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(FrostTombAI, HybridCreatureScriptAI);
        FrostTombAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
			RegisterAIUpdateEvent(1000);
            plr = nullptr;
        };

		void OnCombatStart(Unit* pTarget)
		{
			plr = TO_PLAYER(_unit->GetVictim());
			Despawn(20100);
			_unit->CastSpell(_unit, 62766, true); // Ice Block visual
			_unit->SetRoot(); // root us so we can't move
			ParentClass::OnCombatStart(pTarget);
		}

        void OnDied(Unit* pKilled)
        {
			if (plr != nullptr && plr->HasAura(KELESETH_SPELL_FROST_TOMB_AURA))
				//plr->Unroot();
				plr->RemoveAura(KELESETH_SPELL_FROST_TOMB_AURA);
			_unit->RemoveAura(62766);
				Despawn(1000);
            ParentClass::OnDied(pKilled);
        };

    private:
        Player* plr;
};

enum eSkeletonEnum
{
	SKELE_SPELL_DECREPIFY = 42702,
	SKELE_SPELL_BONE_ARMOR = 59386,
	SKELE_SPELL_SCOURGE_RESURRECTION = 42704,

	SKELE_EVENT_SPELL_DECREPIFY = 1,
	SKELE_EVENT_SPELL_BONE_ARMOR,
	SKELE_EVENT_RESURRECT,
	SKELE_EVENT_RESURRECT_2,
};
class SkeletonAddAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(SkeletonAddAI, HybridCreatureScriptAI);
        SkeletonAddAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {};

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
			_unit->IsKilledByScript = true;
			if (GetRandomPlayerTarget() != NULL)
				_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);

            ParentClass::OnLoad();
        };

        void OnCombatStop(Unit* pTarget)
        {
			events.Reset();
			events.RescheduleEvent(SKELE_EVENT_SPELL_DECREPIFY, RandomUInt(10000, 20000));
			if (IsHeroic())
				events.RescheduleEvent(SKELE_EVENT_SPELL_BONE_ARMOR, RandomUInt(25000, 120000));
        };

		void AIUpdate()
		{
			if (ForceCreatureFind(CN_PRINCE_KELESETH) && ForceCreatureFind(CN_PRINCE_KELESETH)->IsDead()) // Boss is dead so despawn
			{
				_unit->Despawn(500, 0);
			}
			if (_unit->GetHealth() == 1 && _deathPhase == false) // Our version of OnDamageTaken
			{
				_unit->InterruptSpell();
				_unit->RemoveAllAuras();
				_unit->GetAIInterface()->disable_melee = true;
				_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				_unit->GetAIInterface()->StopMovement(0);
				_unit->SetRoot();
				_unit->SetStandState(7);
				_unit->SetFlag(UNIT_FIELD_FLAGS, 536870912);
				_unit->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
				_unit->SetFlag(UNIT_DYNAMIC_FLAGS, 32);
				events.RescheduleEvent(SKELE_EVENT_RESURRECT, 12000);
				_deathPhase = true;
			}

			events.Update(1000);
			if (_unit->IsCasting())
				return;

			switch (events.GetEvent())
			{
			case 0:
				break;
			case SKELE_EVENT_SPELL_DECREPIFY:
				if (!_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2))
					_unit->CastSpell(_unit->GetVictim(), SKELE_SPELL_DECREPIFY, false);
				events.RepeatEvent(RandomUInt(15000, 25000));
				break;
			case SKELE_EVENT_SPELL_BONE_ARMOR:
				if (!_unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2))
					_unit->CastSpell((Unit*)NULL, SKELE_SPELL_BONE_ARMOR, false);
				events.RepeatEvent(RandomUInt(40000, 120000));
				break;
			case SKELE_EVENT_RESURRECT:
				events.PopEvent();
				events.DelayEvents(3500);
				_unit->CastSpell(_unit, SKELE_SPELL_SCOURGE_RESURRECTION, true);
				_unit->SetStandState(0);
				_unit->RemoveFlag(UNIT_FIELD_FLAGS, 536870912);
				_unit->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
				_unit->RemoveFlag(UNIT_DYNAMIC_FLAGS, 32);
				events.RescheduleEvent(SKELE_EVENT_RESURRECT_2, 3000);
				break;
			case SKELE_EVENT_RESURRECT_2:
				_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
				_unit->GetAIInterface()->disable_melee = false;
				_unit->UnsetRoot();
				_unit->GetAIInterface()->AttackReaction(_unit->GetVictim(), 1, 0);
				_deathPhase = false;
				events.PopEvent();
				break;
			}
			ParentClass::AIUpdate();
		}
private:
	bool _deathPhase = false;
};

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Ingvar the Plunderer //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void SpellFunc_ShadowAxe(SpellDesc* pThis, HybridCreatureScriptAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    if (pCreatureAI != NULL)
    {
        if (pTarget == NULL || !pTarget->IsPlayer() || pTarget->IsDead())
            return;

        Creature* pShadowAxe = pTarget->GetMapMgr()->GetInterface()->SpawnCreature(CN_SHADOW_AXE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation(), true, false, 0, 0);

        if (pShadowAxe == NULL)
            return;

        pShadowAxe->CastSpell(pShadowAxe, SHADOW_AXE_SPELL, true);
        pShadowAxe->Despawn(10000, 0);
    };
};

enum IngvarDisplayId
{
	DISPLAYID_DEFAULT = 21953,
	DISPLAYID_UNDEAD = 26351,
};

enum IngvarNPCs
{
	NPC_INGVAR_UNDEAD = 23980,
	NPC_ANNHYLDE = 24068,
	NPC_THROW = 23997,
};

enum IngvarYells
{
	//Yells Ingvar
	INGVAR_YELL_AGGRO_1 = 0,
	INGVAR_YELL_KILL_1 = 1,
	INGVAR_YELL_DEAD_1 = 2,

	INGVAR_YELL_AGGRO_2 = 3,
	INGVAR_YELL_KILL_2 = 4,
	INGVAR_YELL_DEAD_2 = 5,

	INGVAR_EMOTE_ROAR = 6,
	INGVAR_YELL_ANHYLDE_1 = 0,
	INGVAR_YELL_ANHYLDE_2 = 1,
};

enum IngvarSpells
{
	INGVAR_SPELL_SUMMON_VALKYR = 42912,
	INGVAR_SPELL_RESURRECTION_BEAM = 42857,
	INGVAR_SPELL_RESURRECTION_BALL = 42862,
	INGVAR_SPELL_RESURRECTION_HEAL = 42704,
	INGVAR_SPELL_INGVAR_TRANSFORM = 42796,

	INGVAR_SPELL_STAGGERING_ROAR_N = 42708,
	INGVAR_SPELL_STAGGERING_ROAR_H = 59708,
	INGVAR_SPELL_CLEAVE = 42724,
	INGVAR_SPELL_SMASH_N = 42669,
	INGVAR_SPELL_SMASH_H = 59706,
	INGVAR_SPELL_ENRAGE_N = 42705,
	INGVAR_SPELL_ENRAGE_H = 59707,

	INGVAR_SPELL_DREADFUL_ROAR_N = 42729,
	INGVAR_SPELL_DREADFUL_ROAR_H = 59734,
	INGVAR_SPELL_WOE_STRIKE_N = 42730,
	INGVAR_SPELL_WOE_STRIKE_H = 59735,
	INGVAR_SPELL_DARK_SMASH = 42723,
	INGVAR_SPELL_SHADOW_AXE = 42749,
};

#define SPELL_STAGGERING_ROAR		DUNGEON_MODE(SPELL_STAGGERING_ROAR_N, SPELL_STAGGERING_ROAR_H)
#define SPELL_DREADFUL_ROAR			DUNGEON_MODE(SPELL_DREADFUL_ROAR_N, SPELL_DREADFUL_ROAR_H)
#define SPELL_WOE_STRIKE			DUNGEON_MODE(SPELL_WOE_STRIKE_N, SPELL_WOE_STRIKE_H)
#define SPELL_SMASH					DUNGEON_MODE(SPELL_SMASH_N, SPELL_SMASH_H)
#define SPELL_ENRAGE				DUNGEON_MODE(SPELL_ENRAGE_N, SPELL_ENRAGE_H)

enum eEvents
{
	INGVAR_EVENT_START_RESURRECTION = 1,
	INGVAR_EVENT_YELL_DEAD_1,
	INGVAR_EVENT_VALKYR_MOVE,
	INGVAR_EVENT_ANNHYLDE_YELL,
	INGVAR_EVENT_VALKYR_BEAM,
	INGVAR_EVENT_RESURRECTION_BALL,
	INGVAR_EVENT_RESURRECTION_HEAL,
	INGVAR_EVENT_MORPH_TO_UNDEAD,
	INGVAR_EVENT_START_PHASE_2,

	INGVAR_EVENT_UNROOT,
	INGVAR_EVENT_SPELL_ROAR,
	INGVAR_EVENT_SPELL_CLEAVE_OR_WOE_STRIKE,
	INGVAR_EVENT_SPELL_SMASH,
	INGVAR_EVENT_SPELL_ENRAGE_OR_SHADOW_AXE,
	INGVAR_EVENT_AXE_RETURN,
	INGVAR_EVENT_AXE_PICKUP,
};

class IngvarThePlundererAI : public HybridCreatureScriptAI
{
        HYBRIDSCRIPT_FUNCTION(IngvarThePlundererAI, HybridCreatureScriptAI);
        IngvarThePlundererAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
        {
			_done = false;
        }

		uint64 ValkyrGUID;
		uint64 ThrowGUID;

		void OnLoad()
		{
			_unit->IsKilledByScript = true;
			ParentClass::OnLoad();
		}

		void OnCombatStop(Unit* pTarget)
		{
			ValkyrGUID = 0;
			ThrowGUID = 0;
			_unit->IsKilledByScript = true;
			events.Reset();
			_unit->SetDisplayId(DISPLAYID_DEFAULT);
			FeignDeath(false);
			_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			_unit->UnsetRoot();
			ParentClass::OnCombatStop(pTarget);
		}

		void FeignDeath(bool apply)
		{
			if (apply)
			{
				_unit->SetStandState(7);
				_unit->SetFlag(UNIT_FIELD_FLAGS, 536870912);
				_unit->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
				_unit->SetFlag(UNIT_DYNAMIC_FLAGS, 32);
				_unit->GetAIInterface()->disable_melee = true;
			}
			else
			{
				_unit->SetStandState(0);
				_unit->RemoveFlag(UNIT_FIELD_FLAGS, 536870912);
				_unit->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
				_unit->RemoveFlag(UNIT_DYNAMIC_FLAGS, 32);
				_unit->GetAIInterface()->disable_melee = false;
			}
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

		void JustSummoned(Creature* s)
		{
			if (s->GetEntry() == NPC_ANNHYLDE)
			{
				ValkyrGUID = s->GetGUID();
				s->GetAIInterface()->SetFly();
				//s->SetDisableGravity(true); // Not supported
				//s->SetHover(true); // Not supported
				s->SetPosition(s->GetPositionX(), s->GetPositionY(), s->GetPositionZ() + 35.0f, s->GetOrientation());
			}
			else if (s->GetEntry() == NPC_THROW)
			{
				ThrowGUID = s->GetGUID();
				if (Unit* t = GetRandomPlayerTarget())
					s->GetAIInterface()->MoveJump(t->GetPositionX(), t->GetPositionY(), t->GetPositionZ());
			}
		}

        void OnCombatStart(Unit* pTarget)
        {
            _unit->SendScriptTextChatMessage(4468);     // I'll paint my face with your blood!
			RegisterAIUpdateEvent(1000);
			events.Reset();
			// schedule Phase 1 abilities
			events.RescheduleEvent(INGVAR_EVENT_SPELL_ROAR, 15000);
			events.RescheduleEvent(INGVAR_EVENT_SPELL_CLEAVE_OR_WOE_STRIKE, 2000);
			events.RescheduleEvent(INGVAR_EVENT_SPELL_SMASH, 5000);
			events.RescheduleEvent(INGVAR_EVENT_SPELL_ENRAGE_OR_SHADOW_AXE, 10000);
			ParentClass::OnCombatStart(pTarget);
        }

        void OnTargetDied(Unit* pTarget)
        {
			_unit->SendScriptTextChatMessage(4469);     // Mjul orm agn gjor!
			ParentClass::OnTargetDied(pTarget);
        }

        void OnDied(Unit* pKiller)
        {
			events.Reset();
            _unit->SendScriptTextChatMessage(6986);
			SendLFGReward(_unit);
			ParentClass::OnDied(pKiller);
        }

		void AIUpdate()
		{
			if (_unit->GetDisplayId() == DISPLAYID_DEFAULT && _unit->GetHealth() == 1 && _done == false) // Our version of GetDamageTaken
			{
				_unit->InterruptSpell();
				_unit->RemoveAllAuras();
				_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				_unit->SetRoot();
				FeignDeath(true);
				events.Reset();
				events.RescheduleEvent(INGVAR_EVENT_START_RESURRECTION, 1000);
				events.RescheduleEvent(INGVAR_EVENT_YELL_DEAD_1, 0);
				_done = true;
			}
			events.Update(1000);
			if (_unit->IsCasting())
				return;

			switch (events.GetEvent())
			{
			case 0:
				break;
			case INGVAR_EVENT_YELL_DEAD_1:
				Emote("My life for the... Death God!", Text_Yell, 13213);
				events.PopEvent();
				break;
			case INGVAR_EVENT_START_RESURRECTION:
				_unit->CastSpell(_unit, INGVAR_SPELL_SUMMON_VALKYR, true);
				events.PopEvent();
				events.RescheduleEvent(INGVAR_EVENT_VALKYR_BEAM, 7000);
				events.RescheduleEvent(INGVAR_EVENT_VALKYR_MOVE, 1);
				events.RescheduleEvent(INGVAR_EVENT_ANNHYLDE_YELL, 3000);
				break;
			case INGVAR_EVENT_VALKYR_MOVE:
				if (ForceCreatureFind(24068)) 
					ForceCreatureFind(24068)->GetAIInterface()->MoveJump(ForceCreatureFind(24068)->GetPositionX(), ForceCreatureFind(24068)->GetPositionY(), ForceCreatureFind(24068)->GetPositionZ() - 15.0f);
				events.PopEvent();
				break;
			case INGVAR_EVENT_ANNHYLDE_YELL:
				if (ForceCreatureFind(24068))
				ForceCreatureFind(24068)->SendChatMessage(14, 0, "Ingvar! Your pathetic failure will serve as a warning to all... you are damned! Arise and carry out the master's will!");
				_unit->PlaySoundToSet(13754);
				events.PopEvent();
				break;
			case INGVAR_EVENT_VALKYR_BEAM:
				_unit->RemoveAura(INGVAR_SPELL_SUMMON_VALKYR);
				if (ForceCreatureFind(24068))
					ForceCreatureFind(24068)->CastSpell(_unit, INGVAR_SPELL_RESURRECTION_BEAM, false);
					events.PopEvent();
					events.RescheduleEvent(INGVAR_EVENT_RESURRECTION_BALL, 4000);
				break;
			case INGVAR_EVENT_RESURRECTION_BALL:
				_unit->CastSpell(_unit, INGVAR_SPELL_RESURRECTION_BALL, true);
				events.PopEvent();
				events.RescheduleEvent(INGVAR_EVENT_RESURRECTION_HEAL, 4000);
				break;
			case INGVAR_EVENT_RESURRECTION_HEAL:
				_unit->RemoveAura(INGVAR_SPELL_RESURRECTION_BALL);
				_unit->CastSpell(_unit, INGVAR_SPELL_RESURRECTION_HEAL, true);
				FeignDeath(false);
				events.PopEvent();
				events.RescheduleEvent(INGVAR_EVENT_MORPH_TO_UNDEAD, 3000);
				break;
			case INGVAR_EVENT_MORPH_TO_UNDEAD:
				//_unit->CastSpell(_unit, INGVAR_SPELL_INGVAR_TRANSFORM, true); // Not yet supported
				_unit->SetDisplayId(DISPLAYID_UNDEAD);
				events.PopEvent();
				events.RescheduleEvent(INGVAR_EVENT_START_PHASE_2, 1000);
				break;
			case INGVAR_EVENT_START_PHASE_2:
				if (ForceCreatureFind(24068))
				{
					TO_CREATURE(ForceCreatureFind(24068))->Despawn(1000, 0);
				}
				events.PopEvent();
				_unit->IsKilledByScript = false;
				_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
				Emote("I return! A second chance to carve your skull!", Text_Yell, 13209);

				// schedule Phase 2 abilities
				events.RescheduleEvent(INGVAR_EVENT_SPELL_ROAR, 15000);
				events.RescheduleEvent(INGVAR_EVENT_SPELL_CLEAVE_OR_WOE_STRIKE, 2000);
				events.RescheduleEvent(INGVAR_EVENT_SPELL_SMASH, 5000);
				events.RescheduleEvent(INGVAR_EVENT_SPELL_ENRAGE_OR_SHADOW_AXE, 10000);

				break;

				// ABILITIES HERE:
			case INGVAR_EVENT_UNROOT:
				_unit->UnsetRoot();
				events.PopEvent();
				break;
			case INGVAR_EVENT_SPELL_ROAR:
				Announce("Ingvar the Plunderer roars!");
				if (_unit->GetDisplayId() == DISPLAYID_DEFAULT)
					if (IsHeroic())
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_STAGGERING_ROAR_H, false);
					}
					else
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_STAGGERING_ROAR_N, false);
					}
				else
					if (IsHeroic())
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_DREADFUL_ROAR_H, false);
					}
					else
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_DREADFUL_ROAR_N, false);
					}
				events.RepeatEvent(RandomUInt(15000, 20000));
				break;
			case INGVAR_EVENT_SPELL_CLEAVE_OR_WOE_STRIKE:
				if (_unit->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) == 0)
				{
					events.RepeatEvent(3000);
					break;
				}
				if (_unit->GetDisplayId() == DISPLAYID_DEFAULT)
				{
					_unit->CastSpell(_unit->GetVictim(), INGVAR_SPELL_CLEAVE, false);
				}
				else
				{
					if (IsHeroic())
					{
						_unit->CastSpell(_unit->GetVictim(), INGVAR_SPELL_WOE_STRIKE_H, false);
					}
					else
					{
						_unit->CastSpell(_unit->GetVictim(), INGVAR_SPELL_WOE_STRIKE_N, false);
					}
				}
				events.RepeatEvent(RandomUInt(0, 4000) + 3000);
				break;
			case INGVAR_EVENT_SPELL_SMASH:
				if (_unit->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) == 0)
				{
					events.RepeatEvent(3000);
					break;
				}
				_unit->SetRoot();
				if (_unit->GetDisplayId() == DISPLAYID_DEFAULT)
				{
					if (IsHeroic())
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_SMASH_H, false);
					}
					else
					{
						_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_SMASH_N, false);
					}
				}
				else
				{
					_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_DARK_SMASH, false);
				}
				events.RepeatEvent(RandomUInt(9000, 11000));
				events.RescheduleEvent(INGVAR_EVENT_UNROOT, 3750);
				break;
			case INGVAR_EVENT_SPELL_ENRAGE_OR_SHADOW_AXE:
				if (_unit->GetDisplayId() == DISPLAYID_DEFAULT)
				{
					if (IsHeroic())
					{
						_unit->CastSpell(_unit, INGVAR_SPELL_ENRAGE_H, false);
					}
					else
					{
						_unit->CastSpell(_unit, INGVAR_SPELL_ENRAGE_N, false);
					}
					events.RepeatEvent(10000);
				}
				else
				{
					_unit->CastSpell((Unit*)NULL, INGVAR_SPELL_SHADOW_AXE, true);
					_unit->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(0)); // unequip the axe.
					events.RepeatEvent(35000);
					events.RescheduleEvent(INGVAR_EVENT_AXE_RETURN, 10000);
				}
				break;
			case INGVAR_EVENT_AXE_RETURN:
				if (ForceCreatureFind(23997))
					ForceCreatureFind(23997)->GetAIInterface()->MoveCharge(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ() + 0.5f);
					events.PopEvent();
					events.RescheduleEvent(INGVAR_EVENT_AXE_PICKUP, 1500);
				break;
			case INGVAR_EVENT_AXE_PICKUP:
				if (ForceCreatureFind(23997))
				{
					TO_CREATURE(ForceCreatureFind(23997))->Despawn(1000, 0);
				}
				ThrowGUID = 0;
				_unit->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(33177)); // Reequip the axe.
				events.PopEvent();
				break;
			}
			ParentClass::AIUpdate();
		}
		protected:
			bool _done;
};

class CallerAI : public HybridCreatureScriptAI
{
	HYBRIDSCRIPT_FUNCTION(CallerAI, HybridCreatureScriptAI);
	CallerAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
	{
	}

	void OnLoad()
	{
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		ParentClass::OnLoad();
	};
};

void SetupUtgardeKeep(ScriptMgr* mgr)
{
    mgr->register_instance_script(574, &UtgardeKeepScript::Create);
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////// Mobs
    mgr->register_creature_script(CN_DRAGONFLAYER_FORGE_MASTER, &DragonflayerForgeMasterAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_HEARTSPLITTER, &DragonflayerHeartSplitterAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_IRONHELM, &DragonflayerIronhelmAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_METALWORKER, &DragonflayerIronhelmAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_OVERSEER, &DragonflayerOverseerAI::Create);
    mgr->register_creature_script(CN_TUNNELING_GHOUL, &TunnelingGhoulAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_RUNECASTER, &DragonflayerRunecasterAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_SPIRITUALIST, &DragonflayerSpiritualistAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_STRATEGIST, &DragonflayerStrategistAI::Create);
    mgr->register_creature_script(CN_PROTO_DRAKE_HANDLER, &ProtoDrake_HandlerAI::Create);
    mgr->register_creature_script(CN_FRENZIED_GEIST, &FrenziedGeistAI::Create);
    mgr->register_creature_script(CN_SAVAGE_WORG, &SavageWorgAI::Create);
    mgr->register_creature_script(CN_DRAGONFLAYER_BONECRUSHER, &DragonflayerBonecrusherAI::Create);
    mgr->register_creature_script(CN_PROTO_DRAKE_RIDER, &ProtoDrake_RiderAI::Create);
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////// Bosses

    // Skarvald & Dalronn Encounter
    mgr->register_creature_script(CN_SKARVALD, &SkarvaldTheConstructorAI::Create);
    mgr->register_creature_script(CN_DALRONN, &DalronnTheControllerAI::Create);
    mgr->register_creature_script(CN_SKARVALD_GHOST, &SkarvaldTheConstructorGhostAI::Create);
    mgr->register_creature_script(CN_DALRONN_GHOST, &DalronnTheControllerGhostAI::Create);

    // Prince Keleseth Encounter
    mgr->register_creature_script(CN_PRINCE_KELESETH, &PrinceKelesethAI::Create);
    mgr->register_creature_script(CN_FROST_TOMB, &FrostTombAI::Create);
    mgr->register_creature_script(KELESETH_SKELETON_ADD, &SkeletonAddAI::Create);

    // Ingvar the Plunderer Encounter
    mgr->register_creature_script(CN_INGVAR, &IngvarThePlundererAI::Create);
	mgr->register_creature_script(24068, &CallerAI::Create);
};
