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

/// \ todo Finish Kritkhir Encounter, needs more blizzlike, may need InstanceScript | Anuburak | Add's AI and trash

#include "Setup/Setup.h"
#include "Dungeon_AzjolNerub.h"

enum DataIndex
{
	ACHIEVEMENT_FAIL_NO = 0,
	ACHIEVEMENT_FAIL_YES = 1
};

//////////////////////////////////////////////////////////////////////////////////////////
//Azjol-Nerub
class InstanceAzjolNerubScript : public MoonInstanceScript
{
private:
	uint32 m_phaseData[ACHIEVEMENT_FAIL_YES];

    public:

        MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(InstanceAzjolNerubScript, MoonInstanceScript);
        InstanceAzjolNerubScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
        {
            // Way to select bosses
            BuildEncounterMap();
            if (mEncounters.size() == 0)
                return;

            for (EncounterMap::iterator Iter = mEncounters.begin(); Iter != mEncounters.end(); ++Iter)
            {
                if ((*Iter).second.mState != State_Finished)
                    continue;
            }
        }

		void SetData(uint32 pIndex, uint32 pData)
		{
			if (pIndex >= ACHIEVEMENT_FAIL_YES)
				return;

			if (pIndex == ACHIEVEMENT_FAIL_NO)
				mInstance->GetPlayer(0)->GetAchievementMgr().GMCompleteAchievement(NULL, 1296);

			pData = 0;
		};

        void OnGameObjectPushToWorld(GameObject* pGameObject) { }

        void SetInstanceData(uint32 pType, uint32 pIndex, uint32 pData)
        {
            if (pType != Data_EncounterState || pIndex == 0)
                return;

            EncounterMap::iterator Iter = mEncounters.find(pIndex);
            if (Iter == mEncounters.end())
                return;

            (*Iter).second.mState = (EncounterState)pData;
        }

        uint32 GetInstanceData(uint32 pType, uint32 pIndex)
        {
            if (pType != Data_EncounterState || pIndex == 0)
                return 0;

            EncounterMap::iterator Iter = mEncounters.find(pIndex);
            if (Iter == mEncounters.end())
                return 0;

            return (*Iter).second.mState;
        }

        void OnCreatureDeath(Creature* pCreature, Unit* pUnit)
        {
            EncounterMap::iterator Iter = mEncounters.find(pCreature->GetEntry());
            if (Iter == mEncounters.end())
                return;

            (*Iter).second.mState = State_Finished;

            return;
        }
};

//Krikthir The Gatewatcher
enum KrikthirSpells
{
	SPELL_SUBBOSS_AGGRO_TRIGGER = 52343,
	SPELL_SWARM = 52440,
	SPELL_MIND_FLAY = 52586,
	SPELL_CURSE_OF_FATIGUE = 52592,
	SPELL_FRENZY = 28747
};

enum KrikthirEvents
{
	EVENT_KRIK_START_WAVE = 1,
	EVENT_KRIK_SUMMON = 2,
	EVENT_KRIK_MIND_FLAY = 3,
	EVENT_KRIK_CURSE = 4,
	EVENT_KRIK_HEALTH_CHECK = 5,
	EVENT_KRIK_ENTER_COMBAT = 6,
	EVENT_KILL_TALK = 7,
	EVENT_CALL_ADDS = 8,
	EVENT_KRIK_CHECK_EVADE = 9
};

