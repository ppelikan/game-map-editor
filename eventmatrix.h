#ifndef EVENTMATRIX_H
#define EVENTMATRIX_H

#include <QVector>
#include <qrect.h>

struct Event
{
//public:
    QString Name;
    QString Params;
//    int index;
};

struct EventTile
{
private:
    QVector<int> Events;
public:
    EventTile();
    void toggleEvent(int eventNum);
    bool isEmpty();
    int getEventAt(int index);
    bool contains(int eventNum);
    int getCount();
};

class EventMatrix
{
private:
    QVector< QVector<EventTile * > > Matrix;
    QVector<Event *> Events;
public:
    EventMatrix(int SizeX, int SizeY);
    EventMatrix();
    ~EventMatrix();
    void setMatrixSize(int x, int y);
    EventTile* getNumsAt(int x, int y);
    void toggleNumAt(int x, int y, int Num);
    void toggleRectArea(QRect area, int EventNum);
    //void toggleRectArea(QRect area, EventTile * Ev);
    int getSizeX();
    int getSizeY();
    QString getAllNamesAt(int x, int y);

    void createNewEvent(QString Name, QString Params);
    void deleteEvent(int index);
    Event* getEvent(int index);
    int getCount();

    void clear();

//    void clearAll();
};

#endif // EVENTMATRIX _H
