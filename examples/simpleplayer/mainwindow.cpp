#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>

using namespace QtAV;
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	for(int i = 0; i < MAX_VIDEO_OUT; i++) {
		m_videoout[i] = new sVideoWindow;
		m_videoout[i]->label = new QLabel;
		m_videoout[i]->output = new (QtAV::VideoOutput);
		QVBoxLayout *vl =  new QVBoxLayout();
		m_videoout[i]->label->setText("屡");
		m_videoout[i]->label->setAlignment(Qt::AlignHCenter);
		vl->addWidget(m_videoout[i]->label);
		vl->addWidget(m_videoout[i]->output->widget());
		qDebug()<<m_videoout[i]->output->widget()->objectName();
		vl->setStretch(0,0);
		vl->setStretch(1,1);
		ui->loVideoList->addLayout(vl, (i < 3)?0:1, i%3);
		connect(m_videoout[i]->output->widget(), SIGNAL());
	}
}

MainWindow::~MainWindow()
{
	for(int i = 0; i < MAX_VIDEO_OUT; i++) {
		if(m_videoout[i]->label) {
			delete m_videoout[i]->label;
			m_videoout[i]->label = NULL;
		}
		if(m_videoout[i]->output) {
			delete m_videoout[i]->output;
			m_videoout[i]->output = NULL;
		}
		delete m_videoout[i];
		m_videoout[i] = NULL;
	}
	delete ui;
}
