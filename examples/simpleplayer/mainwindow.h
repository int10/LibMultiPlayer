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
#include <QButtonGroup>

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
	void Slot_ClickBtnGroup(int id);
	void Slot_StateChanged(QtAV::AVPlayer::State state);
	void on_btnIdle_clicked();
private:
	void Play(QString xmlfilename);
	void PlayFullScreen(int index);
	void ExitFullScreen(int index);
	void SetStopState();
	void SetPlayState(int audiocount);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *event);
private:
	Ui::MainWindow *ui;
	//sVideoWindow *m_videoout[MAX_VIDEO_OUT];
	QList<sVideoWindow *> m_videoout;
	QtAV::VideoOutput *m_singlevideooutput;
	PlayerGroup *m_playergroup;
	int m_unit;
	int m_index;
	int m_fullscreenindex;
	qint64 m_position;
	QButtonGroup *m_audiobtngroup;
	QString m_xmlfilepath;
};

#endif // MAINWINDOW_H
