#include "levelundostack.h"

//LevelUndoStack::LevelUndoStack(QObject *parent) : QObject(parent)
//{

//}

LevelUndoStack::LevelUndoStack(EntireLevel *lev)
{
    Level = lev;
    UndoTimer = new QTimer(this);
    connect(UndoTimer,SIGNAL(timeout()),this,SLOT(UndoTimerTimeout()));
    undoPosition = 0;
    isModified = false;
    UndoTimer->start(UNDO_CHECK_INTERVAL);
}

LevelUndoStack::~LevelUndoStack()
{
    UndoTimer->stop();
    delete UndoTimer;
    qDeleteAll(UndoStack);
    UndoStack.clear();
}

void LevelUndoStack::notifyModified()
{
    isModified = true;
    isSaved = false;
}

void LevelUndoStack::notifySaved()
{
    isSaved = true;
}

void LevelUndoStack::undo()
{
    UndoTimerTimeout();
    if (undoPosition <= 0) return;
    undoPosition--;
    QDataStream st( /*qUncompress(*/ UndoStack.at(undoPosition) , QIODevice::ReadWrite);
    Level->loadFromStream( st );  
    qDebug() << "UNDO, pos: " << undoPosition << ", count: " << UndoStack.count();
}

void LevelUndoStack::redo()
{
    UndoTimerTimeout();
    if (undoPosition >= UndoStack.count() - 1) return;
    undoPosition++;
    QDataStream st( /*qUncompress(*/ UndoStack.at(undoPosition) , QIODevice::ReadWrite);
    Level->loadFromStream( st );
    qDebug() << "REDO, pos: " << undoPosition << ", count: " << UndoStack.count();
}

bool LevelUndoStack::getIsSaved()
{
    return isSaved;
}

void LevelUndoStack::UndoTimerTimeout()
{
    if (!isModified) return;

    if ( UndoStack.count() )
    {
        while (undoPosition < UndoStack.count() - 1)
        {
            delete UndoStack.last();
            UndoStack.removeLast();
        }
    }

    if (UndoStack.count() >= MAX_UNDO_STACK_SIZE)
    {
        delete UndoStack.first();
        UndoStack.removeFirst();
    }

    QByteArray *buf = new QByteArray();
    QDataStream st(buf, QIODevice::ReadWrite);
    Level->saveToStream( st );
    UndoStack.append( /*qCompress( */ buf/*, 4)*/ );
    undoPosition = UndoStack.count() - 1;

    qDebug() << "MODIFY, pos: " << undoPosition << ", count: " << UndoStack.count();

    isModified = false;
}
