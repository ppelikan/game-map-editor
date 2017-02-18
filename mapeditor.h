#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QWidget>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

class MapEditor : public QWidget
{
    Q_OBJECT
public:
    explicit MapEditor(QWidget *parent = 0);
    ~MapEditor();

    void paintEvent(QPaintEvent *);
    void setDrawingImg(QPixmap *img);
    void setTileSize(int x, int y);

    void mouseMoveEvent(QMouseEvent *ev);
signals:

public slots:

private:
    QPixmap *CursorImg;
    int TileSizeX, TileSizeY;
    bool isMouseHovering;
    bool canDrawCursorImg;
    QPoint Cursor;
};

#endif // MAPEDITOR_H
