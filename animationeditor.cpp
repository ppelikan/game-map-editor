#include "animationeditor.h"

AnimationEditor::AnimationEditor(QWidget *parent) : QWidget(parent)
{
    isMouseHovering=false;
    isMouseDown = false;
    setMouseTracking(true);
    BiggestAnimation = 0;
    MouseHoverIndex =-1;
    PlusIconImg = new QPixmap(":/new/prefix1/add.png");
    DeleteIconImg = new QPixmap(":/new/prefix1/ics/1455649000_Erase.png");
}

AnimationEditor::~AnimationEditor()
{

}

void AnimationEditor::setTileBuffer(TileBuffer *tilebuffer)
{
    Buffer = tilebuffer;
    TileSizeX = Buffer->getTileSizeX();
    TileSizeY = Buffer->getTileSizeY();
    CursorTiles.clear();
    this->repaint();
}

void AnimationEditor::setAnimationBuffer(AnimationBuffer *animbuffer)
{
    AnimBuffer = animbuffer;
    CursorTiles.clear();
    this->setMinimumHeight(TileSizeY*(AnimBuffer->Animations.size()+1));

    BiggestAnimation=0;
    for(int i=0;i<AnimBuffer->Animations.count();i++)
        if ( BiggestAnimation < AnimBuffer->Animations[i].Frames.size() )
            BiggestAnimation = AnimBuffer->Animations[i].Frames.size();

    this->setMinimumWidth(TileSizeX*2 + 5 + BiggestAnimation*TileSizeX);
    this->repaint();
}

void AnimationEditor::paintEvent(QPaintEvent *)
{

    QPainter pt(this);
    QPixmap *frame;

    pt.fillRect(this->rect(),Qt::SolidPattern);

    pt.setPen(Qt::white);
    pt.drawLine(TileSizeX + 3, 0, TileSizeX + 2, this->height());

    if (Buffer->getCount() > 0)
    {
        if (AnimBuffer->Animations.size() > 0)
        {
            for (int i=0;i<AnimBuffer->Animations.size(); i++)
            {
                if (AnimBuffer->Animations[i].Frames.size() > 0)
                {
                    frame = AnimBuffer->getFramePixmap(i);
                    if (frame != NULL)
                        pt.drawPixmap(0,i*TileSizeY, *frame);
                }

                for (int j=0;j<AnimBuffer->Animations[i].Frames.size(); j++)
                {
                    frame = Buffer->getTilePixmapAt( AnimBuffer->Animations[i].Frames[j] );
                    if (frame != NULL)
                        pt.drawPixmap(j*TileSizeX + TileSizeX + 5, i*TileSizeY, *frame );
                }
            }

            pt.setPen(Qt::red);
            pt.setBrush(QColor(255,250,250,45));
            pt.drawRect(GrabbedAreaRect);
            if (isMouseHovering && ! isMouseDown)
            {
                //pt.setPen(Qt::darkRed);
                pt.drawRect(HiglightRect);
            }
        }
    }

    if (MouseHoverIndex >= 0)
    {

        if (MouseHoverIndex >= AnimBuffer->Animations.size())
        {
            pt.drawPixmap(TileSizeX + 5 +TileSizeX/2 - PlusIconImg->width()/2
                          , AnimBuffer->Animations.size()*TileSizeY + TileSizeY/2 - PlusIconImg->height()/2 ,
                          *PlusIconImg);
        }
        else
        {
            if ((RemoveIndex < AnimBuffer->Animations[MouseHoverIndex].Frames.size()))
            {
                if ((RemoveIndex >=0))
                    pt.drawPixmap(TileSizeX+5+ RemoveIndex*TileSizeX + TileSizeX/2 - DeleteIconImg->width()/2 ,
                                  MouseHoverIndex*TileSizeY + TileSizeY/2 - DeleteIconImg->height()/2 ,
                                  *DeleteIconImg);
            }
            else
                pt.drawPixmap(AnimBuffer->Animations[MouseHoverIndex].Frames.size()*TileSizeX+TileSizeX+5 + TileSizeX/2 - PlusIconImg->width()/2 ,
                              MouseHoverIndex*TileSizeY + TileSizeY/2 - PlusIconImg->height()/2 ,
                              *PlusIconImg);
        }
    }
}

