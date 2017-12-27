#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>

using namespace QtAV;
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_unit = 1000;
	m_playergroup = NULL;
	m_index = 0;
	m_xmlfilepath = "";
	m_fullscreenindex = -1;
	for(int i = 0; i < MAX_VIDEO_OUT; i++) {
		sVideoWindow * vo = new sVideoWindow;
		vo->label = new QLabel;
		vo->output = new (QtAV::VideoOutput);
		QVBoxLayout *vl =  new QVBoxLayout();
		vo->label->setText("NULL");
		vo->label->setAlignment(Qt::AlignHCenter);
		vl->addWidget(vo->label);
		vl->addWidget(vo->output->widget());
		vl->setStretch(0,0);
		vl->setStretch(1,1);
		ui->loVideoList->addLayout(vl, (i < 3)?0:1, i%3);
		m_videoout.append(vo);
	}
	m_audiobtngroup = new QButtonGroup(this);
	m_audiobtngroup->addButton(ui->btnAudio1, 0);
	m_audiobtngroup->addButton(ui->btnAudio2, 1);
	m_audiobtngroup->addButton(ui->btnAudio3, 2);
	m_audiobtngroup->addButton(ui->btnAudio4, 3);
	connect(m_audiobtngroup, SIGNAL(buttonClicked(int)), this, SLOT(Slot_ClickBtnGroup(int)));
	foreach(QAbstractButton * btn, m_audiobtngroup->buttons()) {
		btn->setCheckable(true);
	}
	m_singlevideooutput = new (QtAV::VideoOutput);
	ui->loSinVideo->addWidget(m_singlevideooutput->widget());
	QSlider * sl = new QSlider(ui->page_2);
	sl->setOrientation(Qt::Horizontal);
	ui->loSinVideo->addWidget(sl);

	ui->stackedWidget->setCurrentIndex(0);
	SetStopState();
	ui->btnIdle->setVisible(false);

	setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
	foreach (sVideoWindow * vo, m_videoout) {
		if(vo->label) {
			delete vo->label;
			vo->label = NULL;
		}
		if(vo->output) {
			delete vo->output;
			vo->output = NULL;
		}
		delete vo;
	}
	m_videoout.clear();

	if(m_audiobtngroup) {
		delete m_audiobtngroup;
		m_audiobtngroup = NULL;
	}
	if(m_singlevideooutput) {
		delete m_singlevideooutput;
		m_singlevideooutput = NULL;
	}
	delete ui;
}

