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
#include "Dungeon_CullingOfStratholme.h"

uint32 _killAmount = 0;

class InstanceCullingOfStratholmeScript : public MoonInstanceScript
{
public:

	MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(InstanceCullingOfStratholmeScript, MoonInstanceScript);
	InstanceCullingOfStratholmeScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
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

	void DoTurnZombie()
	{
		for (uint8 i = 0; i < 54; i++)
			PushCreature(InstanceSpawns[i].entry, InstanceSpawns[i].x, InstanceSpawns[i].y, InstanceSpawns[i].z, InstanceSpawns[i].o, InstanceSpawns[i].faction);
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

//MeathookAA
enum MeathookSpells
{
	SPELL_CONSTRICTING_CHAINS_N = 52696,
	SPELL_CONSTRICTING_CHAINS_H = 58823,
	SPELL_DISEASE_EXPULSION_N = 52666,
	SPELL_DISEASE_EXPULSION_H = 58824,
	SPELL_FRENZY = 58841,
};

enum MeathookEvents
{
	EVENT_SPELL_CONSTRICTING_CHAINS = 1,
	EVENT_SPELL_DISEASE_EXPULSION = 2,
	EVENT_SPELL_FRENZY = 3,
};

class MeathookAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(MeathookAI, MoonScriptBossAI);
	MeathookAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
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
		Emote("New toys!", Text_Yell, 13429);
		events.Reset();
		events.RescheduleEvent(EVENT_SPELL_CONSTRICTING_CHAINS, 15000);
		events.RescheduleEvent(EVENT_SPELL_DISEASE_EXPULSION, 4000);
		events.RescheduleEvent(EVENT_SPELL_FRENZY, 20000);
		ParentClass::OnCombatStart(who);
	}

	void OnDied(Unit* killer)
	{
		Emote("This not fun... ", Text_Yell, 13433);
		ParentClass::OnDied(killer);
	}

	void OnTargetDied(Unit *victim)
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			Emote("Why you stop moving? ", Text_Yell, 13431);
			break;
		case 1:
			Emote("Get up! Me not done!", Text_Yell, 13432);
			break;
		}
		ParentClass::OnTargetDied(victim);
	}

	void AIUpdate()
	{
		events.Update(1000);
		if (_unit->IsCasting())
			return;

		switch (events.GetEvent())
		{
		case EVENT_SPELL_DISEASE_EXPULSION:
			if (IsHeroic())
			{
				_unit->CastSpell(_unit, SPELL_DISEASE_EXPULSION_H, false);
				events.RepeatEvent(6000);
			}
			else
			{
				_unit->CastSpell(_unit, SPELL_DISEASE_EXPULSION_N, false);
				events.RepeatEvent(6000);
			}
			break;
		case EVENT_SPELL_FRENZY:
			_unit->CastSpell(_unit, SPELL_FRENZY, false);
			events.RepeatEvent(20000);
			break;
		case EVENT_SPELL_CONSTRICTING_CHAINS:
			if (Unit *pTarget = GetRandomPlayerTarget())
				if (IsHeroic())
				{
					_unit->CastSpell(pTarget, SPELL_CONSTRICTING_CHAINS_H, false);
					events.RepeatEvent(14000);
				}
				else
				{
					_unit->CastSpell(pTarget, SPELL_CONSTRICTING_CHAINS_N, false);
					events.RepeatEvent(14000);
				}
			break;
		}
		ParentClass::AIUpdate();
	}
};

enum SalrammSpells
{
	SPELL_CURSE_OF_TWISTED_FLESH = 58845,
	SPELL_EXPLODE_GHOUL = 52480,
	SPELL_SHADOW_BOLT = 57725,
	SPELL_STEAL_FLESH = 52708,
	SPELL_SUMMON_GHOULS = 52451
};
//SalramTheFleshcrafterAI
class SalramTheFleshcrafterAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(SalramTheFleshcrafterAI, MoonScriptBossAI);
	SalramTheFleshcrafterAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		AddEmote(Event_OnTargetDied, "The fun is just beginning!", Text_Yell, 13473);
		AddEmote(Event_OnTargetDied, "Aah, quality materials!", Text_Yell, 13474);
		AddEmote(Event_OnTargetDied, "Don't worry, I'll make good use of you. ", Text_Yell, 13475);
		AddEmote(Event_OnDied, "You only advance... the master's plan...", Text_Yell, 13483);
	}

	void OnLoad()
	{
		SPELL_CURSE_FLESH = 0;
		SPELL_SUMMON_GHOULS = 0;
		SPELL_SHADOW_BOLT = 0;
		SPELL_STEAL_FLESH = 0;

		_unit->SendChatMessage(14, 0, "You are too late, champion of Lordaeron. The dead shall have their day.");
		_unit->PlaySoundToSet(13471);
		ParentClass::OnLoad();
	}

	void OnCombatStart(Unit* mTarget)
	{
		SPELL_CURSE_FLESH = AddTimer(30000);
		SPELL_SUMMON_GHOULS = AddTimer(22000);
		SPELL_SHADOW_BOLT = AddTimer(10000);
		SPELL_STEAL_FLESH = AddTimer(12345); // Need real timer.
		ParentClass::OnCombatStart(mTarget);
	}

	void OnDied(Unit* mKiller)
	{
		SpawnCreature(CN_ARTHAS, 2358.41f, 1200.106f, 130.68f, 3.55f);
		_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 0);
		ParentClass::OnDied(mKiller);
	}

	Player* GetRandomPlayerTarget()
	{
		vector< uint32 > possible_targets;
		for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
		{
			Player* p = TO< Player* >(*iter);
			if (p->isAlive())
				possible_targets.push_back(p->GetLowGUID());
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
		if (IsTimerFinished(SPELL_CURSE_FLESH))
		{
			if (IsCasting())
			{
				ResetTimer(SPELL_CURSE_FLESH, 2500);
			}
			else
			{
				_unit->CastSpell(_unit->GetAIInterface()->getNextTarget(), SPELL_CURSE_OF_TWISTED_FLESH, false);
				ResetTimer(SPELL_CURSE_FLESH, 37000);
			}
		}
		if (IsTimerFinished(SPELL_SUMMON_GHOULS))
		{
			_unit->SendChatMessage(14, 0, "Say hello to some friends of mine.");
			_unit->PlaySoundToSet(13476);
			_unit->CastSpell(_unit, SPELL_SUMMON_GHOULS, false);
			ResetTimer(SPELL_SUMMON_GHOULS, 10000);
		}
		if (IsTimerFinished(SPELL_SHADOW_BOLT))
		{
			if (IsCasting())
			{
				ResetTimer(SPELL_SHADOW_BOLT, 2500);
			}
			else
			{
				_unit->CastSpell(GetRandomPlayerTarget(), SPELL_SHADOW_BOLT, false);
				ResetTimer(SPELL_SHADOW_BOLT, 12000);
			}
		}
		ParentClass::AIUpdate();
	}
private:
	int32 SPELL_CURSE_FLESH;
	int32 SPELL_SUMMON_GHOULS;
	int32 SPELL_SHADOW_BOLT;
	int32 SPELL_STEAL_FLESH;
};

