#ifndef TILEDSELECTABLEWIDGET_H
#define TILEDSELECTABLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

class TiledSelectableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TiledSelectableWidget(QWidget *parent = 0);
    ~TiledSelectableWidget();

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent * ev);
    void mousePressEvent(QMouseEvent * ev);
    void leaveEvent(QEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);

signals:

public slots:

private:
    int TileSizeX, TileSizeY;
    int NumTilesX, NumTilesY;
    QRect HiglightRect;
    QRect GrabbedAreaRect;
    bool isMouseHovering;
    bool isMouseDown;
};

#endif // TILEDSELECTABLEWIDGET_H
