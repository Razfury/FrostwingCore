/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2007 Moon++ <http://www.moonplusplus.info/>
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

#include "Setup.h"

bool EyeForAnEye(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if (apply)
        target->AddProcTriggerSpell(25997, pAura->GetSpellProto()->Id, pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_CRIT_HIT_VICTIM | PROC_ON_RANGED_CRIT_ATTACK_VICTIM | PROC_ON_SPELL_CRIT_HIT_VICTIM, 0, NULL, NULL);
    else
        target->RemoveProcTriggerSpell(25997, pAura->m_casterGuid);

    return true;
}

bool HolyShock(uint32 i, Spell* pSpell)
{
    ///\todo This function returns true on failures (invalid target, invalid spell). Verify this is the correct return value
    Unit* target = pSpell->GetUnitTarget();
    if (target == NULL)
    {
        return true;
    }

    Player* caster = pSpell->p_caster;
    if (caster == NULL)
    {
        return true;
    }

    uint32 spell_id = 0;

    if (isAttackable(caster, target))
    {
        // Cast offensive Holy Shock
        switch (pSpell->GetProto()->Id)
        {
        case 20473: // Rank 1
            spell_id = 25912;
            break;
        case 20929: // Rank 2
            spell_id = 25911;
            break;
        case 20930: // Rank 3
            spell_id = 25902;
            break;
        case 27174: // Rank 4
            spell_id = 27176;
            break;
        case 33072: // Rank 5
            spell_id = 33073;
            break;
        case 48824: // Rank 6
            spell_id = 48822;
            break;
        case 48825: // Rank 7
            spell_id = 48823;
            break;
        default: // Invalid case, spell handler is assigned to wrong spell
            Log.Error("SpellHandlers\\PaladinSpells.cpp", "(Offensive) Holy Shock spell handler assigned to invalid spell id [%u]", pSpell->GetProto()->Id);
            return true;
        }
    }
    else
    {
        // Cast healing Holy Shock
        switch (pSpell->GetProto()->Id)
        {
        case 20473: // Rank 1
            spell_id = 25914;
            break;
        case 20929: // Rank 2
            spell_id = 25913;
            break;
        case 20930: // Rank 3
            spell_id = 25903;
            break;
        case 27174: // Rank 4
            spell_id = 27175;
            break;
        case 33072: // Rank 5
            spell_id = 33074;
            break;
        case 48824: // Rank 6
            spell_id = 48820;
            break;
        case 48825: // Rank 7
            spell_id = 48821;
            break;
        default: // Invalid case, spell handler is assigned to wrong spell
            Log.Error("SpellHandlers\\PaladinSpells.cpp", "(Defensive) Holy Shock spell handler assigned to invalid spell id [%u]", pSpell->GetProto()->Id);
            return true;
        }
    }

    caster->CastSpell(target, spell_id, false);

    return true;
}

bool SealOfRighteousness(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if (i == 0)
    {
        if (apply)
            target->AddProcTriggerSpell(25742, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK, 0, NULL, NULL);
        else
            target->RemoveProcTriggerSpell(25742, pAura->m_casterGuid);
    }

    return true;
}

bool SealOfCorruption(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if (i == 0)
    {
        if (apply)
        {
            target->AddProcTriggerSpell(53742, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK, 0, NULL, NULL);
            target->AddProcTriggerSpell(53739, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK, 0, NULL, NULL);
        }
        else
        {
            target->RemoveProcTriggerSpell(53742, pAura->m_casterGuid);
            target->RemoveProcTriggerSpell(53739, pAura->m_casterGuid);
        }
    }

    return true;
}

bool SealOfVengeance(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if (i == 0)
    {
        if (apply)
        {
            target->AddProcTriggerSpell(31803, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK, 0, NULL, NULL);
            target->AddProcTriggerSpell(42463, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK, 0, NULL, NULL);
        }
        else
        {
            target->RemoveProcTriggerSpell(31803, pAura->m_casterGuid);
            target->RemoveProcTriggerSpell(42463, pAura->m_casterGuid);
        }
    }

    return true;
}