//ChronoLordEpochAI
class ChronoLordEpochAI : public CreatureAIScript
{
    public:

        ADD_CREATURE_FACTORY_FUNCTION(ChronoLordEpochAI);
        ChronoLordEpochAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            heroic = (_unit->GetMapMgr()->iInstanceMode == DUNGEON_HEROIC);
            spells.clear();
            /* SPELLS INIT */
            ScriptSpell* WoundingStrike = new ScriptSpell;
            WoundingStrike->normal_spellid = 52771;
            WoundingStrike->heroic_spellid = 58830;
            WoundingStrike->chance = 50;
            WoundingStrike->timer = 3000;
            WoundingStrike->time = 0;
            WoundingStrike->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(WoundingStrike);

            ScriptSpell* CurseOfExertion = new ScriptSpell;
            CurseOfExertion->normal_spellid = 52772;
            CurseOfExertion->heroic_spellid = 52772;
            CurseOfExertion->chance = 100;
            CurseOfExertion->timer = 5000;
            CurseOfExertion->time = 0;
            CurseOfExertion->target = SPELL_TARGET_RANDOM_PLAYER;
            spells.push_back(CurseOfExertion);

            ScriptSpell* TimeWarp = new ScriptSpell;
            TimeWarp->normal_spellid = 52766;
            TimeWarp->heroic_spellid = 52766;
            TimeWarp->chance = 50;
            TimeWarp->timer = 6000;
            TimeWarp->time = 0;
            TimeWarp->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(TimeWarp);

