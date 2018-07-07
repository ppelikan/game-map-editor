#include "eventmatrix.h"

EventTile::EventTile()
{
    Events.clear();
}

void EventTile::toggleEvent(int eventNum)
{
    if (Events.contains(eventNum))
        Events.removeAll(eventNum);
    else
        Events.append(eventNum);
}

bool EventTile::isEmpty()
{
    return Events.isEmpty();
}

int EventTile::getEventAt(int index)
{
    return Events.at(index);
}

bool EventTile::contains(int eventNum)
{
    return Events.contains(eventNum);
}

int EventTile::getCount()
{
    return Events.count();
}

EventMatrix::EventMatrix(int SizeX, int SizeY)
{
    Matrix.fill(QVector<EventTile* >().fill( NULL ,SizeX),SizeY);
}

EventMatrix::EventMatrix()
{
    Matrix.fill(QVector<EventTile* >().fill( NULL ,50),50);
}

EventMatrix::~EventMatrix()
{
    this->clear();
}

void EventMatrix::setMatrixSize(int x, int y)
{
    Matrix.resize(x);
    for (int i=0;i<x;i++)
        Matrix[i].resize(y);
}

EventTile* EventMatrix::getNumsAt(int x, int y)
{
    if (x >= Matrix.count())
        return NULL;
    if (y >= Matrix[0].count())
        return NULL;
    return Matrix.at(x).at(y);
}

#define MIN(X,Y) ((X)>(Y) ? (Y) : (X))
#define MAX(X,Y) ((X)>(Y) ? (X) : (Y))

void EventMatrix::toggleNumAt(int x, int y, int Num)
{
    if ((x >= Matrix.count()) || (y >= Matrix[0].count()))
        setMatrixSize(MAX(x+1, Matrix.count()), MAX(y+1, Matrix[0].count()));

    if ( Matrix.at(x).at(y) == NULL)
    {
        EventTile *buf = new EventTile();
        Matrix[x][y] = buf;
    }
    Matrix.at(x).at(y)->toggleEvent( Num );

    if ( Matrix.at(x).at(y)->isEmpty() )
    {
        delete Matrix.at(x).at(y);
        Matrix[x][y] = NULL;
    }
}

void EventMatrix::toggleRectArea(QRect area, int EventNum)
{
    if ((area.right() >= Matrix.count()) || (area.bottom() >= Matrix[0].count()))
        setMatrixSize(MAX(area.right()+1, Matrix.count()), MAX(area.bottom()+1, Matrix[0].count()));     //w celu optymalizacji dodane też tutaj

    int ix,iy;
    for (ix= area.left(); ix<area.right(); ix++)
        for (iy= area.top(); iy<area.bottom(); iy++)
            this->toggleNumAt(ix,iy,EventNum);
}

int EventMatrix::getSizeX()
{
    return Matrix.count();
}

int EventMatrix::getSizeY()
{
    return Matrix[0].count();
}

QString EventMatrix::getAllNamesAt(int x, int y)
{
    QString sBuf = "";
    for(int a=0;a<this->getNumsAt(x,y)->getCount();a++)
    {
        sBuf += this->getEvent(this->getNumsAt(x,y)->getEventAt(a))->Name;
        if (a+1 < this->getNumsAt(x,y)->getCount())
            sBuf += " ";
    }
    return sBuf;
}

void EventMatrix::createNewEvent(QString Name, QString Params)
{
    Event *buf = new Event;
    //buf->index = Events.count()-1;
    buf->Name = Name;
    buf->Params = Params;
    Events.append( buf );
}

void EventMatrix::deleteEvent(int index)
{
    int ix,iy;
    for (ix= 0; ix<Matrix.count(); ix++)
        for (iy= 0; iy<Matrix.at(ix).count(); iy++)
        {
            if ( Matrix.at(ix).at(iy) != NULL  )
                if (Matrix.at(ix).at(iy)->contains(index))
                    Matrix.at(ix).at(iy)->toggleEvent(index);
        }

    delete Events.at( index );
    Events.remove( index );
}

Event *EventMatrix::getEvent(int index)
{
    return Events.at(index);
}

int EventMatrix::getCount()
{
    return Events.count();
}

void EventMatrix::clear()
{
    int ix,iy;
    for (ix= 0; ix<Matrix.count(); ix++)
        for (iy= 0; iy<Matrix.at(ix).count(); iy++)
        {
            delete Matrix.at(ix).at(iy);
            Matrix[ix].clear();
        }
    Matrix.clear();
    qDeleteAll(Events);
    Events.clear();

    Matrix.fill(QVector<EventTile* >().fill( NULL ,50),50);
}
