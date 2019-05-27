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

void WanderingPlague(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
    Player* player = sp->p_caster;

    if (spellId == 50526)
    {
        if (SpellEffectType == SPELL_EFFECT_DUMMY)
        {
            Unit* unitTarget = sp->GetUnitTarget();
            if (sp->u_caster != NULL && unitTarget != NULL)
            {
                if (unitTarget->isTrainingDummy() == false)
                {
                    // Move Effect
                    unitTarget->CastSpellAoF(sp->u_caster->GetPositionX(), sp->u_caster->GetPositionY(), sp->u_caster->GetPositionZ(), dbcSpell.LookupEntryForced(49575), true);
                    sp->u_caster->CastSpell(unitTarget, 51399, true); // Taunt Effect
                }
            }
        }
    }
};


#define BLOOD_PLAGUE 55078
#define FROST_FEVER 55095

bool Pestilence(uint32 i, Spell* pSpell)
{
    if(i == 1) // Script Effect that has been identified to handle the spread of diseases.
    {
        if(!pSpell->u_caster || !pSpell->u_caster->GetTargetGUID() || !pSpell->u_caster->IsInWorld())
            return true;

        Unit* u_caster = pSpell->u_caster;
        Unit* Main = u_caster->GetMapMgr()->GetUnit(u_caster->GetTargetGUID());
        if(Main == NULL)
            return true;
        bool blood = Main->HasAura(BLOOD_PLAGUE);
        bool frost = Main->HasAura(FROST_FEVER);
        int inc = (u_caster->HasAura(59309) ? 10 : 5);
        for(Object::InRangeSet::iterator itr = u_caster->GetInRangeSetBegin(); itr != u_caster->GetInRangeSetEnd(); ++itr)
        {
            if(!(*itr)->IsUnit())
                continue;
            Unit* Target = TO< Unit* >((*itr));
            if(Main->GetGUID() == Target->GetGUID() && !u_caster->HasAura(63334))
                continue;
            if(isAttackable(Target, u_caster) && u_caster->CalcDistance((*itr)) <= (pSpell->GetRadius(i) + inc))
            {
                if(blood)
                    u_caster->CastSpell(Target, BLOOD_PLAGUE, true);
                if(frost)
                    u_caster->CastSpell(Target, FROST_FEVER, true);
            }
        }
        return true;
    }
    return true;
}

bool DeathStrike(uint32 i, Spell* pSpell)
{
    if(pSpell->p_caster == NULL || pSpell->GetUnitTarget() == NULL)
        return true;

    Unit* Target = pSpell->GetUnitTarget();

    // Get count of diseases on target which were casted by caster
    uint32 count = Target->GetAuraCountWithDispelType(DISPEL_DISEASE, pSpell->p_caster->GetGUID());

    // Not a logical error, Death Strike should heal only when diseases are presented on its target
    if(count)
    {
        // Calculate heal amount:
        // A deadly attack that deals $s2% weapon damage plus ${$m1*$m2/100}
        // and heals the Death Knight for $F% of $Ghis:her; maximum health for each of $Ghis:her; diseases on the target.
        // $F is dmg_multiplier.
        float amt = static_cast< float >(pSpell->p_caster->GetMaxHealth()) * pSpell->GetProto()->dmg_multiplier[0] / 100.0f;

        // Calculate heal amount with diseases on target
        uint32 val = static_cast< uint32 >(amt * count);

        Aura* aur = pSpell->p_caster->FindAuraByNameHash(SPELL_HASH_IMPROVED_DEATH_STRIKE);
        if(aur != NULL)
            val += val * (aur->GetSpellProto()->EffectBasePoints[2] + 1) / 100;

        if(val > 0)
            pSpell->u_caster->Heal(pSpell->u_caster, pSpell->GetProto()->Id, val);
    }

    return true;
}

bool Strangulate(uint32 i, Aura* pAura, bool apply)
{
    if(!apply)
        return true;

    if(!pAura->GetTarget()->IsPlayer())
        return true;

    Unit* unitTarget = pAura->GetTarget();

    if(unitTarget->IsCasting())
        unitTarget->InterruptSpell();

    return true;
}

