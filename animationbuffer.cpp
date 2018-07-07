#include "animationbuffer.h"

AnimationBuffer::AnimationBuffer()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(UpdateAnimations()));
    timer->start(ANIMATION_INTERVAL);
}

AnimationBuffer::~AnimationBuffer()
{
    timer->stop();
    this->clear();
}

void AnimationBuffer::setTileBuffer(TileBuffer *buf)
{
    Tiles = buf;
}

QPixmap *AnimationBuffer::getFramePixmap(int AnimationIndex)
{
    if (AnimationIndex >= Animations.size()) return NULL;
    if (Animations[AnimationIndex].Frames.size() <= 0 ) return NULL;
    return Tiles->getTilePixmapAt( Animations[AnimationIndex].Frames[
                                   abs( Animations[AnimationIndex].CurrentFrame)]
            );
}

QPixmap *AnimationBuffer::getFirstFramePixmap(int AnimationIndex)
{
    if (AnimationIndex >= Animations.size()) return NULL;
    if (Animations[AnimationIndex].Frames.size() <= 0 ) return NULL;
    return Tiles->getTilePixmapAt( Animations[AnimationIndex].Frames[0]);
}

void AnimationBuffer::syncAllAnimations()
{
    for (int i=0; i<Animations.size(); i++)
    {
        Animations[i].delay = 0;
        Animations[i].CurrentFrame = 0;
    }
}

void AnimationBuffer::clear()
{
    Animations.clear();
}

void AnimationBuffer::UpdateAnimations()
{
    bool hasSomethingChanged = false;
    for (int i=0; i<Animations.size(); i++)
    {
        if (Animations[i].Fps <= 0) continue;

        Animations[i].delay++;
        if (Animations[i].delay >= (1000/ANIMATION_INTERVAL)/(Animations[i].Fps))
        {
            Animations[i].delay = 0;
            Animations[i].CurrentFrame ++;
            if (Animations[i].CurrentFrame >= Animations[i].Frames.size())
            {
                if (Animations[i].isPingPong)
                    Animations[i].CurrentFrame = - Animations[i].Frames.size()+2; else
                    Animations[i].CurrentFrame = 0;
            }
            hasSomethingChanged = true;
        }
    }
    if (hasSomethingChanged) emit onAnimationUpdate();
}