            ScriptSpell* TimeStop = new ScriptSpell;
            TimeStop->normal_spellid = 0;
            TimeStop->heroic_spellid = 58848;
            TimeStop->chance = 20;
            TimeStop->timer = 2000;
            TimeStop->time = 0;
            TimeStop->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(TimeStop);
        }

        void OnCombatStart(Unit* mTarget)
        {
            _unit->SendScriptTextChatMessage(SAY_CHRONOLORD_EPOCH_02);
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
            _unit->GetAIInterface()->SetAIState(STATE_IDLE);
            RemoveAIUpdateEvent();
            _unit->RemoveAllAuras();
        }

        void OnDied(Unit* mKiller)
        {
            RemoveAIUpdateEvent();
        }

        void OnTargetDied(Unit* mTarget)
        {
            switch (RandomUInt(2))
            {
                case 0:
                    _unit->SendScriptTextChatMessage(SAY_CHRONOLORD_EPOCH_06);
                    break;
                case 1:
                    _unit->SendScriptTextChatMessage(SAY_CHRONOLORD_EPOCH_07);
                    break;
                case 2:
                    _unit->SendScriptTextChatMessage(SAY_CHRONOLORD_EPOCH_08);
                    break;
            }
        }

        void AIUpdate()
        {
            if (spells.size() > 0)
            {
                for (uint8 i = 0; i < spells.size(); i++)
                {
                    if (spells[i]->time < getMSTime())
                    {
                        if (Rand(spells[i]->chance))
                        {
                            CastScriptSpell(spells[i]);
                            spells[i]->time = getMSTime() + spells[i]->timer;
                        }
                    }
                }
            }
        }

        Player* GetRandomPlayerTarget()
        {
            vector< uint32 > possible_targets;
            for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
            {
                Player* p = TO< Player* >(*iter);
                if (p->isAlive())
                    possible_targets.push_back(p->GetLowGUID());
            }
            if (possible_targets.size() > 0)
            {
                uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
                return _unit->GetMapMgr()->GetPlayer(random_player);
            }
            return NULL;
        }

        void CastScriptSpell(ScriptSpell* spell)
        {
            _unit->Root();
            uint32 spellid = heroic ? spell->heroic_spellid : spell->normal_spellid;
            if (spellid == 0)
                return;
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
            for (uint32 i = 0; i < spells.size(); ++i)
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

//InfiniteCorruptorAI
class InfiniteCorruptorAI : public CreatureAIScript
{
    public:

        ADD_CREATURE_FACTORY_FUNCTION(InfiniteCorruptorAI);
        InfiniteCorruptorAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            heroic = (_unit->GetMapMgr()->iInstanceMode == DUNGEON_HEROIC);
            spells.clear();
            ScriptSpell* VoidStrike = new ScriptSpell;
            VoidStrike->normal_spellid = 60590;
            VoidStrike->heroic_spellid = 60590;
            VoidStrike->chance = 50;
            VoidStrike->timer = 1000;
            VoidStrike->time = 0;
            VoidStrike->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(VoidStrike);

            ScriptSpell* CorruptingBlight = new ScriptSpell;
            CorruptingBlight->normal_spellid = 60588;
            CorruptingBlight->heroic_spellid = 60588;
            CorruptingBlight->chance = 50;
            CorruptingBlight->timer = 3000;
            CorruptingBlight->time = 0;
            CorruptingBlight->target = SPELL_TARGET_RANDOM_PLAYER;
            spells.push_back(CorruptingBlight);
        }

        void OnCombatStart(Unit* mTarget)
        {
            _unit->SendScriptTextChatMessage(SAY_INFINITE_CORRUP_01);
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
            _unit->GetAIInterface()->SetAIState(STATE_IDLE);
            RemoveAIUpdateEvent();
            _unit->RemoveAllAuras();
        }

        void OnDied(Unit* mKiller)
        {
            _unit->SendScriptTextChatMessage(SAY_INFINITE_CORRUP_02);
            RemoveAIUpdateEvent();
        }

        void AIUpdate()
        {
            if (spells.size() > 0)
            {
                for (uint8 i = 0; i < spells.size(); i++)
                {
                    if (spells[i]->time < getMSTime())
                    {
                        if (Rand(spells[i]->chance))
                        {
                            CastScriptSpell(spells[i]);
                            spells[i]->time = getMSTime() + spells[i]->timer;
                        }
                    }
                }
            }
        }

        Player* GetRandomPlayerTarget()
        {
            vector< uint32 > possible_targets;
            for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
            {
                Player* p = TO< Player* >(*iter);
                if (p->isAlive())
                    possible_targets.push_back(p->GetLowGUID());
            }
            if (possible_targets.size() > 0)
            {
                uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
                return _unit->GetMapMgr()->GetPlayer(random_player);
            }
            return NULL;
        }

        void CastScriptSpell(ScriptSpell* spell)
        {
            _unit->Root();
            uint32 spellid = heroic ? spell->heroic_spellid : spell->normal_spellid;
            if (spellid == 0)
                return;
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
            for (uint32 i = 0; i < spells.size(); ++i)
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

//MalganisAI
class MalganisAI : public CreatureAIScript
{
    public:

        ADD_CREATURE_FACTORY_FUNCTION(MalganisAI);
        MalganisAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            heroic = (_unit->GetMapMgr()->iInstanceMode == DUNGEON_HEROIC);
            scene = true;
            spells.clear();
            ScriptSpell* CarrionSwarm = new ScriptSpell;
            CarrionSwarm->normal_spellid = 52720;
            CarrionSwarm->heroic_spellid = 58852;
            CarrionSwarm->chance = 60;
            CarrionSwarm->timer = 10000;
            CarrionSwarm->time = 0;
            CarrionSwarm->target = SPELL_TARGET_CURRENT_ENEMY;
            spells.push_back(CarrionSwarm);

            ScriptSpell* MindBlast = new ScriptSpell;
            MindBlast->normal_spellid = 52722;
            MindBlast->heroic_spellid = 58850;
            MindBlast->chance = 50;
            MindBlast->timer = 5000;
            MindBlast->time = 0;
            MindBlast->target = SPELL_TARGET_RANDOM_PLAYER;
            spells.push_back(MindBlast);

            ScriptSpell* Sleep = new ScriptSpell;
            Sleep->normal_spellid = 52721;
            Sleep->heroic_spellid = 58849;
            Sleep->chance = 40;
            Sleep->timer = 7000;
            Sleep->time = 0;
            Sleep->target = SPELL_TARGET_RANDOM_PLAYER;
            spells.push_back(Sleep);

            ScriptSpell* VampiricTouch = new ScriptSpell;
            VampiricTouch->normal_spellid = 52723;
            VampiricTouch->heroic_spellid = 52723;
            VampiricTouch->chance = 90;
            VampiricTouch->timer = 30000;
            VampiricTouch->time = 0;
            VampiricTouch->target = SPELL_TARGET_SELF;
            spells.push_back(VampiricTouch);
        }

        void OnCombatStart(Unit* mTarget)
        {
            _unit->SendScriptTextChatMessage(SAY_MALGANIS_03);
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
            _unit->GetAIInterface()->SetAIState(STATE_IDLE);
            RemoveAIUpdateEvent();
            _unit->RemoveAllAuras();
        }

        void OnTargetDied(Unit* mTarget)
        {
            switch (RandomUInt(2))
            {
                case 0:
                    _unit->SendScriptTextChatMessage(SAY_MALGANIS_04);
                    break;
                case 1:
                    _unit->SendScriptTextChatMessage(SAY_MALGANIS_05);
                    break;
                case 2:
                    _unit->SendScriptTextChatMessage(SAY_MALGANIS_06);
                    break;
            }
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            if (_unit->HasAura(52723))    //handling a dummy :)
            {
                _unit->Heal(_unit, 52723, fAmount / 2);
            }
            if (_unit->GetHealthPct() < 2)
            {
                //lala
                _unit->Root();
                _unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
                for (uint8 i = 0; i < 7; i++)
                    _unit->SchoolImmunityList[i] = 1;
                RemoveAIUpdateEvent();
                _unit->SendScriptTextChatMessage(SAY_MALGANIS_17);
                //spawn a chest and go
                GameObject* go = _unit->GetMapMgr()->CreateGameObject(190663);
                go->CreateFromProto(190663, _unit->GetMapMgr()->GetMapId(), _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f);
                go->PushToWorld(_unit->GetMapMgr());
                _unit->Despawn(1, 0);
            }
        }

        void OnDied(Unit* mKiller)
        {
            _unit->SendScriptTextChatMessage(SAY_MALGANIS_16);
            RemoveAIUpdateEvent();
        }

        void AIUpdate()
        {
            if (!scene && spells.size() > 0)
            {
                for (uint8 i = 0; i < spells.size(); i++)
                {
                    if (spells[i]->time < getMSTime())
                    {
                        if (Rand(spells[i]->chance))
                        {
                            CastScriptSpell(spells[i]);
                            spells[i]->time = getMSTime() + spells[i]->timer;
                        }
                    }
                }
            }
            else
            {
                //this is ugly, better ideas?
                scene = false;
                Creature* citizen = NULL;
                uint32 entry = 0;
                for (uint32 i = 0; i != _unit->GetMapMgr()->m_CreatureHighGuid; ++i)
                {
                    if (_unit->GetMapMgr()->CreatureStorage[i] != NULL)
                    {
                        entry = _unit->GetMapMgr()->CreatureStorage[i]->GetEntry();
                        if (entry == 31126 || entry == 31127 || entry == 28167 || entry == 28169)
                        {
                            citizen = _unit->GetMapMgr()->CreatureStorage[i];
                            CreatureProto* cp = CreatureProtoStorage.LookupEntry(27737);//risen zombie
                            CreatureInfo* ci = CreatureNameStorage.LookupEntry(27737);
                            Creature* c = NULL;
                            if (cp && ci)
                            {
                                c = _unit->GetMapMgr()->CreateCreature(27737);
                                if (c)
                                {
                                    //position is guessed
                                    c->Load(cp, citizen->GetPositionX(), citizen->GetPositionY(), citizen->GetPositionZ(), citizen->GetOrientation());
                                    c->PushToWorld(_unit->GetMapMgr());
                                }
                            }
                            citizen->Despawn(0, 0);
                        }
                    }
                }
            }
        }

        Player* GetRandomPlayerTarget()
        {
            vector< uint32 > possible_targets;
            for (set< Object* >::iterator iter = _unit->GetInRangePlayerSetBegin(); iter != _unit->GetInRangePlayerSetEnd(); ++iter)
            {
                Player* p = TO< Player* >(*iter);
                if (p->isAlive())
                    possible_targets.push_back(p->GetLowGUID());
            }
            if (possible_targets.size() > 0)
            {
                uint32 random_player = possible_targets[Rand(uint32(possible_targets.size() - 1))];
                return _unit->GetMapMgr()->GetPlayer(random_player);
            }
            return NULL;
        }

        void CastScriptSpell(ScriptSpell* spell)
        {
            _unit->Root();
            uint32 spellid = heroic ? spell->heroic_spellid : spell->normal_spellid;
            if (spellid == 0)
                return;
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
            for (uint32 i = 0; i < spells.size(); ++i)
            {
                if (spells[i] != NULL)
                    delete spells[i];
            };

            spells.clear();

            delete this;
        };

    protected:

        bool heroic;
        bool scene;
        vector< ScriptSpell* > spells;
};


class Quest_Dispelling_Illusions : public QuestScript
{
    public:

        void OnQuestStart(Player* mTarget, QuestLogEntry* qLogEntry)
        {
            for (uint8 i = 0; i < 5; i++)
                SpawnCrates(i, mTarget->GetMapMgr());
        }
        void SpawnCrates(uint32 id, MapMgr* pMapMgr)
        {
            uint32 entry = 190094;
            float x = 0.0f, y = 0.0f, z = 0.0f, o = 0.0f;
            switch (id)
            {
                case 0:
                {
                    x = 1570.92f;
                    y = 669.933f;
                    z = 102.309f;
                    o = -1.64061f;
                }
                break;
                case 1:
                {
                    x = 1579.42f;
                    y = 621.446f;
                    z = 99.7329f;
                    o = 2.9845f;
                }
                break;
                case 2:
                {
                    x = 1629.68f;
                    y = 731.367f;
                    z = 112.847f;
                    o = -0.837757f;
                }
                break;
                case 3:
                {
                    x = 1674.39f;
                    y = 872.307f;
                    z = 120.394f;
                    o = -1.11701f;
                }
                break;
                case 4:
                {
                    x = 1628.98f;
                    y = 812.142f;
                    z = 120.689f;
                    o = 0.436332f;
                }
                break;
            }
            GameObject* crate = pMapMgr->GetInterface()->GetGameObjectNearestCoords(x, y, z, 190094);
            if (crate)
                crate->Despawn(0, 0);
            GameObject* go = pMapMgr->CreateGameObject(entry);
            go->CreateFromProto(entry, pMapMgr->GetMapId(), x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f);
            go->PushToWorld(pMapMgr);
        }
};


static Location walk[] =
{
    { 0, 0, 0, 0 },
    { 1811.2177f, 1276.5729f, 141.9048f, 0.098f },
    { 1884.9484f, 1284.9110f, 143.7776f, 6.2810f },
    { 1897.4763f, 1291.1870f, 143.5821f, 1.4194f }
};

//UtherAI
/*class UtherAI : public CreatureAIScript
{
    public:

        ADD_CREATURE_FACTORY_FUNCTION(UtherAI);
        UtherAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            _unit->GetAIInterface()->addWayPoint(CreateWaypoint(1, 0, Flag_Run));
            _unit->GetAIInterface()->addWayPoint(CreateWaypoint(2, 0, Flag_Run));
            _unit->GetAIInterface()->addWayPoint(CreateWaypoint(3, 90000, Flag_Run));
            check = true;
        }

        void OnReachWP(uint32 i, bool usl)
        {
            if (i == 3 && check)
            {
                check = false;
                Creature* Arthas = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), CN_ARTHAS);
                Creature* Jaina = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), CN_JAINA);
                if (Arthas && Jaina)  //Show must go on!
                {
                    //we add 0,5s per speech
                    //1 = 0s
                    Arthas->SendScriptTextChatMessage(SAY_ARTHAS_01);
                    //2 = 2,5s
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_01, 2500);
                    //3 = 9s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_02, 9000);
                    //4 = 14,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_03, 14500);
                    //5 = 25s
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_02, 25000);
                    //6 = 26,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_04, 26500);
                    //7 = 29s
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_03, 29000);
                    //8 = 33,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_05, 33500);
                    //9 = 38s
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_04, 38000);
                    //10 = 44,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_06, 44500);
                    //11 = 49s
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_05, 49000);
                    //12 = 53,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_07, 53500);
                    //13 = 65s
                    Jaina->SendTimedScriptTextChatMessage(SAY_JAINA_01, 65000);
                    //14 = 67,5s
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_08, 67500);
                    //15 = 77s
                    //here few knights should leave, after speech, Uther should leave also
                    _unit->SendTimedScriptTextChatMessage(SAY_UTHER_06, 77000);
                    //16 = 80,5s
                    //Jaina begins leaving
                    Arthas->SendTimedScriptTextChatMessage(SAY_ARTHAS_09, 80500);
                    //17 = 82s
                    Jaina->SendTimedScriptTextChatMessage(SAY_JAINA_02, 82000);
                    //trigger Arthas actions = 86,5s
                    sEventMgr.AddEvent(Arthas, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, 86500, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
                    _unit->Despawn(100000, 0);
                }
            }
        }

        inline WayPoint* CreateWaypoint(int id, uint32 waittime, uint32 flags)
        {
            WayPoint* wp = _unit->CreateWaypointStruct();
            wp->id = id;
            wp->x = walk[id].x;
            wp->y = walk[id].y;
            wp->z = walk[id].z;
            wp->o = walk[id].o;
            wp->waittime = waittime;
            wp->flags = flags;
            wp->forwardemoteoneshot = false;
            wp->forwardemoteid = 0;
            wp->backwardemoteoneshot = false;
            wp->backwardemoteid = 0;
            wp->forwardskinid = 0;
            wp->backwardskinid = 0;
            return wp;
        }

    private:

        bool check;
}; */

static Location ArthasWalk[] =
{
    { 0, 0, 0, 0 },
    { 1908.9722f, 1312.8898f, 149.9889f, 0.6858f },
    { 1884.9483f, 1284.9110f, 143.7776f, -2.2802f },
    { 1991.4326f, 1286.5925f, 145.4636f, 1.2591f },
    { 2007.2526f, 1327.5848f, 142.9831f, 5.5553f },
    { 2024.4555f, 1308.2036f, 143.4564f, 4.8492f },
    { 2028.9012f, 1285.9205f, 143.6552f, 0.0661f },
    { 2078.9479f, 1287.9812f, 141.4830f, 0.0308f }
};

enum Npcs
{
	NPC_TIME_RIFT = 28409,
	NPC_INFINITE_ADVERSARY = 27742,
	NPC_INFINITE_HUNTER = 27743,
	NPC_INFINITE_AGENT = 27744,
	NPC_EPOCH = 26532,
};

//ArthasAI
class ArthasAI : public MoonScriptBossAI
{
    public:

		MOONSCRIPT_FACTORY_FUNCTION(ArthasAI, MoonScriptBossAI);
		ArthasAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
			waypoint_timer_1 = 0;
			waypoint_timer_2 = 0;
			waypoint_timer_3 = 0;
			waypoint_timer_4 = 0;
			emote_timer_remove = 0;
			_stage = 0;
			_stage2 = 0;
			lordaeron_crier_spawn_timer = 0;

			SetPhase(1);
        }

		void OnLoad()
		{
			if (_unit->GetPositionX() == 2051.689941f && _unit->GetPositionY() == 1287.449951f) // Purging
			{
				_unit->SetGossipStatus(0);
			}
			if (_unit->GetPositionX() == 2358.41f && _unit->GetPositionY() == 1200.106f) // Town Hall
			{
				_unit->SetFaction(35);
				_unit->SetGossipStatus(1);
				_unit->SetEquippedItem(1, 37579);
			}
			if (_unit->GetPositionX() == 2539.810f && _unit->GetPositionY() == 1125.708f) // Gaunlet (Part 1)
			{
				_unit->SetFaction(35);
				_unit->SetGossipStatus(2);
				_unit->SetEquippedItem(1, 37579);
			}
			ParentClass::OnLoad();
		}

		void AIUpdate()
		{
			if (_unit->GetGossipStatus() == 3) // The Gaunlet (Part 4)
			{
				if (GetPhase() == 1)
				{
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_36, 2000);
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2326.98f, 1414.774f, 127.607f, 2.84f);
					waypoint_timer_4 = AddTimer(7000);
					SetPhase(20);
				}
				if (IsTimerFinished(waypoint_timer_4) && _stage2 == 0)
				{
					SpawnCreature(CN_MALGANIS, 2297.102f, 1503.55f, 128.36f, 5.022f);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_37, 12000);
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2300.57f, 1475.31f, 127.909f, 1.98f);
					_stage2 = 1;
				}
			}
			if (_unit->GetGossipStatus() == 2) // The Gaunlet (Part 3)
			{
				if (GetPhase() == 1)
				{
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_32, 2000);
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2575.91f, 1164.297f, 126.18f, 0.74f);
					waypoint_timer_4 = AddTimer(20000);
					SetPhase(20);
				}
				if (IsTimerFinished(waypoint_timer_4) && _stage2 == 0)
				{
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2513.05f, 1322.43f, 131.682f, 1.83f);
					_stage2 = 1;
					ResetTimer(waypoint_timer_4, 31000);
				}
				if (IsTimerFinished(waypoint_timer_4) && _stage2 == 1)
				{
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_33, 1000);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_34, 10000);
					_stage2 = 2;
					ResetTimer(waypoint_timer_4, 12000);
				}
				if (IsTimerFinished(waypoint_timer_4) && _stage2 == 2)
				{
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2456.637f, 1435.37f, 131.68f, 2.04f);
					_stage2 = 3;
					ResetTimer(waypoint_timer_4, 20000);
				}
				if (IsTimerFinished(waypoint_timer_4) && _stage2 == 3)
				{
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2362.50f, 1404.06f, 128.81f, 2.99f);
					_unit->SetGossipStatus(3); // Part 4
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_35, 1000);
					_unit->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					SetPhase(1);
					_stage2 = 4;
					//ResetTimer(waypoint_timer_4, 20000); // 16 sec
				}
			}
			if (_unit->GetGossipStatus() == 1)
			{
				if (GetPhase() == 1)
				{
					SpawnCreature(28167, 2399.002f, 1207.097f, 134.039f, 3.3f);
					SpawnCreature(28167, 2402.53f, 1203.51f, 134.01f, 3.2f);
					SpawnCreature(28167, 2400.38f, 1199.63f, 133.99f, 3.01f);
					Creature* citizen = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(2399.002f, 1207.097f, 134.039f, 28167);
					if (citizen)
					{
						sEventMgr.AddEvent(citizen, &Creature::SendChatMessage, uint8(12), uint32(0), "Ah, you've finally arrived Prince Arthas. You're here just in the nick of time.", uint32(0), EVENT_UNIT_CHAT_MSG, 8000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
					}
					_unit->SendChatMessage(12, 0, "Follow me, I know the way through.");
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2394.54f, 1204.53f, 134.125f, 0.23f);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_18, 12000);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_19, 15000);
					if (citizen)
					{
						sEventMgr.AddEvent(citizen, &Creature::SendChatMessage, uint8(12), uint32(0), "There's no need for you to understand, Arthas. All you need to do is die.", uint32(0), EVENT_UNIT_CHAT_MSG, 19000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
					}
					waypoint_timer_3 = AddTimer(45000);
					SetPhase(20);
				}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 0)
				{
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_20, 1000);
					_unit->GetAIInterface()->SetRun();
					_unit->GetAIInterface()->MoveTo(2441.74f, 1217.74f, 133.98f, 0.18f);
					_stage = 1;
					ResetTimer(waypoint_timer_3, 9000);
				}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 1)
				{
					_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2447.155f, 1191.51f, 148.076f, 4.93f);
						_stage = 2;
						ResetTimer(waypoint_timer_3, 7000);
				}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 2)
					{
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_21, 1000);
						_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2438.38f, 1191.41f, 148.076f, 3.15f);
						SpawnCreature(NPC_TIME_RIFT, 2431.790039f, 1190.670044f, 148.076004f, 0.187923f);
						SpawnCreature(NPC_INFINITE_ADVERSARY, 2433.857910f, 1185.612061f, 148.075974f, 4.566168f);
						SpawnCreature(NPC_INFINITE_ADVERSARY, 2437.577881f, 1188.241089f, 148.075974f, 0.196999f);
						SpawnCreature(NPC_INFINITE_AGENT, 2437.165527f, 1192.294922f, 148.075974f, 0.169247f);
						SpawnCreature(NPC_INFINITE_HUNTER, 2434.989990f, 1197.679565f, 148.075974f, 0.715971f);
						_stage = 3;
						ResetTimer(waypoint_timer_3, 45000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 3)
					{
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_22, 1000);
						_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2399.195f, 1193.58f, 148.07f, 3.08f);
						_stage = 4;
						ResetTimer(waypoint_timer_3, 10000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 4)
					{
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_23, 7000);
						_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2409.47f, 1156.86f, 148.19f, 4.93f);
						_stage = 5;
						ResetTimer(waypoint_timer_3, 7000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 5)
					{
						SpawnCreature(NPC_TIME_RIFT, 2403.954834f, 1178.815430f, 148.075943f, 4.966126f);
						SpawnCreature(NPC_INFINITE_AGENT, 2403.676758f, 1171.495850f, 148.075607f, 4.902797f);
						SpawnCreature(NPC_INFINITE_HUNTER, 2407.691162f, 1172.162720f, 148.075607f, 4.963010f);
						SpawnCreature(NPC_TIME_RIFT, 2414.217041f, 1133.446167f, 148.076050f, 1.706972f);
						SpawnCreature(NPC_INFINITE_ADVERSARY, 2416.024658f, 1139.456177f, 148.076431f, 1.752129f);
						SpawnCreature(NPC_INFINITE_HUNTER, 2410.866699f, 1139.680542f, 148.076431f, 1.752129f);
						_stage = 6;
						ResetTimer(waypoint_timer_3, 45000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 6)
					{
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_24, 1000);
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_25, 6000);
						_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2422.37f, 1117.22f, 148.076f, 5.52f);
						SpawnCreature(NPC_TIME_RIFT, 2433.081543f, 1099.869751f, 148.076157f, 1.809509f);
						SpawnCreature(NPC_INFINITE_ADVERSARY, 2426.947998f, 1107.471680f, 148.076019f, 1.877580f);
						SpawnCreature(NPC_INFINITE_HUNTER, 2432.944580f, 1108.896362f, 148.208160f, 2.199241f);
						SpawnCreature(NPC_TIME_RIFT, 2444.077637f, 1114.366089f, 148.076157f, 3.049565f);
						SpawnCreature(NPC_INFINITE_ADVERSARY, 2438.190674f, 1118.368164f, 148.076172f, 3.139232f);
						SpawnCreature(NPC_INFINITE_AGENT, 2435.861328f, 1113.402954f, 148.169327f, 2.390271f);
						_stage = 7;
						ResetTimer(waypoint_timer_3, 50000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 7)
					{
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_27, 1000);
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_28, 12000);
						_unit->GetAIInterface()->SetRun();
						_unit->GetAIInterface()->MoveTo(2441.27f, 1111.71f, 148.21f, 0.14f);
						SpawnCreature(NPC_TIME_RIFT, 2463.131592f, 1115.391724f, 152.473129f, 3.409651f);
						SpawnCreature(NPC_EPOCH, 2451.809326f, 1112.901245f, 149.220459f, 3.363617f);
						_stage = 8;
						ResetTimer(waypoint_timer_3, 5000);
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 8)
					{
						Creature* Epoch = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(2451.809326f, 1112.901245f, 149.220459f, NPC_EPOCH);
						if (Epoch && Epoch->IsDead())
						{ 
							_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_29, 1000);
							_unit->GetAIInterface()->SetRun();
							_unit->GetAIInterface()->MoveTo(2471.33f, 1122.80f, 150.03f, 0.14f);
							_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_30, 9000);
							_stage = 9;
							ResetTimer(waypoint_timer_3, 12000);
						}
						else
						{
							ResetTimer(waypoint_timer_3, 5000);
						}
					}
				if (IsTimerFinished(waypoint_timer_3) && _stage == 9)
					{
						GameObject* BookShelf = _unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(2471.33f, 1122.80f, 150.03f, 188686);
						if (BookShelf)
						{
							BookShelf->SetState(GAMEOBJECT_STATE_OPEN);
							_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_31, 2000);
							SpawnCreature(CN_ARTHAS, 2539.81f, 1125.708f, 130.81f, 0.6f);
							_unit->Despawn(4000, 0);
							_stage = 10;
						}
					}
			}
			if (_unit->GetGossipStatus() == 0)
			{
				if (IsTimerFinished(waypoint_timer_1))
				{
					Creature* citizen = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(2092.02f, 1275.04f, 140.83f, 28167, 115683);
					citizen->SetOrientation(2.15f);
					citizen->SendChatMessage(12, 0, "Prince Arthas, may the light be praised! Many people in the town have begun to fall seriously ill, can you help us?");
					_unit->GetAIInterface()->MoveTo(citizen->GetPositionX() - 2, citizen->GetPositionY() + 2, citizen->GetPositionZ(), 5.3f);
					waypoint_timer_2 = AddTimer(7000);
					RemoveTimer(waypoint_timer_1);
				}
				if (IsTimerFinished(waypoint_timer_2))
				{
					Creature* citizen = _unit->GetMapMgr()->GetInterface()->GetCreatureSpawnIdNearestCoords(2092.02f, 1275.04f, 140.83f, 28167, 115683);
					_unit->SetEmoteState(54);
					_unit->DealDamage(citizen, 5000, 0, 0, 0);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_12, 1000);
					emote_timer_remove = AddTimer(500);
					RemoveTimer(waypoint_timer_2);
				}
				if (IsTimerFinished(emote_timer_remove))
				{
					_unit->SetEmoteState(0);
					SetPhase(2);
					RemoveTimer(emote_timer_remove);
				}
				if (IsTimerFinished(lordaeron_crier_spawn_timer))
				{
					SpawnCreature(27913, 2194.87f, 1224.75f, 137.39f);
					RemoveTimer(lordaeron_crier_spawn_timer);
				}
				switch (GetPhase())
				{
				case 1:
				{
					_unit->SetFaction(35);
					_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_11, 300);
					_unit->GetAIInterface()->MoveTo(2081.87f, 1288.31f, 141.291f, 6.2f);
					waypoint_timer_1 = AddTimer(19000);
					SetPhase(17);
				}
				break;
				case 2:
				{
					//we need that tricky animation here
					//spawn Mal'Ganis
					CreatureProto* cp = CreatureProtoStorage.LookupEntry(26533);
					CreatureInfo* ci = CreatureNameStorage.LookupEntry(26533);
					Creature* c = NULL;
					if (cp && ci)
					{
						c = _unit->GetMapMgr()->CreateCreature(26533);
						if (c)
						{
							//position is guessed
							c->Load(cp, 2113.52f, 1288.01f, 136.382f, 2.30383f);
							c->PushToWorld(_unit->GetMapMgr());
						}
					}
					if (c)
					{
						c->bInvincible = true;
						c->GetAIInterface()->m_canMove = false;
						c->GetAIInterface()->SetAllowedToEnterCombat(false);
						for (uint8 i = 0; i < 7; i++)
							c->SchoolImmunityList[i] = 1;
						c->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
						//1 = 0s
						c->SendScriptTextChatMessage(SAY_MALGANIS_01);
						//2 = 13s
						//change all citizens to undeads...
						InstanceCullingOfStratholmeScript* pInstance = (InstanceCullingOfStratholmeScript*)_unit->GetMapMgr()->GetScript();
						if (pInstance)
						{
							pInstance->DoTurnZombie();
						}
						//sEventMgr.AddEvent(c, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, 13000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
						c->SendTimedScriptTextChatMessage(SAY_MALGANIS_02, 13000);
						//2 = 32s
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_14, 32000);
						c->Despawn(38500, 0);
						//3 = 37s
						_unit->SendTimedScriptTextChatMessage(SAY_ARTHAS_15, 40000);
						lordaeron_crier_spawn_timer = AddTimer(45000);
						_unit->Despawn(51000, 144000000);
					}
					SetPhase(3);
				}
				break;
				}
			}
			ParentClass::AIUpdate();
	    }

        inline WayPoint* CreateWaypoint(int id, uint32 waittime, uint32 flags)
        {
            WayPoint* wp = _unit->CreateWaypointStruct();
            wp->id = id;
            wp->x = ArthasWalk[id].x;
            wp->y = ArthasWalk[id].y;
            wp->z = ArthasWalk[id].z;
            wp->o = ArthasWalk[id].o;
            wp->waittime = waittime;
            wp->flags = flags;
            wp->forwardemoteoneshot = false;
            wp->forwardemoteid = 0;
            wp->backwardemoteoneshot = false;
            wp->backwardemoteid = 0;
            wp->forwardskinid = 0;
            wp->backwardskinid = 0;
            return wp;
        }

    protected:
		int32 waypoint_timer_1;
		int32 waypoint_timer_2;
		int32 waypoint_timer_3;
		int32 waypoint_timer_4;
		int32 _stage;
		int32 _stage2;
		int32 emote_timer_remove;
		int32 lordaeron_crier_spawn_timer;
        uint32 phase;
};

