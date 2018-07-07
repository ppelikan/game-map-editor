#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QListWidgetItem>

#include "tileselector.h"
#include "tiledmapeditor.h"
#include "animationeditor.h"
#include "entirelevel.h"
#include "levelundostack.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_saMapSrolled(int);
    void on_editorMouseMove(int x, int y);
    void on_editorLeave(int x, int y);
    void on_btLoadTexture_clicked();
    void TexSelected();
    void AnimationTick();
    void AnimationSelected();
    void sbTileSize_editingFinished();
    void sbLayerSize_editingFinished();

    void on_actionZapisz_triggered();
    void on_actionOtw_rz_triggered();
    void on_actionZapisz_jako_triggered();
    void on_btAddToTexture_clicked();
    void on_btDeleteFromTExture_clicked();
    void on_btDecTextureWidth_clicked();
    void on_btIncTextureWidth_clicked();
    void on_btDeleteAnimation_clicked();
    void on_sbFPS_editingFinished();
    void on_cbPingPongAnimation_toggled(bool checked);
    void on_btAddLayer_clicked();
    void on_btDeleteLayer_clicked();
    void on_cbSelectLayer_currentIndexChanged(int index);
    void on_actionJak_korzysta_z_programu_triggered();
    void on_actionO_programie_triggered();
    void on_actionNowy_triggered();
    void on_actionJako_grafik_triggered();
    void on_actionJako_plik_tekstowy_triggered();
    void on_actionJako_plik_graficzny_triggered();
    void on_actionJako_osobne_pliki_graficzne_triggered();
    void on_actionWype_nij_triggered();
    void on_splitter_splitterMoved(int, int);
    void on_actionZaznacz_triggered();
    void on_actionKopiuj_triggered();
    void on_actionWklej_triggered();
    void on_actionWczytaj_textur_o_rozmiarach_triggered();
    void on_cbTransparent_stateChanged(int);
    void on_btAddEvent_clicked();
    void on_lwEventList_itemDoubleClicked(QListWidgetItem *item);
    void on_btDeleteEvent_clicked();
    void on_btPlaceEvent_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_actionPoka_zdarzenia_triggered();
    void on_actionPoka_wszystkie_warstwy_triggered();
    void on_actionCofnij_triggered();
    void on_actionPowt_rz_triggered();

private:
    void closeEvent(QCloseEvent *event);
    void resizeEvent( QResizeEvent * );
    void applyLevelToWidgets();
    bool promptNotSaved();

    Ui::MainWindow *ui;

    TileSelector *Selector;
    TiledMapEditor *Editor;
    AnimationEditor *AnimEditor;
    QVector<QVector<int> > Clipboard;

    QLabel *slCursorPos;
    QLabel *slCursorMapSize;
    QLabel *slFileName;
    QLabel *slHints;

    QString CurrentFileName;
    QString FilesPath;
    void LoadSettings();
    void SaveSettings();

    EntireLevel *Level;
    LevelUndoStack *UndoStack;
};

#endif // MAINWINDOW_H
