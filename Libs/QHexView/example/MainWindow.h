#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>


class MainWindow: public QMainWindow
{
	Q_OBJECT
	public:
		MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	protected:
		virtual void closeEvent(QCloseEvent *);

	private:
		void process(const QString &fileName);
		void saveCustomData();
		void readCustomData();

	private slots:
		void slotOpen();
		void slotAbout();
		void slotToOffset();
};


#endif
