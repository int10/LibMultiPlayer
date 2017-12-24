#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtAV.h"
#include "PlayerGroup.h"
#include <QLabel>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include "ConfigXml.h"

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

private slots:
	void on_btnOpen_clicked();
	void on_btnPlayPause_clicked();
	void on_btnStop_clicked();
	void seekBySlider(int value);
	void seekBySlider();
	void on_sliProcess_sliderMoved(int position);

	void updateSlider(qint64 value);
	void updateSlider();
	void updateSliderUnit();
private:
	void Play(QString xmlfilename);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	Ui::MainWindow *ui;
	sVideoWindow *m_videoout[MAX_VIDEO_OUT];
	PlayerGroup *m_playergroup;
	int m_unit;
	int m_index;
	qint64 m_position;

};

#endif // MAINWINDOW_H
