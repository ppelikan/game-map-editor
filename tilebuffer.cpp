#include "tilebuffer.h"

TileBuffer::TileBuffer()
{
    TileSizeY = 32;
    TileSizeX = 32;
}

TileBuffer::~TileBuffer()
{

}

int TileBuffer::LoadImage(QString FileName)
{
    QPixmap Img;
    Img.load(FileName);
    TileImages.clear();
    for (int j=0; j<Img.height()/TileSizeY;j++)
        for (int i=0; i<Img.width()/TileSizeX;i++)
        {
            TileImages.append(
                        new QPixmap(Img.copy(i*TileSizeX,
                                             j*TileSizeY,
                                             TileSizeX,
                                             TileSizeY) ));
        }
    return Img.width()/TileSizeX;
}

void TileBuffer::addLoadImage(QString FileName)
{
    QPixmap Img;
    Img.load(FileName);
    for (int j=0; j<Img.height()/TileSizeY;j++)
        for (int i=0; i<Img.width()/TileSizeX;i++)
        {
            TileImages.append(
                        new QPixmap(Img.copy(i*TileSizeX,
                                             j*TileSizeY,
                                             TileSizeX,
                                             TileSizeY) ));
        }
}

void TileBuffer::remove(int index)
{
    TileImages.removeAt(index);
}

void TileBuffer::clear()
{
    qDeleteAll(TileImages);
    TileImages.clear();
}

int TileBuffer::getTileSizeX() const
{
    return TileSizeX;
}

int TileBuffer::getTileSizeY() const
{
    return TileSizeY;
}

void TileBuffer::setTileSize(int x, int y)
{
    TileSizeX = x;
    TileSizeY = y;
}

int TileBuffer::getCount()
{
    return TileImages.size();
}

QPixmap *TileBuffer::getTilePixmapAt(int index)
{
    if (index < TileImages.size())
        return TileImages.at(index);
    else
        return NULL;
}
