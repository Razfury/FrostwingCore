/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2008-2015 Sun++ Team <http://www.sunplusplus.info/>
 * Copyright (C) 2008 WEmu Team
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

class TheDormantShade : public QuestScript
{
    public:
        void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
        {
            Creature* creat = mTarget->GetMapMgr()->GetInterface()->SpawnCreature(1946, 2467.314f, 14.8471f, 23.5950f, 0, true, false, 0, 0);
            creat->Despawn(60000, 0);
            creat->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You have disturbed my rest. Now face my wrath!");
        }
};

class CalvinMontague : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(CalvinMontague);
        CalvinMontague(Creature* pCreature) : CreatureAIScript(pCreature) {}

        void OnLoad()
        {
            _unit->SetFaction(68);
            _unit->SetStandState(STANDSTATE_STAND);
			RegisterAIUpdateEvent(1000);
        }

        void AIUpdate()
        {
			if (_unit->GetFaction() == 14)
			{
				_unit->m_canRegenerateHP = false;
			}
			if (_unit->GetFaction() == 14 && _unit->GetHealthPct() <= 15)
			{
				_unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "Okay, okay! Enough fighting.");
				_unit->RemoveNegativeAuras();
				_unit->SetFaction(68);
				_unit->SetHealth(_unit->GetMaxHealth());
				_unit->GetAIInterface()->WipeTargetList();
				_unit->GetAIInterface()->WipeHateList();
				_unit->GetAIInterface()->HandleEvent(EVENT_LEAVECOMBAT, _unit, 0);
				_unit->GetAIInterface()->disable_melee = true;
				_unit->GetAIInterface()->SetAllowedToEnterCombat(false);
				_unit->m_canRegenerateHP = true;
				_unit->SetUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
			}
        }
};

void ARoguesDeal_OnQuestAccept(Player* pPlayer, Quest* pQuest, Object* pObject)
{
	if (pQuest->id == 590)
	{
		float SSX = pPlayer->GetPositionX();
		float SSY = pPlayer->GetPositionY();
		float SSZ = pPlayer->GetPositionZ();

		Creature* Calvin = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(SSX, SSY, SSZ, 6784);

		if (Calvin == NULL)
			return;

		Calvin->SetFaction(14);
		Calvin->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
		Calvin->GetAIInterface()->disable_melee = false;
		Calvin->GetAIInterface()->disable_combat = false;
		Calvin->GetAIInterface()->SetAllowedToEnterCombat(true);
	}
}

class FieldsofGrief : public QuestScript
{
    public:

        void OnQuestComplete(Player* mTarget, QuestLogEntry* qLogEntry)
        {
            float SSX = mTarget->GetPositionX();
            float SSY = mTarget->GetPositionY();
            float SSZ = mTarget->GetPositionZ();

            Creature* Zealot = mTarget->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(SSX, SSY, SSZ, 1931);

            if(Zealot == NULL)
                return;

            Zealot->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "I. . . I. . .don't. . .feel. . .right. . .");
            sEAS.EventCastSpell(Zealot, Zealot, 3287, 3000);
            Zealot->GetAIInterface()->setMoveType(11);
            Zealot->GetAIInterface()->StopMovement(3000);


            sEAS.CreateCustomWaypointMap(Zealot);

            sEAS.WaypointCreate(Zealot, 2289.649658f, 237.001389f, 27.089531f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2291.700928f, 235.561844f, 27.089531f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2292.835693f, 240.636948f, 27.089531f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2288.737305f, 240.503952f, 27.088022f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2287.812744f, 236.320938f, 27.088022f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2292.643033f, 240.513489f, 27.088022f, 1.323f, 0, 256, 0);
            sEAS.WaypointCreate(Zealot, 2289.628418f, 239.908279f, 27.088022f, 1.323f, 0, 256, 0);

            sEAS.EnableWaypoints(Zealot);
        }
};

class Zealot : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(Zealot);
        Zealot(Creature* pCreature) : CreatureAIScript(pCreature) {}

        void OnReachWP(uint32 iWaypointId, bool bForwards)
        {
            if(!_unit->HasAura(3287))
                return;
            if(iWaypointId == 2)
            {
                _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "My mind. . .me flesh. . .I'm. . .rotting. . . .!");
            }

            if(iWaypointId == 7)
            {
                _unit->CastSpell(_unit, dbcSpell.LookupEntry(5), true);
            }
        }
};


void SetupTirisfalGlades(ScriptMgr* mgr)
{
    mgr->register_quest_script(410, new TheDormantShade());
    mgr->register_creature_script(6784, &CalvinMontague::Create);
    //mgr->register_quest_script(590, new ARoguesDeal());
	mgr->register_hook(SERVER_HOOK_EVENT_ON_QUEST_ACCEPT, (void*)&ARoguesDeal_OnQuestAccept);
    /*mgr->register_quest_script(407, new FieldsofGrief());*/
    mgr->register_creature_script(1931, &Zealot::Create);
}