enum KrikthirNpcs
{
	NPC_WATCHER_NARJIL = 28729,
	NPC_WATCHER_GASHRA = 28730,
	NPC_WATCHER_SILTHIK = 28731,
	NPC_WARRIOR = 28732,
	NPC_SKIRMISHER = 28734,
	NPC_SHADOWCASTER = 28733
};
class KrikthirAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(KrikthirAI, MoonScriptCreatureAI);
    KrikthirAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
		maxSpawn = 0;
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

	void OnCombatStop(Unit* pTarget)
	{
		RemoveAIUpdateEvent();
		events.Reset();
		/*_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_WATCHER_NARJIL, 511.8f, 666.493f, 776.278f, 0.977f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_SHADOWCASTER, 518.13f, 667.0f, 775.74f, 1.0f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_WARRIOR, 506.75f, 670.7f, 776.24f, 0.92f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_WATCHER_GASHRA, 526.66f, 663.605f, 775.805f, 1.23f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_SKIRMISHER, 522.23f, 668.97f, 775.66f, 1.18f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_WARRIOR, 532.4f, 666.47f, 775.67f, 1.45f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_WATCHER_SILTHIK, 543.826f, 665.123f, 776.245f, 1.55f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_SKIRMISHER, 547.5f, 669.96f, 776.1f, 2.3f, true, false, 0, 0);
		_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_SHADOWCASTER, 536.96f, 667.28f, 775.6f, 1.72f, true, false, 0, 0);*/
		ParentClass::OnCombatStop(pTarget);
	}

    void OnCombatStart(Unit* pTarget)
    {
		RegisterAIUpdateEvent(1000);
		events.ScheduleEvent(EVENT_KRIK_HEALTH_CHECK, 1000);
		events.ScheduleEvent(EVENT_KRIK_MIND_FLAY, 13000);
		events.ScheduleEvent(EVENT_KRIK_SUMMON, 17000);
		events.ScheduleEvent(EVENT_KRIK_CURSE, 8000);
		events.ScheduleEvent(EVENT_CALL_ADDS, 1000);
        _unit->SendScriptTextChatMessage(3908);     // This kingdom belongs to the Scourge. Only the dead may enter!
		ParentClass::OnCombatStart(pTarget);
	}

	void SpawnSwarmInsect()
	{
		uint32 randomX = _unit->GetPositionX() + RandomUInt(1, 16);
		uint32 randomY = _unit->GetPositionY() + RandomUInt(1, 8);
		Creature* swarmer = _unit->GetMapMgr()->GetInterface()->SpawnCreature(28735, randomX, randomY, _unit->GetPositionZ(), 0, true, false, 0, 0);
		if (swarmer)
			swarmer->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_KRIK_HEALTH_CHECK:
			if (HealthBelowPct(10))
			{
				events.PopEvent();
				_unit->CastSpell(_unit, SPELL_FRENZY, true);
				Announce("Kirk'thir the Gatewatcher goes into a frenzy!");
				break;
			}
			events.ScheduleEvent(EVENT_KRIK_HEALTH_CHECK, 1000);
			break;
		case EVENT_KRIK_SUMMON:
			_unit->SendScriptTextChatMessage(3913);
			maxSpawn = 0;
			_unit->CastSpell(_unit, SPELL_SWARM, false);
			events.ScheduleEvent(EVENT_KRIK_SUMMON, 20000);
			events.ScheduleEvent(EVENT_CALL_ADDS, 100);
			break;
		case EVENT_KRIK_MIND_FLAY:
			_unit->CastSpell(_unit->GetVictim(), SPELL_MIND_FLAY, false);
			events.ScheduleEvent(EVENT_KRIK_MIND_FLAY, 15000);
			break;
		case EVENT_KRIK_CURSE:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_CURSE_OF_FATIGUE, true);
			events.ScheduleEvent(EVENT_KRIK_CURSE, 10000);
			break;
		case EVENT_CALL_ADDS:
			if (maxSpawn != 50)
			{
				SpawnSwarmInsect();
				++maxSpawn;
				events.ScheduleEvent(EVENT_CALL_ADDS, 100);
			}
			break;
		}
		ParentClass::AIUpdate();
	}

    void OnTargetDied(Unit* mKiller)
    {
        switch (RandomUInt(1))
        {
            case 0:
                _unit->SendScriptTextChatMessage(3910);     // As Anub'arak commands!
                break;
            case 1:
                _unit->SendScriptTextChatMessage(3909);     // You were foolish to come.
                break;
        }
    }

    void OnDied(Unit* pKiller)
    {
		RemoveAIUpdateEvent();
		events.Reset();
        _unit->SendScriptTextChatMessage(3911);         // I should be grateful... but I long ago lost the capacity....
		GameObject* Doors = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(530.38f, 628.602f, 777.98f, 192395);
        if (Doors)
            Doors->Despawn(1000, 0);
        ParentClass::OnDied(pKiller);
    }
