#include "tiledmapeditor.h"

TiledMapEditor::TiledMapEditor(QWidget *parent) : QWidget(parent)
{
    isMouseHovering=false;
    isMouseDown = false;
    setMouseTracking(true);
    //   MapImg = new QPixmap(TileSizeX*50,TileSizeY*50);
}

TiledMapEditor::~TiledMapEditor()
{

}

void TiledMapEditor::setTileBuffer(TileBuffer *tilebuffer)
{
    Buffer = tilebuffer;
    TileSizeX = Buffer->getTileSizeX();
    TileSizeY = Buffer->getTileSizeY();
    CursorTiles.clear();
    this->repaint();
}

void TiledMapEditor::setTileMap(TileMap *tilemap)
{
    Map = tilemap;
    this->setMinimumSize(Map->getSizeX()*TileSizeX,Map->getSizeY( )*TileSizeY);
    //repaintMapImg();
    this->repaint();
}

void TiledMapEditor::setAnimationBuffer(AnimationBuffer *animbuffer)
{
    AnimBuffer = animbuffer;
}

void TiledMapEditor::paintEvent(QPaintEvent *)
{
    QPainter pt(this);

    if (Buffer->getCount() <= 0)
    {
        pt.fillRect(this->rect(),Qt::SolidPattern);
        return;
    }

    for (int j=VisibleRect.top();j<VisibleRect.bottom(); j++)
        for (int i=VisibleRect.left();i<VisibleRect.right(); i++)
        {
            if (Map->getTileAt(i,j) < Buffer->getCount())
            {

                if (Map->getTileAt(i,j) < 0)
                    pt.drawPixmap(i*TileSizeX,
                                  j*TileSizeY,
                                  *AnimBuffer->getFramePixmap(- Map->getTileAt(i,j) -1 )); else
                    pt.drawPixmap(i*TileSizeX,
                                  j*TileSizeY,
                                  *Buffer->getTilePixmapAt(Map->getTileAt(i,j)));
            }
        }

    if (isSelectionModeOn)
    {
        pt.setPen(Qt::red);
        pt.drawRect(SelectionRect);

        if (! isMouseDown)
            if (isMouseHovering)
                pt.drawRect(Cursor.x()*TileSizeX,Cursor.y()*TileSizeY,TileSizeX,TileSizeY);

    } else
        if (isMouseHovering )
        {
            for (int j=0;j<CursorTiles.size(); j++)
                for (int i=0;i<CursorTiles[j].size(); i++)
                    if ((Cursor.x()+i < Map->getSizeX()) &&
                            (Cursor.y()+j < Map->getSizeY()))
                    {
                        if (CursorTiles[j][i] < 0)
                            pt.drawPixmap((Cursor.x()+i)*TileSizeX,
                                          (Cursor.y()+j)*TileSizeY,
                                          *AnimBuffer->getFramePixmap( - CursorTiles[j][i] -1 ));
                        else
                            pt.drawPixmap((Cursor.x()+i)*TileSizeX,
                                          (Cursor.y()+j)*TileSizeY,
                                          *Buffer->getTilePixmapAt( CursorTiles[j][i]) );
                    }
        }
}

