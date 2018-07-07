#include "tilemap.h"

TileMap::TileMap(int SizeX, int SizeY)
{

    Map.fill(QVector<int>().fill(0,SizeX),SizeY);
    WorldSizeX=SizeX;
    WorldSizeY=SizeY;
}

TileMap::~TileMap()
{
    Map.clear();
}

void TileMap::setWorldSize(int x,  int y)
{
    WorldSizeX=x;
    WorldSizeY=y;

    Map.resize(y);
    for (int i=0;i<y;i++)
        Map[i].resize(x);
}

int TileMap::getTileAt(int x, int y)
{
    if (x>=WorldSizeX) return 0;
    if (y>=WorldSizeY) return 0;
    return Map.at(y).at(x);
}

void TileMap::setTileAt(int x, int y, int Tile)
{
    Map[y][x] = Tile;
}
int TileMap::getSizeX() const
{
    if (WorldSizeX > 100000)
        return 1;
    return WorldSizeX;
}
int TileMap::getSizeY() const
{
    if (WorldSizeY > 100000)
        return 1;
    return WorldSizeY;
}

void TileMap::saveLayerAsText(QTextStream &stream)
{
    stream << "MAP SIZE X: " << QString::number(this->getSizeX()) << endl;
    stream << "MAP SIZE Y: " << QString::number(this->getSizeY()) << endl << endl;

    for (int j=0;j<this->getSizeY(); j++)
    {
        for (int i=0;i<this->getSizeX(); i++)
        {
            stream << QString::number( this->getTileAt(i,j) ) << " ";
        }
        stream << endl;
    }
}

void TileMap::clear()
{
   Map.clear();
}