private:
    bool mEnraged;
	uint32 maxSpawn;
};

//Hadronox
enum HadronoxSpells
{
	SPELL_SUMMON_ANUBAR_CHAMPION = 53064,
	SPELL_SUMMON_ANUBAR_CRYPT_FIEND = 53065,
	SPELL_SUMMON_ANUBAR_NECROMANCER = 53066,
	SPELL_WEB_FRONT_DOORS = 53177,
	SPELL_WEB_SIDE_DOORS = 53185,
	SPELL_ACID_CLOUD = 53400,
	SPELL_LEECH_POISON = 53030,
	SPELL_LEECH_POISON_HEAL = 53800,
	SPELL_WEB_GRAB = 57731,
	SPELL_PIERCE_ARMOR = 53418,

	SPELL_SMASH = 53318,
	HADRONOX_SPELL_FRENZY = 53801
};

enum HadronoxEvents
{
	EVENT_HADRONOX_MOVE1 = 1,
	EVENT_HADRONOX_MOVE2 = 2,
	EVENT_HADRONOX_MOVE3 = 3,
	EVENT_HADRONOX_MOVE4 = 4,
	EVENT_HADRONOX_ACID = 5,
	EVENT_HADRONOX_LEECH = 6,
	EVENT_HADRONOX_PIERCE = 7,
	EVENT_HADRONOX_GRAB = 8,
	EVENT_HADRONOX_SUMMON = 9,

	EVENT_CRUSHER_SMASH = 20,
	EVENT_CHECK_HEALTH = 21
};

enum HadronoxMisc
{
	NPC_ANUB_AR_CRUSHER = 28922,

	SAY_CRUSHER_AGGRO = 0,
	SAY_CRUSHER_EMOTE = 1,
	SAY_HADRONOX_EMOTE = 0,

	ACTION_DESPAWN_ADDS = 1,
	ACTION_START_EVENT = 2
};

class HadronoxAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(HadronoxAI, MoonScriptCreatureAI);
    HadronoxAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {};

	void OnCombatStop(Unit* pTarget)
	{
		RegisterAIUpdateEvent(1000);
		//_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_ANUB_AR_CRUSHER, 542.9f, 519.5f, 741.24f, 2.14f, true, false, 0, 0);
		ParentClass::OnCombatStop(pTarget);
	}

	void DoAction()
	{
			RegisterAIUpdateEvent(1000);
			events.ScheduleEvent(EVENT_HADRONOX_MOVE1, 20000);
			events.ScheduleEvent(EVENT_HADRONOX_MOVE2, 40000);
			events.ScheduleEvent(EVENT_HADRONOX_MOVE3, 60000);
			events.ScheduleEvent(EVENT_HADRONOX_MOVE4, 80000);
	}

	void OnTargetDied(Unit* pTarget)
	{
		if (!_unit->isAlive() || !pTarget->HasAura(SPELL_LEECH_POISON))
			return;

		_unit->SetHealth(_unit->GetMaxHealth() / 90); // 10%
		ParentClass::OnTargetDied(pTarget);
	}

	void OnLoad()
	{
		RegisterAIUpdateEvent(1000);
		ParentClass::OnLoad();
	}

	void OnCombatStart(Unit* pTarget)
	{
		events.RescheduleEvent(EVENT_HADRONOX_ACID, 10000);
		events.RescheduleEvent(EVENT_HADRONOX_LEECH, 4000);
		events.RescheduleEvent(EVENT_HADRONOX_PIERCE, 1000);
		events.RescheduleEvent(EVENT_HADRONOX_GRAB, 15000);
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

	Unit* GetRandomTarget()
	{
		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangeOppFactsSetBegin(); iter != _unit->GetInRangeOppFactsSetEnd(); ++iter)
		{
			if ((*iter) && (TO< Unit* >(*iter))->isAlive())
				possible_targets.push_back((uint32)(*iter)->GetGUID());
		}
		if (possible_targets.size() > 0)
		{
			uint32 random_unit = possible_targets[Rand(uint32(possible_targets.size() - 1))];
			return _unit->GetMapMgr()->GetUnit(random_unit);
		}
		return NULL;
	}

	void AIUpdate()
	{
		Creature* crusher = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(520.39f, 548.78f, 732.01f, NPC_ANUB_AR_CRUSHER);
		if (crusher && crusher->spawnid == 119775 && crusher->CombatStatus.IsInCombat() && _start == false)
		{
			DoAction();
			_start = true;
		}
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (uint32 eventId = events.ExecuteEvent())
		{
		case EVENT_HADRONOX_PIERCE:
			_unit->CastSpell(_unit->GetVictim(), SPELL_PIERCE_ARMOR, false);
			events.ScheduleEvent(EVENT_HADRONOX_PIERCE, 8000);
			break;
		case EVENT_HADRONOX_ACID:
			if (randt == 0)
			{
				_unit->CastSpell(_unit->GetAIInterface()->GetMostHated(), SPELL_ACID_CLOUD, false);
				events.ScheduleEvent(EVENT_HADRONOX_ACID, 25000);
			}
			else
			{
				_unit->CastSpell(_unit->GetAIInterface()->GetSecondHated(), SPELL_ACID_CLOUD, false);
				events.ScheduleEvent(EVENT_HADRONOX_ACID, 25000);
			}
			break;
		case EVENT_HADRONOX_LEECH:
			_unit->CastSpell(_unit, SPELL_LEECH_POISON, false);
			events.ScheduleEvent(EVENT_HADRONOX_LEECH, 12000);
			break;
		case EVENT_HADRONOX_GRAB:
			_unit->CastSpell(_unit, SPELL_WEB_GRAB, false);
			events.ScheduleEvent(EVENT_HADRONOX_GRAB, 25000);
			break;
		case EVENT_HADRONOX_MOVE4:
			Announce("Hadronox moves up the tunnel!");
			_unit->GetAIInterface()->MoveCharge(534.87f, 554.0f, 733.0f);
			//_unit->CastSpell(_unit, SPELL_WEB_FRONT_DOORS, true);
		case EVENT_HADRONOX_MOVE1:
			Announce("Hadronox moves up the tunnel!");
			_unit->GetAIInterface()->MoveCharge(607.9f, 512.8f, 695.3f);
			break;
		case EVENT_HADRONOX_MOVE2:
			Announce("Hadronox moves up the tunnel!");
			_unit->GetAIInterface()->MoveCharge(611.67f, 564.11f, 720.0f);
			break;
		case EVENT_HADRONOX_MOVE3:
			Announce("Hadronox moves up the tunnel!");
			_unit->GetAIInterface()->MoveCharge(576.1f, 580.0f, 727.5f);
			break;

		}
		ParentClass::AIUpdate();
	}
	private:
		bool _start = false;
		uint32 randt = RandomUInt(0, 1);
};

class AnubarcrusherAI : public MoonScriptCreatureAI
{
	MOONSCRIPT_FACTORY_FUNCTION(AnubarcrusherAI, MoonScriptCreatureAI);
	AnubarcrusherAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{}

	void OnCombatStart(Unit* pTarget)
	{
		if (_unit->spawnid == 119775) // Prevent double spawns
		{
			if (_spawnComplete == false)
			{
				_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_ANUB_AR_CRUSHER, 519.58f, 573.73f, 734.30f, 4.50f, true, false, 0, 0);
				_unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_ANUB_AR_CRUSHER, 539.38f, 573.25f, 732.20f, 4.738f, true, false, 0, 0);
				_spawnComplete = true;
			}
			Emote("The gate has been breached! Quickly, divert forces to deal with these invaders!", Text_Yell, 13941);
		}
		ParentClass::OnCombatStart(pTarget);
	}