// Scourge waves
enum WaveNpcs
{
	ENRAGING_GHOUL_MOB = 27729,
	ACOLYTE_MOB = 27731,
	MASTER_NECROMANCER_MOB = 27732,
	CRYPT_FIEND_MOB = 27734,
	PATCHWORK_CONSTRUCT_MOB = 27736,
	TOMB_STALKER_MOB = 28199,
	DARK_NECROMANCER_MOB = 28200,
	BILE_GOLEM_MOB = 28201,
	DEVOURING_GHOUL_MOB = 28249,
	ZOMBIE_MOB = 27737,
};

class LordaeronCrierAI : public MoonScriptBossAI
{
public:

	MOONSCRIPT_FACTORY_FUNCTION(LordaeronCrierAI, MoonScriptBossAI);
	LordaeronCrierAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		wavetimer = 0;
		_randomFeel = false;
		_firstWave = true;
	}

	void OnLoad()
	{
		_unit->SetDisplayId(11686);
		_unit->SetFaction(35);
		wavetimer = AddTimer(5000);
		RegisterAIUpdateEvent(1000);
		SetPhase(1);
		ParentClass::OnLoad();
	}

	// Enemies depending on location.
	void RandomEnemies_MRG()
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			SpawnCreature(PATCHWORK_CONSTRUCT_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			break;
		case 1:
			SpawnCreature(DEVOURING_GHOUL_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(CRYPT_FIEND_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(PATCHWORK_CONSTRUCT_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2215.529f, 1329.75f, 128.41f, 3.10f);
			break;

		}
	}

	void RandomEnemies_FLG()
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			SpawnCreature(ZOMBIE_MOB, 2164.698975f, 1255.392944f, 135.040878f, 0.490202f);
			SpawnCreature(ZOMBIE_MOB, 2183.501465f, 1263.079102f, 134.859055f, 3.169981f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2177.512939f, 1247.313843f, 135.846695f, 1.696574f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2171.991943f, 1246.615845f, 135.745026f, 1.696574f);
			break;
		case 1:
			SpawnCreature(DEVOURING_GHOUL_MOB, 2172.686279f, 1259.618164f, 134.391754f, 1.865499f);
			SpawnCreature(CRYPT_FIEND_MOB, 2177.649170f, 1256.061157f, 135.096512f, 1.849572f);
			SpawnCreature(PATCHWORK_CONSTRUCT_MOB, 2170.782959f, 1253.594849f, 134.973022f, 1.849572f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2175.595703f, 1249.041992f, 135.603531f, 1.849572f);
			break;
		}
	}

	void RandomEnemies_ESG()
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			SpawnCreature(DEVOURING_GHOUL_MOB, 2254.434326f, 1163.427612f, 138.055038f, 2.077358f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2254.703613f, 1158.867798f, 138.212234f, 2.345532f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2257.615723f, 1162.310913f, 138.091202f, 2.077358f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2258.258057f, 1157.250732f, 138.272873f, 2.387766f);
			break;
		case 1:
			SpawnCreature(DARK_NECROMANCER_MOB, 2264.013428f, 1174.055908f, 138.093094f, 2.860481f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2264.207764f, 1170.892700f, 138.034973f, 2.860481f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2266.948975f, 1176.898926f, 137.976929f, 2.860481f);
			SpawnCreature(TOMB_STALKER_MOB, 2269.215576f, 1170.109253f, 137.742691f, 2.860481f);
			SpawnCreature(CRYPT_FIEND_MOB, 2273.106689f, 1176.101074f, 137.880508f, 2.860481f);
			break;
		}
	}

	void RandomEnemies_TH()
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			SpawnCreature(TOMB_STALKER_MOB, 2348.120117f, 1202.302490f, 130.491104f, 4.698538f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2352.863525f, 1207.819092f, 130.424271f, 4.949865f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2343.593750f, 1207.915039f, 130.781311f, 4.321547f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2348.257324f, 1212.202515f, 130.670135f, 4.450352f);
			break;
		case 1:
			SpawnCreature(BILE_GOLEM_MOB, 2349.701660f, 1188.436646f, 130.428864f, 3.908642f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2349.909180f, 1194.582642f, 130.417816f, 3.577001f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2354.662598f, 1185.692017f, 130.552032f, 3.577001f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2354.716797f, 1191.614380f, 130.539810f, 3.577001f);
			break;
		}
	}

	void RandomEnemies_KSF()
	{
		switch (RandomUInt(0, 1))
		{
		case 0:
			SpawnCreature(TOMB_STALKER_MOB, 2139.825195f, 1356.277100f, 132.199615f, 5.820131f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2137.073486f, 1362.464844f, 132.271637f, 5.820131f);
			SpawnCreature(DEVOURING_GHOUL_MOB, 2134.075684f, 1354.148071f, 131.885864f, 5.820131f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2133.302246f, 1358.907837f, 132.037689f, 5.820131f);
			break;
		case 1:
			SpawnCreature(PATCHWORK_CONSTRUCT_MOB, 2145.212891f, 1355.288086f, 132.288773f, 6.004838f);
			SpawnCreature(DARK_NECROMANCER_MOB, 2137.078613f, 1357.612671f, 132.173340f, 6.004838f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2139.402100f, 1352.541626f, 132.127518f, 5.812850f);
			SpawnCreature(ENRAGING_GHOUL_MOB, 2142.408447f, 1360.760620f, 132.321564f, 5.812850f);
			break;
		}
	}

	void SpawnWaveCreatureRandom()
	{
		switch (RandomUInt(1, 5))
		{
		case 1:
			RandomEnemies_MRG();
			_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Market Row Gate!");
			_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2226.54f, 1331.22f, 7, 0, 0, "Market Row Gate"); // Send POI on map for players
			break;
		case 2:
			RandomEnemies_FLG();
			_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Festival Lane Gate!");
			_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2181.66f, 1238.40f, 7, 0, 0, "Festival Lane Gate"); // Send POI on map for players
			break;
		case 3:
			RandomEnemies_ESG();
			_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Elder's Sqaure Gate!");
			_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2240.98f, 1173.204f, 7, 0, 0, "Elder's Sqaure Gate"); // Send POI on map for players
			break;
		case 4:
			RandomEnemies_TH();
			_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Town Hall!");
			_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2353.98f, 1195.108f, 7, 0, 0, "Town Hall"); // Send POI on map for players
			break;
		case 5:
			RandomEnemies_KSF();
			_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the King's Square fountain!");
			_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2138.90f, 1354.42f, 7, 0, 0, "King's Square fountain"); // Send POI on map for players
			break;
		}
	};

	void AIUpdate()
	{
		InstanceCullingOfStratholmeScript* pInstance = (InstanceCullingOfStratholmeScript*)_unit->GetMapMgr()->GetScript();
		if (pInstance)
		{
			/*if (IsTimerFinished(wavetimer) == false && _killAmount >= 4) // If timer isnt finished and players already killed monsters reset timer.
			{
				_killAmount = 0; //Reset the KA
				ResetTimer(wavetimer, 5000);
			}*/
			if (IsTimerFinished(wavetimer) && _firstWave == true)
			{
				// 1st wave is ALWAYS festival lane gate.
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 1);
				SpawnCreature(ENRAGING_GHOUL_MOB, 2181.66f, 1238.40f, 136.46f, 2.25f); //Enraging Ghoul
				SpawnCreature(DEVOURING_GHOUL_MOB, 2183.94f, 1240.26f, 136.63f, 2.25f); //Enraging Ghoul
				SpawnCreature(DEVOURING_GHOUL_MOB, 2180.91f, 1239.35f, 136.42f, 2.25f); //Devouring Ghoul
				_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Festival Lane Gate!");
				_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2181.66f, 1238.40f, 7, 0, 0, "Festival Lane Gate"); // Send POI on map for players
				_firstWave = false;
				ResetTimer(wavetimer, 35000);
				SetPhase(2);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 2)
			{
				_killAmount = 0; //Reset the KA
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 2);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(3);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 3)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 3);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(4);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 4)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 4);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(5);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 5)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 5);
				SpawnCreature(CN_MEATHOOK, 2355.53f, 1186.40f, 130.57f, 3.3f); // Boss 1
				_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Town Hall!");
				_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2355.53f, 1186.40f, 0, 0, 0, "Town Hall"); // Send POI on map for players
				ResetTimer(wavetimer, 120000);
				SetPhase(6);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 6)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 6);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(7);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 7)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 7);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(8);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 8)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 8);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(9);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 9)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 9);
				SpawnWaveCreatureRandom();
				ResetTimer(wavetimer, 60000);
				SetPhase(10);
			}
			if (IsTimerFinished(wavetimer) && GetPhase() == 10)
			{
				_unit->GetPlayersInInstance(_unit)->SendWorldStateUpdate(3504, 10);
				SpawnCreature(CN_SALRAMM_THE_FLESHCRAFTER, 2181.66f, 1238.40f, 136.46f, 2.25f); //Boss 2
				_unit->SendChatMessage(14, 0, "Scourge forces have been spotted near the Festival Lane Gate!");
				_unit->GetPlayersInInstance(_unit)->Gossip_SendPOI(2181.66f, 1238.40f, 7, 0, 0, "Festival Lane Gate"); // Send POI on map for players
				RemoveTimer(wavetimer);
				SetPhase(11);
				_unit->Despawn(1000, 0);
			}
		}
		ParentClass::AIUpdate();
	}
