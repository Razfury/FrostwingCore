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

void DeepWounds(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (!sp->p_caster)
		return;

	Player* player = sp->p_caster;

	if (SpellEffectType == SPELL_EFFECT_DUMMY)
	{
		if (spellId == 12868 || spellId == 12850 || spellId == 12162)
		{
			player->CastSpell(sp->GetUnitTarget(), 12721, true);
		}
	}
};

void Execute(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (sp->p_caster == NULL || sp->GetUnitTarget() == NULL)
		return;

	Player* Caster = sp->p_caster;
	Unit* Target = sp->GetUnitTarget();

	uint32 rage = Caster->GetPower(POWER_TYPE_RAGE);

	if (spellId == 5308 || spellId == 20658 || spellId == 20660 || spellId == 20661 || spellId == 20662 || spellId == 25234 || spellId == 25236 || spellId == 47470 || spellId == 47471)
	{
		if (SpellEffectType == SPELL_EFFECT_DUMMY)
		{
			if (Caster->HasAura(58367))     // Glyph of Execution: Your Execute ability deals damage as if you had 10 additional rage.
			{
				rage += 10;
			}

			uint32 toadd = 0;
			int32 dmg = 0;
			uint32 multiple[] = { 0, 3, 6, 9, 12, 15, 18, 21, 30, 38, };

			if (rage >= 30)
			{
				toadd = (multiple[sp->GetProto()->RankNumber] * 30);
			}
			else
			{
				toadd = (multiple[sp->GetProto()->RankNumber] * rage);
			}

			dmg = sp->CalculateEffect(SPELL_EFFECT_DUMMY, sp->GetUnitTarget());
			dmg += Caster->GetAttackPower() / 5;
			dmg += toadd;

			Caster->Strike(Target, 0, sp->GetProto(), 0, 0, dmg, false, false);
		}
	}
};

void OnSpellCastExecute(Player* pPlayer, SpellEntry* pSpell, Spell* spell)
{
	if (pSpell->Id == 5308 || pSpell->Id == 20658 || pSpell->Id == 20660 || pSpell->Id == 20661 || pSpell->Id == 20662 || pSpell->Id == 25234 || pSpell->Id == 25236 || pSpell->Id == 47470 || pSpell->Id == 47471)
	{
		if (pPlayer->HasAura(52437)) // Remove Sudden Death aura on spellcast.
		{
			pPlayer->RemoveAura(52437);
		}
	}
};

void Vigilance(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	if (!sp->p_caster)
		return;

	if (spellId == 50725)
	{
		if (SpellEffectType == SPELL_EFFECT_SCRIPT_EFFECT)
		{
			sp->p_caster->ClearCooldownForSpell(355);   // Taunt
		}
	}
};

void DamageShield(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Unit* target = aur->GetTarget();
	if (spellId == 58872 || spellId == 58874)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (apply)
			{
				target->AddProcTriggerSpell(59653, aur->GetSpellId(), aur->m_casterGuid, aur->GetSpellProto()->procChance, PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_BLOCK_VICTIM, 0, NULL, NULL);
			}
			else
			{
				target->RemoveProcTriggerSpell(59653, aur->m_casterGuid);
			}
		}
	}
};

void HeroicFury(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	Player* p_caster = sp->p_caster;

	if (!p_caster)
	{
		return;
	}

	if (spellId == 60970)
	{
		if (SpellEffectType == SPELL_EFFECT_DUMMY)
		{
			if (p_caster->HasSpell(20252))
			{
				p_caster->ClearCooldownForSpell(20252);
			}

			for (uint32 x = MAX_NEGATIVE_AURAS_EXTEDED_START; x < MAX_NEGATIVE_AURAS_EXTEDED_END; ++x)
			{
				if (p_caster->m_auras[x])
				{
					for (uint32 y = 0; y < 3; ++y)
					{
						switch (p_caster->m_auras[x]->GetSpellProto()->EffectApplyAuraName[y])
						{
						case SPELL_AURA_MOD_ROOT:
						case SPELL_AURA_MOD_DECREASE_SPEED:
							p_caster->m_auras[x]->Remove();
							goto out;
							break;
						}

						continue;
					out:
						break;
					}
				}
			}
		}
	}
};

