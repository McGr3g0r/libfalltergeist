/*
 * Copyright 2012-2015 Falltergeist Developers.
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

// C++ standard includes

// libfalltergeist includes
#include "../src/AafGlyph.h"

// Third party includes

namespace libfalltergeist
{

AafGlyph::AafGlyph(unsigned short width, unsigned short height) : _width(width), _height(height)
{
}

AafGlyph::~AafGlyph()
{
}

unsigned short AafGlyph::height()
{
    return _height;
}

AafGlyph* AafGlyph::setHeight(unsigned short height)
{
    _height = height;
    return this;
}

unsigned short AafGlyph::width()
{
    return _width;
}

AafGlyph* AafGlyph::setWidth(unsigned short width)
{
    _width = width;
    return this;
}

std::vector<unsigned char>* AafGlyph::data()
{
    return &_data;
}

unsigned int AafGlyph::dataOffset()
{
    return _dataOffset;
}

void AafGlyph::setDataOffset(unsigned int value)
{
    _dataOffset = value;
}

}
