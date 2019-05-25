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
#include "Dungeon_UtgardePinnacle.h"

//Svala Sorrowgrave
class SvalaSorrowgraveAI : public HybridBossScriptAI
{
public:

    HYBRIDSCRIPT_FUNCTION(SvalaSorrowgraveAI, HybridBossScriptAI);
    SvalaSorrowgraveAI(Creature* pCreature) : HybridBossScriptAI(pCreature)
    {
        AddEmote(Event_OnTargetDied, "You were a fool to challenge the power of the Lich King!", Text_Yell, 13845);
        AddEmote(Event_OnTargetDied, "Your will is done, my king.", Text_Yell, 13847);
        AddEmote(Event_OnDied, "Nooo! I did not come this far... to...", Text_Yell, 13855);
    }

    void OnCombatStart(Unit* pKiller)
    {
        Emote("I will vanquish your soul! ", Text_Yell, 13842);
        events.ScheduleEvent(EVENT_SORROWGRAVE_SS, 3000);
        events.ScheduleEvent(EVENT_SORROWGRAVE_FLAMES, 11000);
        events.ScheduleEvent(EVENT_SORROWGRAVE_RITUAL, 25000);
        ParentClass::OnCombatStart(pKiller);
    }

    void OnCombatStop(Unit* pKiller)
    {
        events.Reset();
        _unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
        ParentClass::OnCombatStart(pKiller);
    }

    void OnLoad()
    {
        _unit->GetAIInterface()->SetFly();
        _unit->SetUInt64Value(UNIT_FIELD_FLAGS, 2);
        _unit->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(40343));
        RegisterAIUpdateEvent(1000);
        if (IsHeroic()) // This should be set by the database but it doesn't seem to work.
        {
            _unit->SetMaxHealth(431392);
            _unit->SetHealth(_unit->GetMaxHealth());
        }
        SetPhase(1);
        Emote("The sensation is... beyond my imagining. I am yours to command, my king.", Text_Yell, 13857);
        Creature* Arthas = _unit->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(294.392f, -363.229f, 92.63f, 29280);
        if (Arthas)
        {
            Arthas->SendTimedScriptTextChatMessage(814, 9000);
            Arthas->Despawn(12000, 0);
            _unit->SendTimedScriptTextChatMessage(636, 17000);
        }
        _unit->SetOrientation(1.50f);
        events.ScheduleEvent(EVENT_SORROWGRAVE_ATTACKABLE, 26000);
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
        case EVENT_SORROWGRAVE_ATTACKABLE:
            _unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
            break;
        case EVENT_SORROWGRAVE_SS:
            _unit->CastSpell(_unit->GetVictim(), IsHeroic() ? SPELL_SINSTER_STRIKE_H : SPELL_SINSTER_STRIKE_N, false);
            events.ScheduleEvent(EVENT_SORROWGRAVE_SS, RandomUInt(3000, 5000));
            break;
        case EVENT_SORROWGRAVE_FLAMES:
            //summons.DespawnAll();
            _unit->CastSpell(_unit, SPELL_CALL_FLAMES, false);
            events.ScheduleEvent(EVENT_SORROWGRAVE_FLAMES2, 500);
            events.ScheduleEvent(EVENT_SORROWGRAVE_FLAMES2, 1000);
            events.ScheduleEvent(EVENT_SORROWGRAVE_FLAMES, RandomUInt(8000, 12000));
            break;
        case EVENT_SORROWGRAVE_FLAMES2:
            if (Unit* target = GetRandomPlayerTarget())
            {
                target->CastSpell(target, SPELL_BALL_OF_FLAME, true);
            }
            break;
        case EVENT_SORROWGRAVE_RITUAL:
            if (Unit* target = GetRandomPlayerTarget())
            {
                switch (RandomUInt(0, 3))
                {
                case 0:
                    Emote("Your death approaches.", Text_Yell, 13850);
                    break;
                case 1:
                    Emote("Go now to my master.", Text_Yell, 13851);
                    break;
                case 2:
                    Emote("Your end is inevitable.", Text_Yell, 13852);
                    break;
                case 3:
                    Emote("Any last words?", Text_Yell, 13854);
                    break;
                }

                Creature* channeler_1 = _unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_RITUAL_CHANNELER, 296.42f, -355.01f, 90.94f, 0.0f, true, false, 0, 0);
                if (channeler_1)
                {
                    channeler_1->CastSpell(channeler_1, SPELL_TELEPORT_VISUAL, true);
                    channeler_1->CastSpell(target, SPELL_PARALYZE, false);
                }
                Creature* channeler_2 = _unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_RITUAL_CHANNELER, 302.36f, -352.01f, 90.54f, 0.0f, true, false, 0, 0);
                if (channeler_2)
                {
                    channeler_2->CastSpell(channeler_2, SPELL_TELEPORT_VISUAL, true);
                    channeler_2->CastSpell(target, SPELL_PARALYZE, false);
                }
                Creature* channeler_3 = _unit->GetMapMgr()->GetInterface()->SpawnCreature(NPC_RITUAL_CHANNELER, 291.39f, -350.89f, 90.54f, 0.0f, true, false, 0, 0);
                if (channeler_3)
                {
                    channeler_3->CastSpell(channeler_3, SPELL_TELEPORT_VISUAL, true);
                    channeler_3->CastSpell(target, SPELL_PARALYZE, false);
                }

                if (target->IsPlayer())
                {
                    TO_PLAYER(target)->SafeTeleport(575, TO_PLAYER(target)->GetInstanceID(), 296.632f, -346.075f, 90.63f, 4.6f);
                }
                _unit->GetAIInterface()->MoveTeleport(296.632f, -346.075f, 110.0f, 4.6f);
                _unit->SetRoot();
                _unit->RemoveAllAuraType(SPELL_AURA_PERIODIC_DAMAGE);
                _unit->RemoveAllAuraType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                _unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            }

            events.DelayEvents(25001); // +1 just to be sure
            events.ScheduleEvent(EVENT_SORROWGRAVE_RITUAL_SPELLS, 0);
            events.ScheduleEvent(EVENT_SORROWGRAVE_FINISH_RITUAL, 25000);
            return;
        case EVENT_SORROWGRAVE_RITUAL_SPELLS:
            //_unit->CastSpell(me, SPELL_RITUAL_OF_THE_SWORD, false);
            _unit->CastSpell(_unit, SPELL_RITUAL_STRIKE, true);
            return;
        case EVENT_SORROWGRAVE_FINISH_RITUAL:
            _unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            _unit->UnsetRoot();
            _unit->GetAIInterface()->AttackReaction(_unit->GetVictim(), 1, 0);
            break;
            ParentClass::AIUpdate();
        }
    }
};

