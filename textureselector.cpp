#include "textureselector.h"

TextureSelector::TextureSelector(QWidget *parent) : QWidget(parent)
{
    TextureImg = new QPixmap();
    setMouseTracking(true);
    TileSizeX =32;
    TileSizeY =32;
    HiglightRect.setWidth(32);
    HiglightRect.setHeight(32);
    GrabbedAreaRect.setRect(0,0,0,0);
    //TileImages = new QList();
    //GrabbedAreaRect.setWidth(0);
    //= new QRect(0,0,32,32);
    this->resize(32,32);
    this->setMinimumSize(32,32);
}

TextureSelector::~TextureSelector()
{
    delete TextureImg;
    for (int i=0; i<TileImages.size();i++)
        delete TileImages.at(i);
    TileImages.clear();
}

void TextureSelector::LoadImage(QString FileName)
{
    TextureImg->load(FileName);
    for (int j=0; j<TextureImg->height()/TileSizeY;j++)
        for (int i=0; i<TextureImg->width()/TileSizeX;i++)
        {
            TileImages.append(
                        new QPixmap(TextureImg->copy(i*TileSizeX,
                                                     j*TileSizeY,
                                                     TileSizeX,
                                                     TileSizeY) ));
        }
    this->setMinimumSize(TextureImg->width(),TextureImg->height());
    this->resize(TextureImg->width(),TextureImg->height());
}

//void TextureSelector::LoadNewTile(QString FileName)  //TODO
//{
//    TextureImg->load(FileName);
//    for (int j=0; j<TextureImg->height()/TileSizeY;j++)
//        for (int i=0; i<TextureImg->width()/TileSizeX;i++)
//        {
//            TileImages.append(
//                        new QPixmap(TextureImg->copy(i*TileSizeX,
//                                                     j*TileSizeY,
//                                                     i*TileSizeX+TileSizeX,
//                                                     j*TileSizeY+TileSizeY) );
//        }
//}

void TextureSelector::paintEvent(QPaintEvent *)
{
    QPainter pt(this);
    pt.drawPixmap(0,0,*TextureImg);
    //pt.setPen(Qt::yellow);
    pt.setPen(Qt::red);
    pt.drawRect(GrabbedAreaRect);
    if (isMouseHovering && ! isMouseDown)
    {
        pt.setPen(Qt::darkRed);
        pt.drawRect(HiglightRect);
    }
}

void TextureSelector::mouseMoveEvent(QMouseEvent * ev)
{
    if (this->rect().contains(ev->pos()))
    {
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
            //            GrabbedAreaRect.setRect(SelectionRect.normalized().left(),
            //                                    SelectionRect.normalized().top(),
            //                                    SelectionRect.normalized().width(),
            //                                    SelectionRect.normalized().height());
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

void TextureSelector::mousePressEvent(QMouseEvent *ev)
{
    //    GrabbedAreaRect.setRect(SelectionRect.normalized().left(),
    //                            SelectionRect.normalized().top(),
    //                            SelectionRect.normalized().width(),
    //                            SelectionRect.normalized().height());
    GrabbedAreaRect.setLeft((ev->x()/TileSizeX)*TileSizeX);
    GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY);
    GrabbedAreaRect.setWidth(TileSizeX);
    GrabbedAreaRect.setHeight(TileSizeY);
    this->repaint();
}

void TextureSelector::leaveEvent(QEvent *)
{
    isMouseHovering = false;
    isMouseDown = false;
    this->repaint();
}

void TextureSelector::mouseReleaseEvent(QMouseEvent *S)
{
    emit TextureSelected();
}

int TextureSelector::getTileSizeX() const
{
    return TileSizeX;
}

int TextureSelector::getTileSizeY() const
{
    return TileSizeY;
}

void TextureSelector::setTileSize(int x, int y)
{
    TileSizeX = x;
    TileSizeY = y;
}

QPixmap *TextureSelector::getSelectedPixmap()
{
    return new QPixmap( TextureImg->copy(GrabbedAreaRect) );
}

QList<QList<int> > TextureSelector::getSelectedTiles()
{
    QList<QList<int> > ret;
    QList<int> buf;

    for (int j=GrabbedAreaRect.top()/TileSizeY; j<GrabbedAreaRect.bottom()/TileSizeY; j++ )
    {
        for (int i=GrabbedAreaRect.left()/TileSizeX; i<GrabbedAreaRect.right()/TileSizeX; i++ )
        {
            buf.append(i+(j*TextureImg->height()/TileSizeY));
            qDebug()<<QString::number(i+(j*TextureImg->height()/TileSizeY));
        }
        qDebug()<< "\n";
        ret.append(buf);
        buf.clear();
    }

    return ret;
}

//void TextureSelector::TextureSelected()
//{

//}

