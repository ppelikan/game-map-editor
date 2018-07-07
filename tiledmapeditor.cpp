#include "tiledmapeditor.h"

TiledMapEditor::TiledMapEditor(QWidget *parent) : QWidget(parent)
{
    isMouseHovering=false;
    isMouseDown = false;
    setMouseTracking(true);
    TileSizeX = 32;
    TileSizeY = 32;
    isSelectionModeOn=false;
    isModified = false;
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
    TranMaps.clear();
    this->repaint();
}

void TiledMapEditor::setAnimationBuffer(AnimationBuffer *animbuffer)
{
    AnimBuffer = animbuffer;
}

void TiledMapEditor::paintEvent(QPaintEvent *)
{
    QPainter pt(this);
    pt.setBackground(Qt::black);
    QPixmap *frame;

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
                {
                    frame = AnimBuffer->getFramePixmap(- Map->getTileAt(i,j) -1 );
                    if (frame != NULL)
                        pt.drawPixmap(i*TileSizeX,
                                      j*TileSizeY,
                                      *frame );
                }
                else
                    pt.drawPixmap(i*TileSizeX,
                                  j*TileSizeY,
                                  *Buffer->getTilePixmapAt(Map->getTileAt(i,j)));
            }
        }

    if (TranMaps.count() > 0)
    {
        pt.setOpacity(0.3);
        for (int k=0; k<TranMaps.count(); k++)
        {
            for (int j=VisibleRect.top();j<VisibleRect.bottom(); j++)
                for (int i=VisibleRect.left();i<VisibleRect.right(); i++)
                {
                    if (TranMaps[k]->getTileAt(i,j) < Buffer->getCount())
                    {
                        if (TranMaps[k]->getTileAt(i,j) < 0)
                        {
                            frame = AnimBuffer->getFramePixmap(- TranMaps[k]->getTileAt(i,j) -1 );
                            if (frame != NULL)
                                pt.drawPixmap(i*TileSizeX,
                                              j*TileSizeY,
                                              *frame );
                        }
                        else
                            pt.drawPixmap(i*TileSizeX,
                                          j*TileSizeY,
                                          *Buffer->getTilePixmapAt(TranMaps[k]->getTileAt(i,j)));
                    }
                }
        }
        pt.setOpacity(1);
    }

    if (isMatrixVisible)
    {
        QTextOption opt(Qt::AlignCenter);
        opt.setWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );

        QFont font = pt.font() ;
        font.setPointSize( 8 );
        pt.setFont(font);
        pt.setBrush(Qt::black);

        for (int j=VisibleRect.top();j<VisibleRect.bottom(); j++)
            for (int i=VisibleRect.left();i<VisibleRect.right(); i++)
            {
                if ( Matrix->getNumsAt(i,j) != NULL )
                {
                    if (! Matrix->getNumsAt(i,j)->isEmpty())
                    {
                        QString sBuf = Matrix->getAllNamesAt(i,j);

                        pt.setOpacity(0.5);
                        pt.setPen(Qt::transparent);
                        pt.drawRect(QRect(i*TileSizeX,j*TileSizeY,TileSizeX,TileSizeY));
                        pt.setOpacity(1);
                        pt.setPen(Qt::white);
                        pt.drawText(QRect(i*TileSizeX+1,j*TileSizeY-1,TileSizeX,TileSizeY), sBuf, opt);
                    }
                }
            }
    }
    pt.setBrush(Qt::transparent);
    pt.setBrush(QColor(255,250,250,45));

    if (isSelectionModeOn)
    {
        pt.setPen(Qt::red);
        pt.drawRect(SelectionRect.adjusted(0,0,-1,-1));

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
                        {
                            frame = AnimBuffer->getFramePixmap( - CursorTiles[j][i] -1 );
                            if (frame != NULL)
                            pt.drawPixmap((Cursor.x()+i)*TileSizeX,
                                          (Cursor.y()+j)*TileSizeY,
                                          *frame);
                        }
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
            isModified = true;
        }
        emit scrollDir(ev->pos().x()/TileSizeX, ev->pos().y()/TileSizeY);
    }
    emit mouseMove(ev->pos().x()/TileSizeX, ev->pos().y()/TileSizeY);
    this->repaint();
}

void TiledMapEditor::mousePressEvent(QMouseEvent *ev)
{
    if (ev->x() > Map->getSizeX()*TileSizeX) return;
    if (ev->y() > Map->getSizeY()*TileSizeY) return;

    Cursor.setX(ev->pos().x()/TileSizeX);
    Cursor.setY(ev->pos().y()/TileSizeY);

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
        isModified = true;
    }
    isMouseDown = true;
    this->repaint();
}

void TiledMapEditor::mouseReleaseEvent(QMouseEvent *ev)
{
    isMouseDown=false;
    Cursor.setX(ev->pos().x()/TileSizeX);
    Cursor.setY(ev->pos().y()/TileSizeY);
    if (isModified)
        emit editingFinished();
    isModified = false;
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
    emit editingFinished();
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

void TiledMapEditor::addTransparentLayer(TileMap *map)
{
    TranMaps.append(map);
}

void TiledMapEditor::showEventMatrix()
{
    isMatrixVisible = true;
}

void TiledMapEditor::hideEventMatrix()
{
    isMatrixVisible = false;
}

void TiledMapEditor::setEventMatrix(EventMatrix *ev)
{
    Matrix = ev;
}
