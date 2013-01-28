/*
 * Copyright 2012-2013 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBFALLTERGEIST_BIOFILETYPE_H
#define LIBFALLTERGEIST_BIOFILETYPE_H

namespace libfalltergeist
{
class DatFileItem;

class BioFileType
{
protected:
    DatFileItem * _datFileItem;
    char * _text;
public:
    BioFileType(DatFileItem * datFileItem);
    ~BioFileType();

    DatFileItem * datFileItem();

    void open();

    const char * text();
};

}
#endif // LIBFALLTERGEIST_BIOFILETYPE_H