private:
	bool _spawnComplete = false;
};

//Anub'arak Boss
enum AnubArakSpells
{
	SPELL_CARRION_BEETLES = 53520,
	SPELL_SUMMON_CARRION_BEETLES = 53521,
	SPELL_LEECHING_SWARM = 53467,
	SPELL_POUND = 53472,
	SPELL_POUND_DAMAGE = 53509,
	SPELL_IMPALE_PERIODIC = 53456,
	SPELL_EMERGE = 53500,
	SPELL_SUBMERGE = 53421,
	SPELL_SELF_ROOT = 42716,

	SPELL_SUMMON_DARTER = 53599,
	SPELL_SUMMON_ASSASSIN = 53610,
	SPELL_SUMMON_GUARDIAN = 53614,
	SPELL_SUMMON_VENOMANCER = 53615,
};

enum AnubArakYells
{
	SAY_AGGRO = 3897,
	SAY_SLAY = 3898,
	SAY_DEATH = 3901,
	SAY_LOCUST = 3904,
	SAY_SUBMERGE = 3906,
	SAY_INTRO = 3907
};

enum AnubArakMisc
{
	ACHIEV_TIMED_START_EVENT = 20381,

	EVENT_CHECK_HEALTH_25 = 1,
	EVENT_CHECK_HEALTH_50 = 2,
	EVENT_CHECK_HEALTH_75 = 3,
	EVENT_CARRION_BEETELS = 4,
	EVENT_LEECHING_SWARM = 5,
	EVENT_IMPALE = 6,
	EVENT_POUND = 7,
	EVENT_CLOSE_DOORS = 8,
	EVENT_EMERGE = 9,
	EVENT_SUMMON_VENOMANCER = 10,
	EVENT_SUMMON_DARTER = 11,
	EVENT_SUMMON_GUARDIAN = 12,
	EVENT_SUMMON_ASSASSINS = 13,
	EVENT_ENABLE_ROTATE = 14,
	ANUB_EVENT_KILL_TALK = 15
};
class AnubArakAI : public MoonScriptCreatureAI
{
	MOONSCRIPT_FACTORY_FUNCTION(AnubArakAI, MoonScriptCreatureAI);
	AnubArakAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{}

	void OnCombatStart(Unit* pTarget)
	{
		RegisterAIUpdateEvent(1000);
		_unit->SendScriptTextChatMessage(SAY_AGGRO);
		events.ScheduleEvent(EVENT_CARRION_BEETELS, 6500);
		events.ScheduleEvent(EVENT_LEECHING_SWARM, 20000);
		events.ScheduleEvent(EVENT_POUND, 15000);
		events.ScheduleEvent(EVENT_CHECK_HEALTH_75, 1000);
		events.ScheduleEvent(EVENT_CHECK_HEALTH_50, 1000);
		events.ScheduleEvent(EVENT_CHECK_HEALTH_25, 1000);
		events.ScheduleEvent(EVENT_CLOSE_DOORS, 5000);
		ParentClass::OnCombatStart(pTarget);
	}

