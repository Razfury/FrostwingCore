/*
* Copyright (C) 2015-2019 FrostwingCore Team <http://www.ArcEmu.org/>
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

#include "StdAfx.h"

void InitalizeSpellUpdater()
{
    Log.Success("World", "SpellUpdater is Processing %u spells...", dbcSpell.GetNumRows());

    //checking if the DBCs have been extracted from an english client, based on namehash of spell 4, the first with a different name in non-english DBCs
    SpellEntry* sp = dbcSpell.LookupEntry(4);
    if (crc32((const unsigned char*)sp->Name, (unsigned int)strlen(sp->Name)) != SPELL_HASH_WORD_OF_RECALL_OTHER)
    {
        Log.LargeErrorMessage("You are using DBCs extracted from an unsupported client.", "FrostwingCore supports only enUS and enGB!!!", NULL);
        abort();
    }

    uint32 cnt = dbcSpell.GetNumRows();
    uint32 effect;

    map<uint32, uint32> talentSpells;
    map<uint32, uint32>::iterator talentSpellIterator;
    uint32 i, j;
    for (i = 0; i < dbcTalent.GetNumRows(); ++i)
    {
        TalentEntry* tal = dbcTalent.LookupRow(i);
        for (j = 0; j < 5; ++j)
            if (tal->RankID[j] != 0)
                talentSpells.insert(make_pair(tal->RankID[j], tal->TalentTree));
    }

    for (uint32 x = 0; x < cnt; x++)
    {
        sp = dbcSpell.LookupRow(x);


        // hash the name
        //!!!!!!! representing all strings on 32 bits is dangerous. There is a chance to get same hash for a lot of strings ;)
        uint32 namehash = 0;
        namehash = crc32((const unsigned char*)sp->Name, (unsigned int)strlen(sp->Name));
        sp->NameHash = namehash; //need these set before we start processing spells

        // find diminishing status
        sp->DiminishStatus = GetDiminishingGroup(namehash);

        /*float radius = std::max(::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[0])), ::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[1])));
        radius = std::max(::GetRadius(dbcSpellRadius.LookupEntry(sp->EffectRadiusIndex[2])), radius);
        radius = std::max(GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex)), radius);
        sp->base_range_or_radius_sqr = radius * radius;
        sp->ai_target_type = GetAiTargetType(sp);*/

        // Save School as SchoolMask, and set School as an index
        /* (bitwise)
        SCHOOL_NORMAL = 1,
        SCHOOL_HOLY   = 2,
        SCHOOL_FIRE   = 4,
        SCHOOL_NATURE = 8,
        SCHOOL_FROST  = 16,
        SCHOOL_SHADOW = 32,
        SCHOOL_ARCANE = 64
        */
        sp->SchoolMask = sp->School;
        for (i = 0; i < SCHOOL_COUNT; i++)
        {
            if (sp->School & (1 << i))
            {
                sp->School = i;
                break;
            }
        }

        ARCEMU_ASSERT(sp->School < SCHOOL_COUNT);

        // correct caster/target aura states
        if (sp->CasterAuraState > 1)

            sp->CasterAuraState = 1 << (sp->CasterAuraState - 1);

        if (sp->TargetAuraState > 1)

            sp->TargetAuraState = 1 << (sp->TargetAuraState - 1);

        for (uint32 b = 0; b < 3; ++b)
        {
            if (sp->Attributes & ATTRIBUTES_ONLY_OUTDOORS && sp->EffectApplyAuraName[b] == SPELL_AURA_MOUNTED)
            {
                sp->Attributes &= ~ATTRIBUTES_ONLY_OUTDOORS;
            }

            if (sp->EffectApplyAuraName[b] == SPELL_AURA_PREVENT_RESURRECTION)
            {
                sp->Attributes |= ATTRIBUTES_NEGATIVE;
                sp->AttributesExC |= CAN_PERSIST_AND_CASTED_WHILE_DEAD;
            }
        }

        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            switch (sp->Effect[j])
            {
            case SPELL_EFFECT_CHARGE:
            case SPELL_EFFECT_CHARGE_DEST:
            case SPELL_EFFECT_LEAP_41:
            case SPELL_EFFECT_JUMP_DEST:
            case SPELL_EFFECT_LEAP_BACK:
                if (!sp->speed && !sp->SpellFamilyName)
                    sp->speed = 42.0f;
                break;
            }

            // Xinef: i hope this will fix the problem with not working resurrection
            if (sp->Effect[j] == SPELL_EFFECT_SELF_RESURRECT)
                sp->EffectImplicitTargetA[j] = 1;
        }

        // Xinef: Fix range for trajectories and triggered spells
        for (uint8 j = 0; j < 3; ++j)
            if (sp->rangeIndex == 1 && (sp->EffectImplicitTargetA[j] == 89 || sp->EffectImplicitTargetB[j] == 89))
                if (SpellEntry* sp2 = (SpellEntry*)dbcSpell.LookupEntry(sp->EffectTriggerSpell[j]))
                    sp2->rangeIndex = 187; // 300yd

        if (sp->activeIconID == 2158)  // flight
            sp->Attributes |= ATTRIBUTES_PASSIVE;

        //Cronic: Load AP mods and overrides from DB
        QueryResult* resultaax = WorldDatabase.Query("SELECT * FROM spell_ap_override");
        if (resultaax != NULL)
        {
            do
            {
                Field* f;
                f = resultaax->Fetch();
                sp = dbcSpell.LookupEntryForced(f[0].GetUInt32());
                if (sp != NULL)
                {
                    sp->ap_override = f[1].GetFloat();
                    sp->ap_dot_override = f[2].GetFloat();
                }
                else
                    Log.Error("SpellAPOverride", "Has nonexistent spell %u.", f[0].GetUInt32());
            } while (resultaax->NextRow());
            delete resultaax;
        }

        // Set default mechanics if we don't already have one
        if (!sp->MechanicsType)
        {
            //Set Silencing spells mechanic.
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_SILENCE ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_SILENCE ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_SILENCE)
                sp->MechanicsType = MECHANIC_SILENCED;

            //Set Stunning spells mechanic.
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_STUN ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_STUN ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_STUN)
                sp->MechanicsType = MECHANIC_STUNNED;

            //Set Fearing spells mechanic
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_FEAR ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_FEAR ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_FEAR)
                sp->MechanicsType = MECHANIC_FLEEING;

            //Set Interrupted spells mech
            if (sp->Effect[0] == SPELL_EFFECT_INTERRUPT_CAST ||
                sp->Effect[1] == SPELL_EFFECT_INTERRUPT_CAST ||
                sp->Effect[2] == SPELL_EFFECT_INTERRUPT_CAST)
                sp->MechanicsType = MECHANIC_INTERRUPTED;
        }

        for (uint32 y = 0; y < 3; y++)
        {
            effect = sp->Effect[y];
            if (effect == SPELL_EFFECT_APPLY_AURA)
            {
                uint32 aura = sp->EffectApplyAuraName[y];
                if ((aura == SPELL_AURA_PERIODIC_TRIGGER_SPELL || aura == SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE) && sp->EffectAmplitude[y] == 0)
                {
                    sp->EffectAmplitude[y] = 1000;
                }
                else if (aura == SPELL_AURA_SCHOOL_ABSORB && sp->AuraFactoryFunc == NULL)
                    sp->AuraFactoryFunc = (void * (*)) &AbsorbAura::Create;
            }
        }

        switch (sp->Id)
        {
        case 38776: // Evergrove Druid Transform Crow
            sp->DurationIndex = 4; // 120 seconds
            break;
        case 63026: // Force Cast (HACK: Target shouldn't be changed)
        case 63137: // Force Cast (HACK: Target shouldn't be changed; summon position should be untied from spell destination)
            sp->EffectImplicitTargetA[0] = 17;
            break;
        case 53096: // Quetz'lun's Judgment
        case 70743: // AoD Special
        case 70614: // AoD Special - Vegard
            sp->MaxTargets = 1;
            break;
        case 52611: // Summon Skeletons
        case 52612: // Summon Skeletons
            sp->EffectMiscValueB[0] = 64;
            break;
        case 45257: // Using Steam Tonk Controller
        case 45440: // Steam Tonk Controller
        case 60256: // Collect Sample
            // Crashes client on pressing ESC
            sp->AttributesExD &= ~128;
            break;
        case 40244: // Simon Game Visual
        case 40245: // Simon Game Visual
        case 40246: // Simon Game Visual
        case 40247: // Simon Game Visual
        case 42835: // Spout, remove damage effect, only anim is needed
            sp->Effect[0] = 0;
            break;
        case 63665: // Charge (Argent Tournament emote on riders)
        case 31298: // Sleep (needs target selection script)
        case 2895:  // Wrath of Air Totem rank 1 (Aura)
        case 68933: // Wrath of Air Totem rank 2 (Aura)
        case 29200: // Purify Helboar Meat
            sp->EffectImplicitTargetA[0] = 1;
            sp->EffectImplicitTargetB[0] = 0;
            break;
        case 31344: // Howl of Azgalor
            sp->EffectRadiusIndex[0] = 12; // 100yards instead of 50000?!
            break;
        case 42818: // Headless Horseman - Wisp Flight Port
        case 42821: // Headless Horseman - Wisp Flight Missile
            sp->rangeIndex = 6; // 100 yards
            break;
        case 36350: //They Must Burn Bomb Aura (self)
            sp->EffectTriggerSpell[0] = 36325; // They Must Burn Bomb Drop (DND)
            break;
        case 8494: // Mana Shield (rank 2)
            // because of bug in dbc
            sp->procChance = 0;
            break;
        case 63320: // Glyph of Life Tap
            // Entries were not updated after spell effect change, we have to do that manually :/
            sp->AttributesExC |= 67108864;
            break;
        case 31347: // Doom
        case 41635: // Prayer of Mending
        case 39365: // Thundering Storm
        case 52124: // Sky Darkener Assault
        case 42442: // Vengeance Landing Cannonfire
        case 45863: // Cosmetic - Incinerate to Random Target
        case 25425: // Shoot
        case 45761: // Shoot
        case 42611: // Shoot
        case 61588: // Blazing Harpoon
        case 36327: // Shoot Arcane Explosion Arrow
            sp->MaxTargets = 1;
            break;
        case 36384: // Skartax Purple Beam
            sp->MaxTargets = 2;
            break;
        case 37790: // Spread Shot
        case 54172: // Divine Storm (heal)
        case 66588: // Flaming Spear
        case 54171: // Divine Storm
            sp->MaxTargets = 3;
            break;
        case 53385: // Divine Storm (Damage)
            sp->MaxTargets = 4;
            break;
        case 38296: // Spitfire Totem
            sp->MaxTargets = 5;
            break;
        case 40827: // Sinful Beam
        case 40859: // Sinister Beam
        case 40860: // Vile Beam
        case 40861: // Wicked Beam
            sp->MaxTargets = 10;
            break;
        case 50312: // Unholy Frenzy
            sp->MaxTargets = 15;
            break;
        case 17941: // Shadow Trance
        case 22008: // Netherwind Focus
        case 31834: // Light's Grace
        case 34754: // Clearcasting
        case 34936: // Backlash
        case 48108: // Hot Streak
        case 51124: // Killing Machine
        case 54741: // Firestarter
        case 57761: // Fireball!
        case 39805: // Lightning Overload
        case 64823: // Item - Druid T8 Balance 4P Bonus
        case 34477: // Misdirection
        case 44401: // Missile Barrage
        case 18820: // Insight
            sp->procCharges = 1;
            break;
        case 53390: // Tidal Wave
            sp->procCharges = 2;
            break;
        case 37408: // Oscillation Field
            sp->AttributesExC |= 128;
            break;
        case 28200: // Ascendance (Talisman of Ascendance trinket)
            sp->procCharges = 6;
            break;
        case 51852: // The Eye of Acherus (no spawn in phase 2 in db)
            sp->EffectMiscValue[0] |= 1;
            break;
        case 51912: // Crafty's Ultra-Advanced Proto-Typical Shortening Blaster
            sp->EffectAmplitude[0] = 3000;
            break;
        case 29809: // Desecration Arm - 36 instead of 37 - typo? :/
            sp->EffectRadiusIndex[0] = 37;
            break;
            // Master Shapeshifter: missing stance data for forms other than bear - bear version has correct data
            // To prevent aura staying on target after talent unlearned
        case 48420:
            sp->RequiredShapeShift = 1 << (FORM_CAT - 1);
            break;
        case 48421:
            sp->RequiredShapeShift = 1 << (FORM_MOONKIN - 1);
            break;
        case 48422:
            sp->RequiredShapeShift = 1 << (FORM_TREE - 1);
            break;
        case 51466: // Elemental Oath (Rank 1)
        case 51470: // Elemental Oath (Rank 2)
            sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[1] = SPELL_AURA_ADD_FLAT_MODIFIER;
            sp->EffectMiscValue[1] = 12;
            sp->EffectSpellClassMask[1][0] = 0x00000000;
            sp->EffectSpellClassMask[1][1] = 0x00004000;
            sp->EffectSpellClassMask[1][2] = 0x00000000;
            break;
        case 47569: // Improved Shadowform (Rank 1)
            // with this spell atrribute aura can be stacked several times
            sp->Attributes &= ~65536;
            break;
        case 30421: // Nether Portal - Perseverence
            sp->EffectBasePoints[2] += 30000;
            break;
        case 16834: // Natural shapeshifter
        case 16835:
            sp->DurationIndex = 21;
            break;
        case 51735: // Ebon Plague
        case 51734:
        case 51726:
            sp->SpellGroupType[2] = 0x10;
            sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
            sp->AttributesExC |= 128;
            break;
        case 41013: // Parasitic Shadowfiend Passive
            sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY; // proc debuff, and summon infinite fiends
            break;
        case 27892: // To Anchor 1
        case 27928: // To Anchor 1
        case 27935: // To Anchor 1
        case 27915: // Anchor to Skulls
        case 27931: // Anchor to Skulls
        case 27937: // Anchor to Skulls
            sp->rangeIndex = 13;
            break;
            // target allys instead of enemies, target A is src_caster, spells with effect like that have ally target
            // this is the only known exception, probably just wrong data
        case 29214: // Wrath of the Plaguebringer
        case 54836: // Wrath of the Plaguebringer
            sp->EffectImplicitTargetB[0] = 30;
            sp->EffectImplicitTargetB[1] = 30;
            break;
        case 57994: // Wind Shear - improper data for 1 in 3.3.5 DBC, but is correct in 4.x
            sp->Effect[1] = SPELL_EFFECT_MODIFY_THREAT_PCT; // Cronic: Might need implemented.
            sp->EffectBasePoints[1] = -6; // -5%
            break;
        case 63675: // Improved Devouring Plague
            sp->EffectBonusMultiplier[0] = 0;
            sp->AttributesExC |= 536870912; // 536870912
            break;
        case 8145: // Tremor Totem (instant pulse)
        case 6474: // Earthbind Totem (instant pulse)
            sp->AttributesExE |= 512;
            break;
        case 53241: // Marked for Death (Rank 1)
        case 53243: // Marked for Death (Rank 2)
        case 53244: // Marked for Death (Rank 3)
        case 53245: // Marked for Death (Rank 4)
        case 53246: // Marked for Death (Rank 5)
            sp->EffectSpellClassMask[0][0] = 423937;
            sp->EffectSpellClassMask[0][1] = 276955137;
            sp->EffectSpellClassMask[0][2] = 2049;
            break;
        case 70728: // Exploit Weakness
        case 70840: // Devious Minds
            sp->EffectImplicitTargetA[0] = 1;
            sp->EffectImplicitTargetB[0] = 5;
            break;
        case 70893: // Culling The Herd
            sp->EffectImplicitTargetA[0] = 1;
            sp->EffectImplicitTargetB[0] = 27;
            break;
        case 54800: // Sigil of the Frozen Conscience - change class mask to custom extended flags of Icy Touch
            // this is done because another spell also uses the same SpellGroupType as Icy Touch
            // SpellGroupType[0] & 0x00000040 in SPELLFAMILY_DEATHKNIGHT is currently unused (3.3.5a)
            // this needs research on modifier applying rules, does not seem to be in Attributes fields
            sp->EffectSpellClassMask[0][1] = 0x00000040;
            sp->EffectSpellClassMask[0][2] = 0x00000000;
            sp->EffectSpellClassMask[0][3] = 0x00000000;
            break;
        case 64949: // Idol of the Flourishing Life
            sp->EffectSpellClassMask[0][0] = 0x00000000;
            sp->EffectSpellClassMask[0][1] = 0x02000000;
            sp->EffectSpellClassMask[0][2] = 0x00000000;
            sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_FLAT_MODIFIER;
            break;
        case 34231: // Libram of the Lightbringer
        case 60792: // Libram of Tolerance
        case 64956: // Libram of the Resolute
            sp->EffectSpellClassMask[0][1] = 0x80000000;
            sp->EffectSpellClassMask[0][2] = 0x00000000;
            sp->EffectSpellClassMask[0][3] = 0x00000000;
            sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_FLAT_MODIFIER;
            break;
        case 28851: // Libram of Light
        case 28853: // Libram of Divinity
        case 32403: // Blessed Book of Nagrand
            sp->EffectSpellClassMask[0][1] = 0x40000000;
            sp->EffectSpellClassMask[0][2] = 0x00000000;
            sp->EffectSpellClassMask[0][3] = 0x00000000;
            sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_FLAT_MODIFIER;
            break;
        case 45602: // Ride Carpet
            sp->EffectBasePoints[0] = 0; // force seat 0, vehicle doesn't have the required seat flags for "no seat specified (-1)"
            break;
        case 64745: // Item - Death Knight T8 Tank 4P Bonus
        case 64936: // Item - Warrior T8 Protection 4P Bonus
            sp->EffectBasePoints[0] = 100; // 100% chance of procc'ing, not -10% (chance calculated in PrepareTriggersExecutedOnHit)
            break;
        case 61719: // Easter Lay Noblegarden Egg Aura - Interrupt flags copied from aura which this aura is linked with
            sp->AuraInterruptFlags = 1 | 2;
            break;
        case 51640: // spell for item The Flag of Ownership (38578)
            sp->AttributesExC |= 256;
            sp->AttributesExB |= 1;
            sp->MaxTargets = 1;
            break;
        case 34471: // The Beast Within
            sp->AttributesExE |= 262144 | 131072 | 8;
            break;
        case 40055: // Introspection
        case 40165: // Introspection
        case 40166: // Introspection
        case 40167: // Introspection
            sp->Attributes |= 67108864;
            break;
        case 2378: // Minor Fortitude
            sp->manaCost = 0;
            sp->manaPerSecond = 0;
            break;
        case 24314: // Threatening Gaze
            sp->AuraInterruptFlags |= 4 | 8 | 32;
            break;

            /////////////////////////////////////////////
            /////////////////CLASS SPELLS////////////////
            /////////////////////////////////////////////

            /////////////////////////////////
            ///// PALADIN
            /////////////////////////////////
            // Heart of the Crusader
        case 20335:
        case 20336:
        case 20337:
            if (sp->Id == 20335)
                sp->EffectTriggerSpell[0] = 21183;
            else if (sp->Id == 20336)
                sp->EffectTriggerSpell[0] = 54498;
            else
                sp->EffectTriggerSpell[0] = 54499;
            sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
            break;
            // Bleh, need to change FamilyFlags :/ (have the same as original aura - bad!)
        case 63510:
            sp->SpellGroupType[0] = 0;
            sp->SpellGroupType[2] = 0x4000000;
            break;
        case 63514:
            sp->SpellGroupType[0] = 0;
            sp->SpellGroupType[2] = 0x2000000;
            break;
        case 63531:
            sp->SpellGroupType[0] = 0;
            sp->SpellGroupType[2] = 0x8000000;
            break;
            // And affecting spells
        case 20138:
        case 20139:
        case 20140:// Improved Devotion Aura
            sp->EffectSpellClassMask[1][0] = 0;
            sp->EffectSpellClassMask[1][2] = 0x2000000;
            break;
        case 20254:
        case 20255:
        case 20256:// Improved concentration aura
            sp->EffectSpellClassMask[1][0] = 0;
            sp->EffectSpellClassMask[1][2] = 0x4000000;
            sp->EffectSpellClassMask[2][0] = 0;
            sp->EffectSpellClassMask[2][2] = 0x4000000;
            break;
        case 53379:
        case 53484:
        case 53648:// Swift Retribution
            sp->EffectSpellClassMask[0][0] = 0;
            sp->EffectSpellClassMask[0][2] = 0x8000000;
            break;
        case 31869:// Sanctified Retribution
            sp->EffectSpellClassMask[0][0] = 0;
            sp->EffectSpellClassMask[0][2] = 0x8000000;
            break;
        case 20167: // Seal of Light trigger
            sp->spellLevel = 0;
            sp->baseLevel = 0;
            sp->Spell_Dmg_Type = 1;
            break;
            // Light's Beacon, Beacon of Light
        case 53651:
            sp->AttributesExC |= 128;
            break;
            // Hand of Reckoning
        case 62124:
            sp->AttributesEx |= 8;
            break;
            // Redemption
        case 7328:
        case 10322:
        case 10324:
        case 20772:
        case 20773:
        case 48949:
        case 48950:
            sp->SpellFamilyName = 10; // Paladin
            break;
            // hack for seal of light and few spells, judgement consists of few single casts and each of them can proc
            // some spell, base one has disabled proc flag but those dont have this flag
        case 20184:
        case 20185:
        case 20186:
        case 68055:
            sp->AttributesExC |= 65536;
            break;
            // Blessing of sanctuary stats
        case 67480:
            sp->EffectMiscValue[0] = -1;
            sp->SpellFamilyName = 1; // allows stacking
            sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY; // just a marker
            break;
            // Seal of Command trigger
        case 20424:
            sp->AttributesExC &= ~65536;
            break;
            // Glyph of Holy Light, Damage Class should be magic
        case 54968:
            // Beacon of Light heal, Damage Class should be magic
        case 53652:
        case 53654:
            sp->AttributesExC |= 536870912;
            sp->Spell_Dmg_Type = 1;
            break;

            /////////////////////////////////
            ///// HUNTER
            /////////////////////////////////
            // Furious Howl
        case 64491:
        case 64492:
        case 64493:
        case 64494:
        case 64495:
            sp->EffectImplicitTargetA[0] = 1;
            sp->EffectImplicitTargetB[0] = 27;
            sp->EffectImplicitTargetA[1] = 1;
            sp->EffectImplicitTargetB[1] = 27;
            break;
            // Call of the Wild
        case 53434:
            sp->EffectImplicitTargetA[0] = 27;
            sp->EffectImplicitTargetA[1] = 27;
            sp->EffectImplicitTargetB[0] = 1;
            sp->EffectImplicitTargetB[1] = 1;
            break;
            // Wild Hunt
        case 62758:
        case 62762:
            sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
            sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
            break;
            // Intervene
        case 3411:
            sp->Attributes |= 1048576;
            sp->AttributesExC |= 131072;
            break;
            // Roar of Sacrifice
        case 53480:
            sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[1] = SPELL_AURA_SPLIT_DAMAGE;
            sp->EffectImplicitTargetA[1] = 21;
            sp->EffectDieSides[1] = 1;
            sp->EffectBasePoints[1] = 19;
            sp->EffectMiscValue[1] = 127; // all schools
            break;
            // Silencing Shot
        case 34490:
        case 41084:
        case 42671:
            sp->speed = 0.0f;
            break;
            // Monstrous Bite
        case 54680:
        case 55495:
        case 55496:
        case 55497:
        case 55498:
        case 55499:
            sp->EffectImplicitTargetA[1] = 1;
            break;
            // Hunter's Mark
        case 1130:
        case 14323:
        case 14324:
        case 14325:
        case 53338:
            sp->Spell_Dmg_Type = 1;
            sp->AttributesExC |= 262144;
            break;
            // Cobra Strikes
        case 53257:
            sp->procCharges = 2;
            sp->maxstack = 0;
            break;
            // Kill Command
        case 34027:
            sp->procCharges = 0;
            break;
            // Kindred Spirits, damage aura
        case 57458:
            sp->AttributesExD |= 2097152;
            break;
            // Chimera Shot - Serpent trigger
        case 53353:
            sp->AttributesExC |= 536870912;
            break;
            // Entrapment trigger
        case 19185:
        case 64803:
        case 64804:
            sp->EffectImplicitTargetA[0] = 53;
            sp->EffectImplicitTargetB[0] = 16;
            sp->AttributesExE |= 67108864;
            break;
            // Improved Stings (Rank 2)
        case 19465:
            sp->EffectImplicitTargetA[2] = 1;
            break;
            // Explosive Shot (trigger)
        case 53352:
            sp->AttributesExC |= 262144;
            break;
            // Heart of the Phoenix (triggered)
        case 54114:
            sp->AttributesEx &= ~1;
            sp->RecoveryTime = 8 * 60 * IN_MILLISECONDS; // prev 600000
            break;

            /////////////////////////////////
            ///// ROGUE
            /////////////////////////////////
            // Fan of Knives AoE damage fix
        case 51723:
            sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMY_IN_AREA;
            sp->EffectImplicitTargetB[0] = EFF_TARGET_NONE;
            break;
            // Master of Subtlety
        case 31221:
        case 31222:
        case 31223:
            sp->SpellFamilyName = 8;
            break;
            // Master of Subtlety triggers
        case 31666:
            // Overkill
        case 58428:
            sp->Effect[0] = SPELL_EFFECT_SCRIPT_EFFECT;
            break;
            // Honor Among Thieves
        case 51698:
        case 51700:
        case 51701:
            sp->EffectTriggerSpell[0] = 51699;
            break;
            // Slice and Dice
        case 5171:
        case 6774:
            // Distract
        case 1725:
            sp->AttributesExC |= 131072; //131072
            break;
            // Envenom
        case 32645:
        case 32684:
        case 57992:
        case 57993:
            sp->DispelType = 0;
            break;
            // Killing Spree (teleport)
        case 57840:
            sp->rangeIndex = 6; // 100 yards
            break;
            // Killing Spree
        case 51690:
            sp->AttributesEx |= 32; //SPELL_ATTR1_NOT_BREAK_STEALTH
            break;

            /////////////////////////////////
            ///// DEATH KNIGHT
            /////////////////////////////////
            //Blood Boil AoE damage fix
        case 48721:
        case 49939:
        case 49940:
        case 49941:
            sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMY_IN_AREA;
            sp->EffectImplicitTargetB[0] = EFF_TARGET_NONE;
            break;
            // Blood Tap visual cd reset
        case 47804:
            sp->Effect[2] = 0;
            sp->Effect[1] = 0;
            sp->runeCostID = 442;
            break;
            // Chains of Ice
        case 45524:
            sp->Effect[2] = 0;
            break;
            // Impurity
        case 49220:
        case 49633:
        case 49635:
        case 49636:
        case 49638:
            sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
            sp->EffectImplicitTargetA[0] = 1;
            sp->SpellFamilyName = 15;
            break;
            // Deadly Aggression (Deadly Gladiator's Death Knight Relic, item: 42620)
        case 60549:
            sp->Effect[1] = 0;
            break;
            // Magic Suppression
        case 49224:
        case 49610:
        case 49611:
            sp->procCharges = 0;
            break;
            // Wandering Plague
        case 50526:
            sp->AttributesExC |= 536870912; //536870912
            sp->AttributesExC |= 131072; //131072
            break;
            // Dancing Rune Weapon
        case 49028:
            sp->Effect[2] = 0;
            sp->procFlags |= PROC_ON_MELEE_ATTACK;
            break;
            // Death and Decay
        case 52212:
            sp->AttributesExB |= 4; //SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS
            sp->AttributesExF |= 8192; //SPELL_ATTR6_CAN_TARGET_INVISIBLE
            break;
            // T9 blood plague crit bonus
        case 67118:
            sp->Effect[1] = 0;
            break;
            // Pestilence
        case 50842:
            sp->EffectImplicitTargetA[2] = 53; //TARGET_DEST_TARGET_ENEMY
            break;
            // Horn of Winter, stacking issues
        case 57330:
        case 57623:
            sp->EffectImplicitTargetA[1] = 0;
            break;
            // Scourge Strike trigger
        case 70890:
            sp->AttributesExC |= 65536; //SPELL_ATTR3_CANT_TRIGGER_PROC
            break;
            // Blood-caked Blade - Blood-caked Strike trigger
        case 50463:
            sp->AttributesExC |= 65536; //SPELL_ATTR3_CANT_TRIGGER_PROC
            break;
            // Blood Gorged - ARP affect Death Strike and Rune Strike
        case 61274:
        case 61275:
        case 61276:
        case 61277:
        case 61278:
            sp->EffectSpellClassMask[0][0] = 0x1400011;
            sp->EffectSpellClassMask[0][1] = 0x20000000;
            sp->EffectSpellClassMask[0][2] = 0x0;
            break;
            // Death Grip, remove main grip mechanic, leave only effect one
            // Death Grip, should fix taunt on bosses and not break the pull protection at the same time (no aura provides immunity to grip mechanic)
        case 49576:
        case 49560:
            sp->MechanicsType = 0;
            break;
            // Death Grip Jump Dest
        case 57604:
            sp->AttributesExB |= 4; //SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS
            break;
            // Death Pact
        case 48743:
            sp->AttributesEx &= ~524288;
            break;
            // Raise Ally (trigger)
        case 46619:
            sp->Attributes &= ~-2147483648u;
            break;
            // Frost Strike
        case 49143:
        case 51416:
        case 51417:
        case 51418:
        case 51419:
        case 55268:
            sp->AttributesExC |= 8;
            break;
            // Death Knight T10 Tank 2p Bonus
        case 70650:
            sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
            break;

            /////////////////////////////////
            ///// SHAMAN
            /////////////////////////////////
            // Lightning overload
        case 45297:
        case 45284:
            sp->CategoryRecoveryTime = 0;
            sp->RecoveryTime = 0;
            sp->AttributesExF |= 536870912;
            break;
            // Improved Earth Shield
        case 51560:
        case 51561:
            sp->EffectMiscValue[1] = 0;
            break;
            // Tidal Force
        case 55166:
        case 55198:
            sp->DurationIndex = 18;
            sp->procCharges = 0;
            break;
            // Healing Stream Totem
        case 52042:
            sp->spellLevel = 0;
            sp->baseLevel = 0;
            sp->Spell_Dmg_Type = 1;
            break;
            // Earth Shield
        case 379:
            sp->spellLevel = 0;
            sp->baseLevel = 0;
            sp->Spell_Dmg_Type = 1;
            sp->AttributesExC |= 536870912;
            break;
            // Stormstrike
        case 17364:
            sp->AttributesExC |= 128;
            break;
            // Strength of Earth totem effect
        case 8076:
        case 8162:
        case 8163:
        case 10441:
        case 25362:
        case 25527:
        case 57621:
        case 58646:
            sp->EffectRadiusIndex[1] = sp->EffectRadiusIndex[0];
            sp->EffectRadiusIndex[2] = sp->EffectRadiusIndex[0];
            break;
            // Flametongue Totem effect
        case 52109:
        case 52110:
        case 52111:
        case 52112:
        case 52113:
        case 58651:
        case 58654:
        case 58655:
            sp->EffectImplicitTargetB[2] = sp->EffectImplicitTargetB[1] = sp->EffectImplicitTargetB[0] = 0;
            sp->EffectImplicitTargetA[2] = sp->EffectImplicitTargetA[1] = sp->EffectImplicitTargetA[0] = 1;
            break;
            // Sentry Totem
        case 6495:
            sp->EffectRadiusIndex[0] = 0;
            break;
            // Bind Sight (PT)
        case 6277:
            sp->AttributesEx &= ~4;
            sp->Attributes |= 64;
            sp->AttributesExG |= 4; // because it is passive, needs this to be properly removed at death in RemoveAllAurasOnDeath()
            break;
            // Ancestral Awakening Heal
        case 52752:
            sp->AttributesExC |= 536870912;
            break;
        case 32182: // Heroism
            sp->targetAuraSpellNot = 57723; // Exhaustion
            sp->AttributesExB |= 4;
            break;
        case 2825:  // Bloodlust
            sp->targetAuraSpellNot = 57724; // Sated
            sp->AttributesExB |= 4;
            break;

            /////////////////////////////////
            ///// WARLOCK
            /////////////////////////////////
            // Improved Succubus
        case 18754:
        case 18755:
        case 18756:
            sp->EffectImplicitTargetA[0] = 1;
            break;
            // Unstable Affliction
        case 31117:
            sp->PreventionType = 0;
            break;
            // Shadowflame - trigger
        case 47960: // r1
        case 61291: // r2
            sp->AttributesEx |= 8;
            break;
            // Curse of Doom - summoned doomguard duration fix
        case 18662:
            sp->DurationIndex = 6;
            break;
            // Glyph of Voidwalker
        case 56247:
            sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_FLAT_MODIFIER;
            sp->EffectMiscValue[0] = 3;
            sp->EffectSpellClassMask[0][0] = 0x8000000;
            sp->EffectSpellClassMask[0][1] = 0;
            sp->EffectSpellClassMask[0][2] = 0;
            break;
            // Everlasting Affliction
        case 47201:
        case 47202:
        case 47203:
        case 47204:
        case 47205:
            sp->EffectSpellClassMask[1][0] |= 2; // add corruption to affected spells
            break;
            // Death's Embrace
        case 47198:
        case 47199:
        case 47200:
            sp->EffectSpellClassMask[1][0] |= 0x4000; // include Drain Soul
            break;
            // Improved Demonic Tactics
        case 54347:
        case 54348:
        case 54349:
            sp->Effect[1] = sp->Effect[0];
            sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
            sp->EffectImplicitTargetA[1] = sp->EffectImplicitTargetA[0];
            sp->EffectMiscValue[0] = 3;
            sp->EffectMiscValue[1] = 12;
            break;
            // Rain of Fire (Doomguard)
        case 42227:
            sp->speed = 0.0f;
            break;
            // Ritual Enslavement
        case 22987:
            sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CHARM;
            break;


            /////////////////////////////////
            ///// MAGE
            /////////////////////////////////
            //Ignite (HACK! change proc type)
        case 11119:
        case 11120:
        case 12846:
        case 12847:
        case 12848:
            sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
            sp->EffectTriggerSpell[0] = 12654;
            break;
            // Combustion, make this passive
        case 11129:
            sp->DispelType = 0;
            break;
            // Magic Absorption (nigga stole my code)
        case 29441:
        case 29444:
            sp->spellLevel = 0;
            break;
            // Living Bomb
        case 44461:
        case 55361:
        case 55362:
            sp->AttributesExC |= 131072;
            sp->AttributesExD |= 16384;
            break;
            // Evocation
        case 12051:
            sp->InterruptFlags |= 8;
            break;
            // MI Fireblast, WE Frostbolt, MI Frostbolt
        case 59637:
        case 31707:
        case 72898:
            sp->Spell_Dmg_Type = 1;
            break;
            // Blazing Speed
        case 31641:
        case 31642:
            sp->EffectTriggerSpell[0] = 31643;
            break;
            // Summon Water Elemental (permanent), treat it as pet
        case 70908:
            sp->Effect[0] = SPELL_EFFECT_SUMMON_PET;
            break;
            // Burnout, trigger
        case 44450:
            sp->Effect[0] = SPELL_EFFECT_POWER_BURN;
            break;
            // Mirror Image - Summon Spells
        case 58831:
        case 58833:
        case 58834:
        case 65047:
            sp->EffectImplicitTargetA[0] = 18;
            sp->EffectRadiusIndex[0] = 0;
            break;
            // Initialize Images (Mirror Image)
        case 58836:
            sp->EffectImplicitTargetA[1] = 1;
            break;
            // Arcane Blast, can't be dispelled
        case 36032:
            sp->Attributes |= 536870912;
            break;
            // Chilled (frost armor, ice armor proc)
        case 6136:
        case 7321:
            sp->PreventionType = 0;
            break;
            // Mirror Image Frostbolt
        case 59638:
            sp->Spell_Dmg_Type = 1;
            sp->SpellFamilyName = 3;
            sp->SpellGroupType[0] = 0x20;
            sp->SpellGroupType[1] = 0x0;
            sp->SpellGroupType[2] = 0x0;
            break;
            // Fingers of Frost
        case 44544:
            sp->DispelType = 0;
            sp->AttributesExD |= 64;
            sp->EffectSpellClassMask[0][0] = 685904631; // xinef: removed molten armor
            sp->EffectSpellClassMask[0][1] = 1151040; // xinef: removed molten armor
            sp->EffectSpellClassMask[0][2] = 32; // xinef: removed molten armor
            break;
            // Fingers of Frost visual buff
        case 74396:
            sp->procCharges = 2;
            sp->maxstack = 0;
            break;


            /////////////////////////////////
            ///// WARRIOR
            /////////////////////////////////
            // Glyph of blocking
        case 58375:
            sp->EffectTriggerSpell[0] = 58374;
            break;
            // Sweeping Strikes stance change
        case 12328:
            sp->Attributes |= 65536;
            break;
            // Damage Shield
        case 59653:
            sp->Spell_Dmg_Type = 1;
            sp->spellLevel = 0;
            break;
            // Strange shared cooldown
        case 20230: // Retaliation
        case 871: // Shield Wall
        case 1719: // Recklessness
            sp->AttributesExF |= -2147483648u;
            break;
            // Vigilance, fixes bug with empowered renew, single target aura
        case 50720:
            sp->SpellFamilyName = 4;
            break;
            // Sunder Armor - trigger, remove spellfamilyflags because of glyph of sunder armor
        case 58567:
            sp->SpellGroupType[0] = 0x0;
            sp->SpellGroupType[1] = 0x0;
            sp->SpellGroupType[2] = 0x0;
            break;
            // Sunder Armor - Old Ranks
        case 7405:
        case 8380:
        case 11596:
        case 11597:
        case 25225:
        case 47467:
            sp->EffectTriggerSpell[0] = 11971;
            sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE;
            break;
            // Improved Spell Reflection
        case 59725:
            sp->EffectImplicitTargetA[0] = 20;
            break;


            /////////////////////////////////
            ///// PRIEST
            /////////////////////////////////
            // Shadow Weaving
        case 15257:
        case 15331:
        case 15332:
            sp->EffectImplicitTargetA[0] = 1;
            sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
            break;
            // Hymn of Hope - rewrite part of aura system or swap effects...
        case 64904:
            sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT;
            sp->Effect[2] = sp->Effect[0];
            sp->Effect[0] = 0;
            sp->EffectDieSides[2] = sp->EffectDieSides[0];
            sp->EffectImplicitTargetA[2] = sp->EffectImplicitTargetB[0];
            sp->EffectRadiusIndex[2] = sp->EffectRadiusIndex[0];
            sp->EffectBasePoints[2] = sp->EffectBasePoints[0];
            break;
            // Divine Hymn
        case 64844:
            sp->Spell_Dmg_Type = 1;
            sp->spellLevel = 0;
            break;
            // Spiritual Healing affects prayer of mending
        case 14898:     case 15349:     case 15354:     case 15355:     case 15356:
            // Divine Providence affects prayer of mending
        case 47562:     case 47564:     case 47565:     case 47566:     case 47567:
            // Twin Disciplines affects prayer of mending
        case 47586:     case 47587:     case 47588:     case 52802:     case 52803:
            sp->EffectSpellClassMask[0][1] |= 0x20; // prayer of mending
            break;
            // Power Infusion, hack to fix stacking with arcane power
        case 10060:
            sp->Effect[2] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[2] = SPELL_AURA_ADD_PCT_MODIFIER;
            sp->EffectImplicitTargetA[2] = 21;
            break;



            /////////////////////////////////
            ///// DRUID
            /////////////////////////////////
            // Lifebloom final bloom
        case 33778:
            sp->Spell_Dmg_Type = 1;
            sp->spellLevel = 0;
            sp->SpellGroupType[0] = 0;
            sp->SpellGroupType[1] = 0x10;
            sp->SpellGroupType[2] = 0;
            break;
            // Clearcasting
        case 16870:
            sp->DurationIndex = 31; // 8 secs
            break;
            // Owlkin Frenzy
        case 48391:
            sp->Attributes |= 65536;
            break;
            // Item T10 Restoration 4P Bonus
        case 70691:
            sp->AttributesExC |= 536870912;
            break;
            // Faerie Fire, Faerie Fire (Feral)
        case 770:
        case 16857:
            sp->AttributesEx &= ~65536;
            break;
            // Feral Charge - Cat
        case 49376:
        case 61138:
        case 61132:
        case 50259:
            sp->AttributesExC |= 131072;
            break;
            // Glyph of Barkskin
        case 63058:
            sp->EffectApplyAuraName[0] = 187;
            break;




        }

        //Cronic: Load Procs from DB
        QueryResult* resultaaz = WorldDatabase.Query("SELECT * FROM spell_proc_source");
        if (resultaaz != NULL)
        {
            do
            {
                Field* f;
                f = resultaaz->Fetch();
                sp = dbcSpell.LookupEntryForced(f[0].GetUInt32());
                if (sp != NULL)
                {
                    sp->procFlags = f[1].GetUInt32(); // Set proc Flags for spellId
                    if (f[2].GetUInt32() > 0) // If database chance is zero use DBC value.
                    {
                        sp->procChance = f[2].GetUInt32();
                    }
                    if (f[3].GetUInt32() > 0) // If database chance is zero use DBC value. (Cooldown)
                    {
                        sp->proc_interval = f[3].GetUInt32() * IN_MILLISECONDS;
                    }
                    sp->SchoolMask = f[4].GetUInt32(); // Schoolmask
                    sp->SpellFamilyName = f[5].GetUInt32(); // Spell Family Name
                    sp->SpellGroupType[0] = f[6].GetUInt32(); // Spell Group Type 0
                    sp->SpellGroupType[1] = f[7].GetUInt32(); // Spell Group Type 1
                    sp->SpellGroupType[2] = f[8].GetUInt32(); // Spell Group Type 2
                    if (f[1].GetUInt32() == 0 && f[9].GetUInt32() > 0) // if ProcEX has field and procFlags is zero we override procFlags.
                    {
                        sp->procFlags = f[9].GetUInt32(); // Set proc Flags for spellId
                    }
                }
                else
                    Log.Error("SpellProcSource", "Has nonexistent spell %u.", f[0].GetUInt32());
            } while (resultaaz->NextRow());
            delete resultaaz;
        }

    }
}