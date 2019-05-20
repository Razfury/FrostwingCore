/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2008-2015 Sun++ Team <http://www.sunplusplus.info/>
 * Copyright (C) 2005-2007 Ascent Team
 * Copyright (C) 2007-2015 Moon++ Team <http://www.moonplusplus.info/>
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

//Scarlet Enclave Phase Fix (DK AREA)
class MassacreAtDeathsBreach : public QuestScript
{
public:
	void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
	{
		mTarget->m_phase = 1;
	}
};

//Scarlet Enclave Phase Fix (DK AREA)
class ArmiesApproach : public QuestScript
{
public:
	void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
	{
		mTarget->m_phase = 64;
	}
};

class ScarletOnsalughtEmerges : public QuestScript
{
public:
	void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
	{
		mTarget->SafeTeleport(609, 0, 2398.43f, -5636.44f, 377.048f, 0.6f);
	}
};

//Scarlet Enclave
class HowToWinFirneds : public QuestScript
{
public:
	void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
	{
		sHookInterface.OnQuestAccept(mTarget, QuestStorage.LookupEntry(12723), NULL);
	}
};

class Flayer : public CreatureAIScript
{
    public:
        Flayer(Creature* pCreature) : CreatureAIScript(pCreature) { }
        static CreatureAIScript* Create(Creature* c) { return new Flayer(c); }

        void OnDied(Unit* mKiller)
        {
            if(!mKiller->IsPlayer())
                return;

            Creature* creat = _unit->GetMapMgr()->GetInterface()->SpawnCreature(11064, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation(), true, false, 0, 0);
            if(creat)
                creat->Despawn(60000, 0);
        }

};

class Darrowshire_Spirit : public GossipScript
{
    public:

        void GossipHello(Object* pObject, Player* plr)
        {
            QuestLogEntry* en = plr->GetQuestLogForEntry(5211);

            if(en && en->GetMobCount(0) < en->GetQuest()->required_mobcount[0])
            {
                uint32 newcount = en->GetMobCount(0) + 1;

                en->SetMobCount(0, newcount);
                en->SendUpdateAddKill(0);
                en->UpdatePlayerFields();
            }

            GossipMenu* Menu;
            objmgr.CreateGossipMenuForPlayer(&Menu, pObject->GetGUID(), 3873, plr);

            Menu->SendTo(plr);

            if(!pObject->IsCreature())
                return;

            Creature* Spirit = TO_CREATURE(pObject);

            Spirit->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            Spirit->Despawn(4000, 0);
        }

};

class ArajTheSummoner : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(ArajTheSummoner);
        ArajTheSummoner(Creature* pCreature) : CreatureAIScript(pCreature) { }

        void OnDied(Unit* mKiller)
        {
            if(!mKiller->IsPlayer())
                return;

            GameObject* go = sEAS.SpawnGameobject(TO_PLAYER(mKiller), 177241, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation(), 1, 0, 0, 0, 0);
            sEAS.GameobjectDelete(go, 60000);
        }
};

void SetupEasternPlaguelands(ScriptMgr* mgr)
{
    GossipScript* gs = new Darrowshire_Spirit();

	mgr->register_quest_script(12757, new ArmiesApproach());
	mgr->register_quest_script(12756, new ScarletOnsalughtEmerges());
	mgr->register_quest_script(12720, new HowToWinFirneds());
	mgr->register_quest_script(12701, new MassacreAtDeathsBreach());
    mgr->register_gossip_script(11064, gs);
    mgr->register_creature_script(8532, &Flayer::Create);
    mgr->register_creature_script(8531, &Flayer::Create);
    mgr->register_creature_script(8530, &Flayer::Create);
    mgr->register_creature_script(1852, &ArajTheSummoner::Create);
}
