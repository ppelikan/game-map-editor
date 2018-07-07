#include "tileselector.h"

TileSelector::TileSelector(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    TilesImageBuf = new QPixmap(this->width(),this->height());
    QPainter pt(TilesImageBuf);
    pt.fillRect(TilesImageBuf->rect(), Qt::SolidPattern);
    NumTilesInAxisX = 5;
    isMouseHovering = false;
    isMouseDown = false;
}

TileSelector::~TileSelector()
{

}

void TileSelector::paintEvent(QPaintEvent *)
{
    QPainter pt(this);
    pt.fillRect(this->rect(),Qt::SolidPattern);
    pt.drawPixmap(0,0,*TilesImageBuf);
    if (Buffer->getCount() <= 0) return;
    pt.setBrush(QColor(255,250,250,45));
    pt.setPen(Qt::red);
    pt.drawRect(GrabbedAreaRect.adjusted(0,0,-1,-1));
    if (isMouseHovering && ! isMouseDown)
    {
        pt.drawRect(HiglightRect);
    }
}

void TileSelector::mouseMoveEvent(QMouseEvent *ev)
{
    if (this->rect().contains(ev->pos()))
    {
        if (ev->x()/TileSizeX >= TilesImageBuf->width()/TileSizeX ) return;
        if (ev->y()/TileSizeY >= TilesImageBuf->height()/TileSizeY ) return;

        if (isMouseDown)
        {
            if (ev->x() < HiglightRect.left())
                GrabbedAreaRect.setLeft((ev->x()/TileSizeX)*TileSizeX); else
            {
                GrabbedAreaRect.setRight((ev->x()/TileSizeX +1)*TileSizeX);
                GrabbedAreaRect.setLeft(HiglightRect.left());
            }
            if (ev->y() < HiglightRect.top())
                GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY); else
            {
                GrabbedAreaRect.setBottom((ev->y()/TileSizeY+1)*TileSizeY);
                GrabbedAreaRect.setTop(HiglightRect.top());
            }
        } else
        {
            HiglightRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
            HiglightRect.setTop((ev->y()/TileSizeY)*TileSizeY);
            HiglightRect.setWidth(TileSizeX);
            HiglightRect.setHeight(TileSizeY);
        }
        isMouseHovering = true;
    }
    isMouseDown = ev->buttons() & Qt::LeftButton;
    this->repaint();
}

void TileSelector::mousePressEvent(QMouseEvent *ev)
{
    if (ev->x()/TileSizeX >= TilesImageBuf->width()/TileSizeX ) return;
    if (ev->y()/TileSizeY >= TilesImageBuf->height()/TileSizeY ) return;
    GrabbedAreaRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
    GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY);
    GrabbedAreaRect.setWidth(TileSizeX+1);
    GrabbedAreaRect.setHeight(TileSizeY+1);
    isMouseDown = true;
    this->repaint();
}

void TileSelector::leaveEvent(QEvent *)
{
    isMouseHovering = false;
    isMouseDown = false;
    this->repaint();
}

void TileSelector::mouseReleaseEvent(QMouseEvent *ev)
{
    isMouseDown = false;
    HiglightRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
    HiglightRect.setTop((ev->y()/TileSizeY)*TileSizeY);
    HiglightRect.setWidth(TileSizeX);
    HiglightRect.setHeight(TileSizeY);
    emit onTilesSelected();
}

void TileSelector::resizeEvent(QResizeEvent *)
{
    //repaintBuffer();
}

void TileSelector::setTileBuffer(TileBuffer *tilebuffer)
{
    Buffer = tilebuffer;
    TileSizeX = tilebuffer->getTileSizeX();
    TileSizeY = tilebuffer->getTileSizeY();

    repaintBuffer();
    this->repaint();
}

QVector<QVector<int> > TileSelector::getSelectedTiles()
{
    QVector<QVector<int> > ret;
    QVector<int> buf;
    int tmp;

    if (Buffer->getCount() <= 0) return ret;

    for (int j=GrabbedAreaRect.top()/TileSizeY; j<GrabbedAreaRect.bottom()/TileSizeY; j++ )
    {
        for (int i=GrabbedAreaRect.left()/TileSizeX; i<GrabbedAreaRect.right()/TileSizeX; i++ )
        {
            tmp = i+(j*TilesImageBuf->width()/TileSizeX);
            if (tmp < Buffer->getCount())
                buf.append(tmp);
            //         qDebug()<<QString::number(i+(j*TilesImageBuf->width()/TileSizeX));
        }
        //       qDebug()<< "\n";
        ret.append(buf);
        buf.clear();
    }
    return ret;
}

void TileSelector::IncTextureWidth()
{
    NumTilesInAxisX ++;
    repaintBuffer();
}

void TileSelector::DecTextureWidth()
{
    NumTilesInAxisX --;
    if (NumTilesInAxisX < 1) NumTilesInAxisX = 1;
    repaintBuffer();
}

void TileSelector::setTextureWidth(int NumberOfTilesInAxisX)
{
    NumTilesInAxisX = NumberOfTilesInAxisX;
    repaintBuffer();
}

int TileSelector::getTextureWidth()
{
    return NumTilesInAxisX;
}

QPixmap *TileSelector::getTexture() const
{
    return TilesImageBuf;
}

void TileSelector::repaintBuffer()
{
    //  if (this->width() <=0) return;
    //if (this->height() <=0) return;
    if (Buffer->getCount() <= 0)
    {
        this->setMinimumWidth(0);
        this->setMinimumHeight(0);
        this->resize(TileSizeX,TileSizeY);

        delete TilesImageBuf;
        TilesImageBuf = new QPixmap(TileSizeX,TileSizeY);

        QPainter pt(TilesImageBuf);
        pt.fillRect(TilesImageBuf->rect(), Qt::SolidPattern);

        return;
    }
    //    int h = TileSizeY*(Buffer->getCount()/(this->width()/TileSizeX));
    //    if (Buffer->getCount()%(this->width()/TileSizeX) > 0)
    //        h += TileSizeY;

    int h = TileSizeY*(Buffer->getCount()/ NumTilesInAxisX );
    if (Buffer->getCount()%NumTilesInAxisX > 0)
        h += TileSizeY;

    delete TilesImageBuf;
    TilesImageBuf = new QPixmap(NumTilesInAxisX*TileSizeX, h );

    this->setMinimumWidth(TilesImageBuf->width());
    this->setMinimumHeight(TilesImageBuf->height());
    this->resize(TilesImageBuf->width(),TilesImageBuf->height());
    //  this->setGeometry(0,0,TilesImageBuf->width(),TilesImageBuf->height());
    //  this->setMaximumWidth(TilesImageBuf->width());

    QPainter pt(TilesImageBuf);
    pt.fillRect(TilesImageBuf->rect(), Qt::SolidPattern);

    int xx=0;
    int yy=0;
    for (int i=0; i<Buffer->getCount(); i++)
    {
        if (! Buffer->getTilePixmapAt(i)->isNull())
            pt.drawPixmap(xx*TileSizeX,yy*TileSizeY, *Buffer->getTilePixmapAt(i));
        xx++;
        if (xx > NumTilesInAxisX -1)
        {xx =0;
            yy++; }
    }
}

