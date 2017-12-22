#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtAV.h"
#include "PlayerGroup.h"
#include <QLabel>

namespace Ui {
class MainWindow;
}

typedef struct{
	QtAV::VideoOutput *output;
	QLabel *label;
}sVideoWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	sVideoWindow *m_videoout[MAX_VIDEO_OUT];

};

#endif // MAINWINDOW_H
