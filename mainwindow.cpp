#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "neweventdialog.h"
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UndoStack = NULL;

    Level = new EntireLevel();
    connect(Level->AnimBuffer, SIGNAL(onAnimationUpdate()), this, SLOT(AnimationTick()));

    Selector = new TileSelector(this);
    ui->saTexture->setWidget(Selector);

    Editor = new TiledMapEditor(this);
    ui->saMap->setWidget(Editor);

    AnimEditor = new AnimationEditor(this);
    ui->saAnim->setWidget(AnimEditor);

    connect(Editor, SIGNAL(mouseMove(int,int)), this, SLOT(on_editorMouseMove(int,int)));
    connect(Editor, SIGNAL(scrollDir(int,int)), this, SLOT(on_editorLeave(int,int)));

    connect(ui->saMap->verticalScrollBar(), SIGNAL(valueChanged(int)),this,SLOT(on_saMapSrolled(int)));
    connect(ui->saMap->horizontalScrollBar(), SIGNAL(valueChanged(int)),this,SLOT(on_saMapSrolled(int)));

    connect(Selector, SIGNAL(onTilesSelected()), this, SLOT(TexSelected()));
    connect(AnimEditor, SIGNAL(onAnimationSelected()), this, SLOT(AnimationSelected()));

    connect(ui->sbTileHeight, SIGNAL(editingFinished()),this, SLOT(sbTileSize_editingFinished()) );
    connect(ui->sbTileWidth , SIGNAL(editingFinished()),this, SLOT(sbTileSize_editingFinished()) );

    connect(ui->sbLayerHeight, SIGNAL(editingFinished()),this, SLOT(sbLayerSize_editingFinished()) );
    connect(ui->sbLayerWidth , SIGNAL(editingFinished()),this, SLOT(sbLayerSize_editingFinished()) );

    Editor->showEventMatrix();

    slCursorPos = new QLabel(this);
    slFileName = new QLabel(this);
    slCursorMapSize = new QLabel(this);
    slHints = new QLabel(this);
    ui->statusBar->addPermanentWidget(slHints);
    ui->statusBar->addPermanentWidget(slCursorMapSize);
    ui->statusBar->addPermanentWidget(slCursorPos);
    ui->statusBar->addPermanentWidget(slFileName);
    slCursorPos->setMinimumWidth(100);
    slFileName->setMinimumWidth(300);
    slCursorMapSize->setMinimumWidth(100);

    QSizePolicy siz(QSizePolicy::Expanding, QSizePolicy::Preferred);
    slCursorPos->setSizePolicy(siz);
    slCursorMapSize->setSizePolicy(siz);
    siz.setHorizontalStretch(3);
    slHints->setSizePolicy(siz);
    siz.setHorizontalStretch(3);
    slFileName->setSizePolicy(siz);

    ui->actionNowy->trigger();
    LoadSettings();

    on_actionZaznacz_triggered();
}

MainWindow::~MainWindow()
{
    // promptNotSaved();
    SaveSettings();
    delete ui;
}

void MainWindow::on_saMapSrolled(int)
{
    QPoint tl(ui->saMap->horizontalScrollBar()->value(),
              ui->saMap->verticalScrollBar()->value());
    QPoint br = tl + ui->saMap->viewport()->rect().bottomRight();
    Editor->setVisibleRect(QRect(tl,br));   //potrzebne w celu selektywnego rysowania mapy tylko w aktualnie widocznym obszarze (optymalizacja dla rozmiarów mapy > 300x300)
}

void MainWindow::on_editorMouseMove(int x, int y)
{
    slCursorPos->setText("X: "+ QString::number(x)+", Y: "+QString::number(y));
    if (Editor->isInSelectionMode())
    {
        slCursorMapSize->setText("X: "+ QString::number(Editor->getSelectionRect().width()-1)+", Y: "+QString::number(Editor->getSelectionRect().height()-1));
    }
    if ( Level->Matrix->getNumsAt(x,y) != NULL )
        slHints->setText(Level->Matrix->getAllNamesAt(x,y));
    else
        slHints->setText("");
}

