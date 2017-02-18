#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Buffer = new TileBuffer();
    //    Map = new TileMap();
    Layers.append( new TileMap() );

    Selector = new TileSelector(this);
    ui->saTexture->setWidget(Selector);

    Editor = new TiledMapEditor(this);
    ui->saMap->setWidget(Editor);

    AnimBuffer = new AnimationBuffer();

    AnimEditor = new AnimationEditor(this);
    ui->saAnim->setWidget(AnimEditor);

    connect(Editor, SIGNAL(mouseMove(int,int)), this, SLOT(on_editorMouseMove(int,int)));
    connect(Editor, SIGNAL(scrollDir(int,int)), this, SLOT(on_editorLeave(int,int)));

    connect(ui->saMap->verticalScrollBar(), SIGNAL(valueChanged(int)),this,SLOT(on_saMapSrolled(int)));
    connect(ui->saMap->horizontalScrollBar(), SIGNAL(valueChanged(int)),this,SLOT(on_saMapSrolled(int)));

    connect(Selector, SIGNAL(onTilesSelected()), this, SLOT(TexSelected()));
    connect(AnimBuffer, SIGNAL(onAnimationUpdate()), this, SLOT(AnimationTick()));
    connect(AnimEditor, SIGNAL(onAnimationSelected()), this, SLOT(AnimationSelected()));

    connect(ui->sbTileHeight, SIGNAL(editingFinished()),this, SLOT(sbTileSize_editingFinished()) );
    connect(ui->sbTileWidth , SIGNAL(editingFinished()),this, SLOT(sbTileSize_editingFinished()) );

    connect(ui->sbLayerHeight, SIGNAL(editingFinished()),this, SLOT(sbLayerSize_editingFinished()) );
    connect(ui->sbLayerWidth , SIGNAL(editingFinished()),this, SLOT(sbLayerSize_editingFinished()) );

    //ui->cbSelectLayer->addItem("1");

    slCursorPos = new QLabel(this);
    slFileName = new QLabel(this);
    slCursorMapSize = new QLabel(this);
    //  slFileName->setText("Bez nazwy");
    ui->statusBar->addPermanentWidget(slCursorMapSize);
    ui->statusBar->addPermanentWidget(slCursorPos);
    ui->statusBar->addPermanentWidget(slFileName);
    slCursorPos->setMinimumWidth(100);
    slFileName->setMinimumWidth(400);
    slCursorMapSize->setMinimumWidth(100);

    ui->actionNowy->trigger();
    LoadSettings();
}

MainWindow::~MainWindow()
{
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
//     slCursorPos->setText("X: "+ QString::number(Editor->getSelectionRect().left())
  //                        +", Y: "+QString::number(Editor->getSelectionRect().top()));
     slCursorMapSize->setText("X: "+ QString::number(Editor->getSelectionRect().width()-1
                                  )+", Y: "+QString::number(Editor->getSelectionRect().height()-1));
    }
}

void MainWindow::on_editorLeave(int x, int y)
{
    ui->saMap->ensureVisible((x+1)*Buffer->getTileSizeX(),
                             (y+1)*Buffer->getTileSizeY(),
                             Buffer->getTileSizeX(),
                             Buffer->getTileSizeY());  //automatyczne przewijanie się ekranu podczas edycji mapy
}

void MainWindow::on_btLoadTexture_clicked()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Importuj tekstury"),FilesPath,tr("Pliki graficzne (*.png *.jpg *.bmp) \r\nWszystkie pliki (*)"));

    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    //TexSelector->LoadImage(fileName);
    Selector->setTextureWidth(
                Buffer->LoadImage(fileName)
                );
    sbTileSize_editingFinished();

    ui->sbTileHeight->setEnabled( Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Buffer->getCount() <= 0 );
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
    if (AnimEditor->getSelectedAnimation() > AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;

    QVector<QVector<int> > bf;
    QVector<int> b;
    b.append( - AnimEditor->getSelectedAnimation() -1 );
    bf.append(b);
    Editor->setCursorTiles( bf );

    slCursorMapSize->setText("X: 1, Y: 1");
    ui->sbFPS->setValue( AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].Fps );
    ui->cbPingPongAnimation->setChecked( AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].isPingPong );
    //AnimEditor->setCursorTiles(QVector<QVector<int> >(0));
}