void LastStand(Spell* sp, Unit* spellcaster, uint32 spellId, uint32 SpellEffectType)
{
	Player* playerTarget = sp->GetPlayerTarget();

	if (!playerTarget)
	{
		return;
	}

	if (spellId == 12975)
	{
		if (SpellEffectType == SPELL_EFFECT_DUMMY)
		{
			SpellCastTargets tgt;
			tgt.m_unitTarget = playerTarget->GetGUID();

			SpellEntry* inf = dbcSpell.LookupEntry(12976);
			Spell* spe = sSpellFactoryMgr.NewSpell(sp->u_caster, inf, true, NULL);
			spe->prepare(&tgt);
		}
	}
};

void BerserkerRage(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Unit* u = aur->GetTarget();
	Player* p_target = NULL;

	if (spellId == 18499)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (u->IsPlayer())
			{
				p_target = TO_PLAYER(u);
			}

			if (p_target == NULL)
			{
				return;
			}

			if (apply)
			{
				p_target->rageFromDamageTaken += 100;
			}
			else
			{
				p_target->rageFromDamageTaken -= 100;
			}

			for (int32 i = 0; i < 3; i++)
			{
				if (apply)
				{
					p_target->MechanicsDispels[aur->GetSpellProto()->EffectMiscValue[i]]++;
					p_target->RemoveAllAurasByMechanic(aur->GetSpellProto()->EffectMiscValue[i], static_cast<uint32>(-1), false);
				}
				else
				{
					p_target->MechanicsDispels[aur->GetSpellProto()->EffectMiscValue[i]]--;
				}
			}
		}
	}
};

void SweepingStrikes(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Unit* m_target = aur->GetTarget();
	if (spellId == 12328)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (apply)
			{
				m_target->AddExtraStrikeTarget(aur->GetSpellProto(), 10);
			}
			else
			{
				m_target->RemoveExtraStrikeTarget(aur->GetSpellProto());
			}
		}
	}
};

void TacticalAndStanceMastery(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Unit* u_target = aur->GetTarget();
	Modifier* mod;

	if (!u_target->IsPlayer())
	{
		return;
	}

	Player* p_target = TO_PLAYER(u_target);

	if (p_target == NULL)
	{
		return;
	}

	if (spellId == 12295 || spellId == 12676 || spellId == 12677 || spellId == 12678)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (apply)
			{
				p_target->m_retainedrage += (aur->GetModAmount(mod->i) * 10);     //don't really know if value is all value or needs to be multiplied with 10
			}
			else
			{
				p_target->m_retainedrage -= (aur->GetModAmount(mod->i) * 10);
			}
		}
	}
};

void SecondWind(Aura* aur, uint32 spellId, bool apply, uint32 SpellAuraType)
{
	Player* caster = aur->GetPlayerCaster();

	if (caster == NULL)
	{
		return;
	}

	if (spellId == 29834)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (apply)
			{
				caster->SetTriggerStunOrImmobilize(29841, 100, true);  //fixed 100% chance
			}
			else
			{
				caster->SetTriggerStunOrImmobilize(0, 0, true);
			}
		}
	}

	if (spellId == 29838)
	{
		if (SpellAuraType == SPELL_AURA_DUMMY)
		{
			if (apply)
			{
				caster->SetTriggerStunOrImmobilize(29842, 100, true);  //fixed 100% chance
			}
			else
			{
				caster->SetTriggerStunOrImmobilize(0, 0, true);
			}
		}
	}
};

void SetupWarriorSpells(ScriptMgr* mgr)
{
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&DeepWounds);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&Execute);
	mgr->RegisterSpellType(SPELL_EVENT_CAST_SPELL, (void*)&OnSpellCastExecute);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&Vigilance);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&HeroicFury);
	mgr->RegisterSpellType(SPELL_EVENT_EFFECT, (void*)&LastStand);
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&BerserkerRage);
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&DamageShield);
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&SweepingStrikes);
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&TacticalAndStanceMastery);
	mgr->RegisterSpellType(SPELL_EVENT_AURA, (void*)&SecondWind);
}