void JudgementSpell(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	Unit* target = sp->GetUnitTarget();
	if (target == NULL)
		return;

	Player* caster = sp->p_caster;
	if (caster == NULL)
		return;

	if (SpellEffectType == SPELL_EFFECT_SCRIPT_EFFECT)
	{
		if (spellId == 20271 || spellId == 53408 || spellId == 53407)
		{
			// Search for a previous judgement casted by this caster. He can have only 1 judgement active at a time
			uint32 index = 0;
			uint32 judgements[] = { SPELL_HASH_JUDGEMENT_OF_LIGHT, SPELL_HASH_JUDGEMENT_OF_WISDOM, SPELL_HASH_JUDGEMENT_OF_JUSTICE,
				SPELL_HASH_JUDGEMENT_OF_VENGEANCE, SPELL_HASH_JUDGEMENT_OF_CORRUPTION, SPELL_HASH_JUDGEMENT_OF_RIGHTEOUSNESS, 0
			};

			uint64 prev_target = caster->GetCurrentUnitForSingleTargetAura(judgements, &index);
			if (prev_target)
			{
				Unit* t = caster->GetMapMgr()->GetUnit(prev_target);

				if (t != NULL)
					t->RemoveAllAuraByNameHash(judgements[index]);

				caster->RemoveCurrentUnitForSingleTargetAura(judgements[index]);
			}

			// Search for seal to unleash its energy
			uint32 seals[] = { 20375, 20165, 20164, 21084, 31801, 53736, 20166, 0 };

			Aura* aura = caster->FindAura(seals);
			if (aura == NULL)
				return;

			uint32 id = 0;
			switch (aura->GetSpellId())
			{
			case 20375:
				id = 20467;
				break;
			case 20165:
				id = 54158;
				break;
			case 20164:
				id = 54158;
				break;
			case 21084:
				id = 20187;
				break;
			case 31801:
				id = aura->GetSpellProto()->EffectBasePoints[2];
				break;
			case 53736:
				id = aura->GetSpellProto()->EffectBasePoints[2];
				break;
			case 20166:
				id = 54158;
				break;
			}

			caster->CastSpell(target, id, true);

			// Cast judgement spell
			switch (sp->GetProto()->NameHash)
			{
			case SPELL_HASH_JUDGEMENT_OF_JUSTICE:
				id = 20184;
				break;
			case SPELL_HASH_JUDGEMENT_OF_LIGHT:
				id = 20185;
				break;
			case SPELL_HASH_JUDGEMENT_OF_WISDOM:
				id = 20186;
				break;
			}

			caster->CastSpell(target, id, true);

			caster->SetCurrentUnitForSingleTargetAura(sp->GetProto(), target->GetGUID());
		}
	}

};

bool JudgementOfLight(uint32 i, Aura* pAura, bool apply)
{
    Unit* caster = pAura->GetUnitCaster();
    if (caster == NULL)
        return true;

    if (apply)
        caster->AddProcTriggerSpell(20267, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF, 0, NULL, NULL);
    else
        caster->RemoveProcTriggerSpell(20267, pAura->m_casterGuid);

    return true;
}

bool JudgementOfWisdom(uint32 i, Aura* pAura, bool apply)
{
    Unit* caster = pAura->GetUnitCaster();
    if (caster == NULL)
        return true;

    if (apply)
        caster->AddProcTriggerSpell(20268, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF, 0, NULL, NULL);
    else
        caster->RemoveProcTriggerSpell(20268, pAura->m_casterGuid);

    return true;
}

bool RighteousDefense(uint32 i, Spell* s)
{
    //we will try to lure 3 enemies from our target

    Unit* unitTarget = s->GetUnitTarget();

    if (!unitTarget || !s->u_caster)
        return false;

    Unit* targets[3];
    uint32 targets_got = 0;

    for (std::set<Object*>::iterator itr = unitTarget->GetInRangeSetBegin(), i2; itr != unitTarget->GetInRangeSetEnd();)
    {
        i2 = itr++;

        // don't add objects that are not creatures and that are dead
        if (!(*i2)->IsCreature() || !TO< Creature* >((*i2))->isAlive())
            continue;

        Creature* cr = TO< Creature* >((*i2));
        if (cr->GetAIInterface()->getNextTarget() == unitTarget)
            targets[targets_got++] = cr;

        if (targets_got == 3)
            break;
    }

    for (uint32 j = 0; j < targets_got; j++)
    {
        //set threat to this target so we are the msot hated
        uint32 threat_to_him = targets[j]->GetAIInterface()->getThreatByPtr(unitTarget);
        uint32 threat_to_us = targets[j]->GetAIInterface()->getThreatByPtr(s->u_caster);
        int threat_dif = threat_to_him - threat_to_us;
        if (threat_dif > 0) //should nto happen
            targets[j]->GetAIInterface()->modThreatByPtr(s->u_caster, threat_dif);

        targets[j]->GetAIInterface()->AttackReaction(s->u_caster, 1, 0);
        targets[j]->GetAIInterface()->setNextTarget(s->u_caster);
    }

    return true;
}

