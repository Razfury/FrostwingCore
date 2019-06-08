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

class HotStreakSpellProc : public SpellProc
{
    SPELL_PROC_FACTORY_FUNCTION(HotStreakSpellProc);

    void Init(Object* obj)
    {
        mCritsInARow = 0;
    }

    bool DoEffect(Unit* victim, SpellEntry* CastingSpell, uint32 flag, uint32 dmg, uint32 abs, int* dmg_overwrite, uint32 weapon_damage_type)
    {
        // Check for classmask. Should proc only if CastingSpell is one listed in http://www.wowhead.com/spell=44448
        if (!CheckClassMask(victim, CastingSpell))
            return true;

        // If was not a crit, reset counter and don't proc
        if (!(flag & PROC_ON_SPELL_CRIT_HIT))
        {
            mCritsInARow = 0;
            return true;
        }

        // If was not at least 2nd crit in a row, don't proc
        if (++mCritsInARow < 2)
            return true;

        return false;
    }

    private:
    int mCritsInARow;
};

class CombustionSpellProc : public SpellProc
{
    SPELL_PROC_FACTORY_FUNCTION(CombustionSpellProc);

    void Init(Object* obj)
    {
        mCrits = 0;
    }

    bool DoEffect(Unit* victim, SpellEntry* CastingSpell, uint32 flag, uint32 dmg, uint32 abs, int* dmg_overwrite, uint32 weapon_damage_type)
    {
        // If spell was a crit, in the fire school and direct fire damage (non-periodic)
        if ((flag & PROC_ON_SPELL_CRIT_HIT) && CastingSpell->School == 4 && CastingSpell->Effect[0] == SPELL_EFFECT_SCHOOL_DAMAGE || CastingSpell->Effect[1] == SPELL_EFFECT_SCHOOL_DAMAGE || CastingSpell->Effect[2] == SPELL_EFFECT_SCHOOL_DAMAGE)
        {
            ++mCrits; // Add 1 to the crit list
            return true;
        }

        // If it was a 3rd critical remove aura.
        if (++mCrits >= 3)
            if (mTarget)
            mTarget->RemoveAuraByNameHash(SPELL_HASH_COMBUSTION);
            return true;

        return false;
    }

private:
    int mCrits;
};

void SpellProcMgr::SetupMage()
{
    AddById(48108, &HotStreakSpellProc::Create);
    AddById(11129, &CombustionSpellProc::Create);
}