void TiledMapEditor::mouseMoveEvent(QMouseEvent * ev)
{
    if (! this->rect().contains(ev->pos())) return;
    if (ev->x() > Map->getSizeX()*TileSizeX-1) return;
    if (ev->y() > Map->getSizeY()*TileSizeY-1) return;

    isMouseHovering=true;

    if ( !(isSelectionModeOn && isMouseDown) )
    {
        Cursor.setX(ev->pos().x()/TileSizeX);
        Cursor.setY(ev->pos().y()/TileSizeY);
    }

    if (isMouseDown)
    {
        if (isSelectionModeOn)
        {

            if (ev->x() < Cursor.x()*TileSizeX)
                SelectionRect.setLeft((ev->x()/TileSizeX)*TileSizeX); else
            {
                SelectionRect.setRight((ev->x()/TileSizeX +1)*TileSizeX);
                SelectionRect.setLeft(Cursor.x()*TileSizeX);
            }
            if (ev->y() < Cursor.y()*TileSizeY)
                SelectionRect.setTop((ev->y()/TileSizeY)*TileSizeY); else
            {
                SelectionRect.setBottom((ev->y()/TileSizeY+1)*TileSizeY);
                SelectionRect.setTop(Cursor.y()*TileSizeY);
            }

        }else
        {
            for (int j=0;j<CursorTiles.size(); j++)
                for (int i=0;i<CursorTiles[j].size(); i++)
                    if ((Cursor.x()+i < Map->getSizeX()) &&
                            (Cursor.y()+j < Map->getSizeY()))
                        Map->setTileAt((Cursor.x()+i),
                                       (Cursor.y()+j),
                                       CursorTiles[j][i] );
        }
        emit scrollDir(ev->pos().x()/TileSizeX, ev->pos().y()/TileSizeY);
    }
    emit mouseMove(ev->pos().x()/TileSizeX, ev->pos().y()/TileSizeY);
    //repaintMapImg();
    this->repaint();
}

void TiledMapEditor::mousePressEvent(QMouseEvent *ev)
{
    if (ev->x() > Map->getSizeX()*TileSizeX) return;
    if (ev->y() > Map->getSizeY()*TileSizeY) return;

    if (isSelectionModeOn)
    {
        SelectionRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
        SelectionRect.setTop((ev->y()/TileSizeY)*TileSizeY);
        SelectionRect.setWidth(TileSizeX+1);
        SelectionRect.setHeight(TileSizeY+1);
    } else
    {
        for (int j=0;j<CursorTiles.size(); j++)
            for (int i=0;i<CursorTiles[j].size(); i++)
                if ((Cursor.x()+i < Map->getSizeX()) &&
                        (Cursor.y()+j < Map->getSizeY()))
                    Map->setTileAt((Cursor.x()+i),
                                   (Cursor.y()+j),
                                   CursorTiles[j][i] );
    }
    isMouseDown = true;
    //   repaintMapImg();
    this->repaint();
}

void TiledMapEditor::mouseReleaseEvent(QMouseEvent *)
{
    isMouseDown=false;
}

void TiledMapEditor::leaveEvent(QEvent *)
{
    if (isMouseDown)
    {
        return;
    }
    isMouseDown=false;
    isMouseHovering = false;
    this->repaint();
}

void TiledMapEditor::setTileSize(int x, int y)
{
    TileSizeX = x;
    TileSizeY = y;
    this->setMinimumSize(Map->getSizeX()*TileSizeX,Map->getSizeY( )*TileSizeY);
    //  repaintMapImg();
    this->repaint();
}

void TiledMapEditor::setCursorTiles(QVector<QVector<int> > cur)
{
    CursorTiles.clear();
    CursorTiles = cur;
}

QVector<QVector<int> > TiledMapEditor::getCursorTiles()
{
    return CursorTiles;
}

void TiledMapEditor::setVisibleRect(QRect rect)
{
    if (rect.left() >= 0)
        VisibleRect.setLeft(rect.left()/TileSizeX);
    if (rect.top() >= 0)
        VisibleRect.setTop(rect.top()/TileSizeY);
    if (rect.right()/TileSizeX+1 <= Map->getSizeX())
        VisibleRect.setRight(rect.right()/TileSizeX+1);
    else VisibleRect.setRight(Map->getSizeX());
    if (rect.bottom()/TileSizeY+1 <= Map->getSizeY())
        VisibleRect.setBottom(rect.bottom()/TileSizeY+1);
    else VisibleRect.setBottom(Map->getSizeY());
    this->setMinimumSize(Map->getSizeX()*TileSizeX,Map->getSizeY( )*TileSizeY);
    this->repaint();
}

void TiledMapEditor::setSelectionMode(bool mode)
{
    isSelectionModeOn = mode;
}

