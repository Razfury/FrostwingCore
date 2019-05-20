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

// pPlayer->CalcDistance(pPlayer, pSinloren) < 15

#include "StdAfx.h"

class LightningOverloadProc : public Spell
{
	SPELL_FACTORY_FUNCTION(LightningOverloadProc);

	void DoAfterHandleEffect(Unit* target, uint32 i)
	{
		if (p_caster != NULL && target != NULL && p_caster->HasAura(30675)) // rank 1 (Lightning Overload) 11%
		{
			switch (GetProto()->Id)
			{
			case 10392: // Rank 8
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 49269, true);
				}
			break;
			case 15207: // Rank 9
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45293, true);
				}
				break;
			case 15208: // Rank 10
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45294, true);
				}
				break;
			case 25448: // Rank 11
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45295, true);
				}
				break;
			case 25449: // Rank 12
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45296, true);
				}
				break;
			case 49237: // Rank 13
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 49239, true);
				}
				break;
			case 49238: // Rank 14
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 49240, true);
				}
				break;
			case 421: // Rank 1(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45297, true);
				}
				break;
			case 930: // Rank 2(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45298, true);
				}
				break;
			case 2860: // Rank 3(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45299, true);
				}
				break;
			case 10605: // Rank 4(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45300, true);
				}
				break;
			case 25439: // Rank 5(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45301, true);
				}
				break;
			case 25442: // Rank 6(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 45302, true);
				}
				break;
			case 49270: // Rank 7(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 49268, true);
				}
				break;
			case 49271: // Rank 8(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 11)
				{
					p_caster->CastSpell(target, 49269, true);
				}
				break;
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(30678)) // rank 2 (Lightning Overload) 22%
		{
			switch (GetProto()->Id)
			{
			case 10392: // Rank 8
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 49269, true);
				}
				break;
			case 15207: // Rank 9
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45293, true);
				}
				break;
			case 15208: // Rank 10
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45294, true);
				}
				break;
			case 25448: // Rank 11
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45295, true);
				}
				break;
			case 25449: // Rank 12
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45296, true);
				}
				break;
			case 49237: // Rank 13
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 49239, true);
				}
				break;
			case 49238: // Rank 14
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 49240, true);
				}
				break;
			case 421: // Rank 1(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45297, true);
				}
				break;
			case 930: // Rank 2(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45298, true);
				}
				break;
			case 2860: // Rank 3(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45299, true);
				}
				break;
			case 10605: // Rank 4(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45300, true);
				}
				break;
			case 25439: // Rank 5(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45301, true);
				}
				break;
			case 25442: // Rank 6(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 45302, true);
				}
				break;
			case 49270: // Rank 7(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 49268, true);
				}
				break;
			case 49271: // Rank 8(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 22)
				{
					p_caster->CastSpell(target, 49269, true);
				}
				break;
			}
		}
		if (p_caster != NULL && target != NULL && p_caster->HasAura(30679)) // rank 3 (Lightning Overload) 33%
		{
			switch (GetProto()->Id)
			{
			case 10392: // Rank 8(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 49269, true);
				}
				break;
			case 15207: // Rank 9(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45293, true);
				}
				break;
			case 15208: // Rank 10(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45294, true);
				}
				break;
			case 25448: // Rank 11(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45295, true);
				}
				break;
			case 25449: // Rank 12(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45296, true);
				}
				break;
			case 49237: // Rank 13(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 49239, true);
				}
				break;
			case 49238: // Rank 14(Lightning-Bolt)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 49240, true);
				}
				break;
			case 421: // Rank 1(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45297, true);
				}
				break;
			case 930: // Rank 2(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45298, true);
				}
				break;
			case 2860: // Rank 3(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45299, true);
				}
				break;
			case 10605: // Rank 4(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45300, true);
				}
				break;
			case 25439: // Rank 5(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45301, true);
				}
				break;
			case 25442: // Rank 6(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 45302, true);
				}
				break;
			case 49270: // Rank 7(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 49268, true);
				}
				break;
			case 49271: // Rank 8(Chain-Lightning)
				chance = rand() % 100;
				if (chance <= 33)
				{
					p_caster->CastSpell(target, 49269, true);
				}
				break;
			}
		}
	}