private:
	bool _randomFeel;
	int32 wavetimer;
	bool _firstWave;
};


//ArthasGossip
class ArthasGossip : public GossipScript
{
    public:

        void GossipHello(Object* pObject, Player* Plr)
        {
            GossipMenu* Menu;
            objmgr.CreateGossipMenuForPlayer(&Menu, pObject->GetGUID(), 1, Plr);

			if (TO_CREATURE(pObject)->GetGossipStatus() == 0) // Purging (part 1)
			{
				Menu->AddItem(0, "Yes, my Prince, We are ready.", 1);
				Menu->SendTo(Plr);
			}

			if (TO_CREATURE(pObject)->GetGossipStatus() == 1) // Town Hall (part 2)
			{
				Menu->AddItem(0, "We're only doing what is best for Lordaeron your Highness.", 1);
				Menu->SendTo(Plr);
			}

			if (TO_CREATURE(pObject)->GetGossipStatus() == 2) // Gaunlet (part 3)
			{
				Menu->AddItem(0, "I'm ready.", 1);
				Menu->SendTo(Plr);
			}
			if (TO_CREATURE(pObject)->GetGossipStatus() == 3) // Gaunlet (part 4)
			{
				Menu->AddItem(0, "I'm ready to battle the dreadlord, sire.", 1);
				Menu->SendTo(Plr);
			}
        }

