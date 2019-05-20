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

#ifndef _CREFCOUNTER_H
#define _CREFCOUNTER_H

namespace Arcemu
{
    namespace Shared
    {
        //////////////////////////////////////////////////////////////////////////////////////////
        /// class CRefCounter
        /// Reference Counter class.
        /// Reference counting starts with 1 reference, on instantiation
        ///
        //////////////////////////////////////////////////////////////////////////////////////////
        class SERVER_DECL CRefCounter
        {
            public:

                CRefCounter() { Counter.SetVal(1); }

                virtual ~CRefCounter() {}


                //////////////////////////////////////////////////////////////////////////////////////////
                /// void AddRef()
                /// Increases the reference count by 1
                ///
                /// \param none
                ///
                /// \return none
                ///
                //////////////////////////////////////////////////////////////////////////////////////////
                void AddRef() { ++Counter; }


                //////////////////////////////////////////////////////////////////////////////////////////
                /// void DecRef()
                /// Decreases the reference count by 1. When it reaches 0, the object is deleted
                ///
                /// \param none
                ///
                /// \return none
                ///
                //////////////////////////////////////////////////////////////////////////////////////////
                void DecRef()
                {
                    if (--Counter == 0)
                        delete this;
                }

            private:

                Arcemu::Threading::AtomicCounter Counter;

        };
    }
}

#endif      //_CREFCOUNTER_H