bool Illumination(uint32 i, Spell* s)
{
    switch (s->m_triggeredByAura == NULL ? s->GetProto()->Id : s->m_triggeredByAura->GetSpellId())
    {
    case 20210:
    case 20212:
    case 20213:
    case 20214:
    case 20215:
    {
                  if (s->p_caster == NULL)
                      return false;
                  SpellEntry* sp = s->p_caster->last_heal_spell ? s->p_caster->last_heal_spell : s->GetProto();
                  s->p_caster->Energize(s->p_caster, 20272, 60 * s->u_caster->GetBaseMana() * sp->ManaCostPercentage / 10000, POWER_TYPE_MANA);
    }
        break;


    }
    return true;
}

bool JudgementOfTheWise(uint32 i, Spell* s)
{
    if (!s->p_caster)
        return false;

    s->p_caster->Energize(s->p_caster, 31930, uint32(0.15f * s->p_caster->GetBaseMana()), POWER_TYPE_MANA);
    s->p_caster->CastSpell(s->p_caster, 57669, false);

    return true;
}

void ArtOfWar_OnCritAttack(Player* pPlayer)
{
	if (pPlayer)
	{
		if (pPlayer->HasAura(53486) || pPlayer->HasSpell(53486))
		{
			pPlayer->CastSpell(pPlayer, 53489, true);
		}
		if (pPlayer->HasAura(53488) || pPlayer->HasSpell(53488))
		{
			pPlayer->CastSpell(pPlayer, 59578, true);
		}
	}
};

void ArtOfWar_OnAfterSpellCast(Player* pPlayer, SpellEntry* pSpell, Spell* spell)
{
	if (!spell->u_caster->IsPlayer()) // If caster is not a player return
		return;

	if (pSpell->NameHash == SPELL_HASH_EXORCISM || pSpell->NameHash == SPELL_HASH_FLASH_OF_LIGHT)
	{
		if (pPlayer)
		{
			pPlayer->RemoveAuraSpecial(53489);
			pPlayer->RemoveAuraSpecial(59578);
		}
	}
};

void SwiftRetribution(Player* pPlayer, SpellEntry* pSpell, Spell* spell) // Cast paladin Auras
{
	if (spell->u_caster->IsPlayer() == false)
		return;

	if (spell->GetProto()->NameHash == SPELL_HASH_DEVOTION_AURA || spell->GetProto()->NameHash == SPELL_HASH_RETRIBUTION_AURA || spell->GetProto()->NameHash == SPELL_HASH_CONCENTRATION_AURA || spell->GetProto()->NameHash == SPELL_HASH_SHADOW_RESISTANCE_AURA || spell->GetProto()->NameHash == SPELL_HASH_FROST_RESISTANCE_AURA || spell->GetProto()->NameHash == SPELL_HASH_FIRE_RESISTANCE_AURA || spell->GetProto()->NameHash == SPELL_HASH_CRUSADER_AURA)
	{
		if (pPlayer->HasAura(53379)) // Rank 1 1%
		{
			pPlayer->ModAttackSpeed(1, MOD_MELEE);
			pPlayer->ModAttackSpeed(1, MOD_RANGED);
			pPlayer->ModAttackSpeed(1, MOD_SPELL);
			pPlayer->UpdateAttackSpeed();
		}
		if (pPlayer->HasAura(53484)) // Rank 2 2%
		{
			pPlayer->ModAttackSpeed(2, MOD_MELEE);
			pPlayer->ModAttackSpeed(2, MOD_RANGED);
			pPlayer->ModAttackSpeed(2, MOD_SPELL);
			pPlayer->UpdateAttackSpeed();
		}
		if (pPlayer->HasAura(53648)) // Rank 3 3%
		{
			pPlayer->ModAttackSpeed(3, MOD_MELEE);
			pPlayer->ModAttackSpeed(3, MOD_RANGED);
			pPlayer->ModAttackSpeed(3, MOD_SPELL);
			pPlayer->UpdateAttackSpeed();
		}
	}
};

