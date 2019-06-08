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

class BloodCakedBladeSpellProc : public SpellProc
{
	SPELL_PROC_FACTORY_FUNCTION(BloodCakedBladeSpellProc);

	bool CanProc(Unit* victim, SpellEntry* CastingSpell)
	{
		if (CastingSpell) // Do not proc on a casting spell only MELEE attacks
			return false;
		else
			return true;
	}
};


class ButcherySpellProc : public SpellProc
{
    SPELL_PROC_FACTORY_FUNCTION(ButcherySpellProc);

    bool DoEffect(Unit* victim, SpellEntry* CastingSpell, uint32 flag, uint32 dmg, uint32 abs, int* dmg_overwrite, uint32 weapon_damage_type)
    {
        dmg_overwrite[0] = mOrigSpell->EffectBasePoints[0] + 1;

        return false;
    }
};

class NecrosisSpellProc : public SpellProc
{
	SPELL_PROC_FACTORY_FUNCTION(NecrosisSpellProc);

	void Init(Object* obj)
	{
		dk1 = static_cast<DeathKnight*>(mTarget);
	}

	bool DoEffect(Unit* victim, SpellEntry* CastingSpell, uint32 flag, uint32 dmg, uint32 abs, int* dmg_overwrite, uint32 weapon_damage_type)
	{
		if (mTarget != NULL)
		{
			if (mTarget->HasAura(51459)) // Necrosis rank 1
			{
				dmg_overwrite[0] = CalculateDamage(mTarget, victim, MELEE, 0, 0) * 0.03; // 4%
			}
			if (mTarget->HasAura(51462)) // Necrosis rank 2
			{
				dmg_overwrite[0] = CalculateDamage(mTarget, victim, MELEE, 0, 0) * 0.06; // 8%
			}
			if (mTarget->HasAura(51463)) // Necrosis rank 3
			{
				dmg_overwrite[0] = CalculateDamage(mTarget, victim, MELEE, 0, 0) * 0.10; // 12%
			}
			if (mTarget->HasAura(51464)) // Necrosis rank 4
			{
				dmg_overwrite[0] = CalculateDamage(mTarget, victim, MELEE, 0, 0) * 0.14; // 16%
			}
			if (mTarget->HasAura(51465)) // Necrosis rank 5
			{
				dmg_overwrite[0] = CalculateDamage(mTarget, victim, MELEE, 0, 0) * 0.18; // 20%
			}
		}

		return false;
	}

	bool CanProc(Unit* victim, SpellEntry* CastingSpell)
	{
		if (CastingSpell) // Do not proc on a casting spell only MELEE attacks
			return false;
		else
			return true;
	}

private:
	Player* dk1;

};

class BladeBarrierSpellProc : public SpellProc
{
    SPELL_PROC_FACTORY_FUNCTION(BladeBarrierSpellProc);

    void Init(Object* obj)
    {
        mProcFlags = PROC_ON_CAST_SPELL;

        mProcClassMask[0] = mOrigSpell->EffectSpellClassMask[0][0];
        mProcClassMask[1] = mOrigSpell->EffectSpellClassMask[0][1];
        mProcClassMask[2] = mOrigSpell->EffectSpellClassMask[0][2];

		dk = static_cast<DeathKnight*>(mTarget);
    }

    bool CanProc(Unit* victim, SpellEntry* CastingSpell)
    {
		if (!dk->CanUseRunes(2, 0, 0)) // See if DK can use 2 blood runes if not then the runes must be on cooldown
        //if (dk->m_runes)
            return true;

        return false;
    }

    private:
		Player* dk;
};

void SpellProcMgr::SetupDeathKnight()
{
	AddById(49219, &BloodCakedBladeSpellProc::Create);
	AddById(49627, &BloodCakedBladeSpellProc::Create);
	AddById(49628, &BloodCakedBladeSpellProc::Create);
    AddById(50163, &ButcherySpellProc::Create);
	AddById(51460, &NecrosisSpellProc::Create);

    AddByNameHash(SPELL_HASH_BLADE_BARRIER, &BladeBarrierSpellProc::Create);
}
