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

// \todo move most defines to enum, text to db (use SendScriptTextChatMessage(ID))
#include "Setup/Setup.h"
#include "Dungeon_DrakTharonKeep.h"

//////////////////////////////////////////////////////////////////////////////////////////
//Drak'Tharon Keep
class InstanceDrakTharonKeepScript : public MoonInstanceScript
{
    public:

        MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(InstanceDrakTharonKeepScript, MoonInstanceScript);
        InstanceDrakTharonKeepScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
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

/*
 Trollgore - TOO EASY!!
 \todo Whole corpses/consume thingo is wrong
 NOTES:
 Core doesn't support auras on corpses, we are currently unable to script this blizzlike
 */

//TrollgoreAI
//two mobs per 10s

enum Spells
{
	SPELL_SUMMON_INVADER_A = 49456,
	SPELL_SUMMON_INVADER_B = 49457,
	SPELL_SUMMON_INVADER_C = 49458,

	SPELL_INFECTED_WOUND = 49637,
	SPELL_CRUSH = 49639,
	SPELL_CONSUME = 49380,
	SPELL_CORPSE_EXPLODE = 49555,

	SPELL_CORPSE_EXPLODE_DAMAGE = 49618,
	SPELL_CONSUME_AURA = 49381,
};

enum Events_Trollgore
{
	EVENT_SPELL_INFECTED_WOUND = 1,
	EVENT_SPELL_CRUSH = 2,
	EVENT_SPELL_CONSUME = 3,
	EVENT_SPELL_CORPSE_EXPLODE = 4,
	EVENT_SPAWN_INVADERS = 5,
	EVENT_KILL_TALK = 6
};

class TrollgoreAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(TrollgoreAI, MoonScriptBossAI);
	TrollgoreAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnCombatStart(Unit* mAttacker)
	{
		RegisterAIUpdateEvent(1000);
		events.ScheduleEvent(EVENT_SPELL_INFECTED_WOUND, RandomUInt(6000, 10000));
		events.ScheduleEvent(EVENT_SPELL_CRUSH, RandomUInt(3000, 5000));
		events.ScheduleEvent(EVENT_SPELL_CONSUME, 15000);
		events.ScheduleEvent(EVENT_SPELL_CORPSE_EXPLODE, 35000);
		events.ScheduleEvent(EVENT_SPAWN_INVADERS, 20000, 30000);
		ParentClass::OnCombatStart(mAttacker);
	}

	void AIUpdate()
	{
		events.Update(1000);

		if (_unit->hasStateFlag(0x00008000))
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_SPELL_INFECTED_WOUND:
			_unit->CastSpell(_unit->GetVictim(), SPELL_INFECTED_WOUND, false);
			events.ScheduleEvent(EVENT_SPELL_INFECTED_WOUND, RandomUInt(25000, 35000));
			break;
		case EVENT_SPELL_CRUSH:
			_unit->CastSpell(_unit->GetVictim(), SPELL_CRUSH, false);
			events.ScheduleEvent(EVENT_SPELL_CRUSH, RandomUInt(10000, 15000));
			break;
		case EVENT_SPELL_CONSUME:
			Emote("So hungry! Must feed!", Text_Yell, 13182);
			_unit->CastSpell(_unit, SPELL_CONSUME, false);
			events.ScheduleEvent(EVENT_SPELL_CONSUME, 15000);
			break;
		case EVENT_SPELL_CORPSE_EXPLODE:
			Emote("Corpse go boom!", Text_Yell, 13184);
			_unit->CastSpell(_unit, SPELL_CORPSE_EXPLODE, false);
			events.ScheduleEvent(EVENT_SPELL_CORPSE_EXPLODE, RandomUInt(15000, 19000));
			break;
		}
		ParentClass::AIUpdate();
	}
};

/*
 Novos the Summoner
 \todo
 - Crystal should be actived/deactived instead of created/deleted, minor
 - Create waypoints for summons, we need them coz Core doesn't not have path finding
 */

enum Spells_Novos
{
	SPELL_BEAM_CHANNEL = 52106,
	SPELL_ARCANE_BLAST = 49198,
	//SPELL_ARCANE_FIELD = 47346,
	SPELL_SUMMON_FETID_TROLL_CORPSE = 49103,
	SPELL_SUMMON_HULKING_CORPSE = 49104,
	SPELL_SUMMON_RISEN_SHADOWCASTER = 49105,
	SPELL_SUMMON_CRYSTAL_HANDLER = 49179,
	SPELL_DESPAWN_CRYSTAL_HANDLER = 51403,

