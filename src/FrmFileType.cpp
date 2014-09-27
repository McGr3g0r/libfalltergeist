/*
 * Copyright 2012-2014 Falltergeist Developers.
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
#include <algorithm>
#include <iostream>

// libfalltergeist includes
#include "../src/FrmFileType.h"
#include "../src/PalFileType.h"
#include "../src/PalColor.h"

// Third party includes

namespace libfalltergeist
{

FrmFileType::FrmFileType(std::shared_ptr<DatFileEntry> datFileEntry) : DatFileItem(datFileEntry)
{
}

FrmFileType::FrmFileType(std::ifstream * stream) : DatFileItem(stream)
{
}

FrmFileType::~FrmFileType()
{
    delete [] _rgba;
}

void FrmFileType::_initialize()
{
    if (_initialized) return;
    DatFileItem::_initialize();
    DatFileItem::setPosition(0);

    *this >> _version >> _framesPerSecond >> _actionFrame >> _framesPerDirection;

    for (unsigned int i = 0; i != 6; ++i) *this >> _shiftX[i];
    for (unsigned int i = 0; i != 6; ++i) *this >> _shiftY[i];
    for (unsigned int i = 0; i != 6; ++i)
    {
        *this >> _dataOffset[i];
        if (i > 0 && _dataOffset[i-1] == _dataOffset[i]) continue;
        _directions++;
    }

    // for each direction
    for (unsigned int i = 0; i!= _directions; ++i)
    {
        // jump to frames data at frames area
        DatFileItem::setPosition(_dataOffset[i] + 62);

        // read all frames
        for (unsigned int j = 0; j != _framesPerDirection; ++j)
        {
            unsigned short width, height;
            short offsetX, offsetY;

            *this >> width >> height;
            _width[i].push_back(width);
            _height[i].push_back(height);

            // Number of pixels for this frame
            // We don't need this, because we already have width*height
            this->skipBytes(4);

            *this >> offsetX >> offsetY;
            _offsetX[i].push_back(offsetX);
            _offsetY[i].push_back(offsetY);

            // will be used later
            this->skipBytes(width*height);
        }
    }
}

unsigned int FrmFileType::version()
{
    _initialize();
    return _version;
}

unsigned short FrmFileType::framesPerSecond()
{
    _initialize();
    return _framesPerSecond;
}

unsigned short FrmFileType::framesPerDirection()
{
    _initialize();
    return _framesPerDirection;
}

unsigned short FrmFileType::actionFrame()
{
    _initialize();
    return _actionFrame;
}

unsigned int FrmFileType::directions()
{
    _initialize();
    return _directions;
}

unsigned int FrmFileType::width()
{
    _initialize();
    std::vector<int> widths;
    for (unsigned int d = 0; d != _directions; ++d)
    {
        widths.push_back(this->width(d) * framesPerDirection());
    }
    return *std::max_element(widths.begin(), widths.end());
}

unsigned int FrmFileType::height()
{
    _initialize();
    unsigned int height = 0;

    for (unsigned int d = 0; d != _directions; ++d)
    {
        height += this->height(d);
    };
    return height;
}

unsigned int FrmFileType::width(unsigned int direction, unsigned int frame)
{
    return _width[direction].at(frame);
}

unsigned int FrmFileType::height(unsigned int direction)
{
    _initialize();
    return *std::max_element(_height[direction].begin(), _height[direction].end());
}

unsigned int FrmFileType::width(unsigned int direction)
{
    _initialize();
    return *std::max_element(_width[direction].begin(), _width[direction].end());
}


unsigned int FrmFileType::height(unsigned int direction, unsigned int frame)
{
    return _height[direction].at(frame);
}

unsigned int* FrmFileType::rgba(std::shared_ptr<PalFileType> palFile)
{
    if (_rgba) return _rgba;
    _initialize();
    unsigned int w = width();
    unsigned int h = height();
    _rgba = new unsigned int[w*h]();


    unsigned int positionY = 0;
    for (unsigned int d = 0; d != _directions; ++d)
    {
        unsigned int positionX = 0;
        DatFileItem::setPosition(_dataOffset[d] + 62);
        for (unsigned int f = 0; f != _framesPerDirection; ++f)
        {
            this->skipBytes(4*3);
            // read frame data and add to _rgba
            for (unsigned int y = 0; y != height(d, f); ++y)
            {
                for (unsigned int x = 0; x != width(d, f); ++x)
                {
                    unsigned char index;
                    *this >> index;
                    _rgba[((y + positionY)*w) + x + positionX] = *palFile->color(index);
                }
            }
            positionX += width(d);
        }
        positionY += height(d);
    }
    return _rgba;
}

int FrmFileType::offsetX(unsigned int direction, unsigned int frame)
{
    return _offsetX[direction].at(frame);
}

int FrmFileType::offsetY(unsigned int direction, unsigned int frame)
{
    return _offsetY[direction].at(frame);
}

int FrmFileType::shiftX(unsigned int direction)
{
    return _shiftX[direction];
}

int FrmFileType::shiftY(unsigned int direction)
{
    return _shiftY[direction];
}

bool FrmFileType::animatedPalette()
{
    if (_animatedMasks.empty()) animatedMasks();
    return _animatedPalette;
}

std::map<unsigned int, uint8_t*>* FrmFileType::animatedMasks()
{
    if (!_animatedMasks.empty()) return &_animatedMasks;
    _initialize();

    unsigned int w = width();
    unsigned int h = height();


    unsigned int positionY = 0;
    for (unsigned int d = 0; d != _directions; ++d)
    {
        unsigned int positionX = 0;
        DatFileItem::setPosition(_dataOffset[d] + 62);
        for (unsigned int f = 0; f != _framesPerDirection; ++f)
        {
            this->skipBytes(4*3);
            // read frame data and add to _rgba
            for (unsigned int y = 0; y != height(d, f); ++y)
            {
                for (unsigned int x = 0; x != width(d, f); ++x)
                {
                    unsigned char index;
                    *this >> index;
                    if (index >=229 && index <= 232)
                    {
                        if (!_animatedMasks[MASK_SLIME])  _animatedMasks[MASK_SLIME] = new uint8_t [w*h]();
                        _animatedMasks[MASK_SLIME][((y + positionY)*w) + x + positionX] = index;
                    }
                    else if (index >=233 && index <= 237)
                    {
                        if (!_animatedMasks[MASK_MONITOR])  _animatedMasks[MASK_MONITOR] = new uint8_t [w*h]();
                        _animatedMasks[MASK_MONITOR][((y + positionY)*w) + x + positionX] = index;

                    }
                    else if (index >=238 && index <= 242)
                    {
                        if (!_animatedMasks[MASK_FIRE_SLOW])  _animatedMasks[MASK_FIRE_SLOW] = new uint8_t [w*h]();
                        _animatedMasks[MASK_FIRE_SLOW][((y + positionY)*w) + x + positionX] = index;
                    }
                    else if (index >=243 && index <= 247)
                    {
                        if (!_animatedMasks[MASK_FIRE_FAST])  _animatedMasks[MASK_FIRE_FAST] = new uint8_t [w*h]();
                        _animatedMasks[MASK_FIRE_FAST][((y + positionY)*w) + x + positionX] = index;
                    }
                    else if (index >=248 && index <= 253)
                    {
                        if (!_animatedMasks[MASK_SHORE])  _animatedMasks[MASK_SHORE] = new uint8_t [w*h]();
                        _animatedMasks[MASK_SHORE][((y + positionY)*w) + x + positionX] = index;
                    }
                    else if (index == 254)
                    {
                        if (!_animatedMasks[MASK_REDDOT])  _animatedMasks[MASK_REDDOT] = new uint8_t [w*h]();
                        _animatedMasks[MASK_REDDOT][((y + positionY)*w) + x + positionX] = index;
                    }
                    if (index >= 229 && index <= 254)
                    {
                        _animatedPalette = true;
                    }
                }
            }
            positionX += width(d);
        }
        positionY += height(d);
    }
    return &_animatedMasks;
}

}