void MainWindow::sbTileSize_editingFinished()
{
    Buffer->setTileSize(ui->sbTileWidth->value(),ui->sbTileHeight->value());
    Selector->setTileBuffer(Buffer);
    Editor->setTileBuffer(Buffer);
    on_saMapSrolled(0);
    AnimBuffer->setTileBuffer(Buffer);
    AnimEditor->setTileBuffer(Buffer);
    AnimEditor->setAnimationBuffer(AnimBuffer);
    ui->actionWczytaj_textur_o_rozmiarach->setText("Wczytaj texturę ["+
                                                   QString::number(ui->sbTileWidth->value())+"x"+
                                                   QString::number(ui->sbTileHeight->value())+"px]");
}

void MainWindow::sbLayerSize_editingFinished()
{
    Layers[ui->cbSelectLayer->currentIndex()]->setWorldSize(
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

    out << (int) 99;
    out << (int) 85;
    out << QString("nazwa planszy bedzie tu");

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

    out << (int) Layers.size();

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

    Selector->getTexture()->save(CurrentFileName+".png");
}

void MainWindow::on_actionOtw_rz_triggered()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Otwórz mapę"),FilesPath,tr("Pliki map (*.map *.txp *.bin *.dat) \r\nWszystkie pliki (*)"));
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();
    CurrentFileName = fileName;
    slFileName->setText( CurrentFileName );

    on_actionNowy_triggered();

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file

    QString buf_str;
    int buf_int,buf_int2;
    // in >> str >> a;           // extract "the answer is" and 42

    in >> buf_int;
    in >> buf_int;
    in >> buf_str;

    in >> buf_int;
    ui->sbTileWidth->setValue(buf_int);
    in >> buf_int;
    ui->sbTileHeight->setValue(buf_int);
    sbTileSize_editingFinished();

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

    int num_layers;
    in >> num_layers;
    ui->cbSelectLayer->clear();
    for (int k=0; k<num_layers; k++ )
    {
        in >> buf_int;
        in >> buf_int2;
        Layers.append( new TileMap(buf_int,buf_int2));
        Layers[k]->setWorldSize(buf_int,buf_int2);
        ui->cbSelectLayer->addItem(QString::number(k+1));
        //Map->setWorldSize(buf_int,buf_int2);
        for (int j=0; j<Layers[k]->getSizeY(); j++)
            for (int i=0; i<Layers[k]->getSizeX(); i++)
            {
                in >> buf_int;
                Layers[k]->setTileAt(i,j,buf_int);
            }
    }
    in >> buf_int;
    in >> buf_int;

    QFileInfo texture_file(fileName+".png");
    if (texture_file.exists() && texture_file.isFile())
    {
        Selector->setTextureWidth(
                    Buffer->LoadImage(fileName+".png")
                    );
        sbTileSize_editingFinished();

        ui->sbTileHeight->setEnabled( Buffer->getCount() <= 0 );
        ui->sbTileWidth->setEnabled( Buffer->getCount() <= 0 );
    } else
    {
        if (Buffer->getCount() <= 0)
        {
            QMessageBox msgBox;
            msgBox.setText("Ta plansza nie posiada załączonego pliku textur. Wczytaj texturę ręcznie.");
            //msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok );
            msgBox.exec();
        }
    }
    on_saMapSrolled(0);
}