	SPELL_SUMMON_MINIONS = 59910,
	SPELL_COPY_OF_SUMMON_MINIONS = 59933,
	SPELL_BLIZZARD = 49034,
	SPELL_FROSTBOLT = 49037,
	SPELL_TOUCH_OF_MISERY = 50090
};

enum Misc_Novos
{
	NPC_CRYSTAL_CHANNEL_TARGET = 26712,
	NPC_CRYSTAL_HANDLER = 26627,

	EVENT_SUMMON_FETID_TROLL = 1,
	EVENT_SUMMON_SHADOWCASTER = 2,
	EVENT_SUMMON_HULKING_CORPSE = 3,
	EVENT_SUMMON_CRYSTAL_HANDLER = 4,
	EVENT_CAST_OFFENSIVE_SPELL = 5,
//	EVENT_KILL_TALK = 6,
	EVENT_CHECK_PHASE = 7,
	EVENT_SPELL_SUMMON_MINIONS = 8
};

class NovosTheSummonerAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(NovosTheSummonerAI, MoonScriptBossAI);
	NovosTheSummonerAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		_unit->_crystalCount = 0;
		_crystalCounter = 0;
		_unit->Root();
		ParentClass::OnLoad();
	}

	void OnCombatStart(Unit* mAttacker)
	{
		RegisterAIUpdateEvent(1000);
		Emote("The chill that you feel is the herald of your doom! ", Text_Yell, 13173);
		events.ScheduleEvent(EVENT_SUMMON_FETID_TROLL, 3000);
		events.ScheduleEvent(EVENT_SUMMON_SHADOWCASTER, 9000);
		events.ScheduleEvent(EVENT_SUMMON_HULKING_CORPSE, 30000);
		events.ScheduleEvent(EVENT_SUMMON_CRYSTAL_HANDLER, 20000);
		events.ScheduleEvent(EVENT_CHECK_PHASE, 80000);

		_unit->CastSpell(_unit, SPELL_ARCANE_BLAST, true);
		_unit->CastSpell(_unit, SPELL_DESPAWN_CRYSTAL_HANDLER, true);
		_unit->CastSpell(_unit, SPELL_BEAM_CHANNEL, true);
		_unit->CastSpell(_unit, 47346, false);
		SpawnCreature(NPC_CRYSTAL_CHANNEL_TARGET, -378.40f, -813.13f, 59.74f, 0.0f);

		_unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
		//_unit->RemoveAllAuras();
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		_unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		ParentClass::OnCombatStart(mAttacker);
	}

	void OnDied(Unit* mKiller)
	{
		Emote("Your efforts... are in vain.", Text_Yell, 13174);
		ParentClass::OnDied(mKiller);
	}

	void OnKillUnit(Unit* pTarget)
	{
		Emote("Such is the fate of all who oppose the Lich King.", Text_Yell, 13175);
		ParentClass::OnTargetDied(pTarget);
	}

	void OnCombatStop(Unit* mAttacker)
	{
		_crystalCounter = 0;
		_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
		_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		ParentClass::OnCombatStop(mAttacker);
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
		if (_unit->_crystalCount == 4)
		{
			_unit->RemoveAura(SPELL_BEAM_CHANNEL);
		}
		events.Update(1000);
		switch (events.ExecuteEvent())
		{
		case EVENT_SUMMON_FETID_TROLL:
			if (Creature* trigger = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(-378.40f, -813.13f, 59.74f, NPC_CRYSTAL_CHANNEL_TARGET))
				trigger->CastSpell(trigger, SPELL_SUMMON_FETID_TROLL_CORPSE, true);
			events.ScheduleEvent(EVENT_SUMMON_FETID_TROLL, 3000);
			break;
		case EVENT_SUMMON_HULKING_CORPSE:
			if (Creature* trigger = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(-378.40f, -813.13f, 59.74f, NPC_CRYSTAL_CHANNEL_TARGET))
				trigger->CastSpell(trigger, SPELL_SUMMON_HULKING_CORPSE, true);
			events.ScheduleEvent(EVENT_SUMMON_HULKING_CORPSE, 30000);
			break;
		case EVENT_SUMMON_SHADOWCASTER:
			if (Creature* trigger = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(-378.40f, -813.13f, 59.74f, NPC_CRYSTAL_CHANNEL_TARGET))
				trigger->CastSpell(trigger, SPELL_SUMMON_RISEN_SHADOWCASTER, true);
			events.ScheduleEvent(EVENT_SUMMON_SHADOWCASTER, 10000);
			break;
		case EVENT_SUMMON_CRYSTAL_HANDLER:
			if (_crystalCounter++ < 4)
			{
				Emote("Bolster my defenses! Hurry, curse you!", Text_Yell, 13176);
				Announce("Novos the Summoner calls for assistance!");
				if (Creature* trigger = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(-378.40f, -813.13f, 59.74f, NPC_CRYSTAL_CHANNEL_TARGET))
					trigger->CastSpell(trigger, SPELL_SUMMON_CRYSTAL_HANDLER, true);
				events.ScheduleEvent(EVENT_SUMMON_CRYSTAL_HANDLER, 30000);
			}
			break;
		case EVENT_CHECK_PHASE:
			if (_unit->HasAura(SPELL_BEAM_CHANNEL))
			{
				events.ScheduleEvent(EVENT_CHECK_PHASE, 2000);
				break;
			}
			events.Reset();
			events.ScheduleEvent(EVENT_CAST_OFFENSIVE_SPELL, 3000);
			events.ScheduleEvent(EVENT_SPELL_SUMMON_MINIONS, 10000);
			_unit->Unroot();
			_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
			_unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
			break;
		case EVENT_CAST_OFFENSIVE_SPELL:
			if (!_unit->IsCasting())
				if (Unit *target = GetRandomPlayerTarget()) // Needs a random Selection
					_unit->CastSpell(target, SPELL_BLIZZARD, false);

			events.ScheduleEvent(EVENT_CAST_OFFENSIVE_SPELL, 500);
			break;
		case EVENT_SPELL_SUMMON_MINIONS:
			if (_unit->hasStateFlag(0x00008000))
			{
				_unit->CastSpell(_unit, SPELL_SUMMON_MINIONS, false);
				events.ScheduleEvent(EVENT_SPELL_SUMMON_MINIONS, 15000);
				break;
			}
			events.ScheduleEvent(EVENT_SPELL_SUMMON_MINIONS, 500);
			break;
		}
		ParentClass::AIUpdate();
	}