void DeathGrip(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	Player* player = sp->p_caster;

	if (spellId == 49576)
	{
		if (SpellEffectType == SPELL_EFFECT_DUMMY)
		{
			Unit* unitTarget = sp->GetUnitTarget();
			if (sp->u_caster != NULL && unitTarget != NULL)
			{
				if (unitTarget->isTrainingDummy() == false)
				{
					// Move Effect
					unitTarget->CastSpellAoF(sp->u_caster->GetPositionX(), sp->u_caster->GetPositionY(), sp->u_caster->GetPositionZ(), dbcSpell.LookupEntryForced(49575), true);
					sp->u_caster->CastSpell(unitTarget, 51399, true); // Taunt Effect
				}
			}
		}
	}
};

bool DeathCoil(uint32 i, Spell* s)
{
    Unit* unitTarget = s->GetUnitTarget();

    if(s->p_caster == NULL || unitTarget == NULL)
        return false;

	int32 dmg = s->damage + (s->p_caster->GetAP()*0.30);

    if(isAttackable(s->p_caster, unitTarget, false))
    {
        s->p_caster->CastSpell(unitTarget, 47632, dmg, true);
    }
    else if(unitTarget->IsPlayer() && unitTarget->getRace() == RACE_UNDEAD)
    {
        dmg *= 1.5;
        s->p_caster->CastSpell(unitTarget, 47633, dmg, true);
    }

	if (s->p_caster->HasAura(49194)) // Has "Unholy Blight" talent
	{
		s->p_caster->CastSpell(unitTarget, 50536, true);
	}

    return true;
}

bool HungeringCold(uint32 i, Spell* pSpell)
{
	if (pSpell->u_caster != NULL)
	{
		Spell* newSpell = new Spell(pSpell->u_caster, dbcSpell.LookupEntry(61058), true, NULL);
		SpellCastTargets targets;
		targets.m_destX = pSpell->u_caster->GetPositionX();
		targets.m_destY = pSpell->u_caster->GetPositionY();
		targets.m_destZ = pSpell->u_caster->GetPositionZ();
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		newSpell->prepare(&targets);
	}
	return true;
}

bool WanderingPlague(uint32 i, Spell* pSpell)
{
	if (pSpell->u_caster != NULL)
	{
		Spell* newSpell = new Spell(pSpell->u_caster, dbcSpell.LookupEntry(50526), true, NULL);
		SpellCastTargets targets;
		targets.m_destX = pSpell->u_caster->GetPositionX();
		targets.m_destY = pSpell->u_caster->GetPositionY();
		targets.m_destZ = pSpell->u_caster->GetPositionZ();
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		newSpell->prepare(&targets);
	}
	return true;
}

bool BladedArmor(uint32 i, Spell* s)
{
    /********************************************************************************************************
    /* SPELL_EFFECT_DUMMY is used in this spell, in DBC, only to store data for in-game tooltip output.
    /* Description: Increases your attack power by $s2 for every ${$m1*$m2} armor value you have.
    /* Where $s2 is base points of Effect 1 and $m1*$m2 I guess it's a mod.
    /* So for example spell id 49393: Increases your attack power by 5 for every 180 armor value you have.
    /* Effect 0: Base Points/mod->m_amount = 36; Effect 1: Base Points = 5;
    /* $s2 = 5 and ${$m1*$m2} = 36*5 = 180.
    /* Calculations are already done by Blizzard and set into BasePoints field,
    /* and passed to SpellAuraModAttackPowerOfArmor, so there is no need to do handle this here.
    /* Either way Blizzard has some weird Chinese developers or they are smoking some really good stuff.
    ********************************************************************************************************/
    return true;
}

