#include "tilemap.h"

TileMap::TileMap()
{
//Map = new QVector<int(50);
//    for (int i=0;i<50;i++)
//        Map[i] = new QVector(50);
        Map.fill(QVector<int>().fill(0,50),50);
        WorldSizeX=50;
        WorldSizeY=50;
}

TileMap::TileMap(int SizeX, int SizeY)
{
   // Map = new QVector(SizeY);

    Map.fill(QVector<int>().fill(0,SizeX),SizeY);
    WorldSizeX=SizeX;
    WorldSizeY=SizeY;
    //setWorldSize(SizeX,SizeY);  //redundantnte, wiem... ale inaczej się wykszacza
}

TileMap::~TileMap()
{
    Map.clear();
}

void TileMap::setWorldSize(int x, int y)
{
    WorldSizeX=x;
    WorldSizeY=y;

    Map.resize(y);
    for (int i=0;i<y;i++)
        Map[i].resize(x);
}

int TileMap::getTileAt(int x, int y)
{
    // return Map[x][y];
    return Map.at(y).at(x);
}

void TileMap::setTileAt(int x, int y, int Tile)
{
    Map[y][x] = Tile;
}
int TileMap::getSizeX() const
{
    return WorldSizeX;
}
int TileMap::getSizeY() const
{
    return WorldSizeY;
}