private:
	uint8 _crystalCounter;
	//bool _achievement;
};

void spell_novos_summon_minions(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (spellId == 59933)
	{
		if (SpellEffectType == SPELL_EFFECT_SCRIPT_EFFECT)
		{
			for (uint8 i = 0; i < 4; ++i)
			spellcaster->CastSpell((Unit*)NULL, 59933, true);
		}
	}
};

//CrystalHandlerAI
class CrystalHandlerAI : public CreatureAIScript
{
    public:

        ADD_CREATURE_FACTORY_FUNCTION(CrystalHandlerAI);
        CrystalHandlerAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            heroic = (_unit->GetMapMgr()->iInstanceMode == DUNGEON_HEROIC);
            spells.clear();
            /* SPELLS INIT */
            ScriptSpell* FlashofDarkness = new ScriptSpell;
            FlashofDarkness->normal_spellid = 49668;
            FlashofDarkness->heroic_spellid = 59004;
            FlashofDarkness->chance = 50;
            FlashofDarkness->timer = 4000;
            FlashofDarkness->time = 0;
            FlashofDarkness->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(FlashofDarkness);
        }

		void OnLoad()
		{
			_unit->_crystalCount = 0;
			_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
		}

        void OnCombatStart(Unit* mTarget)
        {
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
            _unit->GetAIInterface()->SetAIState(STATE_IDLE);
            RemoveAIUpdateEvent();
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

        void OnDied(Unit*  mKiller)
        {
            RemoveAIUpdateEvent();
			GameObject* go_1 = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-363.82f, -725.80f, 28.58f, 189301);
			GameObject* go_2 = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-365.02f, -749.71f, 28.93f, 189300);
			GameObject* go_3 = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-390.96f, -750.89f, 28.93f, 189302);
			GameObject* go_4 = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-392.12f, -726.23f, 28.95f, 189299);
			Creature* novos = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(-379.26f, -737.72f, 27.31f, CN_NOVOS_THE_SUMMONER);
			if (go_1 && novos->_crystalCount == 0)
			{
				go_1->Despawn(1000, 0);
				if (novos)
					novos->_crystalCount = 1;
				return; // Do not allow any further
			}
			if (go_2 && novos->_crystalCount == 1)
			{
				go_2->Despawn(1000, 0);
				if (novos)
					novos->_crystalCount = 2;
				return;
			}
			if (go_3 && novos->_crystalCount == 2)
			{
				go_3->Despawn(1000, 0);
				if (novos)
					novos->_crystalCount = 3;
				return;
			}
			if (go_4 && novos->_crystalCount == 3)
			{
				go_4->Despawn(1000, 0);
				if (novos)
					novos->_crystalCount = 4;
				return;
			}
        }

        void AIUpdate()
        {
        }

        void CastScriptSpell(ScriptSpell* spell)
        {
            _unit->Root();
            uint32 spellid = heroic ? spell->heroic_spellid : spell->normal_spellid;
            Unit* spelltarget = NULL;
            switch (spell->target)
            {
                case SPELL_TARGET_SELF:
                {
                    spelltarget = _unit;
                }
                break;
                case SPELL_TARGET_GENERATE:
                {
                    spelltarget = NULL;
                }
                break;
                case SPELL_TARGET_CURRENT_ENEMY:
                {
                    spelltarget = _unit->GetAIInterface()->getNextTarget();
                }
                break;
                case SPELL_TARGET_RANDOM_PLAYER:
                {
                    spelltarget = GetRandomPlayerTarget();
                }
                break;
            }
            _unit->CastSpell(spelltarget, spellid, false);
            _unit->Unroot();
        }

        void Destroy()
        {
            for (uint8 i = 0; i < spells.size(); ++i)
            {
                if (spells[i] != NULL)
                    delete spells[i];
            };

            spells.clear();

            delete this;
        };

    protected:

        bool heroic;
        vector< ScriptSpell* > spells;
};

class FetidTrollAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(FetidTrollAI, MoonScriptBossAI);
	FetidTrollAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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

class HulkingCorpseAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(HulkingCorpseAI, MoonScriptBossAI);
	HulkingCorpseAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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

class RisenShadowcasterAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(RisenShadowcasterAI, MoonScriptBossAI);
	RisenShadowcasterAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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


// KingDreadAI
enum InformationKingDredd
{
	SPELL_BELLOWING_ROAR = 22686,
	SPELL_GRIEVOUS_BITE = 48920,
	SPELL_MANGLING_SLASH = 48873,
	SPELL_FEARSOME_ROAR = 48849,
	SPELL_PIERCING_SLASH = 48878,
	SPELL_RAPTOR_CALL = 59416,
	NPC_DRAKKARI_SCYTHECLAW = 26628,
	NPC_DRAKKARI_GUTRIPPER = 26641,

	SAY_CLAW_EMOTE = 0,

	EVENT_SPELL_BELLOWING_ROAR = 1,
	EVENT_SPELL_GRIEVOUS_BITE = 2,
	EVENT_SPELL_MANGLING_SLASH = 3,
	EVENT_SPELL_FEARSOME_ROAR = 4,
	EVENT_SPELL_PIERCING_SLASH = 5,
	EVENT_SPELL_RAPTOR_CALL = 6,
	EVENT_MENACING_CLAW = 7
};

class KingDreadAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(KingDreadAI, MoonScriptBossAI);
	KingDreadAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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

	void OnCombatStop(Unit* mTarget)
	{
		_raptorCount = 0;
		RemoveAIUpdateEvent();
		ParentClass::OnCombatStop(mTarget);
	}

	void OnCombatStart(Unit* who)
	{
		_raptorCount = 0;

		RegisterAIUpdateEvent(1000);
		events.ScheduleEvent(EVENT_SPELL_BELLOWING_ROAR, 33000);
		events.ScheduleEvent(EVENT_SPELL_GRIEVOUS_BITE, 20000);
		events.ScheduleEvent(EVENT_SPELL_MANGLING_SLASH, 18500);
		events.ScheduleEvent(EVENT_SPELL_FEARSOME_ROAR, RandomUInt(10000, 20000));
		events.ScheduleEvent(EVENT_SPELL_PIERCING_SLASH, 17000);
		if (IsHeroic())
		{
			events.ScheduleEvent(EVENT_MENACING_CLAW, 21000);
			events.ScheduleEvent(EVENT_SPELL_RAPTOR_CALL, RandomUInt(20000, 25000));
		}
		ParentClass::OnCombatStart(who);
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_SPELL_BELLOWING_ROAR:
			_unit->CastSpell(_unit, SPELL_BELLOWING_ROAR, false);
			events.ScheduleEvent(EVENT_SPELL_BELLOWING_ROAR, 40000);
			break;
		case EVENT_SPELL_GRIEVOUS_BITE:
			_unit->CastSpell(_unit->GetVictim(), SPELL_GRIEVOUS_BITE, false);
			events.ScheduleEvent(EVENT_SPELL_GRIEVOUS_BITE, 20000);
			break;
		case EVENT_SPELL_MANGLING_SLASH:
			_unit->CastSpell(_unit->GetVictim(), SPELL_MANGLING_SLASH, false);
			events.ScheduleEvent(EVENT_SPELL_MANGLING_SLASH, 20000);
			break;
		case EVENT_SPELL_FEARSOME_ROAR:
			_unit->CastSpell(_unit, SPELL_FEARSOME_ROAR, false);
			events.ScheduleEvent(EVENT_SPELL_FEARSOME_ROAR, 17000);
			break;
		case EVENT_SPELL_PIERCING_SLASH:
			_unit->CastSpell(_unit->GetVictim(), SPELL_PIERCING_SLASH, false);
			events.ScheduleEvent(EVENT_SPELL_PIERCING_SLASH, 20000);
			break;
		case EVENT_SPELL_RAPTOR_CALL:
			_unit->CastSpell(_unit, SPELL_RAPTOR_CALL, false);
			events.ScheduleEvent(EVENT_SPELL_RAPTOR_CALL, 20000);
			break;
		case EVENT_MENACING_CLAW:
			Announce("King Dred raises his talons menacingly!");
			events.ScheduleEvent(EVENT_MENACING_CLAW, 20000);
			break;
		}
	}
private:
	uint8 _raptorCount;
};

void spell_dred_raptor_call(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (spellId == 59416)
	{
		if (SpellEffectType == SPELL_EFFECT_DUMMY)
		{
			switch (RandomUInt(0, 1))
			{
			case 0: // Drakkari Gutripper
				if (spellcaster)
					spellcaster->GetMapMgr()->GetInterface()->SpawnCreature(NPC_DRAKKARI_GUTRIPPER, -522.02f, -718.89f, 30.26f, 2.41f, true, false, 0, 0);
				break;
			case 1:// Drakkari Scytheclaw
				if (spellcaster)
					spellcaster->GetMapMgr()->GetInterface()->SpawnCreature(NPC_DRAKKARI_SCYTHECLAW, -522.02f, -718.89f, 30.26f, 2.41f, true, false, 0, 0);
				break;
			}
		}
	}	
};

class DrakkariGutripperAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(DrakkariGutripperAI, MoonScriptBossAI);
	DrakkariGutripperAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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
		if (_unit->GetPositionX() == -522.02f && _unit->GetPositionY() == -718.89f && _unit->GetPositionZ() == 30.26f)
		{
			_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
		}
		ParentClass::OnLoad();
	}
};

class DrakkariScytheclawAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(DrakkariScytheclawAI, MoonScriptBossAI);
	DrakkariScytheclawAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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
		if (_unit->GetPositionX() == -522.02f && _unit->GetPositionY() == -718.89f && _unit->GetPositionZ() == 30.26f)
		{
			_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
		}
		ParentClass::OnLoad();
	}
};

enum ProphetInformation
{
	SPELL_CURSE_OF_LIFE = 49527,
	SPELL_RAIN_OF_FIRE = 49518,
	SPELL_SHADOW_VOLLEY = 49528,

	// flesh spells
	SPELL_EYE_BEAM = 49544,
	SPELL_LIGHTNING_BREATH = 49537,
	SPELL_POISON_CLOUD = 49548,

