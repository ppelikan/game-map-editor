#ifndef ANIMATIONEDITOR_H
#define ANIMATIONEDITOR_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include "tilebuffer.h"
#include "animationbuffer.h"

class AnimationEditor : public QWidget
{
    Q_OBJECT
public:
    explicit AnimationEditor(QWidget *parent = 0);
    ~AnimationEditor();

    void setTileBuffer(TileBuffer *tilebuffer);
    void setAnimationBuffer(AnimationBuffer *animbuffer);

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);

    void setCursorTiles(QVector<QVector<int> > cur);

    int getSelectedAnimation();

signals:
    void onAnimationSelected();

public slots:

private:
    void AddAnimation();
    void AddFramesFromCursorTo(int AnimationIndex);

    int TileSizeX, TileSizeY;
    bool isMouseHovering;
    bool isMouseDown;
    QPoint Cursor;
    QVector<QVector<int> > CursorTiles;
    TileBuffer *Buffer;
    AnimationBuffer *AnimBuffer;
    QRect HiglightRect;
    QRect GrabbedAreaRect;
    int BiggestAnimation;
    int MouseHoverIndex; //-1 - brak, 0..animbuffer.size-1 - dodaj do animacji, animbuffer.size - dodaj animację
    QPixmap *PlusIconImg;
};

#endif // ANIMATIONEDITOR_H
