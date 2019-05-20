/*
 * FrostwingCore <https://gitlab.com/cronicx321/AscEmu>
 * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org>
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
 */

#ifndef _COMPANIONSUMMON_H
#define _COMPANIONSUMMON_H

//////////////////////////////////////////////////////////////////////////////////////////
/// class CompanionSummon
/// \note   Class implementing companions/vanity pets/critterpets
///         These are totally passive and inattackable, they only serve iCandy purposes
///
//////////////////////////////////////////////////////////////////////////////////////////
class CompanionSummon : public Summon
{
    public:
        CompanionSummon(uint64 GUID);
        ~CompanionSummon();

        virtual void Load(CreatureProto* proto, Unit* owner, LocationVector& position, uint32 spellid, int32 summonslot);

        void OnPushToWorld();
        void OnPreRemoveFromWorld();
    private:
};

#endif // _COMPANIONSUMMON_H
