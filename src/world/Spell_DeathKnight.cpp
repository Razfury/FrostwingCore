/*
 * FrostwingCore <https://gitlab.com/cronicx321/AscEmu>
 * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org/>
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2005-2007 Ascent Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

class AnnihilationTalent : public Spell
{
	SPELL_FACTORY_FUNCTION(AnnihilationTalent);

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster->IsPlayer() == false)
			return;

		// Annihilation Talent fix below
		int32 chance;
		chance = rand() % 100;
		if (target != NULL && p_caster->HasAura(51468)) // annihilation rank 1 - 33%
		{
			if (chance <= 33)
			{
				return;
			}
			else
			{
				target->RemoveAllAuraByNameHash(SPELL_HASH_FROST_FEVER);
				target->RemoveAllAuraByNameHash(SPELL_HASH_BLOOD_PLAGUE);
				target->RemoveAllAuraByNameHash(SPELL_HASH_CRYPT_FEVER);
				target->RemoveAllAuraByNameHash(SPELL_HASH_EBON_PLAGUE);
			}
		}
		if (target != NULL && p_caster->HasAura(51472)) // annihilation rank 2 - 66%
		{
			if (chance <= 66)
			{
				return;
			}
			else
			{
				target->RemoveAllAuraByNameHash(SPELL_HASH_FROST_FEVER);
				target->RemoveAllAuraByNameHash(SPELL_HASH_BLOOD_PLAGUE);
				target->RemoveAllAuraByNameHash(SPELL_HASH_CRYPT_FEVER);
				target->RemoveAllAuraByNameHash(SPELL_HASH_EBON_PLAGUE);
			}
		}
		if (target != NULL && p_caster->HasAura(51473)) // annihilation rank 3 - 100% consumes NO diseases.
		{
			return;
		}
		if (target != NULL) // does player does not have annihilation talent
		{
			target->RemoveAllAuraByNameHash(SPELL_HASH_FROST_FEVER);
			target->RemoveAllAuraByNameHash(SPELL_HASH_BLOOD_PLAGUE);
			target->RemoveAllAuraByNameHash(SPELL_HASH_CRYPT_FEVER);
			target->RemoveAllAuraByNameHash(SPELL_HASH_EBON_PLAGUE);
		}
	}
};

class BloodPlagueSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(BloodPlagueSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        if (p_caster != NULL && i == 0)
            value += (uint32)(p_caster->GetAP() * 0.055 * 1.15);

        return value;
    }

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster == NULL)
			return;

		if (p_caster->HasAura(49032))// Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50508, true);
		}
		if (p_caster->HasAura(49631)) // Rank 2
		{
			p_caster->CastSpell(target, 50509, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51099) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51099) == true
		{
			p_caster->CastSpell(target, 51726, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51160) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51160) == true
		{
			p_caster->CastSpell(target, 51734, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51161) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51161) == true
		{
			p_caster->CastSpell(target, 51735, true);
		}
	}
};

class FrostFeverSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(FrostFeverSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        if (p_caster != NULL && i == 0)
            value += (uint32)(p_caster->GetAP() * 0.055 * 1.15);

        return value;
    }

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster == NULL)
			return;

		if (p_caster->HasAura(49032))// Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50508, true);
		}
		if (p_caster->HasAura(49631)) // Rank 2
		{
			p_caster->CastSpell(target, 50509, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51099) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51099) == true
		{
			p_caster->CastSpell(target, 51726, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51160) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51160) == true
		{
			p_caster->CastSpell(target, 51734, true);
		}
		if (p_caster->HasAura(49632) && p_caster->HasAura(51161) == false) // Player has Crypt Fever talent but not Ebon Plaguebringer
		{
			p_caster->CastSpell(target, 50510, true);
		}
		else // p_caster->HasAura(51161) == true
		{
			p_caster->CastSpell(target, 51735, true);
		}
	}
};

class BloodBoilSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(BloodBoilSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        if (p_caster != NULL && i == 0)
        {
            int32 ap = p_caster->GetAP();

            value += (uint32)(ap * 0.08);

            // Does additional damage if target has diseases (http://www.tankspot.com/forums/f14/48814-3-1-blood-boil-mechanics-tested.html)
            if (target != NULL && (target->HasAura(55078) || target->HasAura(55095)))
                value += (uint32)(ap * 0.015 + 95);
        }

        return value;
    }
};

class BloodStrikeSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(BloodStrikeSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        if (target != NULL)
        {
            uint32 count = target->GetAuraCountWithDispelType(DISPEL_DISEASE, m_caster->GetGUID());
            if (count)
                value += value * count * (GetProto()->EffectBasePoints[2] + 1) / 200;
        }

        return value;
    }

    void DoAfterHandleEffect(Unit* target, uint32 i)
    {
        if (p_caster == NULL || i != 1)
            return;

        Aura* aur = p_caster->FindAuraByNameHash(SPELL_HASH_SUDDEN_DOOM);

        if (aur == NULL)
            return;

        if (!Rand(aur->GetSpellProto()->procChance))
            return;

        p_caster->CastSpell(target, 47632, false);
    }
};

class DeathCoilSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(DeathCoilSpell);

    uint8 CanCast(bool tolerate)
    {
        uint8 result = Spell::CanCast(tolerate);

        if (result == SPELL_CANCAST_OK)
        {
            if (m_caster != NULL && m_caster->IsInWorld())
            {
                Unit* target = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);

                if (target == NULL || !(isAttackable(m_caster, target, false) || target->getRace() == RACE_UNDEAD))
                    result = SPELL_FAILED_BAD_TARGETS;
            }
        }

        return result;
    }
};

class RuneStrileSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(RuneStrileSpell);

    void HandleEffects(uint64 guid, uint32 i)
    {
        Spell::HandleEffects(guid, i);

        if (u_caster != NULL)
            u_caster->RemoveAura(56817);
    }
};

class AntiMagicShellAura : public AbsorbAura
{
    public:
    static Aura* Create(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL) { return new AntiMagicShellAura(proto, duration, caster, target, temporary, i_caster); }

    AntiMagicShellAura(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL)
        : AbsorbAura(proto, duration, caster, target, temporary, i_caster) {}

    int32 CalcAbsorbAmount()
    {
        Player* caster = GetPlayerCaster();
        if (caster != NULL)
            return caster->GetMaxHealth() * (GetSpellProto()->EffectBasePoints[1] + 1) / 100;
        else
            return mod->m_amount;
    }

    int32 CalcPctDamage()
    {
        return GetSpellProto()->EffectBasePoints[0] + 1;
    }
};

class SpellDeflectionAura : public AbsorbAura
{
    public:
    static Aura* Create(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL) { return new SpellDeflectionAura(proto, duration, caster, target, temporary, i_caster); }

    SpellDeflectionAura(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL)
        : AbsorbAura(proto, duration, caster, target, temporary, i_caster) {}

    uint32 AbsorbDamage(uint32 School, uint32* dmg)
    {
        uint32 mask = GetSchoolMask();
        if (!(mask & g_spellSchoolConversionTable[School]))
            return 0;

        Player* caster = GetPlayerCaster();
        if (caster == NULL)
            return 0;

        if (!Rand(caster->GetParryChance()))
            return 0;

        uint32 dmg_absorbed = *dmg * GetModAmount(0) / 100;
        *dmg -= dmg_absorbed;

        return dmg_absorbed;
    }
};

class BloodwormSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(BloodwormSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        return 2 + RandomUInt(2);
    }
};

class WillOfTheNecropolisAura : public AbsorbAura
{
    public:
    static Aura* Create(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL) { return new WillOfTheNecropolisAura(proto, duration, caster, target, temporary, i_caster); }

    WillOfTheNecropolisAura(SpellEntry* proto, int32 duration, Object* caster, Unit* target, bool temporary = false, Item* i_caster = NULL)
        : AbsorbAura(proto, duration, caster, target, temporary, i_caster) {}

    uint32 AbsorbDamage(uint32 School, uint32* dmg)
    {
        Unit* caster = GetUnitCaster();
        if (caster == NULL)
            return 0;

        int health_pct = caster->GetHealthPct();
        uint32 cur_health = caster->GetHealth();
        uint32 max_health = caster->GetMaxHealth();
        uint32 new_health_pct = (cur_health - *dmg) * 100 / max_health;

        // "Damage that would take you below $s1% health or taken while you are at $s1% health is reduced by $52284s1%."
        if ((health_pct > 35 && new_health_pct < 35) || health_pct == 35)
        {
            uint32 dmg_absorbed = *dmg * (GetSpellProto()->EffectBasePoints[0] + 1) / 100;
            *dmg -= dmg_absorbed;

            return dmg_absorbed;
        }
        else
            return 0;
    }
};

class VampiricBloodSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(VampiricBloodSpell);

    int32 DoCalculateEffect(uint32 i, Unit* target, int32 value)
    {
        if (i == 1 && p_caster != NULL)
            value = p_caster->GetMaxHealth() * (GetProto()->EffectBasePoints[i] + 1) / 100;

        return value;
    }
};

class HeartStrikeSpell : public Spell
{
    SPELL_FACTORY_FUNCTION(HeartStrikeSpell);

    void DoAfterHandleEffect(Unit* target, uint32 i)
    {
        if (p_caster == NULL || i != 1)
            return;

        Aura* aur = p_caster->FindAuraByNameHash(SPELL_HASH_SUDDEN_DOOM);

        if (aur == NULL)
            return;

        if (!Rand(aur->GetSpellProto()->procChance))
            return;

        p_caster->CastSpell(target, 47632, false);
    }
};

void SpellFactoryMgr::SetupDeathKnight()
{
	AddSpellByNameHash(SPELL_HASH_OBLITERATE, &AnnihilationTalent::Create);

	//AddSpellById(46584, &RaiseDeadCastCheck::Create);

    AddSpellById(55078, &BloodPlagueSpell::Create);
    AddSpellById(55095, &FrostFeverSpell::Create);

    AddSpellById(48721, &BloodBoilSpell::Create);   // Rank 1
    AddSpellById(49939, &BloodBoilSpell::Create);   // Rank 2
    AddSpellById(49940, &BloodBoilSpell::Create);   // Rank 3
    AddSpellById(49941, &BloodBoilSpell::Create);   // Rank 4

    AddSpellById(45902, &BloodStrikeSpell::Create);   // Rank 1
    AddSpellById(49926, &BloodStrikeSpell::Create);   // Rank 2
    AddSpellById(49927, &BloodStrikeSpell::Create);   // Rank 3
    AddSpellById(49928, &BloodStrikeSpell::Create);   // Rank 4
    AddSpellById(49929, &BloodStrikeSpell::Create);   // Rank 5
    AddSpellById(49930, &BloodStrikeSpell::Create);   // Rank 6

    AddSpellById(47541, &DeathCoilSpell::Create);   // Rank 1
    AddSpellById(49892, &DeathCoilSpell::Create);   // Rank 2
    AddSpellById(49893, &DeathCoilSpell::Create);   // Rank 3
    AddSpellById(49894, &DeathCoilSpell::Create);   // Rank 4
    AddSpellById(49895, &DeathCoilSpell::Create);   // Rank 5

    AddSpellById(56815, &RuneStrileSpell::Create); //QUEST_11715

    AddAuraById(48707, &AntiMagicShellAura::Create);

    AddAuraById(49145, &SpellDeflectionAura::Create);   // Rank 1
    AddAuraById(49495, &SpellDeflectionAura::Create);   // Rank 2
    AddAuraById(49497, &SpellDeflectionAura::Create);   // Rank 3

    AddSpellById(50452, &BloodwormSpell::Create);

    AddAuraById(52284, &WillOfTheNecropolisAura::Create);   // Rank 1
    AddAuraById(52285, &WillOfTheNecropolisAura::Create);   // Rank 1
    AddAuraById(52286, &WillOfTheNecropolisAura::Create);   // Rank 1

    AddSpellById(55233, &VampiricBloodSpell::Create);

    AddSpellById(55050, &HeartStrikeSpell::Create);   // Rank 1
    AddSpellById(55258, &HeartStrikeSpell::Create);   // Rank 2
    AddSpellById(55259, &HeartStrikeSpell::Create);   // Rank 3
    AddSpellById(55260, &HeartStrikeSpell::Create);   // Rank 4
    AddSpellById(55261, &HeartStrikeSpell::Create);   // Rank 5
    AddSpellById(55262, &HeartStrikeSpell::Create);   // Rank 6
}
