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

#ifndef _WOWSERVER_DEFINITION_H
#define _WOWSERVER_DEFINITION_H


const unsigned NUM_MAPS = 800;
const unsigned NUM_INSTANCE_MODES = 4;


/// Sorry...need this enumeration in Player.*
enum INSTANCE_MODE
{
    DUNGEON_NORMAL = 0,
    DUNGEON_HEROIC = 1
};

enum RAID_MODE
{
    RAID_10MAN_NORMAL    = 0,
    RAID_25MAN_NORMAL    = 1,
    RAID_10MAN_HEROIC    = 2,
    RAID_25MAN_HEROIC    = 3,
    TOTAL_RAID_MODES     = 4
};

enum ACHIEVEMENT_NAMES
{
    ACHIEVEMENT_DEFENSELESS = 1816 // Defeat Cyanigosa in The Violet Hold without using Defense Control Crystals and with Prison Seal Integrity at 100% while in Heroic Difficulty.
};

enum TimeConstants
{
    MINUTE          = 60,
    HOUR            = MINUTE * 60,
    DAY             = HOUR * 24,
    WEEK            = DAY * 7,
    MONTH           = DAY * 30,
    YEAR            = MONTH * 12,
    IN_MILLISECONDS = 1000
};

#define MAX_RACES         12

#define RACEMASK_ALL_PLAYABLE \
    ((1<<(RACE_HUMAN-1))   |(1<<(RACE_ORC-1))          |(1<<(RACE_DWARF-1))   | \
    (1<<(RACE_NIGHTELF-1))|(1<<(RACE_UNDEAD-1))|(1<<(RACE_TAUREN-1))  | \
    (1<<(RACE_GNOME-1))   |(1<<(RACE_TROLL-1))        |(1<<(RACE_BLOODELF-1))| \
    (1<<(RACE_DRAENEI-1)))

#define RACEMASK_ALLIANCE \
    ((1<<(RACE_HUMAN-1)) | (1<<(RACE_DWARF-1)) | (1<<(RACE_NIGHTELF-1)) | \
    (1<<(RACE_GNOME-1)) | (1<<(RACE_DRAENEI-1)))

#define RACEMASK_HORDE RACEMASK_ALL_PLAYABLE & ~RACEMASK_ALLIANCE

#define MAKE_NEW_GUID(l, e, h)   uint64(uint64(l) | (uint64(e) << 24) | (uint64(h) << 48))

#endif // _WOWSERVER_DEFINITION_H