	SPELL_TURN_FLESH = 49356,
	SPELL_TURN_BONES = 53463,
	SPELL_GIFT_OF_THARON_JA = 52509,
	SPELL_DUMMY = 49551,
	SPELL_FLESH_VISUAL = 52582,
	SPELL_CLEAR_GIFT = 53242,

	SPELL_ACHIEVEMENT_CHECK = 61863,
	ACTION_TURN_BONES = 1,

	EVENT_SPELL_CURSE_OF_LIFE = 1,
	EVENT_SPELL_RAIN_OF_FIRE = 2,
	EVENT_SPELL_SHADOW_VOLLEY = 3,
	EVENT_SPELL_EYE_BEAM = 4,
	EVENT_SPELL_LIGHTNING_BREATH = 5,
	EVENT_SPELL_POISON_CLOUD = 6,
	EVENT_SPELL_TURN_FLESH = 7,
	EVENT_TURN_FLESH_REAL = 9,
	EVENT_TURN_BONES_REAL = 10
};
// TheProphetTaronjaAI
class TheProphetTaronjaAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(TheProphetTaronjaAI, MoonScriptBossAI);
	TheProphetTaronjaAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
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

	void OnCombatStart(Unit* who)
	{
		RegisterAIUpdateEvent(1000);
		Emote("Tharon'ja sees all! The work of mortals shall not end the eternal dynasty!", Text_Yell, 13862);
		events.ScheduleEvent(EVENT_SPELL_CURSE_OF_LIFE, 5000);
		events.ScheduleEvent(EVENT_SPELL_RAIN_OF_FIRE, RandomUInt(14000, 18000));
		events.ScheduleEvent(EVENT_SPELL_SHADOW_VOLLEY, RandomUInt(8000, 10000));
		events.ScheduleEvent(EVENT_SPELL_TURN_FLESH, 1000);
		ParentClass::OnCombatStart(who);
	}

	void OnDied(Unit* killer)
	{
		Emote("Im...impossible! Tharon'ja is eternal! Tharon'ja...is...", Text_Yell, 13869);
		_unit->CastSpell(_unit, SPELL_ACHIEVEMENT_CHECK, true);
		_unit->CastSpell(_unit, SPELL_CLEAR_GIFT, true);
		if (_unit->GetDisplayId() != _unit->GetNativeDisplayId())
		{
			_unit->SetDisplayId(_unit->GetNativeDisplayId());
			_unit->CastSpell(_unit, SPELL_FLESH_VISUAL, true);
		}
		ParentClass::OnDied(killer);
	}

	void OnTargetDied(Unit *victim)
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			Emote("As Tharon'ja predicted!", Text_Yell, 13863);
			break;
		case 1:
			Emote("As it was written!", Text_Yell, 13864);
			break;
		}
		ParentClass::OnTargetDied(victim);
	}

	/*void DoAction(int32 param)
	{
		if (param == ACTION_TURN_BONES)
		{
			Emote("Your flesh serves Tharon'ja now!", Text_Yell, 13865);
			events.Reset();
			events.ScheduleEvent(EVENT_TURN_BONES_REAL, 3000);
		}
	}*/

	void OnLoad()
	{
		_unit->SetDisplayId(_unit->GetNativeDisplayId());
		_unit->CastSpell(_unit, SPELL_CLEAR_GIFT, true);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.ExecuteEvent())
		{
		case EVENT_SPELL_CURSE_OF_LIFE:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_CURSE_OF_LIFE, false);
			events.ScheduleEvent(EVENT_SPELL_CURSE_OF_LIFE, 13000);
			break;
		case EVENT_SPELL_RAIN_OF_FIRE:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_RAIN_OF_FIRE, false);
			events.ScheduleEvent(EVENT_SPELL_RAIN_OF_FIRE, 16000);
			break;
		case EVENT_SPELL_SHADOW_VOLLEY:
			_unit->CastSpell(_unit, SPELL_SHADOW_VOLLEY, false);
			events.ScheduleEvent(EVENT_SPELL_SHADOW_VOLLEY, 9000);
			break;
		case EVENT_SPELL_TURN_FLESH:
			if (_unit->GetHealthPct() < 50)
			{
				Emote("No! A taste...all too brief!", Text_Yell, 13867);
				_unit->ClearHateList();
				_unit->CastSpell((Unit*)NULL, SPELL_TURN_FLESH, false);

				events.Reset();
				events.ScheduleEvent(EVENT_TURN_FLESH_REAL, 3000);
				return;
			}
			events.ScheduleEvent(EVENT_SPELL_TURN_FLESH, 1000);
			break;
		case EVENT_TURN_FLESH_REAL:
			_unit->CastSpell(_unit, SPELL_DUMMY, true);
			_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
			events.ScheduleEvent(EVENT_SPELL_EYE_BEAM, 11000);
			events.ScheduleEvent(EVENT_SPELL_LIGHTNING_BREATH, 3000);
			events.ScheduleEvent(EVENT_SPELL_POISON_CLOUD, 6000);
			break;
		case EVENT_SPELL_EYE_BEAM:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_EYE_BEAM, false);
			break;
		case EVENT_SPELL_LIGHTNING_BREATH:
			_unit->CastSpell(_unit->GetVictim(), SPELL_LIGHTNING_BREATH, false);
			events.ScheduleEvent(EVENT_SPELL_LIGHTNING_BREATH, 8000);
			break;
		case EVENT_SPELL_POISON_CLOUD:
			if (Unit* target = GetRandomPlayerTarget())
				_unit->CastSpell(target, SPELL_POISON_CLOUD, false);
			events.ScheduleEvent(EVENT_SPELL_POISON_CLOUD, 10000);
			break;
		case EVENT_TURN_BONES_REAL:
			_unit->SetDisplayId(_unit->GetNativeDisplayId());
			_unit->CastSpell(_unit, SPELL_FLESH_VISUAL, true);
			_unit->CastSpell(_unit, SPELL_CLEAR_GIFT, true);
			events.Reset();
			events.ScheduleEvent(EVENT_SPELL_CURSE_OF_LIFE, 1000);
			events.ScheduleEvent(EVENT_SPELL_RAIN_OF_FIRE, RandomUInt(12000, 14000));
			events.ScheduleEvent(EVENT_SPELL_SHADOW_VOLLEY, RandomUInt(8000, 10000));
			break;
		}
		ParentClass::AIUpdate();
	}
};