void MainWindow::on_editorLeave(int x, int y)
{
    ui->saMap->ensureVisible((x+1)*Level->Buffer->getTileSizeX(),
                             (y+1)*Level->Buffer->getTileSizeY(),
                             Level->Buffer->getTileSizeX(),
                             Level->Buffer->getTileSizeY());  //automatyczne przewijanie się ekranu podczas edycji mapy
}

void MainWindow::on_btLoadTexture_clicked()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Import textures"),FilesPath,tr("Image files (*.png *.jpg *.bmp) \r\nAll files (*)"));

    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    Selector->setTextureWidth(
                Level->Buffer->LoadImage(fileName)
                );
    sbTileSize_editingFinished();

    ui->sbTileHeight->setEnabled( Level->Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Level->Buffer->getCount() <= 0 );
}

void MainWindow::TexSelected()
{
    QVector<QVector<int> > bf = Selector->getSelectedTiles();

    Editor->setCursorTiles(bf);
    AnimEditor->setCursorTiles(bf);
    if (bf.size() > 0)
        slCursorMapSize->setText("X: "+ QString::number(bf[0].size())+", Y: "+QString::number(bf.size()));
    //    Editor->setSelectionMode( false );
    //    ui->actionZaznacz->setChecked( false );
    //    ui->actionKopiuj->setEnabled( false );
    //    on_saMapSrolled(0);
}

void MainWindow::AnimationTick()
{
    Editor->repaint();
    AnimEditor->repaint();
}

void MainWindow::AnimationSelected()
{
    if (AnimEditor->getSelectedAnimation() >= Level->AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;

    QVector<QVector<int> > bf;
    QVector<int> b;
    b.append( - AnimEditor->getSelectedAnimation() -1 );
    bf.append(b);
    Editor->setCursorTiles( bf );

    slCursorMapSize->setText("X: 1, Y: 1");
    ui->sbFPS->setValue( Level->AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].Fps );
    ui->cbPingPongAnimation->setChecked( Level->AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].isPingPong );
}

void MainWindow::sbTileSize_editingFinished()
{
    Level->setTileSize(ui->sbTileWidth->value(),ui->sbTileHeight->value());
    applyLevelToWidgets();
    UndoStack->notifyModified();
}

void MainWindow::sbLayerSize_editingFinished()
{
    Level->Layers[ui->cbSelectLayer->currentIndex()]->setWorldSize(
                ui->sbLayerWidth->value(),
                ui->sbLayerHeight->value()
                );
    on_saMapSrolled(0);
}

void MainWindow::on_actionZapisz_triggered()
{
    if (CurrentFileName == "")
    {ui->actionZapisz_jako->trigger(); return; }

    QFile file(CurrentFileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    Level->saveToStream(out);

    Selector->getTexture()->save(CurrentFileName+".png");
    UndoStack->notifySaved();
}

void MainWindow::on_actionOtw_rz_triggered()
{
    if (!promptNotSaved()) return;
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open map"),FilesPath,tr("Level files (*.map *.txp *.bin *.dat) \r\nAll files (*)"));
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();
    CurrentFileName = fileName;
    slFileName->setText( CurrentFileName );

    Level->clearAll();

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file

    Level->loadFromStream(in);

    ui->sbTileWidth->setValue( Level->Buffer->getTileSizeX() );
    ui->sbTileHeight->setValue( Level->Buffer->getTileSizeY() );


    QFileInfo texture_file(fileName+".png");
    if (texture_file.exists() && texture_file.isFile())
    {
        Selector->setTextureWidth(
                    Level->Buffer->LoadImage(fileName+".png")
                    );
        sbTileSize_editingFinished();

        ui->sbTileHeight->setEnabled( Level->Buffer->getCount() <= 0 );
        ui->sbTileWidth->setEnabled( Level->Buffer->getCount() <= 0 );
    } else
    {
        if (Level->Buffer->getCount() <= 0)
        {
            QMessageBox msgBox;
            msgBox.setText(tr("This map does not have textures file linked. Please open texture manually."));
            //msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok );
            msgBox.exec();
        }
    }

    if (UndoStack != NULL)
        delete UndoStack;
    UndoStack = new LevelUndoStack(Level);
    connect(Editor,SIGNAL(editingFinished()),UndoStack,SLOT(notifyModified()));
    connect(AnimEditor,SIGNAL(editingFinished()),UndoStack,SLOT(notifyModified()));

    applyLevelToWidgets();
    UndoStack->notifyModified();
    UndoStack->notifySaved();

    slFileName->setText( CurrentFileName );
}

void MainWindow::on_actionZapisz_jako_triggered()
{
    QFileDialog dial( this, tr("Save map"),FilesPath,tr("MAP Files (*.map);;TXP Files (*.txp);;BIN Files (*.bin);;DAT Files (*.dat);;All files (*)"));
    dial.setDefaultSuffix(".map");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    CurrentFileName = fileName;
    slFileName->setText( CurrentFileName );

    ui->actionZapisz->trigger();
}

void MainWindow::on_btAddToTexture_clicked()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Import textures"),FilesPath,tr("Image files (*.png *.jpg *.bmp) \r\nAll files (*)"));

    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    Level->Buffer->addLoadImage(fileName);
    sbTileSize_editingFinished();
    ui->sbTileHeight->setEnabled( Level->Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Level->Buffer->getCount() <= 0 );
}