void SwiftRetribution_OnAuraRemove(Aura* aur)
{
	if (aur->GetPlayerCaster())
	{
		if (aur->GetSpellProto()->NameHash == SPELL_HASH_DEVOTION_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_RETRIBUTION_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_CONCENTRATION_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_SHADOW_RESISTANCE_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_FROST_RESISTANCE_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_FIRE_RESISTANCE_AURA || aur->GetSpellProto()->NameHash == SPELL_HASH_CRUSADER_AURA)
		{
			if (aur->GetPlayerCaster()->HasAura(53379)) // Rank 1 1%
			{
				aur->GetPlayerCaster()->ModAttackSpeed(-1, MOD_MELEE);
				aur->GetPlayerCaster()->ModAttackSpeed(-1, MOD_RANGED);
				aur->GetPlayerCaster()->ModAttackSpeed(-1, MOD_SPELL);
				aur->GetPlayerCaster()->UpdateAttackSpeed();
			}
			if (aur->GetPlayerCaster()->HasAura(53484)) // Rank 2 2%
			{
				aur->GetPlayerCaster()->ModAttackSpeed(-2, MOD_MELEE);
				aur->GetPlayerCaster()->ModAttackSpeed(-2, MOD_RANGED);
				aur->GetPlayerCaster()->ModAttackSpeed(-2, MOD_SPELL);
				aur->GetPlayerCaster()->UpdateAttackSpeed();
			}
			if (aur->GetPlayerCaster()->HasAura(53648)) // Rank 3 3%
			{
				aur->GetPlayerCaster()->ModAttackSpeed(-3, MOD_MELEE);
				aur->GetPlayerCaster()->ModAttackSpeed(-3, MOD_RANGED);
				aur->GetPlayerCaster()->ModAttackSpeed(-3, MOD_SPELL);
				aur->GetPlayerCaster()->UpdateAttackSpeed();
			}
		}
	}
};

