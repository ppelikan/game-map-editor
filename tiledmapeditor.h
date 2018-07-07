#ifndef TILEDMAPEDITOR_H
#define TILEDMAPEDITOR_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include "tilebuffer.h"
#include "tilemap.h"
#include "animationbuffer.h"
#include "eventmatrix.h"

class TiledMapEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TiledMapEditor(QWidget *parent = 0);
    ~TiledMapEditor();

    void setTileBuffer(TileBuffer *tilebuffer);
    void setTileMap(TileMap *tilemap);
    void setAnimationBuffer(AnimationBuffer *animbuffer);

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *);

    void setTileSize(int x, int y);
    void setCursorTiles(QVector<QVector<int> > cur);
    QVector<QVector<int> > getCursorTiles();

    void setVisibleRect(QRect rect);
    void setSelectionMode(bool mode);
    bool isInSelectionMode();
    QVector <QVector<int> > getSelectionMap();
    QRect getSelectionRect();
    void fillWithTileCursor();

    QPixmap *getMapPixmap() const;

    void addTransparentLayer(TileMap * map);
    void setEventMatrix(EventMatrix *ev);
    void showEventMatrix();
    void hideEventMatrix();

signals:
    void mouseMove(int x, int y);
    void scrollDir(int x, int y);
    void editingFinished();

public slots:

private:

//    QPixmap *MapImg;
    QRect VisibleRect;
    int TileSizeX, TileSizeY;
    bool isModified;
    bool isMouseHovering;
    bool isMouseDown;
    bool isSelectionModeOn;
    QRect SelectionRect;
    QPoint Cursor;
    QVector<QVector<int> > CursorTiles;

    TileBuffer *Buffer;
    TileMap *Map;
    AnimationBuffer * AnimBuffer;
    QVector<TileMap * > TranMaps;
    EventMatrix *Matrix;
    bool isMatrixVisible;
};

#endif // TILEDMAPEDITOR_H