void Dummy49551_OnAuraRemove(Aura* aur)
{
	if (aur->GetSpellId() == 49551)
	{
		if (aur->GetUnitCaster())
		{
			aur->GetUnitCaster()->ClearHateList();
			aur->GetUnitCaster()->CastSpell((Unit*)NULL, SPELL_TURN_BONES, false);
			//CreatureAIScript* prophet = aur->GetUnitCaster()->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(aur->GetUnitCaster()->GetPositionX(), aur->GetUnitCaster()->GetPositionY(), aur->GetUnitCaster()->GetPositionZ(), 26632)->GetScript();
			//if (prophet)
			//prophet->DoAction(ACTION_TURN_BONES);
		}
	}
};

void SetupDrakTharonKeep(ScriptMgr* mgr)
{
	// Spells
	mgr->RegisterSpellType(SPELL_EVENT_AURA_REMOVE, (void*)&Dummy49551_OnAuraRemove);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&spell_novos_summon_minions);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&spell_dred_raptor_call);

    //Instance
    mgr->register_instance_script(MAP_DRAK_THARON_KEEP, &InstanceDrakTharonKeepScript::Create);

    //Trash Mobs
	mgr->register_creature_script(NPC_DRAKKARI_GUTRIPPER, &DrakkariGutripperAI::Create);
	mgr->register_creature_script(NPC_DRAKKARI_SCYTHECLAW, &DrakkariScytheclawAI::Create);
	mgr->register_creature_script(27597, &HulkingCorpseAI::Create);
	mgr->register_creature_script(27600, &RisenShadowcasterAI::Create);
    //Bosses
    mgr->register_creature_script(CN_TROLLGORE, &TrollgoreAI::Create);
    mgr->register_creature_script(CN_NOVOS_THE_SUMMONER, &NovosTheSummonerAI::Create);
    mgr->register_creature_script(CN_CRYSTAL_HANDLER, &CrystalHandlerAI::Create);
    mgr->register_creature_script(CN_KING_DRED, &KingDreadAI::Create);
    mgr->register_creature_script(CN_THE_PROPHET_THARONJA, &TheProphetTaronjaAI::Create);
}