	void OnCombatStop(Unit* pTarget)
	{
		RemoveAIUpdateEvent();
		_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2 | UNIT_FLAG_NOT_SELECTABLE);
		ParentClass::OnCombatStop(pTarget);
	}

	void OnTargetDied(Unit* pTarget)
	{
		_unit->SendScriptTextChatMessage(SAY_SLAY);
		ParentClass::OnTargetDied(pTarget);
	}

	void OnDied(Unit* pTarget)
	{
		_unit->SendScriptTextChatMessage(SAY_DEATH);
		SendLFGReward(_unit);
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

		switch (uint32 eventId = events.ExecuteEvent())
		{
		case EVENT_CLOSE_DOORS:
			//_EnterCombat();
			break;
		case EVENT_CARRION_BEETELS:
			_unit->CastSpell(_unit, SPELL_CARRION_BEETLES, false);
			events.ScheduleEvent(EVENT_CARRION_BEETELS, 25000);
			break;
		case EVENT_LEECHING_SWARM:
			_unit->SendScriptTextChatMessage(SAY_LOCUST);
			_unit->CastSpell(_unit, SPELL_LEECHING_SWARM, false);
			events.ScheduleEvent(EVENT_LEECHING_SWARM, 20000);
			break;
		case EVENT_POUND:
			if (Unit* target = GetRandomPlayerTarget())
			{
				_unit->Root();
				_unit->CastSpell(target, SPELL_POUND, false);
				events.ScheduleEvent(EVENT_ENABLE_ROTATE, 3300);
			}
			events.ScheduleEvent(EVENT_POUND, 18000);
			break;
		case EVENT_ENABLE_ROTATE:
			_unit->Unroot();
			break;
		case EVENT_CHECK_HEALTH_25:
		case EVENT_CHECK_HEALTH_50:
		case EVENT_CHECK_HEALTH_75:
			if (HealthBelowPct(eventId * 25))
			{
				_unit->SendScriptTextChatMessage(SAY_SUBMERGE);
				_unit->Root();
				_unit->GetAIInterface()->disable_melee = true;
				_unit->SetDisplayId(11686);
				_unit->CastSpell(_unit, SPELL_IMPALE_PERIODIC, true);
				//_unit->CastSpell(_unit, SPELL_SUBMERGE, false); // Not supported by core.
				_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2 | UNIT_FLAG_NOT_SELECTABLE);

				events.DelayEvents(46000, 0);
				events.ScheduleEvent(EVENT_EMERGE, 45000);
				events.ScheduleEvent(EVENT_SUMMON_ASSASSINS, 2000);
				events.ScheduleEvent(EVENT_SUMMON_GUARDIAN, 4000);
				events.ScheduleEvent(EVENT_SUMMON_ASSASSINS, 15000);
				events.ScheduleEvent(EVENT_SUMMON_VENOMANCER, 20000);
				events.ScheduleEvent(EVENT_SUMMON_DARTER, 30000);
				events.ScheduleEvent(EVENT_SUMMON_ASSASSINS, 35000);
				break;
			}
			events.ScheduleEvent(eventId, 500);
			break;
		case EVENT_EMERGE:
			//_unit->RemoveAura(SPELL_SUBMERGE); // Not supported by core.
			_unit->RemoveAura(SPELL_IMPALE_PERIODIC);
			_unit->Unroot();
			_unit->GetAIInterface()->disable_melee = false;
			_unit->SetDisplayId(_unit->GetNativeDisplayId());
			//_unit->CastSpell(_unit, SPELL_EMERGE, true); // Not supported
			_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2 | UNIT_FLAG_NOT_SELECTABLE);
			break;
		case EVENT_SUMMON_ASSASSINS:
			SpawnCreature(29214, 509.32f, 247.42f, 239.48f);
			SpawnCreature(29214, 589.51f, 240.19f, 236.0f);
			break;
		case EVENT_SUMMON_DARTER:
			SpawnCreature(29213, 509.32f, 247.42f, 239.48f);
			SpawnCreature(29213, 589.51f, 240.19f, 236.0f);
			break;
		case EVENT_SUMMON_GUARDIAN:
			SpawnCreature(29216, 550.34f, 316.00f, 234.30f);
			break;
		case EVENT_SUMMON_VENOMANCER:
			SpawnCreature(29217, 550.34f, 316.00f, 234.30f);
			break;
		}
		ParentClass::AIUpdate();
	}
};

