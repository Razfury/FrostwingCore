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

#ifndef _POSSESSEDSUMMON_H
#define _POSSESSEDSUMMON_H


//////////////////////////////////////////////////////////////////////////////////////////
/// class PossessedSummon
/// Class that implements summons that are possessed by the player after spawning.
/// They despawn when killed or dismissed
///
//////////////////////////////////////////////////////////////////////////////////////////
class PossessedSummon : public Summon
{
    public:

        PossessedSummon(uint64 GUID);
        ~PossessedSummon();

        void Load(CreatureProto* proto, Unit* owner, LocationVector & position, uint32 spellid, int32 summonslot);

        void OnPushToWorld();
        void OnPreRemoveFromWorld();
    private:

};

#endif // _POSSESSEDSUMMON_H
