#include "MainWindow.h"

#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QScrollArea>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>

#include <QDebug>

#include "QHexView.h"


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags):
QMainWindow(parent, flags)
{
	QToolBar *ptb = addToolBar("File");

	QAction *pactOpen = ptb -> addAction("Open...");
	pactOpen -> setShortcut(QKeySequence("Ctrl+O"));
	connect(pactOpen, SIGNAL(triggered()), SLOT(slotOpen()));

	QMenu *pmenu = menuBar() -> addMenu("&File");
	pmenu -> addAction(pactOpen);
	addAction (pactOpen);
	pmenu -> addAction("Go to offset...", this, SLOT(slotToOffset())); 
	pmenu -> addAction("About...", this, SLOT(slotAbout()));
	pmenu -> addAction("Exit", this, SLOT(close()));

	QHexView *pwgt = new QHexView;
	setCentralWidget(pwgt);

	readCustomData();
}


void MainWindow::process(const QString &fileName)
{
	QFile file(fileName);


	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "File opening problem", "Problem with open file `" + fileName + "`for reading");
		return;
	}

	QHexView *pcntwgt = dynamic_cast<QHexView *>(centralWidget());

	pcntwgt -> clear();

	QByteArray arr = file.readAll();
	pcntwgt -> setData(new QHexView::DataStorageArray(arr));

}


void MainWindow::slotOpen()
{
	QString dir = QDir::currentPath();
	QSettings settings("QHexView", "QHexView");

	if(settings.value("QHexView/PrevDir").toString().length())
		dir = settings.value("QHexView/PrevDir").toString();

	QString fileName = QFileDialog::getOpenFileName(this, "Select file", dir);
	if(!fileName.isEmpty())
	{
		process(fileName);
		QFileInfo info(fileName);
		settings.setValue("QHexView/PrevDir", info.absoluteDir().absolutePath());

		setWindowTitle(info.fileName());
	}
}


void MainWindow::slotAbout()
{
	QString message;
	message = "<center><b>QHexView example</b></center>";
	message += "<center>virinext@gmail.com</center>";
	QMessageBox::about(this, "About", message);
}


void MainWindow::slotToOffset()
{
	bool ok;
	int offset = QInputDialog::getInt(0, "Offset", "Offset:", 0, 0, 2147483647, 1, &ok);

	if(ok)
	{
		QHexView *pcntwgt = dynamic_cast<QHexView *>(centralWidget());
		pcntwgt -> showFromOffset(offset);
	}
}


void MainWindow::closeEvent(QCloseEvent *pevent)
{
	saveCustomData();
	QWidget::closeEvent(pevent);
}


void MainWindow::saveCustomData()
{
	QSettings settings("QHexView", "QHexView");
	settings.setValue("MainWindow/geometry", saveGeometry());
}


void MainWindow::readCustomData()
{
	QSettings settings("QHexView", "QHexView");
	restoreGeometry(settings.value("MainWindow/geometry").toByteArray()); 
}