bool DeathAndDecay(uint32 i, Aura* pAura, bool apply)
{
    if(apply)
    {
        Player* caster = pAura->GetPlayerCaster();
        if(caster == NULL)
            return true;

        int32 value = pAura->GetModAmount(i) + (int32) caster->GetAP() * 0.064;

        caster->CastSpell(pAura->GetTarget(), 52212, value, true);
    }

    return true;
}

bool Butchery(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if(apply)
        target->AddProcTriggerSpell(50163, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_GAIN_EXPIERIENCE | PROC_TARGET_SELF, 0, NULL, NULL);
    else
        target->RemoveProcTriggerSpell(50163, pAura->m_casterGuid);

    return true;
}

bool DeathRuneMastery(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if(apply)
    {
        static uint32 classMask[3] = { 0x10, 0x20000, 0 };
        target->AddProcTriggerSpell(50806, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, PROC_ON_CAST_SPELL | PROC_TARGET_SELF, 0, NULL, classMask);
    }
    else
        target->RemoveProcTriggerSpell(50806, pAura->m_casterGuid);

    return true;
}

bool MarkOfBlood(uint32 i, Aura* pAura, bool apply)
{
    Unit* target = pAura->GetTarget();

    if(apply)
        target->AddProcTriggerSpell(61607, pAura->GetSpellId(), pAura->m_casterGuid, pAura->GetSpellProto()->procChance, pAura->GetSpellProto()->procFlags, pAura->GetSpellProto()->procCharges, NULL, NULL);
    else if(target->GetAuraStackCount(49005) <= 1)
        target->RemoveProcTriggerSpell(61607, pAura->m_casterGuid);

    return true;
}

bool Hysteria(uint32 i, Aura* pAura, bool apply)
{
    if(! apply)
        return true;

    Unit* target = pAura->GetTarget();

    uint32 dmg = (uint32) target->GetMaxHealth() * (pAura->GetSpellProto()->EffectBasePoints[i] + 1) / 100;
    target->DealDamage(target, dmg, 0, 0, 0);

    return true;
}

bool WillOfTheNecropolis(uint32 i, Spell* spell)
{
    if(i != 0)
        return true;

    Player* plr = spell->p_caster;

    if(plr == NULL)
        return true;

    switch(spell->GetProto()->Id)
    {
        case 49189:
            plr->removeSpell(52285, false, false, 0);
            plr->removeSpell(52286, false, false, 0);
            break;

        case 50149:
            plr->removeSpell(52284, false, false, 0);
            plr->removeSpell(52286, false, false, 0);
            break;

        case 50150:
            plr->removeSpell(52284, false, false, 0);
            plr->removeSpell(52285, false, false, 0);
            break;
    }

    return true;
}

bool RuneOfCinderglacier(uint32 i, Spell* pSpell)

