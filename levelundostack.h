#ifndef LEVELUNDOSTACK_H
#define LEVELUNDOSTACK_H

#include "entirelevel.h"
//#include <QObject>
#include <QtCore>

#define UNDO_CHECK_INTERVAL 2000
#define MAX_UNDO_STACK_SIZE 10


class LevelUndoStack : public QObject
{
    Q_OBJECT
public:
    //    explicit LevelUndoStack(QObject *parent = nullptr);
    LevelUndoStack(EntireLevel *lev);
    ~LevelUndoStack();
    void undo();
    void redo();
    bool getIsSaved();

public slots:
    void notifyModified();
    void notifySaved();

private:
    EntireLevel *Level;
    QVector< QByteArray* > UndoStack;
    QTimer *UndoTimer;
    bool isModified;
    int undoPosition;
    bool isSaved;

private slots:
    void UndoTimerTimeout();
};

#endif // LEVELUNDOSTACK_H