class SvalaAI : public HybridBossScriptAI
{
public:
    HYBRIDSCRIPT_FUNCTION(SvalaAI, HybridBossScriptAI);
    SvalaAI(Creature* pCreature) : HybridBossScriptAI(pCreature)
    {
        _started = false;
    }

    EventMap2 events2;
    void OnLoad()
    {
        RegisterAIUpdateEvent(1000); // Register AIUpdate so we can do our checks.
        SetPhase(1);
        _unit->SetDisplayId(25944);
        ParentClass::OnLoad();
    }

    void AIUpdate()
    {
        if (_unit->GetInRangePlayersCount() >= 1 && _started == false)
        {
            events2.ScheduleEvent(EVENT_SVALA_START, 5000);
            _started = true;
        }

        events2.Update(1000);
        switch (events2.ExecuteEvent())
        {
        case EVENT_SVALA_START:
            Emote("My liege! I have done as you asked, and now beseech you for your blessing!", Text_Yell, 13856);
            SpawnCreature(29280, 294.392f, -363.229f, 92.63f, 1.44f); // Image of Arthas
            events2.ScheduleEvent(EVENT_SVALA_TALK1, 8000);
            break;
        case EVENT_SVALA_TALK1:
            if (ForceCreatureFind(29280))
            {
                ForceCreatureFind(29280)->SendChatMessage(14, 0, "Your sacrifice is a testament to your obedience. Indeed you are worthy of this charge. Arise, and forever be known as Svala Sorrowgrave!");
                _unit->PlaySoundToSet(14732);
            }
            events2.ScheduleEvent(EVENT_SVALA_TALK2, 9000);
            break;
        case EVENT_SVALA_TALK2:
            if (ForceCreatureFind(29280))
            {
                ForceCreatureFind(29280)->CastSpell(_unit, 54142, false);
                _unit->CastSpell(_unit, 54140, true);
            }
            events2.ScheduleEvent(EVENT_SVALA_TALK3, 3000);
            break;
        case EVENT_SVALA_TALK3:
            _unit->SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 6.0f);
            _unit->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
            events2.ScheduleEvent(30, 1000);
            events2.ScheduleEvent(EVENT_SVALA_TALK4, 9000);
            break;
        case 30:
        {
            /*WorldPacket data(SMSG_SPLINE_MOVE_SET_HOVER, 9);
            data.append(_unit->GetPackGUID());
            _unit->SendMessageToSet(&data, false);*/
            break;
        }
        case EVENT_SVALA_TALK4:
        {
            _unit->CastSpell(_unit, 54205, true);
            SpawnCreature(NPC_SVALA_SORROWGRAVE, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation());
            if (ForceCreatureFind(NPC_ARTHAS))
            {
                ForceCreatureFind(NPC_ARTHAS)->InterruptSpell();
            }
            Despawn(1000, 0); // Despawn me !!
            break;
        }
        }
        ParentClass::AIUpdate();
    }
protected:
    bool _started;
};

class DragonflayerSpectatorAI : public HybridCreatureScriptAI
{
public:
    HYBRIDSCRIPT_FUNCTION(DragonflayerSpectatorAI, HybridCreatureScriptAI);
    DragonflayerSpectatorAI(Creature* pCreature) : HybridCreatureScriptAI(pCreature)
    {}

    void OnLoad()
    {
        _unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
        RegisterAIUpdateEvent(1000);
        ParentClass::OnLoad();
    };

    void AIUpdate()
    {
        if (ForceCreatureFind(NPC_SVALA_SORROWGRAVE))
        {
            _unit->GetAIInterface()->SetRun();
            _unit->GetAIInterface()->MoveTo(298.224f, 275.69f, 86.36f, 1.57f);
            Despawn(15000);
        }
        ParentClass::AIUpdate();
    }
};

void SetupUtgardePinnacle(ScriptMgr* mgr)
{
    //Svala Sorrowgrave encounter
    mgr->register_creature_script(NPC_SVALA, &SvalaAI::Create);
    mgr->register_creature_script(NPC_SVALA_SORROWGRAVE, &SvalaSorrowgraveAI::Create);
    mgr->register_creature_script(NPC_DRAGONFLAYER_SPECTATOR, &DragonflayerSpectatorAI::Create);
};