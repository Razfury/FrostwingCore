/*
  * FrostwingCore <https://gitlab.com/cronicx321/AscEmu>
  * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org>
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
#include "Raid_IceCrownCitadel.h"

//////////////////////////////////////////////////////////////////////////////////////////
//ICC zone: 4812
//Prepared creature entry:
//
//#define CN_DEATHBRINGER_SAURFANG    37813
//#define CN_FESTERGUT                36626
//#define CN_ROTFACE                  36627
//#define CN_PROFESSOR_PUTRICIDE      36678
//#define CN_PRINCE_VALANAR           37970
//#define CN_BLOOD_QUEEN_LANATHEL     37955
//#define CN_SINDRAGOSA               36853
//#define CN_THE_LICHKING             36597
//
///\todo  start boss scripts
//////////////////////////////////////////////////////////////////////////////////////////
//Event: GunshipBattle
//
//Affects:
//Available teleports. If GunshipBattle done -> Teleportlocation 4 available.
//
//Devnotes:
//Far away from implementing this :(
//////////////////////////////////////////////////////////////////////////////////////////
enum IceCrown_Encounters
{
    DATA_LORD_MARROWGAR,
    DATA_COLDFLAME,
    DATA_BONE_SPIKE,
    DATA_LADY_DEATHWHISPER,
    DATA_VALITHRIA_DREAM,

    ICC_DATA_END
};


//////////////////////////////////////////////////////////////////////////////////////////
//IceCrownCitadel Instance
class IceCrownCitadelScript : public MoonInstanceScript
{
    friend class ICCTeleporterAI; // Friendship forever ;-)

    public:

        MOONSCRIPT_INSTANCE_FACTORY_FUNCTION(IceCrownCitadelScript, MoonInstanceScript);
        IceCrownCitadelScript(MapMgr* pMapMgr) : MoonInstanceScript(pMapMgr)
        {
            // Way to select bosses
            BuildEncounterMap();
            if (mEncounters.size() == 0)
                return;

            for (EncounterMap::iterator Iter = mEncounters.begin(); Iter != mEncounters.end(); ++Iter)
            {
                if ((*Iter).second.mState != State_Finished)
                    continue;

                switch ((*Iter).first)
                {
                    case CN_LORD_MARROWGAR:
                        AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_1, State_Inactive);    // Icewall 1
                        AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_2, State_Inactive);    // Icewall 2
                        AddGameObjectStateByEntry(GO_MARROWGAR_DOOR, State_Inactive);         // Door
                        break;
                    default:
                        continue;
                };
            };
        }

        void OnGameObjectPushToWorld(GameObject* pGameObject)
        {
            // Gos which are not visible by killing a boss needs a second check...
            if (GetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR) == State_Finished)
            {
                AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_1, State_Active);    // Icewall 1
                AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_2, State_Active);    // Icewall 2
                AddGameObjectStateByEntry(GO_MARROWGAR_DOOR, State_Active);         // Door
            }
        }

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

        void OnCreatureDeath(Creature* pCreature, Unit* pUnit)
        {
            EncounterMap::iterator Iter = mEncounters.find(pCreature->GetEntry());
            if (Iter == mEncounters.end())
                return;

            (*Iter).second.mState = State_Finished;

            switch (pCreature->GetEntry())
            {
                case CN_LORD_MARROWGAR:
                {
                    AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_1, State_Active);    // Icewall 1
                    AddGameObjectStateByEntry(GO_MARROWGAR_ICEWALL_2, State_Active);    // Icewall 2
                    AddGameObjectStateByEntry(GO_MARROWGAR_DOOR, State_Active);         // Door
                }break;
				case CN_LADY_DEATHWHISPER:
				{
					AddGameObjectStateByEntry(202220, State_Active);    // elevator
				}break;
                default:
                    break;
            }
            return;
        }

        void OnPlayerEnter(Player* player)
        {
            // setup only the npcs with the correct team...
            switch (player->GetTeam())
            {
                case TEAM_ALLIANCE:
                    for (uint8 i = 0; i < 13; i++)
                        PushCreature(AllySpawns[i].entry, AllySpawns[i].x, AllySpawns[i].y, AllySpawns[i].z, AllySpawns[i].o, AllySpawns[i].faction);
                    break;
                case TEAM_HORDE:
                    for (uint8 i = 0; i < 13; i++)
                        PushCreature(HordeSpawns[i].entry, HordeSpawns[i].x, HordeSpawns[i].y, HordeSpawns[i].z, HordeSpawns[i].o, HordeSpawns[i].faction);
                    break;
            }
        }

};

//////////////////////////////////////////////////////////////////////////////////////////
// IceCrown Teleporter
class ICCTeleporterAI : public GameObjectAIScript
{
public:

    ICCTeleporterAI(GameObject* go) : GameObjectAIScript(go){}

    ~ICCTeleporterAI(){}

    static GameObjectAIScript* Create(GameObject* go) { return new ICCTeleporterAI(go); }

    void OnActivate(Player* player)
    {
        IceCrownCitadelScript* pInstance = (IceCrownCitadelScript*)player->GetMapMgr()->GetScript();
        if (!pInstance)
            return;

        GossipMenu* menu = NULL;
        objmgr.CreateGossipMenuForPlayer(&menu, _gameobject->GetGUID(), 15221, player);
        menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(515), 0);     // Teleport to Light's Hammer.

        if (pInstance->GetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR) == State_Finished)
            menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(516), 1);      // Teleport to Oratory of The Damned.

        if (pInstance->GetInstanceData(Data_EncounterState, CN_LADY_DEATHWHISPER) == State_Finished)
            menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(517), 2);      // Teleport to Rampart of Skulls.

        // GunshipBattle has to be finished...
        //menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(518), 3);        // Teleport to Deathbringer's Rise.

        if (pInstance->GetInstanceData(Data_EncounterState, CN_VALITHRIA_DREAMWALKER) == State_Finished)
            menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(519), 4);      // Teleport to the Upper Spire.

        if (pInstance->GetInstanceData(Data_EncounterState, CN_COLDFLAME) == State_Finished)
            menu->AddItem(ICON_CHAT, player->GetSession()->LocalizedGossipOption(520), 5);      // Teleport to Sindragosa's Lair.

        menu->SendTo(player);
    }

};

class ICCTeleporterGossip : public GossipScript
{
public:
    ICCTeleporterGossip() : GossipScript(){}

    void OnSelectOption(Object* object, Player* player, uint32 Id, const char* enteredcode)
    {
        Arcemu::Gossip::Menu::Complete(player);

        if (Id >= 6)
            return;

        switch (Id)
        {
            case 0:
                player->CastSpell(player, 70781, true);     // Light's Hammer
                break;
            case 1:
                player->CastSpell(player, 70856, true);     // Oratory of The Damned
                break;
            case 2:
                player->CastSpell(player, 70857, true);     // Rampart of Skulls
                break;
            case 3:
                player->CastSpell(player, 70858, true);     // Deathbringer's Rise
                break;
            case 4:
                player->CastSpell(player, 70859, true);     // Upper Spire
                break;
            case 5:
                player->CastSpell(player, 70861, true);     // Sindragosa's Lair
                break;
        }
    }
};

//King Varian If players want the buff to be removed..
class Varian_Gossip : public Arcemu::Gossip::Script
{
public:

	void OnHello(Object* pObject, Player* plr)
	{
		Arcemu::Gossip::Menu::SendQuickMenu(pObject->GetGUID(), 1, plr, 1, ICON_CHAT, "With all due respect, your Highness, we can do this alone.");
	}

	void OnSelectOption(Object* pObject, Player* plr, uint32 Id, const char* Code)
	{
		//if (Id == 1)
		//Arcemu::Gossip::Menu::SendQuickMenu(pObject->GetGUID(), 1, plr, 2, ICON_CHAT, "We do wish for you to stand down, your Highness.");
		//plr->Gossip_Complete();
        if (Id == 1)
            pObject->GetMapMgr()->RemoveAuraFromGroup(73828, 631);
		    plr->Gossip_Complete();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// Boss: Lord Marrowgar
enum LMSpells
{
	// Lord Marrowgar
	SPELL_BONE_SLICE = 69055,
	SPELL_BONE_STORM = 69076,
	SPELL_BONE_SPIKE_GRAVEYARD = 69057,
	SPELL_COLDFLAME_NORMAL = 69140,
	SPELL_COLDFLAME_BONE_STORM = 72705,

	// Bone Spike
	SPELL_IMPALED = 69065,
	SPELL_RIDE_VEHICLE = 46598,

	// Coldflame
	SPELL_COLDFLAME_PASSIVE = 69145,
	SPELL_COLDFLAME_SUMMON = 69147,
};


enum LMEvents
{
	EVENT_ENABLE_BONE_SLICE = 1,
	EVENT_SPELL_BONE_SPIKE_GRAVEYARD,
	EVENT_SPELL_COLDFLAME,
	EVENT_SPELL_COLDFLAME_BONE_STORM,
	EVENT_WARN_BONE_STORM,
	EVENT_BEGIN_BONE_STORM,
	EVENT_BONE_STORM_MOVE,
	EVENT_END_BONE_STORM,
	EVENT_ENRAGE,
};

uint32 const boneSpikeSummonId[3] = { 69062, 72669, 72670 };

struct BoneStormMoveTargetSelector : public std::unary_function<Unit*, bool>
{
	public:
		BoneStormMoveTargetSelector(Creature* source) : _source(source) { }
		bool operator()(Unit const* target) const
		{
			if (!target)
				return false;

			//if (target->GetExactDist(_source) > 175.0f)
				//return false;

			if (target->GetTypeId() != TYPEID_PLAYER)
				return false;

			if (target->GetPositionX() > -337.0f)
				return false;

			return target != _source->GetVictim();
		}

	private:
		Creature const* _source;
};

class LordMarrowgarAI : public MoonScriptBossAI
{
    public:

        MOONSCRIPT_FACTORY_FUNCTION(LordMarrowgarAI, MoonScriptBossAI);
        LordMarrowgarAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
       { 
			_boneSlice = false;
		}

		bool _boneSlice;
		uint64 _lastBoneSliceTargets[3];
        IceCrownCitadelScript* pInstance = (IceCrownCitadelScript*)_unit->GetMapMgr()->GetScript();

		void OnCombatStop(Unit* pTarget)
		{
			_boneSlice = false;
			memset(_lastBoneSliceTargets, 0, 3 * sizeof(uint64));
            if (pInstance)
            {
                pInstance->SetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR, State_Performed);
                pInstance->AddGameObjectStateByEntry(GO_MARROWGAR_DOOR, State_Active); // Open the door group wiped.
            }
			ParentClass::OnCombatStop(pTarget);
		}

		void OnLoad()
		{
			_unit->SendScriptTextChatMessage(922);      // This is the beginning AND the end, mortals. None may enter the master's sanctum!
            if (pInstance)
            {
                pInstance->SetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR, State_NotStarted);
            }
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

		void OnCombatStart(Unit* pTarget)
		{
			_unit->SendScriptTextChatMessage(923);      // The Scourge will wash over this world as a swarm of death and destruction!
			RegisterAIUpdateEvent(1000);
			events.ScheduleEvent(EVENT_ENABLE_BONE_SLICE, 10000);
			events.ScheduleEvent(EVENT_SPELL_BONE_SPIKE_GRAVEYARD, RandomUInt(10000, 15000));
			events.ScheduleEvent(EVENT_SPELL_COLDFLAME, 5000);
			events.ScheduleEvent(EVENT_WARN_BONE_STORM, RandomUInt(45000, 50000));
			events.ScheduleEvent(EVENT_ENRAGE, 600000);
            if (pInstance)
            {
                pInstance->SetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR, State_InProgress);
                pInstance->AddGameObjectStateByEntry(GO_MARROWGAR_DOOR, State_Inactive); // Close the door group aggro.
            }
			ParentClass::OnCombatStart(pTarget);
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
			case EVENT_ENABLE_BONE_SLICE:
				_boneSlice = true;
				events.PopEvent();
				break;
			case EVENT_SPELL_BONE_SPIKE_GRAVEYARD:
			{
				bool a = _unit->HasAura(SPELL_BONE_STORM);
				if (IsHeroic() || !a)
					_unit->CastSpell(_unit, SPELL_BONE_SPIKE_GRAVEYARD, a);
				events.RepeatEvent(RandomUInt(15000, 20000));
			}
			break;
			case EVENT_SPELL_COLDFLAME:
				if (!_unit->HasAura(SPELL_BONE_STORM))
					_unit->CastSpell((Unit*)NULL, SPELL_COLDFLAME_NORMAL, false);
				events.RepeatEvent(5000);
				break;
			case EVENT_SPELL_COLDFLAME_BONE_STORM:
				_unit->CastSpell(_unit, SPELL_COLDFLAME_BONE_STORM, false);
				events.PopEvent();
				break;
			case EVENT_WARN_BONE_STORM:
				_boneSlice = false;
				Emote("BONE STORM!", Text_Yell, 16946);
				Announce("Lord Marrowgar creates a whirling storm of bone!");
				_unit->CastSpell(_unit, SPELL_BONE_STORM, false);
				events.RepeatEvent(RandomUInt(90000, 95000));
				events.ScheduleEvent(EVENT_BEGIN_BONE_STORM, 3050);
				break;
			case EVENT_BEGIN_BONE_STORM:
			{
				if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
				{
					if (Aura* pStorm = _unit->FindAura(SPELL_BONE_STORM))
						pStorm->SetDuration(20000);
					events.PopEvent();
					events.ScheduleEvent(EVENT_BONE_STORM_MOVE, 0);
					events.ScheduleEvent(EVENT_END_BONE_STORM, 20000 + 1);
				}
				if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
				{
					if (Aura* pStorm = _unit->FindAura(SPELL_BONE_STORM))
						pStorm->SetDuration(30000);
					events.PopEvent();
					events.ScheduleEvent(EVENT_BONE_STORM_MOVE, 0);
					events.ScheduleEvent(EVENT_END_BONE_STORM, 30000 + 1);
				}
				if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
				{
					if (Aura* pStorm = _unit->FindAura(SPELL_BONE_STORM))
						pStorm->SetDuration(20000);
					events.PopEvent();
					events.ScheduleEvent(EVENT_BONE_STORM_MOVE, 0);
					events.ScheduleEvent(EVENT_END_BONE_STORM, 20000 + 1);
				}
				if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
				{
					if (Aura* pStorm = _unit->FindAura(SPELL_BONE_STORM))
						pStorm->SetDuration(30000);
					events.PopEvent();
					events.ScheduleEvent(EVENT_BONE_STORM_MOVE, 0);
					events.ScheduleEvent(EVENT_END_BONE_STORM, 30000 + 1);
				}
			}
			break;
			case EVENT_BONE_STORM_MOVE:
			{
				events.RepeatEvent(5000);
				Unit* unit = GetRandomPlayerTarget();
				if (!unit)
				{
					if (unit = GetBestPlayerTarget(TargetFilter_NotCurrent, 0.0f, 120.0f))
						if (unit->GetPositionX() > -337.0f)
						{
							_unit->GetAIInterface()->SetAIState(STATE_EVADE);
							return;
						}
				}
				if (unit)
					_unit->GetAIInterface()->MoveCharge(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ());
				break;
			}
			break;
			case EVENT_END_BONE_STORM:
				_unit->GetAIInterface()->StopMovement(0);
				_unit->GetAIInterface()->AttackReaction(GetRandomPlayerTarget(), 1, 0);
				events.PopEvent();
				events.CancelEvent(EVENT_BONE_STORM_MOVE);
				events.ScheduleEvent(EVENT_ENABLE_BONE_SLICE, 10000);
				if (!IsHeroic())
					events.RescheduleEvent(EVENT_SPELL_BONE_SPIKE_GRAVEYARD, RandomUInt(15000, 20000));
				break;
			case EVENT_ENRAGE:
				_unit->CastSpell(_unit, 26662, true);
				Emote("THE MASTER'S RAGE COURSES THROUGH ME!", Text_Yell, 16945);
				events.PopEvent();
				break;
			}

			if (_unit->HasAura(SPELL_BONE_STORM))
				return;

			if (_boneSlice) // && !_unit->GetCurrentSpell(CURRENT_MELEE_SPELL))
				_unit->CastSpell(_unit->GetVictim(), SPELL_BONE_SLICE, true);

			if (_boneSlice && _unit->isAttackReady(false) && _unit->GetVictim() && !_unit->IsCasting())
				memset(_lastBoneSliceTargets, 0, 3 * sizeof(uint64));

        }


        void OnTargetDied(Unit* pTarget)
        {
            switch (RandomUInt(1))
            {
                case 0:
                    _unit->SendScriptTextChatMessage(928);      // More bones for the offering!
                    break;
                case 1:
                    _unit->SendScriptTextChatMessage(929);      // Languish in damnation!
                    break;
            }
			ParentClass::OnTargetDied(pTarget);
        }

        void OnDied(Unit* pTarget)
        {
            _unit->SendScriptTextChatMessage(930);      // I see... Only darkness.
            if (pInstance)
            {
                pInstance->SetInstanceData(Data_EncounterState, CN_LORD_MARROWGAR, State_Finished);
            }
			ParentClass::OnDied(pTarget);
        }
};

#define IMPALED 69065

class BoneSpikeAI : public MoonScriptBossAI
{
    public:

        MOONSCRIPT_FACTORY_FUNCTION(BoneSpikeAI, MoonScriptBossAI);
        BoneSpikeAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
            _unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
            _unit->Despawn(8000, 0);
        }

};

class ColdFlameAI : public MoonScriptBossAI
{
public:

    MOONSCRIPT_FACTORY_FUNCTION(ColdFlameAI, MoonScriptBossAI);
    ColdFlameAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
    {}

    void OnLoad()
    {
        _unit->SetFaction(14);
        _unit->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); //Unselectable.
        events.ScheduleEvent(1, 450);
        events.ScheduleEvent(2, 12000);
        _unit->z_axisposition = 42.5f;
        ParentClass::OnLoad();
    }

    void AIUpdate()
    {
        events.Update(1000);

        switch (events.GetEvent())
        {
        case 0:
            break;
        case 1:
        {
            _unit->z_axisposition = 42.5f;
            _unit->CastSpell(_unit, SPELL_COLDFLAME_SUMMON, true);
            float nx = _unit->GetPositionX() + 5.0f*cos(_unit->GetOrientation());
            float ny = _unit->GetPositionY() + 5.0f*sin(_unit->GetOrientation());
            if (!_unit->IsWithinLOS(nx, ny, 42.5f))
            {
                events.PopEvent();
                break;
            }
           // _unit->NearTeleportTo(nx, ny, 42.5f, me->GetOrientation());
            _unit->GetAIInterface()->MoveTeleport(nx, ny, 42.5f, _unit->GetOrientation());
            events.RepeatEvent(450);
        }
        break;
        case 2:
            events.Reset();
            break;
        }

        ParentClass::AIUpdate();
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// Boss: Lady Deathwhisper
// MANA_BARRIER = 70842
// DEATH_AND_DECAY = 71001
// TOUCH_OF_INSIGNIFICANCE = 71204
// SHADOW_BOLT = 71254
// DOMINATE_MIND_H = 71289
// SUMMON_SHADE = 71363
// FROSTBOLT = 71420
// FROSTBOLT_VOLLEY = 72905
// ...

class DeathwhisperAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(DeathwhisperAI, MoonScriptBossAI);
	DeathwhisperAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			AddEmote(Event_OnTargetDied, "Do you yet grasp the futility of your actions?", Text_Yell, 16869);
			AddEmote(Event_OnTargetDied, "Embrace the darkness... Darkness eternal!", Text_Yell, 16870);

			AddSpell(71001, Target_RandomPlayer, 70, 0, 5); // Death n Decay
			AddPhaseSpell(1, AddSpell(71254, Target_RandomPlayer, 70, 2, 3)); // ShadowBolt
			AddPhaseSpell(2, AddSpell(71420, Target_Current, 70, 2, 6)); // Frostbolt
			AddPhaseSpell(2, AddSpell(72905, Target_RandomPlayer, 70, 0, 7)); // Frostbolt Volley
			AddPhaseSpell(2, AddSpell(71204, Target_Current, 70, 0, 6)); // Touch of Insign
			AddPhaseSpell(2, AddSpell(71363, Target_RandomPlayer, 70, 0, 6)); // Summon Vengeful SPirit
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			AddEmote(Event_OnTargetDied, "Do you yet grasp of the futility of your actions?", Text_Yell, 16869);
			AddEmote(Event_OnTargetDied, "Embrace the darkness... Darkness eternal!", Text_Yell, 16870);

			AddSpell(71001, Target_RandomPlayer, 70, 0, 5); // Death n Decay
			AddPhaseSpell(1, AddSpell(71254, Target_RandomPlayer, 70, 2, 3)); // ShadowBolt
			AddPhaseSpell(2, AddSpell(71420, Target_Current, 70, 2, 6)); // Frostbolt
			AddPhaseSpell(2, AddSpell(72905, Target_RandomPlayer, 70, 0, 7)); // Frostbolt Volley
			AddPhaseSpell(2, AddSpell(71204, Target_Current, 70, 0, 6)); // Touch of Insign
			AddPhaseSpell(2, AddSpell(71363, Target_RandomPlayer, 70, 0, 6)); // Summon Vengeful SPirit
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			AddEmote(Event_OnTargetDied, "Do you yet grasp of the futility of your actions?", Text_Yell, 16869);
			AddEmote(Event_OnTargetDied, "Embrace the darkness... Darkness eternal!", Text_Yell, 16870);

			AddSpell(71001, Target_RandomPlayer, 70, 0, 5); // Death n Decay
			AddSpell(71289, Target_RandomPlayer, 70, 0, 21); // Dominate Mind (HEROIC ONLY) 1 for 10 man 3 for 25 man
			AddPhaseSpell(1, AddSpell(71254, Target_RandomPlayer, 70, 2, 3)); // ShadowBolt
			AddPhaseSpell(2, AddSpell(71420, Target_Current, 70, 2, 6)); // Frostbolt
			AddPhaseSpell(2, AddSpell(72905, Target_RandomPlayer, 70, 0, 7)); // Frostbolt Volley
			AddPhaseSpell(2, AddSpell(71204, Target_Current, 70, 0, 6)); // Touch of Insign
			AddPhaseSpell(2, AddSpell(71363, Target_RandomPlayer, 70, 0, 6)); // Summon Vengeful SPirit
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			AddEmote(Event_OnTargetDied, "Do you yet grasp of the futility of your actions?", Text_Yell, 16869);
			AddEmote(Event_OnTargetDied, "Embrace the darkness... Darkness eternal!", Text_Yell, 16870);

			AddSpell(71001, Target_RandomPlayer, 70, 0, 5); // Death n Decay
			AddSpell(71289, Target_RandomPlayer, 70, 0, 21); // Dominate Mind (HEROIC ONLY) 1 for 10 man 3 for 25 man
			AddPhaseSpell(1, AddSpell(71254, Target_RandomPlayer, 70, 2, 3)); // ShadowBolt
			AddPhaseSpell(2, AddSpell(71420, Target_Current, 70, 2, 6)); // Frostbolt
			AddPhaseSpell(2, AddSpell(72905, Target_RandomPlayer, 70, 0, 7)); // Frostbolt Volley
			AddPhaseSpell(2, AddSpell(71204, Target_Current, 70, 0, 6)); // Touch of Insign
			AddPhaseSpell(2, AddSpell(71363, Target_RandomPlayer, 70, 0, 6)); // Summon Vengeful SPirit
		}
	};

	void OnDied(Unit* mKiller)
	{
		Emote("All part of the masters plan! Your end is... inevitable!", Text_Yell, 16871);
		_unit->GetMapMgr()->CreateAndSpawnGameObject(202220, -645.171f, 2211.225f, 51.55f, 6.2f, 1.0f);
		ParentClass::OnDied(mKiller);
	}

	void OnLoad()
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			//GameObject* Elevator = Elevator->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-646.94f, 2211.739f, 198.621f, 202220);
			_unit->SetMaxHealth(3000000);
			_unit->SetHealth(3000000);
			_unit->SetMaxPower(POWER_TYPE_MANA, 3300000);
			_unit->SetPower(POWER_TYPE_MANA, _unit->GetMaxPower(POWER_TYPE_MANA));
			_unit->SetMinDamage(23030.0f);
			_unit->SetMaxDamage(31360.0f);
			_unit->DelayPowerRegeneration(345345443);
			//Elevator->Deactivate();
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			//GameObject* Elevator = Elevator->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-646.94f, 2211.739f, 198.621f, 202220);
			_unit->SetMaxHealth(6000000);
			_unit->SetHealth(6000000);
			_unit->SetMaxPower(POWER_TYPE_MANA, 3300000);
			_unit->SetPower(POWER_TYPE_MANA, _unit->GetMaxPower(POWER_TYPE_MANA));
			_unit->SetMinDamage(33840.0f);
			_unit->SetMaxDamage(46080.0f);
			_unit->DelayPowerRegeneration(345345443);
			//Elevator->Deactivate();
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			//GameObject* Elevator = Elevator->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-646.94f, 2211.739f, 198.621f, 202220);
			_unit->SetMaxHealth(13400000);
			_unit->SetHealth(13400000);
			_unit->SetMaxPower(POWER_TYPE_MANA, 11000000);
			_unit->SetPower(POWER_TYPE_MANA, _unit->GetMaxPower(POWER_TYPE_MANA));
			_unit->SetMinDamage(23030.0f);
			_unit->SetMaxDamage(31360.0f);
			_unit->DelayPowerRegeneration(345345443);
			//Elevator->Deactivate();
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			//GameObject* Elevator = Elevator->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-646.94f, 2211.739f, 198.621f, 202220);
			_unit->SetMaxHealth(26800000);
			_unit->SetHealth(26800000);
			_unit->SetMaxPower(POWER_TYPE_MANA, 13900000);
			_unit->SetPower(POWER_TYPE_MANA, _unit->GetMaxPower(POWER_TYPE_MANA));
			_unit->SetMinDamage(33840.0f);
			_unit->SetMaxDamage(46080.0f);
			_unit->DelayPowerRegeneration(345345443);
			//Elevator->Deactivate();
		}
		ParentClass::OnLoad();
	}

	void OnCombatStart(Unit* mTarget)
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			Emote("What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.", Text_Yell, 16868);
			//Emote("TEST 10m NORMAL", Text_Yell, 0);
			_unit->CastSpell(_unit, 70842, false);
			//SetCanMove(false);
			//TriggerCooldownOnAllSpells();
			WaveAdds = AddTimer(5000);
			Beserk = AddTimer(600000);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			Emote("What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.", Text_Yell, 16868);
			//Emote("TEST 25m NORMAL", Text_Yell, 0);
			_unit->CastSpell(_unit, 70842, false);
			//SetCanMove(false);
			//TriggerCooldownOnAllSpells();
			WaveAdds = AddTimer(5000);
			Beserk = AddTimer(600000);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			Emote("What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.", Text_Yell, 16868);
			//Emote("TEST 10m HERO", Text_Yell, 0);
			_unit->CastSpell(_unit, 70842, false);
			//SetCanMove(false);
			//TriggerCooldownOnAllSpells();
			WaveAddsH = AddTimer(5000);
			Beserk = AddTimer(600000);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			Emote("What is this disturbance?! You dare trespass upon this hallowed ground? This shall be your final resting place.", Text_Yell, 16868);
			//Emote("TEST 25m HERO", Text_Yell, 0);
			_unit->CastSpell(_unit, 70842, false);
			SetCanMove(false);
			//TriggerCooldownOnAllSpells();
			WaveAddsH = AddTimer(5000);
			Beserk = AddTimer(600000);
		}
		ParentClass::OnCombatStart(mTarget);
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->RemoveAura(26662);
		_unit->RemoveAura(70482);
		ParentClass::OnCombatStop(mTarget);
	}

	void SpawnWave()
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL) // 3 adds spawn one side of the room then to the other.
		{
			switch (RandomUInt(2))
			{
			case 0:
				SpawnCreature(37890, -578.7066f, 2154.167f, 51.01529f, 1.692969f); // Left Cult Fanatic
				SpawnCreature(37949, -598.9028f, 2155.005f, 51.01530f, 1.692969f); // Left Cult Adherant
				SpawnCreature(37890, -619.2864f, 2154.460f, 51.01530f, 1.692969f); // Left Cult Fanatic
				break;
			case 1:
				SpawnCreature(37949, -578.6996f, 2269.856f, 51.01529f, 4.590216f); // Right Cult Adherant
				SpawnCreature(37890, -598.9688f, 2269.264f, 51.01529f, 4.590216f); // Right Cult Fanatic
				SpawnCreature(37949, -619.4323f, 2268.523f, 51.01530f, 4.590216f); // right cult adherant
				break;
			}
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL) // adds spawn on both sides.
		{
				SpawnCreature(37890, -578.7066f, 2154.167f, 51.01529f, 1.692969f); // Left Cult Fanatic
				SpawnCreature(37949, -598.9028f, 2155.005f, 51.01530f, 1.692969f); // Left Cult Adherant
				SpawnCreature(37890, -619.2864f, 2154.460f, 51.01530f, 1.692969f); // Left Cult Fanatic
				SpawnCreature(37949, -578.6996f, 2269.856f, 51.01529f, 4.590216f); // Right Cult Adherant
				SpawnCreature(37890, -598.9688f, 2269.264f, 51.01529f, 4.590216f); // Right Cult Fanatic
				SpawnCreature(37949, -619.4323f, 2268.523f, 51.01530f, 4.590216f); // right cult adherant
		}
	}

	void SpawnWaveH()
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC) // 3 adds spawn one side of the room then to the other.
		{
			switch (RandomUInt(2))
			{
			case 0:
				SpawnCreature(37890, -578.7066f, 2154.167f, 51.01529f, 1.692969f); // Left Cult Fanatic
				SpawnCreature(37949, -598.9028f, 2155.005f, 51.01530f, 1.692969f); // Left Cult Adherant
				SpawnCreature(37890, -619.2864f, 2154.460f, 51.01530f, 1.692969f); // Left Cult Fanatic
				break;
			case 1:
				SpawnCreature(37949, -578.6996f, 2269.856f, 51.01529f, 4.590216f); // Right Cult Adherant
				SpawnCreature(37890, -598.9688f, 2269.264f, 51.01529f, 4.590216f); // Right Cult Fanatic
				SpawnCreature(37949, -619.4323f, 2268.523f, 51.01530f, 4.590216f); // right cult adherant
				break;
			}
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC) // adds spawn on both sides.
		{
			SpawnCreature(37890, -578.7066f, 2154.167f, 51.01529f, 1.692969f); // Left Cult Fanatic
			SpawnCreature(37949, -598.9028f, 2155.005f, 51.01530f, 1.692969f); // Left Cult Adherant
			SpawnCreature(37890, -619.2864f, 2154.460f, 51.01530f, 1.692969f); // Left Cult Fanatic
			SpawnCreature(37949, -578.6996f, 2269.856f, 51.01529f, 4.590216f); // Right Cult Adherant
			SpawnCreature(37890, -598.9688f, 2269.264f, 51.01529f, 4.590216f); // Right Cult Fanatic
			SpawnCreature(37949, -619.4323f, 2268.523f, 51.01530f, 4.590216f); // right cult adherant
		}
	}

	void AIUpdate()
	{
		if (_unit->GetPower(POWER_TYPE_MANA) == 0 && GetPhase() == 1)
		{
			_unit->SendChatMessage(42, 0, "Lady Deathwhisper's Mana Barrier shimmers and fades away!");
			Emote("Enough! I see I must take matters into my own hands!", Text_Yell, 16877);
			_unit->RemoveAura(70482);
			SetCanMove(true);
			SetPhase(2);
		}
		if (IsTimerFinished(WaveAddsH))
		{
			SpawnWaveH();
			ResetTimer(WaveAdds, 45000);
		}
		if (IsTimerFinished(WaveAdds) && GetPhase() == 1)
		{
			SpawnWave();
			ResetTimer(WaveAdds, 60000);
		}
		if (IsTimerFinished(Beserk))
		{
			_unit->CastSpell(_unit, 26662, false);
				Emote("This charade has gone on long enough.", Text_Yell, 16872);
				RemoveTimer(Beserk);
		}
		ParentClass::AIUpdate();
	}
protected:
	int32 Beserk; int32 WaveAdds; int32 WaveAddsH;
};

///////////////////////////////////////////////////////
// Boss: Valithria Dreamwalker
// ...
//
//
//
//

///////////////////////////////////////////////////////
// Boss: Cold Flame
// ...
//
//
//
//

//Deathbringer Saurfang
//Comments Below
/*
Encounter Status #: 2
0 = Non-Functional (nothing is scripted/working)
1 = Basic Scripted (means boss is has basic scripts but major parts are not working)
2 = semi Functional (means boss is killable but some parts maybe missing or not working)
3 = blizzlike functional (means boss is as close to blizzlike as possible)
*/

