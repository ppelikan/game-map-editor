#ifndef TILEBUFFER_H
#define TILEBUFFER_H

#include <QPixmap>
#include <QString>

class TileBuffer
{
public:
    TileBuffer();
    ~TileBuffer();

    int LoadImage(QString FileName);
    void addLoadImage(QString FileName);
    void remove(int index);
    void clear();
    int getTileSizeX() const;
    int getTileSizeY() const;
    void setTileSize(int x, int y);
    int getCount();

    QPixmap *getTilePixmapAt(int index);

private:
    QList<QPixmap*> TileImages;
    int TileSizeX, TileSizeY;
};

#endif // TILEBUFFER_H