// Hack! Will replace soon but for some reason the spell effect for school damage will not damage the target so we manually do this.
void AvengersShield(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (!sp->p_caster)
		return;

	uint32 damage_override = 0;
	uint32 procChance = RandomUInt(0, 100);
	Player* player = sp->p_caster;

	if (SpellEffectType == SPELL_EFFECT_SCHOOL_DAMAGE)
	{
		if (spellId == 31935) // Rank 1
		{
			uint32 damage_override = 0;
			if (sp->GetUnitTarget())
			{
				uint32 dmg = RandomUInt(440, 536);
				damage_override = 0.07 * player->GetDamageDoneMod(SCHOOL_HOLY) + 0.07 * player->GetAP();
				dmg += damage_override;
				uint32 crit_chance = RandomUInt(0, 100);
				if (crit_chance >= 16)
				{
					player->DealDamage(sp->GetUnitTarget(), dmg, 0, 0, 31935);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 31935, dmg, SCHOOL_HOLY, 0, 0, false, 0, false, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
				else
				{
					player->DealDamage(sp->GetUnitTarget(), dmg * 1.5, 0, 0, 31935);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 31935, dmg * 1.5, SCHOOL_HOLY, 0, 0, false, 0, true, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
			}
		}
		if (spellId == 32699) // Rank 2
		{
			uint32 damage_override = 0;
			if (sp->GetUnitTarget())
			{
				uint32 dmg = RandomUInt(601, 733);
				damage_override = 0.07 * player->GetDamageDoneMod(SCHOOL_HOLY) + 0.07 * player->GetAP();
				dmg += damage_override;
				uint32 crit_chance = RandomUInt(0, 100);
				if (crit_chance >= 16)
				{
					player->DealDamage(sp->GetUnitTarget(), dmg, 0, 0, 32699);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 32699, dmg, SCHOOL_HOLY, 0, 0, false, 0, false, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
				else
				{
					player->DealDamage(sp->GetUnitTarget(), dmg * 1.5, 0, 0, 32699);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 32699, dmg * 1.5, SCHOOL_HOLY, 0, 0, false, 0, true, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
			}
		}
		if (spellId == 32700) // Rank 3
		{
			uint32 damage_override = 0;
			if (sp->GetUnitTarget())
			{
				uint32 dmg = RandomUInt(796, 972);
				damage_override = 0.07 * player->GetDamageDoneMod(SCHOOL_HOLY) + 0.07 * player->GetAP();
				dmg += damage_override;
				uint32 crit_chance = RandomUInt(0, 100);
				if (crit_chance >= 16)
				{
					player->DealDamage(sp->GetUnitTarget(), dmg, 0, 0, 32700);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 32700, dmg, SCHOOL_HOLY, 0, 0, false, 0, false, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
				else
				{
					player->DealDamage(sp->GetUnitTarget(), dmg * 1.5, 0, 0, 32700);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 32700, dmg * 1.5, SCHOOL_HOLY, 0, 0, false, 0, true, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
			}
		}
		if (spellId == 48826) // Rank 4
		{
			uint32 damage_override = 0;
			if (sp->GetUnitTarget())
			{
				uint32 dmg = RandomUInt(913, 1115);
				damage_override = 0.07 * player->GetDamageDoneMod(SCHOOL_HOLY) + 0.07 * player->GetAP();
				dmg += damage_override;
				uint32 crit_chance = RandomUInt(0, 100);
				if (crit_chance >= 16)
				{
					player->DealDamage(sp->GetUnitTarget(), dmg, 0, 0, 48826);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 48826, dmg, SCHOOL_HOLY, 0, 0, false, 0, false, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
				else
				{
					player->DealDamage(sp->GetUnitTarget(), dmg * 1.5, 0, 0, 48826);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 48826, dmg * 1.5, SCHOOL_HOLY, 0, 0, false, 0, true, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
			}
		}
		if (spellId == 48827) // Rank 5
		{
			uint32 damage_override = 0;
			if (sp->GetUnitTarget())
			{
				uint32 dmg = RandomUInt(1100, 1344);
				damage_override = 0.07 * player->GetDamageDoneMod(SCHOOL_HOLY) + 0.07 * player->GetAP();
				dmg += damage_override;
				uint32 crit_chance = RandomUInt(0, 100);
				if (crit_chance >= 16)
				{
					player->DealDamage(sp->GetUnitTarget(), dmg, 0, 0, 48827);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 48827, dmg, SCHOOL_HOLY, 0, 0, false, 0, false, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
				else
				{
					player->DealDamage(sp->GetUnitTarget(), dmg * 1.5, 0, 0, 48827);
					player->SendSpellNonMeleeDamageLog(player, sp->GetUnitTarget(), 48827, dmg * 1.5, SCHOOL_HOLY, 0, 0, false, 0, true, true);
					if (player->HasAura(53709) && procChance <= 33) // Prot Talent: Shield Of The Templar Rank 1 (33% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53710) && procChance <= 66) // Prot Talent: Shield Of The Templar Rank 2 (66% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
					if (player->HasAura(53711)) // Prot Talent: Shield Of The Templar Rank 3 (100% chance)
					{
						player->CastSpell(sp->GetUnitTarget(), 63529, true);
					}
				}
			}
		}
	}
};

void SetupPaladinSpells(ScriptMgr* mgr)
{
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&AvengersShield);
	mgr->RegisterSpellType(SPELL_EVENT_AFTER_CAST_SPELL, (void*)&SwiftRetribution);
	mgr->RegisterSpellType(SPELL_EVENT_AURA_REMOVE, (void*)&SwiftRetribution_OnAuraRemove);
	mgr->RegisterSpellType(SPELL_EVENT_AFTER_CAST_SPELL, (void*)&ArtOfWar_OnAfterSpellCast);
	mgr->RegisterSpellType(SPELL_EVENT_ON_CRIT_ATTACK, (void*)&ArtOfWar_OnCritAttack);
    mgr->register_dummy_aura(9799, &EyeForAnEye);
    mgr->register_dummy_aura(25988, &EyeForAnEye);

    uint32 HolyShockIds[] = { 20473, 20929, 20930, 27174, 33072, 48824, 48825, 0 };
    mgr->register_dummy_spell(HolyShockIds, &HolyShock);

    mgr->register_dummy_aura(21084, &SealOfRighteousness);
    mgr->register_dummy_aura(53736, &SealOfCorruption);
    mgr->register_dummy_aura(31801, &SealOfVengeance);

	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&JudgementSpell);

    mgr->register_dummy_aura(20185, &JudgementOfLight);
    mgr->register_dummy_aura(20186, &JudgementOfWisdom);

    mgr->register_dummy_spell(31789, &RighteousDefense);
    mgr->register_dummy_spell(18350, &Illumination);

    mgr->register_dummy_spell(54180, &JudgementOfTheWise);
}
