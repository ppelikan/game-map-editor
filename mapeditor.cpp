#include "mapeditor.h"

MapEditor::MapEditor(QWidget *parent) : QWidget(parent)
{
    this->setTileSize(32,32);
    isMouseHovering=false;
       this->resize(32,32);
    this->setMinimumSize(32,32);
    canDrawCursorImg = false;
        setMouseTracking(true);
}

MapEditor::~MapEditor()
{

}

void MapEditor::setDrawingImg(QPixmap *img)
{
    CursorImg = img;
    canDrawCursorImg = true;
}

void MapEditor::paintEvent(QPaintEvent *)
{
    QPainter pt(this);

  //  if (isMouseHovering )
    {
        if (canDrawCursorImg)
            pt.drawPixmap(Cursor.x()*TileSizeX,Cursor.y()*TileSizeY,*CursorImg);
    }

}

void MapEditor::mouseMoveEvent(QMouseEvent * ev)
{
    if (this->rect().contains(ev->pos()))
    {
        isMouseHovering=true;
        Cursor.setX(ev->pos().x()/TileSizeX);
        Cursor.setY(ev->pos().y()/TileSizeY);
    }
    this->repaint();
}

void MapEditor::setTileSize(int x, int y)
{
    TileSizeX = x;
    TileSizeY = y;
}