enum DeathbringerSpells
{
	// Deathbringer Saurfang
	SPELL_GRIP_OF_AGONY = 70572, // Intro
	SPELL_BLOOD_LINK = 72178,
	SPELL_MARK_OF_THE_FALLEN_CHAMPION_S = 72256,
	SPELL_RUNE_OF_BLOOD_S = 72408,

	SPELL_SUMMON_BLOOD_BEAST = 72172,
	SPELL_SUMMON_BLOOD_BEAST_25_MAN = 72356, // Additional cast, does not replace
	SPELL_FRENZY = 72737,
	SPELL_BLOOD_NOVA_TRIGGER = 72378,
	SPELL_BLOOD_NOVA = 72380,
	SPELL_BLOOD_POWER = 72371,
	SPELL_BLOOD_LINK_POWER = 72195,
	SPELL_BLOOD_LINK_DUMMY = 72202,
	SPELL_MARK_OF_THE_FALLEN_CHAMPION = 72293,
	SPELL_BOILING_BLOOD = 72385,
	SPELL_RUNE_OF_BLOOD = 72410,

	// Blood Beast
	SPELL_BLOOD_LINK_BEAST = 72176,
	SPELL_RESISTANT_SKIN = 72723,
	SPELL_SCENT_OF_BLOOD = 72769, // Heroic only

	SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION = 72257,
	SPELL_PERMANENT_FEIGN_DEATH = 70628,
};

class DeathbringerAI : public MoonScriptBossAI
{
public:
	MOONSCRIPT_FACTORY_FUNCTION(DeathbringerAI, MoonScriptBossAI);
	DeathbringerAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
	{
		// 10 man//
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			AddSpell(SPELL_SUMMON_BLOOD_BEAST, Target_Self, 70, 0, 30, 0.0f, 0.0f, false, "Feast, my minions!", Text_Yell, 16700);
			AddSpell(SPELL_BLOOD_NOVA_TRIGGER, Target_RandomPlayer, 70, 1, 11);
			AddSpell(SPELL_RUNE_OF_BLOOD, Target_Current, 70, 0, 11);
			AddSpell(SPELL_BOILING_BLOOD, Target_Self, 20, 0, 9);
			BerserkTimer = AddTimer(480000);
		}
		// 10 h man//
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			AddSpell(SPELL_SUMMON_BLOOD_BEAST, Target_Self, 70, 0, 30, 0.0f, 0.0f, false, "Feast, my minions!", Text_Yell, 16700);
			AddSpell(SPELL_BLOOD_NOVA_TRIGGER, Target_RandomPlayer, 70, 1, 11);
			AddSpell(SPELL_RUNE_OF_BLOOD, Target_Current, 70, 0, 11);
			AddSpell(SPELL_BOILING_BLOOD, Target_Self, 20, 0, 9);
			BerserkTimerH = AddTimer(360000);
		}
		// 25 man//
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			AddSpell(SPELL_SUMMON_BLOOD_BEAST_25_MAN, Target_Self, 70, 0, 30, 0.0f, 0.0f, false, "Feast, my minions!", Text_Yell, 16700);
			AddSpell(SPELL_BLOOD_NOVA_TRIGGER, Target_RandomPlayer, 70, 1, 11);
			AddSpell(SPELL_RUNE_OF_BLOOD, Target_Current, 70, 0, 11);
			AddSpell(SPELL_BOILING_BLOOD, Target_Self, 20, 0, 11);
			BerserkTimer = AddTimer(480000);
		}
		// 25 h man//
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			AddSpell(SPELL_SUMMON_BLOOD_BEAST_25_MAN, Target_Self, 70, 0, 30, 0.0f, 0.0f, false, "Feast, my minions!", Text_Yell, 16700);
			AddSpell(SPELL_BLOOD_NOVA_TRIGGER, Target_RandomPlayer, 70, 1, 11);
			AddSpell(SPELL_RUNE_OF_BLOOD, Target_Current, 70, 0, 11);
			AddSpell(SPELL_BOILING_BLOOD, Target_Self, 20, 0, 9);
			BerserkTimerH = AddTimer(360000);
		}
	}

	void OnLoad() // Set Stats (Stats from openwow wotlk)
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			_unit->SetMaxHealth(8785000);
			_unit->SetHealth(8785000);
			_unit->SetMinDamage(30331.0f);
			_unit->SetMaxDamage(41464.0f);
			_unit->DelayPowerRegeneration(345345443);
			_unit->SetPower(POWER_TYPE_MANA, 0);
			_unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			_unit->SetMaxHealth(12300000);
			_unit->SetHealth(12300000);
			_unit->SetMinDamage(45550.0f);
			_unit->SetMaxDamage(62184.0f);
			_unit->DelayPowerRegeneration(345345443);
			_unit->SetPower(POWER_TYPE_MANA, 0);
			_unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			_unit->SetMaxHealth(31860000);
			_unit->SetHealth(31860000);
			_unit->SetMinDamage(30331.0f);
			_unit->SetMaxDamage(41464.0f);
			_unit->DelayPowerRegeneration(345345443);
			_unit->SetPower(POWER_TYPE_MANA, 0);
			_unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			_unit->SetMaxHealth(43930000);
			_unit->SetHealth(43930000);
			_unit->SetMinDamage(45550.0f);
			_unit->SetMaxDamage(62184.0f);
			_unit->DelayPowerRegeneration(345345443);
			_unit->SetPower(POWER_TYPE_MANA, 0);
			_unit->SetUInt64Value(UNIT_FIELD_FLAGS, 0);
		}
		ParentClass::OnLoad();
	}

	void OnCombatStart(Unit* mTarget)
	{
		Emote("BY THE MIGHT OF THE LICH KING!", Text_Yell, 16694);
		_unit->CastSpell(_unit, SPELL_BLOOD_LINK, false);
		//since blood link does not work we are going to randomize blood power regenerate.
		LinkAdd = AddTimer((RandomUInt(4000)));
		_unit->SetPower(POWER_TYPE_MANA, 0);
		ParentClass::OnCombatStart(mTarget);
	}

	void OnDied(Unit* mKiller)
	{
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_NORMAL)
		{
			_unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-471.57f, 2211.48f, 541.11f, 201825)->Despawn(1000, 0);
			Emote("I... Am... Released.", Text_Yell, 16697);
			_unit->CastSpellAoF(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), dbcSpell.LookupEntry(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION), true);
			_unit->GetMapMgr()->CreateAndSpawnGameObject(202239, -495.451f, 2174.187f, 539.288f, 2.77f, 1.0f);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_10MAN_HEROIC)
		{
			_unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-471.57f, 2211.48f, 541.11f, 201825)->Despawn(1000, 0);
			Emote("I... Am... Released.", Text_Yell, 16697);
			_unit->CastSpellAoF(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), dbcSpell.LookupEntry(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION), true);
			_unit->GetMapMgr()->CreateAndSpawnGameObject(202240, -495.451f, 2174.187f, 539.288f, 2.77f, 1.0f);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_NORMAL)
		{
			_unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-471.57f, 2211.48f, 541.11f, 201825)->Despawn(1000, 0);
			Emote("I... Am... Released.", Text_Yell, 16697);
			_unit->CastSpellAoF(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), dbcSpell.LookupEntry(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION), true);
			_unit->GetMapMgr()->CreateAndSpawnGameObject(202238, -495.451f, 2174.187f, 539.288f, 2.77f, 1.0f);
		}
		if (_unit->GetMapMgr()->iInstanceMode == RAID_25MAN_HEROIC)
		{
			_unit->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(-471.57f, 2211.48f, 541.11f, 201825)->Despawn(1000, 0);
			Emote("I... Am... Released.", Text_Yell, 16697);
			_unit->CastSpellAoF(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), dbcSpell.LookupEntry(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION), true);
			_unit->GetMapMgr()->CreateAndSpawnGameObject(202241, -495.451f, 2174.187f, 539.288f, 2.77f, 1.0f);
		}
		ParentClass::OnDied(mKiller);
	}

	void OnCombatStop(Unit* mTarget)
	{
		_unit->RemoveAura(64238);
		_unit->RemoveAura(SPELL_BLOOD_LINK);
		ParentClass::OnCombatStop(mTarget);
	}

	void AIUpdate()
	{
		if (IsTimerFinished(BerserkTimer))
		{
			_unit->CastSpell(_unit, 26662, false);
			Emote("I have become...DEATH!", Text_Yell, 16698);
			RemoveTimer(BerserkTimer);
		}
		if (IsTimerFinished(BerserkTimerH))
		{
			_unit->CastSpell(_unit, 26662, false);
			Emote("I have become...DEATH!", Text_Yell, 16698);
			RemoveTimer(BerserkTimerH);
		}
		if (IsTimerFinished(LinkAdd) && _unit->GetPower(POWER_TYPE_MANA) <= 99)
		{
			_unit->SetPower(POWER_TYPE_MANA, _unit->GetPower(POWER_TYPE_MANA)+RandomUInt(3));
			ResetTimer(LinkAdd, RandomUInt(4000));
		}
		if (IsTimerFinished(LinkAdd) && _unit->GetPower(POWER_TYPE_MANA) == 100)
		{
			_unit->SetPower(POWER_TYPE_MANA, 0);
			_unit->CastSpell(TargetGen_RandomPlayer, SPELL_MARK_OF_THE_FALLEN_CHAMPION, false);
			_unit->CastSpell(_unit, SPELL_MARK_OF_THE_FALLEN_CHAMPION_S, true);
			Emote("The ground runs red with your blood!", Text_Yell, 16699);
			ResetTimer(LinkAdd, RandomUInt(4000));
		}
		ParentClass::AIUpdate();
	}