protected:
	int32 chance;
};

class FireNova : public Spell
{
    SPELL_FACTORY_FUNCTION(FireNova);
    bool HasFireTotem = false;
    uint8 CanCast(bool tolerate)
    {
        uint8 result = Spell::CanCast(tolerate);

        if (result == SPELL_CANCAST_OK)
        {
            if (u_caster)
            {
                // If someone has a better solutionen, your welcome :-)
                int totem_ids[32] = {
                //Searing Totems
                2523, 3902, 3903, 3904, 7400, 7402, 15480, 31162, 31164, 31165,
                //Magma Totems
                8929, 7464, 7435, 7466, 15484, 31166, 31167,
                //Fire Elementel
                15439,
                //Flametongue Totems
                5950, 6012, 7423, 10557, 15485, 31132, 31158, 31133,
                //Frost Resistance Totems
                5926, 7412, 7413, 15486, 31171, 31172
                };
                Unit* totem;
                for (int i = 0; i < 32; i++)
                {
                    totem = u_caster->summonhandler.GetSummonWithEntry(totem_ids[i]);   // Get possible firetotem
                    if (totem != NULL)
                    {
                        HasFireTotem = true;
                        CastSpell(totem);
                    }
                }
                if (!HasFireTotem)
                {
                    SetExtraCastResult(SPELL_EXTRA_ERROR_MUST_HAVE_FIRE_TOTEM);
                    result = SPELL_FAILED_CUSTOM_ERROR;
                }
            }
        }
        return result;
    }

    void CastSpell(Unit* totem)
    {
        uint32 fireNovaSpells = Spell::GetProto()->Id;
        //Cast spell. NOTICE All ranks are linked with a extra spell in HackFixes.cpp
        totem->CastSpellAoF(totem->GetPositionX(), totem->GetPositionY(), totem->GetPositionZ(), dbcSpell.LookupEntryForced(fireNovaSpells), true);
    }
};

void SpellFactoryMgr::SetupShaman()
{
	//AddSpellById(45990, Quest_11715::Create);

    AddSpellById(1535, FireNova::Create);   //Rank 1
    AddSpellById(8498, FireNova::Create);   //Rank 2
    AddSpellById(8499, FireNova::Create);   //Rank 3
    AddSpellById(11314, FireNova::Create);  //Rank 4
    AddSpellById(11315, FireNova::Create);  //Rank 5
    AddSpellById(25546, FireNova::Create);  //Rank 6
    AddSpellById(25547, FireNova::Create);  //Rank 7
    AddSpellById(61649, FireNova::Create);  //Rank 8
    AddSpellById(61657, FireNova::Create);  //Rank 9
	// Lightning Bolt Proc for Talent: Lightning Overload
	AddSpellById(10392, LightningOverloadProc::Create);   //Rank 8 (Minimum Level For Talent Starts at Level 44)
	AddSpellById(15207, LightningOverloadProc::Create);   //Rank 9
	AddSpellById(15208, LightningOverloadProc::Create);   //Rank 10
	AddSpellById(25448, LightningOverloadProc::Create);   //Rank 11
	AddSpellById(25449, LightningOverloadProc::Create);   //Rank 12
	AddSpellById(49237, LightningOverloadProc::Create);   //Rank 13
	AddSpellById(49238, LightningOverloadProc::Create);   //Rank 14
	// Chain Lightning Proc for Talent: Lightning Overload
	AddSpellById(421, LightningOverloadProc::Create); // rank 1
	AddSpellById(930, LightningOverloadProc::Create); // rank 2
	AddSpellById(2860, LightningOverloadProc::Create); // rank 3
	AddSpellById(10605, LightningOverloadProc::Create); // rank 4
	AddSpellById(25439, LightningOverloadProc::Create); // rank 5
	AddSpellById(25442, LightningOverloadProc::Create); // rank 6
	AddSpellById(49270, LightningOverloadProc::Create); // rank 7
	AddSpellById(49271, LightningOverloadProc::Create); // rank 8
}
