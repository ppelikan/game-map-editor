#ifndef TILEMAP_H
#define TILEMAP_H

#include <QVector>

class TileMap
{
public:
    TileMap();
    TileMap(int SizeX, int SizeY);
    ~TileMap();

    void setWorldSize(int x, int y);
    int getTileAt(int x, int y);
    void setTileAt(int x, int y, int Tile);

    int getSizeX() const;
    int getSizeY() const;

private:
    int WorldSizeX, WorldSizeY;
    QVector<QVector<int> > Map;
};

#endif // TILEMAP_H