protected:
	int32 BerserkTimer;
	int32 BerserkTimerH;
	int32 LinkAdd;
};

void SetupICC(ScriptMgr* mgr)
{
    //Instance
    mgr->register_instance_script(MAP_ICECROWNCITADEL, &IceCrownCitadelScript::Create);

    //Teleporters
    mgr->register_gameobject_script(GO_TELE_1, &ICCTeleporterAI::Create);
    mgr->register_go_gossip_script(GO_TELE_1, new ICCTeleporterGossip());

    mgr->register_gameobject_script(GO_TELE_2, &ICCTeleporterAI::Create);
    mgr->register_go_gossip_script(GO_TELE_2, new ICCTeleporterGossip());

    mgr->register_gameobject_script(GO_TELE_3, &ICCTeleporterAI::Create);
    mgr->register_go_gossip_script(GO_TELE_3, new ICCTeleporterGossip());

    mgr->register_gameobject_script(GO_TELE_4, &ICCTeleporterAI::Create);
    mgr->register_go_gossip_script(GO_TELE_4, new ICCTeleporterGossip());

    mgr->register_gameobject_script(GO_TELE_5, &ICCTeleporterAI::Create);
    mgr->register_go_gossip_script(GO_TELE_5, new ICCTeleporterGossip());

    //Bosses
    mgr->register_creature_script(CN_LORD_MARROWGAR, &LordMarrowgarAI::Create);
    mgr->register_creature_script(CN_LADY_DEATHWHISPER, &DeathwhisperAI::Create);
	mgr->register_creature_script(CN_DEATHBRINGER_SAURFANG, &DeathbringerAI::Create);
    //mgr->register_creature_script(CN_VALITHRIA_DREAMWALKER, &ValithriaDreamwalkerAI::Create);
    mgr->register_creature_script(CN_COLDFLAME, &ColdFlameAI::Create);

    //Misc
    mgr->register_creature_script(CN_BONE_SPIKE, &BoneSpikeAI::Create);
	mgr->register_creature_gossip(39371, new Varian_Gossip());
}
