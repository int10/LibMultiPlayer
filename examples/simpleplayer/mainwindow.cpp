#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>

using namespace QtAV;
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_unit = 1000;
	m_playergroup = NULL;
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
	}
	setAcceptDrops(true);
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

void MainWindow::seekBySlider(int value)
{

	if(m_playergroup->IsPlaying()){
		m_playergroup->Seek(qint64(value * m_unit));
	}

}

void MainWindow::seekBySlider()
{
	seekBySlider(ui->sliProcess->value());
}

void MainWindow::on_btnOpen_clicked()
{
	QString file = QFileDialog::getOpenFileName(0, tr("Open a xml"), "", "xml file(*.xml)");
	if (file.isEmpty()) {
		return;
	}
	Play(file);
}

void MainWindow::on_btnPlayPause_clicked()
{
	m_playergroup->PlayPause();
}

void MainWindow::on_btnStop_clicked()
{
	m_playergroup->Stop();
}

void MainWindow::updateSlider(qint64 value)
{
	ui->sliProcess->setRange(0, int(m_playergroup->duration()/m_unit));
	ui->sliProcess->setValue(int(value/m_unit));
}

void MainWindow::updateSlider()
{
	updateSlider(m_playergroup->position());
}

void MainWindow::updateSliderUnit()
{
	m_unit = m_playergroup->notifyInterval();
	updateSlider();
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();

	if (urls.isEmpty()) {
		return;
	}

	QString fileName = urls.first().toLocalFile();
	if (fileName.isEmpty()) {
		return;
	}
	if(!fileName.endsWith(".xml")){
		return;
	}
	Play(fileName);
}

void MainWindow::Play(QString xmlfilename)
{
	QFileInfo fileinfo(xmlfilename);
	QList<sAudioInfo> audiolist;
	QList<sVideoInfo> videolist;
	ConfigXml xml;
	if(!xml.ParseXml(xmlfilename, audiolist, videolist)) {
		QMessageBox::critical(this, "error", "Can't parse the xml!");
		return;
	}

	QStringList audiofilelist;
	QStringList videofilelist;
#if 0
	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/1_20171017170849.mp3");
	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/2_20171017170849.mp3");
	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/3_20171017170849.mp3");
	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/4_20171017170849.mp3");

	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-1-20171017170848966.asf");
	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-2-20171017170849018.asf");
	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-3-20171017170849174.asf");
	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-4-20171017170849207.asf");
	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-5-20171017170848962.asf");
#else
	foreach(sAudioInfo ainfo, audiolist) {
		audiofilelist.append(fileinfo.absolutePath() + "/" + ainfo.file);
	}
	foreach(sVideoInfo vinfo, videolist) {
		videofilelist.append(fileinfo.absolutePath() + "/" + vinfo.file);
	}

#endif
	if(m_playergroup) {
		m_playergroup->Stop();
		delete m_playergroup;
	}
	QList<QtAV::VideoOutput *> output;
	for(int i = 0; i < MAX_VIDEO_OUT; i++) {
		output.append(m_videoout[i]->output);
	}
	m_playergroup = new PlayerGroup(audiofilelist, videofilelist, output);

	connect(m_playergroup, SIGNAL(Signal_PositionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playergroup, SIGNAL(Signal_Started()), SLOT(updateSlider()));
	connect(m_playergroup, SIGNAL(Signal_UpdateSliderUnit()), SLOT(updateSliderUnit()));
	m_playergroup->Play(m_index);

}

void MainWindow::on_sliProcess_sliderMoved(int position)
{
	seekBySlider(position);
}
