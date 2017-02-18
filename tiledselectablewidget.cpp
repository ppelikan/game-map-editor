#include "tiledselectablewidget.h"

TiledSelectableWidget::TiledSelectableWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

TiledSelectableWidget::~TiledSelectableWidget()
{

}

void TiledSelectableWidget::paintEvent(QPaintEvent *)
{
    QPainter pt(this);
    pt.fillRect(this->rect(),Qt::SolidPattern);
    //pt.setPen(Qt::yellow);
    pt.setPen(Qt::red);
    pt.drawRect(GrabbedAreaRect);
    if (isMouseHovering && ! isMouseDown)
    {
        //pt.setPen(Qt::darkRed);
        pt.drawRect(HiglightRect);
    }
}


void TiledSelectableWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (this->rect().contains(ev->pos()))
    {
        if (ev->x()/TileSizeX >= NumTilesX ) return;
        if (ev->y()/TileSizeY >= NumTilesY ) return;

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

void TiledSelectableWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->x()/TileSizeX >= NumTilesX ) return;
    if (ev->y()/TileSizeY >= NumTilesY ) return;
    GrabbedAreaRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
    GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY);
    GrabbedAreaRect.setWidth(TileSizeX+1);
    GrabbedAreaRect.setHeight(TileSizeY+1);
    isMouseDown = true;
    this->repaint();
}

void TiledSelectableWidget::leaveEvent(QEvent *)
{
    isMouseHovering = false;
    isMouseDown = false;
    this->repaint();
}

void TiledSelectableWidget::mouseReleaseEvent(QMouseEvent *)
{
    isMouseDown = false;
    //emit onTilesSelected();
}

void TiledSelectableWidget::resizeEvent(QResizeEvent *)
{
    //repaintBuffer();
}
