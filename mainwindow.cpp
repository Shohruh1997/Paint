#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QFileDialog>
#include "ScribbleArea.h"
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>
MainWindow::MainWindow()

{
    scrib = new ScribbleArea;
    setCentralWidget(scrib);
    createActions();
    createMenus();
    setWindowTitle(tr("Paint"));
    resize(1200,800);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        event->accept();
    }else
    {
        event->ignore();
    }
}

void MainWindow::open()
{
    if(maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open FIle"),QDir::currentPath());
        if(!fileName.isEmpty())
        {
            scrib->openImage(fileName);
        }
    }
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(scrib->penColor());
    if(newColor.isValid())
    {
        scrib->setPenColor(newColor);
    }
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Paint"),
                                        tr("Select Pen Width : "),
                                        scrib->penWidth(),
                                        1,50,1,&ok);
    if(ok)
    {
        scrib->setPenWidth(newWidth);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this,tr("About Paint"),
                       tr("<p>The<b>Paint</b> example is Awesome</p>"));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"),this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct,SIGNAL(triggered()),this,SLOT(open()));
    foreach(QByteArray format, QImageWriter::supportedImageFormats())
    {
        QString text= tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text,this);
        action->setData(format);
        connect(action,SIGNAL(triggered()),this,SLOT(save()));
        saveAsActs.append(action);
    }
    printAct = new QAction(tr("&Print..."),this);
    connect(printAct,SIGNAL(triggered()),scrib, SLOT(print()));
    exitAct = new QAction(tr("E&xit..."),this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    penColorAct = new QAction(tr("Pen Color..."),this);
    connect(penColorAct,SIGNAL(triggered()),this, SLOT(penColor()));

    penWidthAct = new QAction(tr("Pen &Width..."),this);
    connect(penWidthAct,SIGNAL(triggered()),this, SLOT(penWidth()));

    clearScreenAct = new QAction(tr("&Clear Screen..."),this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct,SIGNAL(triggered()),scrib, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About..."),this);
    connect(aboutAct,SIGNAL(triggered()),SLOT(about()));

    aboutQtAct = new QAction(tr("About &QT..."),this);
    connect(aboutQtAct,SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("Save As"), this);
    foreach(QAction *action, saveAsActs)
        saveAsMenu->addAction(action);
    fileMenu = new QMenu(tr("&File"),this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"),this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    helpMenu = new QMenu(tr("&Help"),this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave()
{
    if(scrib->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this,tr("Paint"),
                                   tr("Image has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret==QMessageBox::Save)
        {
            return saveFile("png");
        }else if (ret==QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath()+"/untitled."+fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    initialPath,
                                                    tr("%1 File (*.%2);; All File(*)")
                                                    .arg(QString::fromLatin1(fileFormat.toUpper()))
                                                    .arg(QString::fromLatin1(fileFormat)));
    if(fileName.isEmpty())
    {
        return false;
    }else
    {
        return scrib->saveImage(fileName,fileFormat.constData());
    }



}
