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

class DecimationSpell : public Spell
{
	SPELL_FACTORY_FUNCTION(DecimationSpell);

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster != NULL && target != NULL && p_caster->HasAura(63156) && target->GetHealthPct() <= 35) // rank 1
			p_caster->CastSpell(p_caster, 63165, false);
		if (p_caster != NULL && target != NULL && p_caster->HasAura(63158) && target->GetHealthPct() <= 35) // rank 2
			p_caster->CastSpell(p_caster, 63167, false);
	}
};

class EverlastingAffliction : public Spell
{
	SPELL_FACTORY_FUNCTION(EverlastingAffliction);

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster != NULL && target != NULL && p_caster->HasAura(47201)) // rank 1 20%
		{
			chance = rand() % 100;
			if (chance <= 20 && target->HasAura(172)) // 1
			{
				target->FindAura(172)->Refresh();
			}
			if (chance <= 20 && target->HasAura(6222)) // 2
			{
				target->FindAura(6222)->Refresh();
			}
			if (chance <= 20 && target->HasAura(6223)) // 3
			{
				target->FindAura(6223)->Refresh();
			}
			if (chance <= 20 && target->HasAura(7648)) // 4
			{
				target->FindAura(7648)->Refresh();
			}
			if (chance <= 20 && target->HasAura(11671)) // 5
			{
				target->FindAura(11671)->Refresh();
			}
			if (chance <= 20 && target->HasAura(11672)) // 6
			{
				target->FindAura(11672)->Refresh();
			}
			if (chance <= 20 && target->HasAura(25311)) // 7
			{
				target->FindAura(25311)->Refresh();
			}
			if (chance <= 20 && target->HasAura(27216)) // 8
			{
				target->FindAura(27216)->Refresh();
			}
			if (chance <= 20 && target->HasAura(47812)) // 9
			{
				target->FindAura(47812)->Refresh();
			}
			if (chance <= 20 && target->HasAura(47813)) // 10
			{
				target->FindAura(47813)->Refresh();
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(47202)) // rank 2
		{
			chance = rand() % 100;
			if (chance <= 40 && target->HasAura(172)) // 1
			{
				target->FindAura(172)->Refresh();
			}
			if (chance <= 40 && target->HasAura(6222)) // 2
			{
				target->FindAura(6222)->Refresh();
			}
			if (chance <= 40 && target->HasAura(6223)) // 3
			{
				target->FindAura(6223)->Refresh();
			}
			if (chance <= 40 && target->HasAura(7648)) // 4
			{
				target->FindAura(7648)->Refresh();
			}
			if (chance <= 40 && target->HasAura(11671)) // 5
			{
				target->FindAura(11671)->Refresh();
			}
			if (chance <= 40 && target->HasAura(11672)) // 6
			{
				target->FindAura(11672)->Refresh();
			}
			if (chance <= 40 && target->HasAura(25311)) // 7
			{
				target->FindAura(25311)->Refresh();
			}
			if (chance <= 40 && target->HasAura(27216)) // 8
			{
				target->FindAura(27216)->Refresh();
			}
			if (chance <= 40 && target->HasAura(47812)) // 9
			{
				target->FindAura(47812)->Refresh();
			}
			if (chance <= 40 && target->HasAura(47813)) // 10
			{
				target->FindAura(47813)->Refresh();
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(47203)) // rank 3
		{
			chance = rand() % 100;
			if (chance <= 60 && target->HasAura(172)) // 1
			{
				target->FindAura(172)->Refresh();
			}
			if (chance <= 60 && target->HasAura(6222)) // 2
			{
				target->FindAura(6222)->Refresh();
			}
			if (chance <= 60 && target->HasAura(6223)) // 3
			{
				target->FindAura(6223)->Refresh();
			}
			if (chance <= 60 && target->HasAura(7648)) // 4
			{
				target->FindAura(7648)->Refresh();
			}
			if (chance <= 60 && target->HasAura(11671)) // 5
			{
				target->FindAura(11671)->Refresh();
			}
			if (chance <= 60 && target->HasAura(11672)) // 6
			{
				target->FindAura(11672)->Refresh();
			}
			if (chance <= 60 && target->HasAura(25311)) // 7
			{
				target->FindAura(25311)->Refresh();
			}
			if (chance <= 60 && target->HasAura(27216)) // 8
			{
				target->FindAura(27216)->Refresh();
			}
			if (chance <= 60 && target->HasAura(47812)) // 9
			{
				target->FindAura(47812)->Refresh();
			}
			if (chance <= 60 && target->HasAura(47813)) // 10
			{
				target->FindAura(47813)->Refresh();
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(47204)) // rank 4
		{
			chance = rand() % 100;
			if (chance <= 80 && target->HasAura(172)) // 1
			{
				target->FindAura(172)->Refresh();
			}
			if (chance <= 80 && target->HasAura(6222)) // 2
			{
				target->FindAura(6222)->Refresh();
			}
			if (chance <= 80 && target->HasAura(6223)) // 3
			{
				target->FindAura(6223)->Refresh();
			}
			if (chance <= 80 && target->HasAura(7648)) // 4
			{
				target->FindAura(7648)->Refresh();
			}
			if (chance <= 80 && target->HasAura(11671)) // 5
			{
				target->FindAura(11671)->Refresh();
			}
			if (chance <= 80 && target->HasAura(11672)) // 6
			{
				target->FindAura(11672)->Refresh();
			}
			if (chance <= 80 && target->HasAura(25311)) // 7
			{
				target->FindAura(25311)->Refresh();
			}
			if (chance <= 80 && target->HasAura(27216)) // 8
			{
				target->FindAura(27216)->Refresh();
			}
			if (chance <= 80 && target->HasAura(47812)) // 9
			{
				target->FindAura(47812)->Refresh();
			}
			if (chance <= 80 && target->HasAura(47813)) // 10
			{
				target->FindAura(47813)->Refresh();
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(47205)) // rank 5
		{
			if (target->HasAura(172)) // 1
			{
				target->FindAura(172)->Refresh();
			}
			if (target->HasAura(6222)) // 2
			{
				target->FindAura(6222)->Refresh();
			}
			if (target->HasAura(6223)) // 3
			{
				target->FindAura(6223)->Refresh();
			}
			if (target->HasAura(7648)) // 4
			{
				target->FindAura(7648)->Refresh();
			}
			if (target->HasAura(11671)) // 5
			{
				target->FindAura(11671)->Refresh();
			}
			if (target->HasAura(11672)) // 6
			{
				target->FindAura(11672)->Refresh();
			}
			if (target->HasAura(25311)) // 7
			{
				target->FindAura(25311)->Refresh();
			}
			if (target->HasAura(27216)) // 8
			{
				target->FindAura(27216)->Refresh();
			}
			if (target->HasAura(47812)) // 9
			{
				target->FindAura(47812)->Refresh();
			}
			if (target->HasAura(47813)) // 10
			{
				target->FindAura(47813)->Refresh();
			}
		}
	}
protected:
	int32 chance;
};

void SpellFactoryMgr::SetupWarlock()
{
	AddSpellByNameHash(SPELL_HASH_INCINERATE, &DecimationSpell::Create);
	AddSpellByNameHash(SPELL_HASH_SHADOW_BOLT, &DecimationSpell::Create);
	AddSpellByNameHash(SPELL_HASH_SOUL_FIRE, &DecimationSpell::Create);
	// EverlastingAffliction Procs
	AddSpellByNameHash(SPELL_HASH_HAUNT, EverlastingAffliction::Create);
	AddSpellByNameHash(SPELL_HASH_DRAIN_LIFE, EverlastingAffliction::Create);
	AddSpellByNameHash(SPELL_HASH_DRAIN_SOUL, EverlastingAffliction::Create);
	AddSpellByNameHash(SPELL_HASH_SHADOW_BOLT, EverlastingAffliction::Create);
}