bool TiledMapEditor::isInSelectionMode()
{
    return isSelectionModeOn;
}

QVector<QVector<int> > TiledMapEditor::getSelectionMap()
{
    QVector<QVector<int> > ret;
    QVector<int> buf;

    for (int j=SelectionRect.top()/TileSizeY; j<SelectionRect.bottom()/TileSizeY; j++ )
    {
        for (int i=SelectionRect.left()/TileSizeX; i<SelectionRect.right()/TileSizeX; i++ )
        {
            buf.append( Map->getTileAt(i,j) );
        }
        ret.append(buf);
        buf.clear();
    }
    return ret;
}

QRect TiledMapEditor::getSelectionRect()
{
    return QRect(SelectionRect.left()/TileSizeX,
                 SelectionRect.top()/TileSizeY,
                 SelectionRect.width()/TileSizeX +1,
                 SelectionRect.height()/TileSizeY +1);
}

void TiledMapEditor::fillWithTileCursor()
{
    if (CursorTiles.size() <= 0) return;

    int bx,by;
    int ex,ey;
    if (isSelectionModeOn)
    {
        bx = SelectionRect.left()/TileSizeX;
        by = SelectionRect.top()/TileSizeY;
        ex = SelectionRect.right()/TileSizeX;
        ey = SelectionRect.bottom()/TileSizeY;
    } else
    {
        bx = 0;
        by = 0;
        ex = Map->getSizeX();
        ey = Map->getSizeY();
    }

    int tx=0;
    int ty=0;
    for (int j=by; j<ey;j++)
    {
        for (int i=bx; i<ex;i++)
        {
            Map->setTileAt(i,j, CursorTiles[ty][tx] );
            tx++;
            if (tx >= CursorTiles[0].size()) tx = 0;
        }
        ty++;
        if (ty >= CursorTiles.size()) ty = 0;
    }

    this->repaint();
}

QPixmap *TiledMapEditor::getMapPixmap() const
{
    QPixmap *img = new QPixmap(this->size());
    QPainter pt(img);

    for (int j=0;j<Map->getSizeY(); j++)
        for (int i=0;i<Map->getSizeX(); i++)
        {
            if (Map->getTileAt(i,j) < Buffer->getCount())
            {

                if (Map->getTileAt(i,j) < 0)
                    pt.drawPixmap(i*TileSizeX,
                                  j*TileSizeY,
                                  *AnimBuffer->getFirstFramePixmap(- Map->getTileAt(i,j) -1 )); else
                    pt.drawPixmap(i*TileSizeX,
                                  j*TileSizeY,
                                  *Buffer->getTilePixmapAt(Map->getTileAt(i,j)));
            }
        }
    return img;
}

//void TiledMapEditor::repaintMapImg()
//{
////    if (MapImg != NULL)
//        delete MapImg;
//    MapImg = new QPixmap(TileSizeX*Map->getSizeX(),TileSizeY*Map->getSizeY());

//    this->setMinimumSize(Map->getSizeX()*TileSizeX,Map->getSizeY( )*TileSizeY);

//    if (Buffer->getCount() <= 0) return;

//    QPainter pt(MapImg);

//    for (int j=0;j<Map->getSizeY(); j++)
//        for (int i=0;i<Map->getSizeX(); i++)
//        {
//            //         if (Map->getTileAt(i,j) < Buffer->getCount())
//            //       {
//            //                if (Map->getTileAt(i,j) < 0)
//            //                    pt.drawPixmap(i*TileSizeX,
//            //                                  j*TileSizeY,
//            //                                  *AnimBuffer->getFramePixmap(- Map->getTileAt(i,j) -1 )); else
//            if (Map->getTileAt(i,j) >= 0)
//                pt.drawPixmap(i*TileSizeX,
//                              j*TileSizeY,
//                              *Buffer->getTilePixmapAt(Map->getTileAt(i,j)));
//            //     }
//        }
//    this->repaint();

//}
