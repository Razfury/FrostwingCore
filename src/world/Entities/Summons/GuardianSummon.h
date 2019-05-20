/*
 * FrostwingCore <https://gitlab.com/cronicx321/AscEmu>
 * Copyright (C) 2005-2007 Ascent Team
 * Copyright (C) 2008-2011 <http://www.ArcEmu.org/>
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

#ifndef _GUARDIANSUMMON_H
#define _GUARDIANSUMMON_H

//////////////////////////////////////////////////////////////////////////////////////////
/// class GuardianSummon
/// Class that implements guardians
/// Guardians are summons that follow and protect their owner
//////////////////////////////////////////////////////////////////////////////////////////
class GuardianSummon : public Summon
{
    public:
        GuardianSummon(uint64 GUID);
        ~GuardianSummon();

        void Load(CreatureProto* proto, Unit* owner, LocationVector & position, uint32 spellid, int32 summonslot);
        void OnPushToWorld();
        void OnPreRemoveFromWorld();
    private:
};

#endif // _GUARDIANSUMMON_H
