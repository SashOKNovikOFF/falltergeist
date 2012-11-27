#include "Engine/Animation.h"
#include "Engine/ResourceManager.h"
#include "Fallout/FrmFileType.h"
#include "Fallout/PalFileType.h"
#include <iostream>

namespace Falltergeist
{

Animation::Animation(const char * filename, int x, int y) : InteractiveSurface(0, 0, x, y)
{
    _surfaceSets = new std::vector<std::vector<Surface *> *>;
    _currentFrame = 0;
    _currentSurfaceSet = 0;
    _frameRate = 400;
    _lastTicks = SDL_GetTicks();
    loadFromFrmFile(filename);
}

void Animation::think()
{
    if(_lastTicks + _frameRate > SDL_GetTicks()) return;
    _lastTicks = SDL_GetTicks();
    _currentFrame++;
    if (_currentFrame >= _surfaceSets->at(_currentSurfaceSet)->size())
    {
        _currentFrame = 0;
    }
}

void Animation::loadFromFrmFile(const char * filename)
{
    FrmFileType * frm = ResourceManager::getFrmFileType(filename);
    if (!frm)
    {
        std::cout << "can't find FRM file " << filename << std::endl;
    }
    PalFileType * pal = ResourceManager::getPalFileType("color.pal");

    _frameRate = 1000 / frm->getFramesPerSecond();

    // for each direction
    for (unsigned int i = 0; i != 6; ++i)
    {
        if (i > 0 && frm->getDirections()[i].dataOffset == frm->getDirections()[0].dataOffset) break;
        std::vector<Surface *> * frameset = new std::vector<Surface *>;
        // for each frame
        for (unsigned int j = 0; j != frm->getFramesPerDirection(); ++j)
        {
            int width = frm->getDirections()[i].frames->width;
            int height = frm->getDirections()[i].frames->height;
            Surface * surface = new Surface(width,height);
            int z = 0;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // 12 - frame data offset
                    unsigned int colorIndex = frm->getDirections()[i].frames[j].data[z];
                    unsigned int color = *pal->getColor(colorIndex);
                    if (colorIndex == 0) color = 0;
                    surface->setPixel(x,y,color);
                    z++;
                }
            }
            frameset->push_back(surface);
        }
        _surfaceSets->push_back(frameset);
    }
}

SDL_Surface * Animation::getSurface()
{
    if (needRedraw)
    {
        draw();
        needRedraw = false;
    }
    return _surfaceSets->at(_currentSurfaceSet)->at(_currentFrame)->getSurface();
}


}