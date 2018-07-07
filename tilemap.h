#ifndef TILEMAP_H
#define TILEMAP_H

#include <QVector>
#include <QTextStream>

class TileMap
{
public:
    TileMap(int SizeX = 50, int SizeY = 50);
    ~TileMap();

    void setWorldSize(int x, int y);
    int getTileAt(int x, int y);
    void setTileAt(int x, int y, int Tile);

    int getSizeX() const;
    int getSizeY() const;

    void saveLayerAsText(QTextStream &stream);
    void clear();

private:
    int WorldSizeX, WorldSizeY;
    QVector<QVector<int> > Map;
};

#endif // TILEMAP_H
