#ifndef ANIMATIONBUFFER_H
#define ANIMATIONBUFFER_H

#include <QPixmap>
#include <QVector>
#include <QtCore>

#include "tilebuffer.h"

//enum AnimationType {

//};

#define ANIMATION_INTERVAL  20  //w milisekundach (1/1000s)

struct Animation {
    int Fps;
    QVector<int> Frames;  //każda klatka animacji jest indexem z TileBuffer'a
    bool isPingPong;

    int CurrentFrame;
    int delay;
};

class AnimationBuffer : public QObject
{
    Q_OBJECT

public:
    AnimationBuffer();
    ~AnimationBuffer();
    void setTileBuffer(TileBuffer *buf);
    QPixmap *getFramePixmap(int AnimationIndex);
    QPixmap *getFirstFramePixmap(int AnimationIndex);
    void syncAllAnimations();
    void clear();

    QVector<Animation> Animations;

private:
    TileBuffer *Tiles;
    QTimer *timer;

public slots:
    void UpdateAnimations();

signals:
    void onAnimationUpdate();
};



#endif // ANIMATIONBUFFER_H