void MainWindow::seekBySlider(int value)
{

	if(m_playergroup && m_playergroup->IsPlaying()){
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
	SetStopState();
}

void MainWindow::updateSlider(qint64 value)
{
	ui->sliProcess->setRange(0, int(m_playergroup->duration()/m_unit));
	ui->sliProcess->setValue(int(value/m_unit));

	int total_sec = m_playergroup->duration()/1000;
	int min = total_sec/60;
	int sec = total_sec - min * 60;

	int ptotal_sec = value/1000;
	int pmin = ptotal_sec/60;
	int psec = ptotal_sec - pmin * 60;
	QString str;
	str.sprintf("%0d:%02d/%0d:%02d", pmin, psec, min, sec);
	ui->lbProcess->setText(str);
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

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->button() != Qt::LeftButton) return;
	if(NULL == m_playergroup) return;

	if(-1 == m_fullscreenindex){
		int index = 0;
		for(int row = 0; row < ui->loVideoList->rowCount(); row++){
			for(int column = 0; column < ui->loVideoList->columnCount(); column++) {
				if(ui->loVideoList->cellRect(row, column).contains(event->pos())){
					index = row * ui->loVideoList->columnCount() + column;
					PlayFullScreen(index);
					break;
				}
			}
		}
	} else {
		ExitFullScreen(m_fullscreenindex);
	}
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

	for(int i = 0; i < videolist.size(); i++) {
		videofilelist.append(fileinfo.absolutePath() + "/" + videolist.at(i).file);
		m_videoout[i]->label->setText(videolist.at(i).desc);
	}
//	foreach(sVideoInfo vinfo, videolist) {
//		videofilelist.append(fileinfo.absolutePath() + "/" + vinfo.file);
//	}

#endif
	if(m_playergroup) {
		m_playergroup->Stop();
		delete m_playergroup;
	}
	QList<QtAV::VideoOutput *> output;
	foreach(sVideoWindow *vo, m_videoout) {
		output.append(vo->output);
	}

	SetPlayState(audiolist);
	m_playergroup = new PlayerGroup(audiofilelist, videofilelist, output);

	connect(m_playergroup, SIGNAL(Signal_PositionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playergroup, SIGNAL(Signal_Started()), SLOT(updateSlider()));
	connect(m_playergroup, SIGNAL(Signal_UpdateSliderUnit()), SLOT(updateSliderUnit()));
	m_index = 0;
	m_fullscreenindex = -1;
	m_playergroup->Play(m_index);
	m_audiobtngroup->button(m_index)->setChecked(true);
	m_xmlfilepath = xmlfilename;

}

void MainWindow::on_sliProcess_sliderMoved(int position)
{
	seekBySlider(position);
}

void MainWindow::Slot_ClickBtnGroup(int id)
{
	m_playergroup->SwitchAudio(id);
	foreach (QAbstractButton * btn, m_audiobtngroup->buttons()) {
		btn->setChecked(false);
	}
	m_audiobtngroup->button(id)->setChecked(true);
	m_index = id;
}

void MainWindow::PlayFullScreen(int index)
{
	if(m_playergroup->AddVideoOutput(index, m_singlevideooutput)) {
		ui->stackedWidget->setCurrentIndex(1);
		this->showFullScreen();
		m_fullscreenindex = index;
	}
}

void MainWindow::ExitFullScreen(int index)
{
	if(-1 != m_fullscreenindex) {
		this->showNormal();
		ui->stackedWidget->setCurrentIndex(0);
		m_playergroup->RemoveVideoOutput(index, m_singlevideooutput);
		m_fullscreenindex = -1;
	}
}

void MainWindow::on_btnIdle_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
	m_singlevideooutput->widget()->setWindowFlags (Qt::Window);
	m_singlevideooutput->widget()->showFullScreen();
}

void MainWindow::SetStopState()
{
	for(int i = 0; i < m_audiobtngroup->buttons().size(); i++) {
		m_audiobtngroup->buttons().at(i)->setEnabled(false);
		m_audiobtngroup->buttons().at(i)->setText(QString::number(i));
	}

	ui->btnPlayPause->setEnabled(false);
	ui->btnStop->setEnabled(false);
	ui->sliProcess->setEnabled(false);
	ui->stackedWidget->setCurrentIndex(0);
	ExitFullScreen(m_fullscreenindex);
}

void MainWindow::SetPlayState(QList<sAudioInfo> audiolist)
{
	for(int i = 0; i < MAX_AUDIO_FILE; i++) {
		if(i < audiolist.size()) {
			m_audiobtngroup->buttons().at(i)->setEnabled(true);
			m_audiobtngroup->buttons().at(i)->setText(audiolist.at(i).desc);
		} else {
			m_audiobtngroup->buttons().at(i)->setEnabled(false);
			m_audiobtngroup->buttons().at(i)->setText(QString::number(i+1));
		}
	}
	ui->btnPlayPause->setEnabled(true);
	ui->btnStop->setEnabled(true);
	ui->sliProcess->setEnabled(true);
}

void MainWindow::Slot_StateChanged(QtAV::AVPlayer::State state)
{
	if(QtAV::AVPlayer::StoppedState == state) {
		m_playergroup->Stop();
		delete m_playergroup;
		m_playergroup = NULL;
	}
	SetStopState();
}