void AnimationEditor::mouseMoveEvent(QMouseEvent *ev)
{
    if (ev->pos().y()/TileSizeY >= AnimBuffer->Animations.size())
    {
        MouseHoverIndex = AnimBuffer->Animations.size();
        HiglightRect.moveTo(-1000,-1000);
    } else
    {
        if (ev->pos().x() <= TileSizeX)
        {
            if (isMouseDown)
            {
                GrabbedAreaRect.setLeft(0/*(ev->x()/TileSizeX)*TileSizeX*/);
                GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY);
                GrabbedAreaRect.setWidth(TileSizeX);
                GrabbedAreaRect.setHeight(TileSizeY);
            }else
            {
                HiglightRect.setLeft(0/*(ev->x()/TileSizeX)*TileSizeX*/);
                HiglightRect.setTop((ev->y()/TileSizeY)*TileSizeY);
                HiglightRect.setWidth(TileSizeX);
                HiglightRect.setHeight(TileSizeY);
            }
            isMouseHovering = true;
            isMouseDown = ev->buttons() & Qt::LeftButton;
            // MouseHoverIndex = -1;
            RemoveIndex = -1;
        } else
        {
            RemoveIndex = ((ev->x()-TileSizeX-5)/TileSizeX);
            HiglightRect.moveTo(-1000,-1000);
        }
        MouseHoverIndex = (ev->y()/TileSizeY);
    }
    this->repaint();
}

void AnimationEditor::mousePressEvent(QMouseEvent *ev)
{
    if (ev->pos().x() <= TileSizeX)
    {
        if (ev->pos().y()/TileSizeY >= AnimBuffer->Animations.size()) return;
        GrabbedAreaRect.setLeft(0/*(ev->x()/TileSizeX)*TileSizeX*/);
        GrabbedAreaRect.setTop((ev->y()/TileSizeY)*TileSizeY);
        GrabbedAreaRect.setWidth(TileSizeX);
        GrabbedAreaRect.setHeight(TileSizeY);
        isMouseDown = true;
    } else
    {
        int index = ev->pos().y()/TileSizeY;

        if (index < AnimBuffer->Animations.size())
        {
            if (RemoveIndex >= AnimBuffer->Animations[index].Frames.size() )
            {
                if (CursorTiles.size() > 0 )
                {
                    AddFramesFromCursorTo(index);
                    emit editingFinished();

                    if ( BiggestAnimation < AnimBuffer->Animations[index].Frames.size() )
                        BiggestAnimation = AnimBuffer->Animations[index].Frames.size();
                }
            }
            else
            {
                if (RemoveIndex >= 0)
                {
                    AnimBuffer->Animations[index].Frames.remove(RemoveIndex);
                    if (AnimBuffer->Animations[index].Frames.size() <= 0)
                    {
                        AnimBuffer->Animations.remove(index);
                    }
                    emit editingFinished();
                    AnimBuffer->syncAllAnimations();
                }
            }
        }
        else
        {
            //            if (index >= AnimBuffer->Animations.size())
            {
                AddAnimation();
                index = AnimBuffer->Animations.size()-1;
                AddFramesFromCursorTo(index);
                emit editingFinished();
                if ( BiggestAnimation < AnimBuffer->Animations[index].Frames.size() )
                    BiggestAnimation = AnimBuffer->Animations[index].Frames.size();
            }
        }
    }

    this->setMinimumHeight(TileSizeY*(AnimBuffer->Animations.size()+1));
    this->setMinimumWidth(TileSizeX*2 + 5 + BiggestAnimation*TileSizeX);

    MouseHoverIndex =-1;
    this->repaint();
}

void AnimationEditor::mouseReleaseEvent(QMouseEvent *ev)
{
    isMouseDown = false;
    if (ev->pos().x() <= TileSizeX)
    {
        emit onAnimationSelected();
    }
    MouseHoverIndex =-1;
    mouseMoveEvent(ev);
    this->repaint();
}

void AnimationEditor::leaveEvent(QEvent *)
{
    isMouseHovering = false;
    isMouseDown = false;
    MouseHoverIndex =-1;
    this->repaint();
}

void AnimationEditor::setCursorTiles(QVector<QVector<int> > cur)
{
    CursorTiles.clear();
    CursorTiles = cur;
}

int AnimationEditor::getSelectedAnimation()
{
    if (AnimBuffer->Animations.size() <= 0) return -1;
    return GrabbedAreaRect.top()/TileSizeY;
}

void AnimationEditor::AddAnimation()
{
    AnimBuffer->Animations.append(Animation());
    int index = AnimBuffer->Animations.size()-1;
    AnimBuffer->Animations[index].Fps=5;
    AnimBuffer->Animations[index].isPingPong=false;
    AnimBuffer->Animations[index].CurrentFrame = 0;
    AnimBuffer->Animations[index].delay = 0;
    AnimBuffer->syncAllAnimations();
}

void AnimationEditor::AddFramesFromCursorTo(int AnimationIndex)
{
    for (int j=0;j<CursorTiles.size(); j++)
        for (int i=0;i<CursorTiles[j].size(); i++)
        {
            AnimBuffer->Animations[AnimationIndex].Frames.append(CursorTiles[j][i]);
        }
    AnimBuffer->syncAllAnimations();
    emit editingFinished();
}

