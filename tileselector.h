#ifndef TILESELECTOR_H
#define TILESELECTOR_H

#include <QWidget>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
//#include <QResizeEvent>
#include "tilebuffer.h"

class TileSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TileSelector(QWidget *parent = 0);
    ~TileSelector();

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent * ev);
    void mousePressEvent(QMouseEvent * ev);
    void leaveEvent(QEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void setTileBuffer(TileBuffer *tilebuffer);
    QVector<QVector<int> > getSelectedTiles();
    void IncTextureWidth();
    void DecTextureWidth();
    void setTextureWidth(int NumberOfTilesInAxisX);

    QPixmap *getTexture() const;

signals:
    void onTilesSelected();

public slots:

private:
    void repaintBuffer();

    TileBuffer *Buffer;
    int TileSizeX, TileSizeY;
    QPixmap *TilesImageBuf;
    QRect HiglightRect;
    QRect GrabbedAreaRect;
    bool isMouseHovering;
    bool isMouseDown;
    int NumTilesInAxisX;
};

#endif // TILESELECTOR_H