void spell_azjol_nerub_carrion_beetels(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Unit* target = aur->GetTarget();
	if (spellId == 53520)
	{
		if (SpellAuraType == SPELL_AURA_PERIODIC_TRIGGER_DUMMY)
		{
			if (apply)
			{
				aur->GetUnitCaster()->CastSpell(aur->GetUnitCaster(), SPELL_SUMMON_CARRION_BEETLES, true);
				aur->GetUnitCaster()->CastSpell(aur->GetUnitCaster(), SPELL_SUMMON_CARRION_BEETLES, true);
		
			}
		}
	}
};

class AddsAI_Anub : public MoonScriptCreatureAI
{
	MOONSCRIPT_FACTORY_FUNCTION(AddsAI_Anub, MoonScriptCreatureAI);
	AddsAI_Anub(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{
		AddSpell(GASHRA_WEBWRAP, Target_RandomPlayer, 22, 0, 35, 0, 0);
		AddSpell(GASHRA_INFECTEDBITE, Target_ClosestPlayer, 35, 0, 12, 0, 0);
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
		_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
		ParentClass::OnLoad();
	}

};


//Watcher Gashra
class GashraAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(GashraAI, MoonScriptCreatureAI);
    GashraAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
        AddSpell(GASHRA_WEBWRAP, Target_RandomPlayer, 22, 0, 35, 0, 0);
        AddSpell(GASHRA_INFECTEDBITE, Target_ClosestPlayer, 35, 0, 12, 0, 0);
    }

};

//Watcher Narjil
class NarjilAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(NarjilAI, MoonScriptCreatureAI);
    NarjilAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
        AddSpell(NARJIL_WEBWRAP, Target_RandomPlayer, 22, 0, 35, 0, 0);
        AddSpell(NARJIL_INFECTEDBITE, Target_ClosestPlayer, 35, 0, 12, 0, 0);
        AddSpell(NARJIL_BLINDINGWEBS, Target_ClosestPlayer, 16, 0, 9, 0, 0);
    }

};

//Watcher Silthik
class SilthikAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(SilthikAI, MoonScriptCreatureAI);
    SilthikAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
        AddSpell(NARJIL_WEBWRAP, Target_RandomPlayer, 22, 0, 35, 0, 0);
        AddSpell(NARJIL_INFECTEDBITE, Target_ClosestPlayer, 35, 0, 12, 0, 0);
        AddSpell(SILTHIK_POISONSPRAY, Target_RandomPlayer, 30, 0, 15, 0, 0);
    }

};

//Anub'ar Shadowcaster (anub shadowcaster)
class AnubShadowcasterAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(AnubShadowcasterAI, MoonScriptCreatureAI);
    AnubShadowcasterAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
        AddSpell(SHADOWCASTER_SHADOWBOLT, Target_RandomPlayer, 36, 0, 8);
        AddSpell(SHADOWCASTER_SHADOW_NOVA, Target_Self, 22, 0, 15);
    }

};

void SetupAzjolNerub(ScriptMgr* mgr)
{
	// Spells
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&spell_azjol_nerub_carrion_beetels);
    //Instance
    mgr->register_instance_script(MAP_AZJOL_NERUB, &InstanceAzjolNerubScript::Create);

	//Trash
	mgr->register_creature_script(NPC_ANUB_AR_CRUSHER, &AnubarcrusherAI::Create);
	mgr->register_creature_script(29213, &AddsAI_Anub::Create);
	mgr->register_creature_script(29214, &AddsAI_Anub::Create);
	mgr->register_creature_script(29216, &AddsAI_Anub::Create);
	mgr->register_creature_script(29217, &AddsAI_Anub::Create);

    //Bosses
    mgr->register_creature_script(BOSS_KRIKTHIR, &KrikthirAI::Create);
    mgr->register_creature_script(BOSS_HADRONOX, &HadronoxAI::Create);
	mgr->register_creature_script(29120, &AnubArakAI::Create);

    // watchers
    mgr->register_creature_script(CN_GASHRA, &GashraAI::Create);
    mgr->register_creature_script(CN_NARJIL, &NarjilAI::Create);
    mgr->register_creature_script(CN_SILTHIK, &SilthikAI::Create);
}
