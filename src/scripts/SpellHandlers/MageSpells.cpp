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

void MissleBarrage_OnAfterSpellCast(Player* pPlayer, SpellEntry* pSpell, Spell* spell)
{
	if (pSpell->NameHash == SPELL_HASH_ARCANE_MISSILES)
	{
		if (pPlayer)
		{
			pPlayer->RemoveAura(44401); // Remove "Missile Barrage"
			pPlayer->RemoveAura(36032); // Remove "Arcane Blast" damage increase if any "until any arcane damage spell except arcane blast is casted"
		}
	}
}

// SpellId: 42897 "lasts 6sec or until any Arcane damage spell except Arcane Blast is cast."
void ArcaneBlastDamageIncrease_OnAfterSpellCast(Player* pPlayer, SpellEntry* pSpell, Spell* spell)
{
	if (!spell->u_caster->IsPlayer()) // If caster is not a player return
		return;

	if (pSpell->NameHash == SPELL_HASH_ARCANE_MISSILES || pSpell->NameHash == SPELL_HASH_ARCANE_EXPLOSION || pSpell->NameHash == SPELL_HASH_ARCANE_BARRAGE)
	{
		if (pPlayer)
		{
			pPlayer->RemoveAura(36032); // Remove "Arcane Blast" damage increase if any "until any arcane damage spell except arcane blast is casted"
		}
	}
}

//Invisbility
void Invisibility_OnSpellCast(Player* pPlayer, SpellEntry* pSpell, Spell* spell)
{
	if (pSpell->Id == 66)
	{
		if (pPlayer)
		{
			if (pPlayer->HasAura(54354)) // prismatic cloak rank 3
			{
				pPlayer->CastSpell(spell->p_caster, 32612, true);
			}
			pPlayer->InterruptSpell(); //Override broken shit.. Do not remove :{
		}
		
	}
}

void Invisibility_OnAuraRemove(Aura* aur)
{
	if (aur->GetSpellId() == 66)
	{
		if (aur->GetPlayerCaster())
		{
			aur->GetPlayerCaster()->CastSpell(aur->GetPlayerCaster(), 32612, true);
		}
	}
}

bool Cold_Snap(uint32 i, Spell* pSpell)
{
    if(!pSpell->p_caster) return true;
    pSpell->p_caster->ClearCooldownsOnLine(6, pSpell->GetProto()->Id);
    return true;
}

bool Living_Bomb(uint32 i, Aura* pAura, bool apply)
{
    Unit* caster = pAura->GetUnitCaster();
    if(caster && !apply)
        caster->CastSpell(pAura->GetTarget(), pAura->GetSpellProto()->EffectBasePoints[i] + 1, true);
    return true;
}

bool HotStreak(uint32 i, Aura* pAura, bool apply)
{
    if(i == 0)
    {
        auto caster = pAura->GetUnitCaster();
        if (caster == nullptr)
            return true;

        if(apply)
        {
            static uint32 classMask[3] = { 0x13, 0x21000, 0 };
            caster->AddProcTriggerSpell(48108, pAura->GetSpellProto()->Id, caster->GetGUID(), pAura->GetSpellProto()->EffectBasePoints[i] + 1, PROC_ON_SPELL_CRIT_HIT | PROC_ON_SPELL_HIT, 0, pAura->GetSpellProto()->EffectSpellClassMask[i], classMask);
        }
        else
            caster->RemoveProcTriggerSpell(48108);
    }

    return true;
}

bool SummonWaterElemental(uint32 i, Spell* pSpell)
{
    Unit* caster = pSpell->u_caster;
    if(caster == NULL)
        return true;

    if(caster->HasAura(70937))    // Glyph of Eternal Water
        caster->CastSpell(caster, 70908, true);
    else
        caster->CastSpell(caster, 70907, true);

    return true;
}

bool FingersOfFrost(uint32 i, Aura* a, bool apply) // Should be visible to client by using ID 74396
{
    Player* caster = a->GetPlayerCaster();

    if(caster == NULL)
        return true;

    if(apply)
        caster->SetTriggerChill(44544, a->GetModAmount(i), false);
    else
        caster->SetTriggerChill(0, 0, false);

    return true;
}

bool BrainFreeze(uint32 i, Aura* a, bool apply)
{
    Player* caster = a->GetPlayerCaster();

    if(caster == NULL)
        return true;

    if(apply)
        caster->SetTriggerChill(57761, a->GetModAmount(i), false);
    else
        caster->SetTriggerChill(0, 0, false);

    return true;
}

bool MagicAbsorbtion(uint32 i, Aura* a, bool apply)
{
    Unit* m_target = a->GetTarget();

    if(m_target->IsPlayer())
    {
        Player* p_target = TO_PLAYER(m_target);

        if(apply)
            p_target->m_RegenManaOnSpellResist += (a->GetModAmount(i) / 100);
        else
            p_target->m_RegenManaOnSpellResist -= (a->GetModAmount(i) / 100);
    }

    return true;
}

bool MirrorImage(uint32 i, Aura* pAura, bool apply)
{
    Unit* caster = pAura->GetUnitCaster();
    if(caster != NULL && apply && i == 2)
        if(caster->GetGUID() == pAura->GetTarget()->GetCreatedByGUID())
            caster->CastSpell(pAura->GetTarget(), pAura->GetSpellProto()->EffectTriggerSpell[i], true);

    return true;
}

void SetupMageSpells(ScriptMgr* mgr)
{
	mgr->RegisterSpellType(SPELL_EVENT_AFTER_CAST_SPELL, (void*)&MissleBarrage_OnAfterSpellCast);
	mgr->RegisterSpellType(SPELL_EVENT_AFTER_CAST_SPELL, (void*)&ArcaneBlastDamageIncrease_OnAfterSpellCast);
    mgr->register_dummy_spell(11958, &Cold_Snap);
    mgr->register_dummy_aura(44457, &Living_Bomb);
    mgr->register_dummy_aura(55359, &Living_Bomb);
    mgr->register_dummy_aura(55360, &Living_Bomb);

    uint32 HotStreakIds[] = { 44445, 44446, 44448, 0 };
    mgr->register_dummy_aura(HotStreakIds, &HotStreak);

    mgr->register_dummy_spell(31687, &SummonWaterElemental);

    uint32 fingersoffrostids[] =
    {
        44543,
        44545,
        0
    };
    mgr->register_dummy_aura(fingersoffrostids, &FingersOfFrost);

    uint32 brainfreezeids[] =
    {
        44546,
        44548,
        44549,
        0
    };
    mgr->register_dummy_aura(brainfreezeids, &BrainFreeze);

    uint32 magicabsorbtionids[] =
    {
        29441,
        29444,
        0
    };
    mgr->register_dummy_aura(magicabsorbtionids, &MagicAbsorbtion);

    mgr->register_dummy_aura(55342, &MirrorImage);

	mgr->RegisterSpellType(SPELL_EVENT_CAST_SPELL, (void*)&Invisibility_OnSpellCast);
	mgr->RegisterSpellType(SPELL_EVENT_AURA_REMOVE, (void*)&Invisibility_OnAuraRemove);
}