        void GossipSelectOption(Object* pObject, Player*  Plr, uint32 Id, uint32 IntId, const char* Code)
        {
            switch (IntId)
            {
                case 0:
                {
                    GossipHello(pObject, Plr);
                }
                break;
                case 1:
                {
                    TO_CREATURE(pObject)->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
					TO_CREATURE(pObject)->GetScript()->RegisterAIUpdateEvent(1000);
                }
                break;
            }
        }
};

class StratholmeCitizenAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(StratholmeCitizenAI, MoonScriptBossAI);
	StratholmeCitizenAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		change = 0;
	}

	void OnLoad()
	{
		if (_unit->GetPositionX() == 2399.002f && _unit->GetPositionY() == 1207.097f || _unit->GetPositionX() == 2402.53f && _unit->GetPositionY() == 1203.51f || _unit->GetPositionX() == 2400.38f && _unit->GetPositionY() == 1199.63f)
		{
			_unit->SetFaction(35);
			RegisterAIUpdateEvent(22000);
		}
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		SpawnCreature(NPC_INFINITE_ADVERSARY, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
		_unit->Despawn(1000, 0);
		ParentClass::AIUpdate();
	}
private:
	int32 change;
};

class OnDiedAI : public MoonScriptCreatureAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(OnDiedAI, MoonScriptCreatureAI);
	OnDiedAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
	{}

	void OnDied(Unit* mKiller)
	{
		_unit->Despawn(8000, 0);
		++_killAmount;
		ParentClass::OnDied(mKiller);
	}
};

class RisenZombieAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(RisenZombieAI, MoonScriptBossAI);
	RisenZombieAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
	}

	void OnLoad()
	{
		SetPhase(2);
		RegisterAIUpdateEvent(500);
		ParentClass::OnLoad();
	}

	void AIUpdate()
	{
		Creature* citizen_1 = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 28167);
		Creature* citizen_2 = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 28169);
		Creature* citizen_3 = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 31126);
		Creature* citizen_4 = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), 31127);
		if (GetPhase() == 2)
		{
			if (citizen_1)
			{
				citizen_1->Despawn(500, 144000000);
			}
			if (citizen_2)
			{
				citizen_2->Despawn(500, 144000000);
			}
			if (citizen_3)
			{
				citizen_3->Despawn(500, 144000000);
			}
			if (citizen_4)
			{
				citizen_4->Despawn(500, 144000000);
			}
		}
		ParentClass::AIUpdate();
	}
};

// \todo check the unregistered scripts (broeken? outdated?)
void SetupCullingOfStratholme(ScriptMgr* mgr)
{
	mgr->register_instance_script(595, &InstanceCullingOfStratholmeScript::Create);
    //////////////////////////////////////////
    // TRASH MOBS
    //////////////////////////////////////////
	mgr->register_creature_script(27737, &RisenZombieAI::Create);
	mgr->register_creature_script(28167, &StratholmeCitizenAI::Create);
	mgr->register_creature_script(ENRAGING_GHOUL_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(MASTER_NECROMANCER_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(BILE_GOLEM_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(CRYPT_FIEND_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(PATCHWORK_CONSTRUCT_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(ZOMBIE_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(DARK_NECROMANCER_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(DEVOURING_GHOUL_MOB, &OnDiedAI::Create);
	mgr->register_creature_script(TOMB_STALKER_MOB, &OnDiedAI::Create);
    //////////////////////////////////////////
    // BOSSES
    //////////////////////////////////////////
    mgr->register_creature_script(CN_MEATHOOK, &MeathookAI::Create);
    mgr->register_creature_script(CN_SALRAMM_THE_FLESHCRAFTER, &SalramTheFleshcrafterAI::Create);
    mgr->register_creature_script(CN_CHRONO_LORD_EPOCH, &ChronoLordEpochAI::Create);
    mgr->register_creature_script(CN_INFINITE_CORRUPTOR, &InfiniteCorruptorAI::Create);
    mgr->register_creature_script(CN_MALGANIS, &MalganisAI::Create);
    //////////////////////////////////////////
    // QUESTS & STUFF
    //////////////////////////////////////////
    //UPDATE `quests` SET `ReqKillMobOrGOCount1`='5' WHERE (`entry`='13149');
    QuestScript* Dispelling_Illusions = new Quest_Dispelling_Illusions();
    mgr->register_quest_script(13149, Dispelling_Illusions);
	mgr->register_creature_script(27913, &LordaeronCrierAI::Create);
    mgr->register_creature_script(CN_ARTHAS, &ArthasAI::Create);
    GossipScript * AG = new ArthasGossip();
    mgr->register_gossip_script(CN_ARTHAS, AG);
}
