#include "entirelevel.h"

EntireLevel::EntireLevel()
{
    Buffer = new TileBuffer();
    Layers.append( new TileMap() );
    AnimBuffer = new AnimationBuffer();

    Matrix = new EventMatrix();
    AnimBuffer->setTileBuffer(Buffer);
}

EntireLevel::~EntireLevel()
{
    this->clearAll();
}

void EntireLevel::setTileSize(int x, int y)
{
    Buffer->setTileSize(x,y);
    AnimBuffer->setTileBuffer(Buffer);
}

void EntireLevel::saveToStream(QDataStream &out)
{
    out << (int) 99;
    out << (int) 85;
    out << Name;

    out << (int) Buffer->getTileSizeX();
    out << (int) Buffer->getTileSizeY();

    out << (int) AnimBuffer->Animations.size();
    for (int i=0; i<AnimBuffer->Animations.size(); i++)
    {
        out << (int) AnimBuffer->Animations[i].Frames.size();
        out << (bool) AnimBuffer->Animations[i].isPingPong;
        out << (int) AnimBuffer->Animations[i].Fps;
        for (int j=0;j<AnimBuffer->Animations[i].Frames.size(); j++)
        {
            out << (int) AnimBuffer->Animations[i].Frames[j];
        }
    }
    out << (int) 99;
    out << (int) 80;

    qDebug() << "Save, layers count: " << Layers.count();
    out << (int) Layers.count();

    for (int k=0; k<Layers.size(); k++ )
    {
        out << (int) Layers[k]->getSizeX();
        out << (int) Layers[k]->getSizeY();
        for (int j=0; j<Layers[k]->getSizeY(); j++)
            for (int i=0; i<Layers[k]->getSizeX(); i++)
            {
                out << (int) Layers[k]->getTileAt(i,j);
            }
    }
    out << (int) 99;
    out << (int) 90;

    QVector<QPoint> bf;
    out << (int) Matrix->getCount();
    for(int k=0;k<Matrix->getCount();k++)
    {
        out << QString( Matrix->getEvent(k)->Name );
        out << QString( Matrix->getEvent(k)->Params );

        for (int j=0; j<Matrix->getSizeY(); j++)
            for (int i=0; i<Matrix->getSizeX(); i++)
            {
                if (Matrix->getNumsAt(i,j) != NULL)
                    if (Matrix->getNumsAt(i,j)->contains(k))
                    {
                        //out << (int) i;
                        //out << (int) j;
                        bf.append(QPoint(i,j));
                    }
            }

        out << (int) bf.count();
        for (int i=0; i<bf.count(); i++)
        {
            out << (int) bf.at(i).x();
            out << (int) bf.at(i).y();
        }
        bf.clear();

    }
    out << (int) 99;
    out << (int) 99;
}

void EntireLevel::loadFromStream(QDataStream &in)
{
    this->clear();

    QString buf_str,buf_str2;
    int buf_int,buf_int2;

    in >> buf_int;
    in >> buf_int;
    in >> Name;

    in >> buf_int;
    in >> buf_int2;
    this->setTileSize(buf_int,buf_int2);


    int anim_size, frames_size;
    bool buf_bool;
    in >> anim_size;
    for (int i=0; i<anim_size; i++)
    {
        AnimBuffer->Animations.append(Animation());

        in >> frames_size;
        in >> buf_bool;
        in >> buf_int;
        AnimBuffer->Animations[i].isPingPong = buf_bool;
        AnimBuffer->Animations[i].Fps = buf_int;

        for (int j=0;j<frames_size;j++)
        {
            in >> buf_int;
            AnimBuffer->Animations[i].Frames.append(buf_int);
        }
    }

    in >> buf_int;
    in >> buf_int;

    //    qDeleteAll(Layers); //wywołujemy destrukotyry
    //    Layers.clear();     //usuwamy wskaźniki

    int num_layers;
    in >> num_layers;

    qDebug() << "Load, layers count: " << num_layers;
    //  ui->cbSelectLayer->clear();
    int maxx = 0,maxy = 0;
    for (int k=0; k<num_layers; k++ )
    {
        in >> buf_int;
        in >> buf_int2;
        Layers.append( new TileMap(buf_int,buf_int2));
        Layers[k]->setWorldSize(buf_int,buf_int2);
        if (maxx < buf_int) maxx = buf_int;
        if (maxy < buf_int2) maxy = buf_int2;
        //  ui->cbSelectLayer->addItem(QString::number(k+1));
        for (int j=0; j<Layers[k]->getSizeY(); j++)
            for (int i=0; i<Layers[k]->getSizeX(); i++)
            {
                in >> buf_int;
                Layers[k]->setTileAt(i,j,buf_int);
            }
    }
    in >> buf_int;
    in >> buf_int;

    Matrix->setMatrixSize(maxx,maxy);
    in >> buf_int;
    for(int k=0;k<buf_int;k++)
    {
        in >> buf_str;
        in >> buf_str2;
        Matrix->createNewEvent(buf_str,buf_str2);
        // ui->lwEventList->addItem( buf_str );
        in >> buf_int2;
        for(int i=0;i<buf_int2;i++)
        {
            int ax,ay;
            in >> ax;
            in >> ay;
            Matrix->toggleNumAt(ax,ay,k);
        }
    }
    in >> buf_int;
    in >> buf_int;
}

void EntireLevel::setName(QString s)
{
    Name = s;
}

void EntireLevel::clearAll()
{
    clear();
    Buffer->clear();
}

void EntireLevel::clear()
{
    qDeleteAll(Layers); //wywołujemy destrukotyry
    Layers.clear();     //usuwamy wskaźniki
    //Buffer->clear();
    AnimBuffer->clear();
    Matrix->clear();
}

