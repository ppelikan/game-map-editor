#ifndef ENTIRELEVEL_H
#define ENTIRELEVEL_H

#include <QVector>

#include "tilemap.h"
#include "tilebuffer.h"
#include "animationbuffer.h"
#include "eventmatrix.h"

class EntireLevel
{
public:
    EntireLevel();
    ~EntireLevel();
    void setTileSize(int x, int y);
//    int getTileWidth();
//    int getTileHeight();
    void saveToStream(QDataStream &out);
    void loadFromStream(QDataStream &in);
    void setName(QString s);
    void clear();
    void clearAll();

    EventMatrix *Matrix;
    QVector<TileMap *> Layers;
    TileBuffer *Buffer;
    AnimationBuffer *AnimBuffer;
    QString Name;
};

#endif // ENTIRELEVEL_H