void MainWindow::on_actionZapisz_jako_triggered()
{
    QFileDialog dial( this, tr("Zapisz mapę"),FilesPath,tr("Pliki MAP (*.map);;Pliki TXP (*.txp);;Pliki BIN (*.bin);;Piki DAT (*.dat);;Wszystkie pliki (*)"));
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
    QString fileName = QFileDialog::getOpenFileName( this, tr("Importuj tekstury"),FilesPath,tr("Pliki graficzne (*.png *.jpg *.bmp) \r\nWszystkie pliki (*)"));

    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    Buffer->addLoadImage(fileName);
    sbTileSize_editingFinished();
    ui->sbTileHeight->setEnabled( Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Buffer->getCount() <= 0 );
}

void MainWindow::on_btDeleteFromTExture_clicked()
{
    QVector<QVector<int> > buf = Selector->getSelectedTiles();

    for (int j=0; j<buf.size(); j++)
        for (int i=0; i<buf[j].size(); i++)
            Buffer->remove(buf[j][i]);

    buf.clear();
    sbTileSize_editingFinished();
    ui->sbTileHeight->setEnabled( Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Buffer->getCount() <= 0 );
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
    if (AnimEditor->getSelectedAnimation() > AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    AnimBuffer->Animations.remove(
                AnimEditor->getSelectedAnimation());
    TexSelected();
    AnimEditor->repaint();
}

void MainWindow::on_sbFPS_editingFinished()
{
    if (AnimEditor->getSelectedAnimation() > AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].Fps = ui->sbFPS->value();
    AnimBuffer->syncAllAnimations();
}

void MainWindow::on_cbPingPongAnimation_toggled(bool checked)
{
    if (AnimEditor->getSelectedAnimation() > AnimBuffer->Animations.size()) return;
    if (AnimEditor->getSelectedAnimation() < 0) return;
    AnimBuffer->Animations[AnimEditor->getSelectedAnimation()].isPingPong = checked;
    AnimBuffer->syncAllAnimations();
    //ui->cbPingPongAnimation->
}

void MainWindow::on_btAddLayer_clicked()
{
    Layers.append(new TileMap(ui->sbLayerWidth->value(),ui->sbLayerHeight->value()));
    ui->cbSelectLayer->addItem(QString::number(Layers.size()));
    for (int i=0; i<ui->cbSelectLayer->count(); i++)
        ui->cbSelectLayer->setItemText(i, QString::number(i+1) );
    ui->cbSelectLayer->setCurrentIndex(ui->cbSelectLayer->count()-1);
    on_saMapSrolled(0);
}

void MainWindow::on_btDeleteLayer_clicked()
{
    if (Layers.size() <= 1) return;
    delete Layers[ui->cbSelectLayer->currentIndex()];
    Layers.remove( ui->cbSelectLayer->currentIndex() );
    ui->cbSelectLayer->removeItem( ui->cbSelectLayer->currentIndex() );
    for (int i=0; i<ui->cbSelectLayer->count(); i++)
        ui->cbSelectLayer->setItemText(i, QString::number(i+1) );
    on_saMapSrolled(0);
}

void MainWindow::on_cbSelectLayer_currentIndexChanged(int index)
{
    if (ui->cbSelectLayer->count() <= 0) return;
    Editor->setTileMap(Layers.at(index));
    ui->sbLayerWidth->setValue( Layers[index]->getSizeX() );
    ui->sbLayerHeight->setValue( Layers[index]->getSizeY() );
    on_saMapSrolled(0);
}

void MainWindow::on_actionJak_korzysta_z_programu_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Jak korzystać z programu?");
    msgBox.setInformativeText("1. Wybierz wymiary kafelków tekstury, jaką zamierzasz użyć \n 2. Wczytaj teksturę. \n 3. Ustaw wymiary warstwy jakie powinna mieć plansza. \n 4. Narysuj planszę używając kafelków z okna tekstur. \n 5. Zapisz planszę.");
    msgBox.setStandardButtons(QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_actionO_programie_triggered()
{
    QMessageBox msgBox;
    msgBox.setIconPixmap( QPixmap(":/new/prefix1/ics/New Piskel.png") );
    msgBox.setText(" ~ Map Editor ~");
    msgBox.setInformativeText("Idiotycznie prosty i uniwersalny edytor plansz do gier. \n\n(2016) \nby Paweł Pelikan");
    msgBox.setStandardButtons(QMessageBox::Ok );
    msgBox.exec();
}

void MainWindow::on_actionNowy_triggered()
{
    qDeleteAll(Layers); //wywołujemy destrukotyry
    Layers.clear();     //usuwamy wskaźniki
    Buffer->clear();
    AnimBuffer->clear();

    Layers.append( new TileMap() );
    slFileName->setText("Bez nazwy");

    AnimBuffer->setTileBuffer(Buffer);
    Selector->setTileBuffer(Buffer);
    AnimEditor->setTileBuffer(Buffer);
    AnimEditor->setAnimationBuffer(AnimBuffer);
    Editor->setTileBuffer(Buffer);
    Editor->setAnimationBuffer(AnimBuffer);
    Editor->setTileMap(Layers[0]);

    ui->cbSelectLayer->clear();
    ui->cbSelectLayer->addItem("1");

    ui->sbTileHeight->setEnabled( Buffer->getCount() <= 0 );
    ui->sbTileWidth->setEnabled( Buffer->getCount() <= 0 );

    on_saMapSrolled(0);
}

void MainWindow::on_actionJako_grafik_triggered()
{
    QFileDialog dial( this, tr("Eksportuj mapę jako grafikę"),FilesPath,tr("Pliki PNG (*.png);;Pliki JPEG (*.jpg);;Pliki BMP (*.bmp);;Wszystkie pliki (*)"));
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
    QFileDialog dial( this, tr("Eksportuj mapę jako tekst"),FilesPath,tr("Plik tekstowy (*.txt);;Wszystkie pliki (*)"));
    dial.setDefaultSuffix(".txt");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();

    QFile file( fileName );
    if ( file.open(QIODevice::WriteOnly) )
    {
        TileMap *Map = Layers[ui->cbSelectLayer->currentIndex()];
        QTextStream stream( &file );
        stream << "MAP SIZE X: " << QString::number(Map->getSizeX()) << endl;
        stream << "MAP SIZE Y: " << QString::number(Map->getSizeY()) << endl << endl;

        for (int j=0;j<Map->getSizeY(); j++)
        {
            for (int i=0;i<Map->getSizeX(); i++)
            {
                stream << QString::number( Map->getTileAt(i,j) ) << " ";
            }
            stream << endl;
        }
        file.close();
    }
}

void MainWindow::on_actionJako_plik_graficzny_triggered()
{
    QFileDialog dial( this, tr("Eksportuj texturę jako grafikę"),FilesPath,tr("Pliki PNG (*.png);;Pliki JPEG (*.jpg);;Pliki BMP (*.bmp);;Wszystkie pliki (*)"));
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
    QFileDialog dial( this, tr("Wybierz folder by wksportować texturę jako osobne grafiki"),FilesPath,tr("Pliki PNG (*.png);;Pliki JPEG (*.jpg);;Pliki BMP (*.bmp);;Wszystkie pliki (*)"));
    dial.setDefaultSuffix(".png");
    dial.setAcceptMode(QFileDialog::AcceptSave);
    if (! dial.exec() ) return;
    QString fileName = dial.selectedFiles().at(0);
    if (fileName == "") return;
    FilesPath = QFileInfo(fileName).absolutePath();
    QString ext =  QFileInfo(fileName).completeSuffix();
    fileName = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();

    for (int i=0;i<Buffer->getCount();i++)
    {
        Buffer->getTilePixmapAt(i)->save( fileName+QString::number(i)+"."+ext );
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


    //    if (settings.value("SelectedPort",0).toInt() <= ui->portsBox->count())
    //        ui->portsBox->setCurrentIndex(settings.value("SelectedPort",0).toInt());
    //    // ui->portsBox->setCurrentText( ui->portsBox->itemText(ui->portsBox->currentIndex()));
    //    // settings.setValue("SelectedPort",ui->portsBox->currentIndex());

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
