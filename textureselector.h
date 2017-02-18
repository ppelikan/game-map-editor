#ifndef TEXTURESELECTOR_H
#define TEXTURESELECTOR_H

#include <QWidget>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

#include <QDebug>

class TextureSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TextureSelector(QWidget *parent = 0);
    ~TextureSelector();
    void LoadImage(QString FileName);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent * ev);
    void mousePressEvent(QMouseEvent * ev);
    void leaveEvent(QEvent *);
    void mouseReleaseEvent(QMouseEvent *S);

    int getTileSizeX() const;
    int getTileSizeY() const;
    void setTileSize(int x, int y);

    QPixmap *getSelectedPixmap();
    QList<QList<int> > getSelectedTiles();

signals:
void TextureSelected();

public slots:


private:
    QPixmap *TextureImg;
    QList<QPixmap*> TileImages;
        int TileSizeX, TileSizeY;
        QRect HiglightRect;
        QRect GrabbedAreaRect;
        bool isMouseHovering;
        bool isMouseDown;
};

#endif // TEXTURESELECTOR_H