{

	Player* pPlayer = pSpell->p_caster;

	if (!pPlayer)

		return true;

	pPlayer->Insert_DB_NewRune(pPlayer->GetGUID(), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	return true;

}

bool RuneOfFallenCrusader(uint32 i, Spell* pSpell)

{

	Player* pPlayer = pSpell->p_caster;

	if (!pPlayer)

		return true;

	pPlayer->Insert_DB_NewRune(pPlayer->GetGUID(), 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);

	return true;

}

int32 IcyTouch(Spell* pSpell, uint32 spellId, uint32 i, Unit* target, int32 value)
{
	if (spellId == 45477 || spellId == 49896 || spellId == 49903 || spellId == 49904)
	{
		if (pSpell->p_caster != NULL && i == 0) // DOES NOT HAVE KILLIN MACHINE
			value += (uint32)(pSpell->p_caster->GetAP() * 0.1); //0.1 5 is for testing

		return value;
	}
};

/*bool CryptFeverSpell(uint32 i, Spell* pSpell)
{
	Unit* unitTarget = pSpell->GetUnitTarget();
	Player* pPlayer = pSpell->p_caster;

	if (!pPlayer)

		return true;

	if (pPlayer->HasAura(51099) || pPlayer->HasAura(51160) || pPlayer->HasAura(51161))
		return true;
	else
		for (set<uint32>::iterator itr = pPlayer->mSpells.begin(); itr != pPlayer->mSpells.end(); ++itr)

		{

			if (*itr == 49032)

				pPlayer->CastSpell(unitTarget, 50508, true);



			if (*itr == 49631)

				pPlayer->CastSpell(unitTarget, 50509, true);


			if (*itr == 49632)

				pPlayer->CastSpell(unitTarget, 50510, true);

		}

	return true;

}*/

void SetupDeathKnightSpells(ScriptMgr* mgr)
{
	mgr->RegisterSpellType(SPELL_EVENT_CALCULATE_EFFECT, (int32*)&IcyTouch);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&DeathGrip);
    mgr->register_dummy_spell(50842, &Pestilence);
    uint32 DeathStrikeIds[] =
    {
        49998, // Rank 1
        49999, // Rank 2
        45463, // Rank 3
        49923, // Rank 4
        49924, // Rank 5
        0,
    };
    mgr->register_dummy_spell(DeathStrikeIds, &DeathStrike);

	/*uint32 PlagueStrikeIds[] =
	{
		45462, // Rank 1
		49917, // Rank 2
		49918, // Rank 3
		49919, // Rank 4
		49920, // Rank 5
		49921, // Rank 6
		0,
	};
	uint32 IcyTouchIds[] =
	{
		45477, // Rank 1
		49896, // Rank 2
		49903, // Rank 3
		49904, // Rank 4
		49909, // Rank 5
		0,
	};
	mgr->register_dummy_spell(PlagueStrikeIds, &CryptFeverSpell);
	mgr->register_dummy_spell(IcyTouchIds, &CryptFeverSpell);*/

    mgr->register_dummy_aura(47476, &Strangulate);
    mgr->register_dummy_aura(49913, &Strangulate);
    mgr->register_dummy_aura(49914, &Strangulate);
    mgr->register_dummy_aura(49915, &Strangulate);
    mgr->register_dummy_aura(49916, &Strangulate);

	mgr->register_dummy_spell(53341, &RuneOfCinderglacier);
	mgr->register_dummy_spell(53344, &RuneOfFallenCrusader);
	mgr->register_dummy_spell(49203, &HungeringCold);
	mgr->register_dummy_spell(50526, &WanderingPlague);

    mgr->register_dummy_spell(47541, &DeathCoil);   // Rank 1
    mgr->register_dummy_spell(49892, &DeathCoil);   // Rank 2
    mgr->register_dummy_spell(49893, &DeathCoil);   // Rank 3
    mgr->register_dummy_spell(49894, &DeathCoil);   // Rank 4
    mgr->register_dummy_spell(49895, &DeathCoil);   // Rank 5

    uint32 bladedarmorids[] =
    {
        48978,
        49390,
        49391,
        49392,
        49393,
        0
    };
    mgr->register_dummy_spell(bladedarmorids, &BladedArmor);

    mgr->register_dummy_aura(43265, &DeathAndDecay);
    mgr->register_dummy_aura(49936, &DeathAndDecay);
    mgr->register_dummy_aura(49937, &DeathAndDecay);
    mgr->register_dummy_aura(49938, &DeathAndDecay);

    mgr->register_dummy_aura(48979, &Butchery);   // Rank 1
    mgr->register_dummy_aura(49483, &Butchery);   // Rank 2

    mgr->register_dummy_aura(49467, &DeathRuneMastery);   // Rank 1
    mgr->register_dummy_aura(50033, &DeathRuneMastery);   // Rank 2
    mgr->register_dummy_aura(50034, &DeathRuneMastery);   // Rank 3

    mgr->register_dummy_aura(49005 , &MarkOfBlood);

    mgr->register_dummy_aura(49016 , &Hysteria);

    mgr->register_dummy_spell(49189, &WillOfTheNecropolis);   // Rank 1
    mgr->register_dummy_spell(50149, &WillOfTheNecropolis);   // Rank 2
    mgr->register_dummy_spell(50150, &WillOfTheNecropolis);   // Rank 3
}