void MainWindow::on_btDeleteFromTExture_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to proceed?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setInformativeText(tr("This operation cannot be undone! Deleting selected textures will modify the level."));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    if ( msgBox.exec() == QMessageBox::No) return;

    QVector<QVector<int> > buf = Selector->getSelectedTiles();

    for (int j=0; j<buf.size(); j++)
        for (int i=0; i<buf[j].size(); i++)
            Level->Buffer->remove(buf[0][0] + j*( Selector->getTextureWidth() - buf[0].size() ));

    buf.clear();
    sbTileSize_editingFinished();
    ui->sbTileHeight->setEnabled( Level->Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Level->Buffer->getCount() <= 0 );
}

void MainWindow::on_btDecTextureWidth_clicked()
{
    Selector->DecTextureWidth();
}

void MainWindow::on_btIncTextureWidth_clicked()
{
    Selector->IncTextureWidth();
}

void MainWindow::on_btDeleteAnimation_clicked()
{
    if (AnimEditor->getSelectedAnimation() >= Level->AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    Level->AnimBuffer->Animations.remove(
                AnimEditor->getSelectedAnimation());
    TexSelected();
    AnimEditor->repaint();
    UndoStack->notifyModified();
}

void MainWindow::on_sbFPS_editingFinished()
{
    if (AnimEditor->getSelectedAnimation() >= Level->AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    Level->AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].Fps = ui->sbFPS->value();
    Level->AnimBuffer->syncAllAnimations();
    UndoStack->notifyModified();
}

void MainWindow::on_cbPingPongAnimation_toggled(bool checked)
{
    if (AnimEditor->getSelectedAnimation() >= Level->AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    Level->AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].isPingPong = checked;
    Level->AnimBuffer->syncAllAnimations();
    UndoStack->notifyModified();
}

void MainWindow::on_btAddLayer_clicked()
{
    if (Level->Layers.count() > 20 ) return;
    Level->Layers.append(new TileMap(ui->sbLayerWidth->value(),ui->sbLayerHeight->value()));
    ui->cbSelectLayer->addItem(QString::number(Level->Layers.size()));
    for (int i=0; i<ui->cbSelectLayer->count(); i++)
        ui->cbSelectLayer->setItemText(i, QString::number(i+1) );
    ui->cbSelectLayer->setCurrentIndex(ui->cbSelectLayer->count()-1);
    on_saMapSrolled(0);
    if (Level->Layers.count() > 1 )
        ui->cbTransparent->setEnabled(true);
    UndoStack->notifyModified();
}

void MainWindow::on_btDeleteLayer_clicked()
{
    if (Level->Layers.size() <= 1) return;
    delete Level->Layers[ui->cbSelectLayer->currentIndex()];
    Level->Layers.remove( ui->cbSelectLayer->currentIndex() );
    ui->cbSelectLayer->removeItem( ui->cbSelectLayer->currentIndex() );
    for (int i=0; i<ui->cbSelectLayer->count(); i++)
        ui->cbSelectLayer->setItemText(i, QString::number(i+1) );
    on_saMapSrolled(0);
    UndoStack->notifyModified();
}

void MainWindow::on_cbSelectLayer_currentIndexChanged(int index)
{
    if (ui->cbSelectLayer->count() <= 0) return;
    ui->sbLayerWidth->setValue( Level->Layers[index]->getSizeX() );
    ui->sbLayerHeight->setValue( Level->Layers[index]->getSizeY() );
    Level->Layers[ui->cbSelectLayer->currentIndex()]->setWorldSize(
                Level->Layers[index]->getSizeX(),
                Level->Layers[index]->getSizeY()
                );
    Editor->setTileMap(Level->Layers.at(index));
    if (ui->cbTransparent->isChecked())
        for(int i=0; i<Level->Layers.count(); i++)
            if (i != index)
                Editor->addTransparentLayer(Level->Layers.at(i));
    on_saMapSrolled(0);
}

void MainWindow::on_actionJak_korzysta_z_programu_triggered()
{
    QMessageBox msgBox;
    msgBox.setText(tr("How to use this program?"));
    msgBox.setInformativeText(tr("1. Set the tile size of the textures you are willing to use. \n2. Open textures. \n3. Set the map size of the layer. \n4. Draw the level using tiles form the texture window. \n5. Save the level."));
    msgBox.setStandardButtons(QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_actionO_programie_triggered()
{
    QMessageBox msgBox;
    msgBox.setIconPixmap( QPixmap(":/new/prefix1/ics/New Piskel.png") );
    msgBox.setText(" ~ Map Editor ~");
    QString bb = "\n(2016) \nby Pawel Pelikan \nppelikan@gmail.com";
    msgBox.setInformativeText(tr("Super simple level editor for games. \n\nIf this software was somehow useful for you or you have found some bugs, feel free to share your opinion with me.\n")+bb);
    msgBox.setStandardButtons(QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_actionNowy_triggered()
{
    if (!promptNotSaved()) return;

    Level->clearAll();
    Level->Layers.append( new TileMap() );

    slFileName->setText(tr("Unnamed"));

    if (UndoStack != NULL)
        delete UndoStack;
    UndoStack = new LevelUndoStack(Level);
    connect(Editor,SIGNAL(editingFinished()),UndoStack,SLOT(notifyModified()));
    connect(AnimEditor,SIGNAL(editingFinished()),UndoStack,SLOT(notifyModified()));

    applyLevelToWidgets();
    UndoStack->notifySaved();
}

void MainWindow::on_actionJako_grafik_triggered()
{
    QFileDialog dial( this, tr("Export map as image"),FilesPath,tr("PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;All files (*)"));
    dial.setDefaultSuffix(".png");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    Editor->getMapPixmap()->save(fileName);
}

void MainWindow::on_actionJako_plik_tekstowy_triggered()
{
    QFileDialog dial( this, tr("Export map as text"),FilesPath,tr("Text file (*.txt);;All files (*)"));
    dial.setDefaultSuffix(".txt");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    QFile file( fileName );
    if ( file.open(QIODevice::WriteOnly) )
    {
        QTextStream stream( &file );
        Level->Layers[ui->cbSelectLayer->currentIndex()]->saveLayerAsText(stream);
        file.close();
    }
}

void MainWindow::on_actionJako_plik_graficzny_triggered()
{
    QFileDialog dial( this, tr("Export texture as image"),FilesPath,tr("PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;All files (*)"));
    dial.setDefaultSuffix(".png");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();
    Selector->getTexture()->save(fileName);
}

void MainWindow::on_actionJako_osobne_pliki_graficzne_triggered()
{
    QFileDialog dial( this, tr("Select directory to export texture as separate image files"),FilesPath,tr("PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;All files (*)"));
    dial.setDefaultSuffix(".png");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();
    QString ext =  QFileInfo(fileName).completeSuffix();
    fileName = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();

    for (int i=0;i<Level->Buffer->getCount();i++)
    {
        Level->Buffer->getTilePixmapAt(i)->save( fileName+QString::number(i)+"."+ext );
    }
}

void MainWindow::on_actionWype_nij_triggered()
{
    Editor->fillWithTileCursor();
}

void MainWindow::on_splitter_splitterMoved(int, int)
{
    on_saMapSrolled(0);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    on_saMapSrolled(0);
}

void MainWindow::applyLevelToWidgets()
{
    int layerIndex = ui->cbSelectLayer->currentIndex();
    if (layerIndex < 0) layerIndex = 0;
    if (layerIndex >= Level->Layers.count() ) layerIndex = 0;

    ui->cbSelectLayer->clear();
    for (int k=0; k<Level->Layers.count(); k++ )
    {
        ui->cbSelectLayer->addItem(QString::number(k+1));
    }

    QModelIndex eventIndex = ui->lwEventList->currentIndex();
    ui->lwEventList->clear();
    for (int k=0; k<Level->Matrix->getCount(); k++ )
    {
        ui->lwEventList->addItem( Level->Matrix->getEvent(k)->Name );
    }

    ui->sbTileWidth->setValue( Level->Buffer->getTileSizeX() );
    ui->sbTileHeight->setValue( Level->Buffer->getTileSizeY() );
    ui->sbTileHeight->setEnabled( Level->Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Level->Buffer->getCount() <= 0 );

    Selector->setTileBuffer(Level->Buffer);
    Editor->setTileBuffer(Level->Buffer);
    Editor->setAnimationBuffer(Level->AnimBuffer);
    Editor->setTileMap(Level->Layers[ layerIndex ]);
    Editor->setEventMatrix(Level->Matrix);
    AnimEditor->setTileBuffer(Level->Buffer);
    AnimEditor->setAnimationBuffer(Level->AnimBuffer);

    ui->actionWczytaj_textur_o_rozmiarach->setText(tr("Open textures [")+
                                                   QString::number(ui->sbTileWidth->value())+"x"+
                                                   QString::number(ui->sbTileHeight->value())+"px]");

    if (eventIndex.row() >= 0)
        if (eventIndex.row() < Level->Matrix->getCount() )
            ui->lwEventList->setCurrentIndex( eventIndex );
    ui->cbSelectLayer->setCurrentIndex( layerIndex );
    if (Level->Layers.count() > 1 )
        ui->cbTransparent->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if ( promptNotSaved() )
    {
        event->accept();
    }
}

bool MainWindow::promptNotSaved()
{
    if (UndoStack != NULL)
    {
        if ( !UndoStack->getIsSaved() )
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Do you want to save current level before closing it?"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setInformativeText(" ");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
            int i = msgBox.exec();
            if ( i == QMessageBox::Yes)
            {
                on_actionZapisz_triggered();
            }
            else
                if (i == QMessageBox::Cancel)
                    return false;
        }
    }
    return true;
}

void MainWindow::on_actionZaznacz_triggered()
{
    Editor->setSelectionMode( ui->actionZaznacz->isChecked() );
    ui->actionKopiuj->setEnabled( ui->actionZaznacz->isChecked() );
    on_saMapSrolled(0);
}

void MainWindow::on_actionKopiuj_triggered()
{
    Clipboard.clear();
    Clipboard = Editor->getSelectionMap();
    Editor->setCursorTiles(Clipboard);
    if (Clipboard.size() > 0)
        slCursorMapSize->setText("X: "+ QString::number(Clipboard[0].size())+", Y: "+QString::number(Clipboard.size()));
    Editor->setSelectionMode( false );
    ui->actionZaznacz->setChecked( false );
    ui->actionKopiuj->setEnabled( false );
    ui->actionWklej->setEnabled( true );
    on_saMapSrolled(0);
}

void MainWindow::on_actionWklej_triggered()
{
    ui->actionZaznacz->setChecked( false );
    Editor->setSelectionMode( false );
    ui->actionKopiuj->setEnabled( false );
    Editor->setCursorTiles(Clipboard);
    if (Clipboard.size() > 0)
        slCursorMapSize->setText("X: "+ QString::number(Clipboard[0].size())+", Y: "+QString::number(Clipboard.size()));
    on_saMapSrolled(0);
}

void MainWindow::on_actionWczytaj_textur_o_rozmiarach_triggered()
{
    ui->btLoadTexture->click();
}

void MainWindow::LoadSettings()
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    FilesPath = settings.value("FilesPath","").toString();

    settings.endGroup();

    settings.beginGroup("AdaptToParticularGame");

    settings.endGroup();
}

void MainWindow::SaveSettings()
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("FilesPath",FilesPath);
    settings.endGroup();

    //    settings.beginGroup("AdaptToParticularGame");
    //    settings.endGroup();
}

void MainWindow::on_cbTransparent_stateChanged(int)
{
    Editor->setTileMap(Level->Layers.at( ui->cbSelectLayer->currentIndex() ));
    if (ui->cbTransparent->isChecked())
        for(int i=0; i<Level->Layers.count(); i++)
            if (i != ui->cbSelectLayer->currentIndex())
                Editor->addTransparentLayer(Level->Layers.at(i));

    on_saMapSrolled(0);
    ui->actionPoka_wszystkie_warstwy->setChecked( ui->cbTransparent->isChecked() );
}

void MainWindow::on_btAddEvent_clicked()
{
    NewEventDialog dial;
    dial.setWindowTitle(tr("Add new event"));
    if ( dial.exec() == QDialog::Accepted )
    {
        ui->lwEventList->addItem( dial.getName() );
        Level->Matrix->createNewEvent(dial.getName(), dial.getParams());
        UndoStack->notifyModified();
    }
}

void MainWindow::on_lwEventList_itemDoubleClicked(QListWidgetItem *item)
{
    if (ui->lwEventList->currentIndex().row() <0 ) return;

    NewEventDialog dial;
    dial.setWindowTitle(tr("Edit event"));
    dial.setName( Level->Matrix->getEvent( ui->lwEventList->currentIndex().row() )->Name );
    dial.setParams( Level->Matrix->getEvent( ui->lwEventList->currentIndex().row() )->Params );

    if ( dial.exec() == QDialog::Accepted )
    {
        item->setText( dial.getName() );
        Level->Matrix->getEvent( ui->lwEventList->currentIndex().row() )->Name = dial.getName();
        Level->Matrix->getEvent( ui->lwEventList->currentIndex().row() )->Params = dial.getParams();
        UndoStack->notifyModified();
    }
}

void MainWindow::on_btDeleteEvent_clicked()
{
    if (ui->lwEventList->currentIndex().row() <0 ) return;
    Level->Matrix->deleteEvent( ui->lwEventList->currentIndex().row() );
    delete ui->lwEventList->currentItem();
    on_saMapSrolled(0);
    UndoStack->notifyModified();
}

void MainWindow::on_btPlaceEvent_clicked()
{
    if (ui->lwEventList->currentIndex().row() <0 ) return;
    Level->Matrix->toggleRectArea( Editor->getSelectionRect(), ui->lwEventList->currentIndex().row() );
    on_saMapSrolled(0);
    UndoStack->notifyModified();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 3)
    {
        Editor->setSelectionMode( true );
        ui->actionKopiuj->setEnabled( false );
        ui->actionWklej->setEnabled( false );
        ui->actionZaznacz->setEnabled( false );
        ui->actionWype_nij->setEnabled( false );

        ui->actionPoka_zdarzenia->setChecked( true );
        on_actionPoka_zdarzenia_triggered();
    }
    else
    {
        ui->actionWype_nij->setEnabled( true );
        ui->actionZaznacz->setEnabled( true );
        if (Clipboard.size() > 0)
            ui->actionWklej->setEnabled( true );
        on_actionZaznacz_triggered();
    }
}

void MainWindow::on_actionPoka_zdarzenia_triggered()
{
    if ( ui->actionPoka_zdarzenia->isChecked() )
        Editor->showEventMatrix();
    else
        Editor->hideEventMatrix();
    on_saMapSrolled(0);
}

void MainWindow::on_actionPoka_wszystkie_warstwy_triggered()
{
    ui->cbTransparent->setChecked( ui->actionPoka_wszystkie_warstwy->isChecked() );
    on_cbTransparent_stateChanged(0);
}

void MainWindow::on_actionCofnij_triggered()
{
    QVector<QVector<int> > bf = Editor->getCursorTiles();

    UndoStack->undo();
    applyLevelToWidgets();

    Editor->setCursorTiles(bf);
    AnimEditor->setCursorTiles(bf);
}

void MainWindow::on_actionPowt_rz_triggered()
{
    QVector<QVector<int> > bf = Editor->getCursorTiles();

    UndoStack->redo();
    applyLevelToWidgets();

    Editor->setCursorTiles(bf);
    AnimEditor->setCursorTiles(bf);
}
