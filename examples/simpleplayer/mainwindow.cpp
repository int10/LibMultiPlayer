#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#define VERSION "1.2"

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
	QStringList videolabel;
	videolabel<<"律师"<<"法官"<<"检察官"<<"嫌犯"<<"近镜证人"<<"证人";
	for(int i = 0; i < MAX_VIDEO_OUT; i++) {
		sVideoWindow * vo = new sVideoWindow;
		vo->label = new QLabel;
		vo->output = new (QtAV::VideoOutput);
		QVBoxLayout *vl =  new QVBoxLayout();
		vo->label->setText("NULL");
		vo->label->setAlignment(Qt::AlignHCenter);
		vo->label->setStyleSheet("font: 15pt \"微软雅黑\"; color: rgb(255, 255, 255);");
		vo->label->setText(videolabel.at(i));
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
	m_sliderprocess2 = new QSlider(ui->page_2);
	m_sliderprocess2->setOrientation(Qt::Horizontal);
	ui->loSinVideo->addWidget(m_sliderprocess2);
	connect(m_sliderprocess2, SIGNAL(sliderMoved(int)), this, SLOT(on_sliProcess_sliderMoved(int)));

	ui->stackedWidget->setCurrentIndex(0);
	SetStopState();
	ui->btnIdle->setVisible(false);
	ui->btnOpen->setVisible(false);

	this->setWindowTitle(QString("Multi player ") + VERSION);

	setAcceptDrops(true);

	/// 设置控件stylesheet ///
	QFile qssfile(QApplication::applicationDirPath() + "/skin.qss");
	qssfile.open(QIODevice::ReadOnly);
	qApp->setStyleSheet(qssfile.readAll());
	qssfile.close();

	Play();
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
//	QString file = QFileDialog::getOpenFileName(0, tr("Open a xml"), "", "xml file(*.xml)");
//	if (file.isEmpty()) {
//		return;
//	}
//	Play(file);
}

void MainWindow::on_btnPlay_clicked()
{
	if(m_playergroup->IsPlaying()) return;
	m_playergroup->PlayPause();
	ui->btnPlay->setVisible(false);
	ui->btnPause->setVisible(true);
}

void MainWindow::on_btnPause_clicked()
{
	if(!m_playergroup->IsPlaying()) return;
	m_playergroup->PlayPause();
	ui->btnPlay->setVisible(true);
	ui->btnPause->setVisible(false);
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

	m_sliderprocess2->setRange(0, int(m_playergroup->duration()/m_unit));
	m_sliderprocess2->setValue(int(value/m_unit));

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
//	QList<QUrl> urls = event->mimeData()->urls();

//	if (urls.isEmpty()) {
//		return;
//	}

//	QString fileName = urls.first().toLocalFile();
//	if (fileName.isEmpty()) {
//		return;
//	}
//	if(!fileName.endsWith(".xml")){
//		return;
//	}
//	Play(fileName);
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


//void MainWindow::Play(QString xmlfilename)
//{
//	QFileInfo fileinfo(xmlfilename);
//	QList<sAudioInfo> audiolist;
//	QList<sVideoInfo> videolist;
//	ConfigXml xml;
//	if(!xml.ParseXml(xmlfilename, audiolist, videolist)) {
//		QMessageBox::critical(this, "error", "Can't parse the xml!");
//		return;
//	}

//	QStringList audiofilelist;
//	QStringList videofilelist;
//#if 0
//	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/1_20171017170849.mp3");
//	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/2_20171017170849.mp3");
//	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/3_20171017170849.mp3");
//	audiolist.append("E:/SysFolder/Desktop/Court_1_20171017/4_20171017170849.mp3");

//	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-1-20171017170848966.asf");
//	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-2-20171017170849018.asf");
//	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-3-20171017170849174.asf");
//	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-4-20171017170849207.asf");
//	videolist.append("E:/SysFolder/Desktop/Court_1_20171017/Court_1-5-20171017170848962.asf");
//#else
//	foreach(sAudioInfo ainfo, audiolist) {
//		audiofilelist.append(fileinfo.absolutePath() + "/" + ainfo.file);
//	}

//	for(int i = 0; i < videolist.size(); i++) {
//		videofilelist.append(fileinfo.absolutePath() + "/" + videolist.at(i).file);
//		m_videoout[i]->label->setText(videolist.at(i).desc);
//	}
////	foreach(sVideoInfo vinfo, videolist) {
////		videofilelist.append(fileinfo.absolutePath() + "/" + vinfo.file);
////	}

//#endif
//	if(m_playergroup) {
//		m_playergroup->Stop();
//		delete m_playergroup;
//	}
//	QList<QtAV::VideoOutput *> output;
//	foreach(sVideoWindow *vo, m_videoout) {
//		output.append(vo->output);
//	}

//	SetPlayState(audiolist);
//	m_playergroup = new PlayerGroup(audiofilelist, videofilelist, output);

//	connect(m_playergroup, SIGNAL(Signal_PositionChanged(qint64)), SLOT(updateSlider(qint64)));
//	connect(m_playergroup, SIGNAL(Signal_Started()), SLOT(updateSlider()));
//	connect(m_playergroup, SIGNAL(Signal_UpdateSliderUnit()), SLOT(updateSliderUnit()));
//	connect(m_playergroup, SIGNAL(Signal_mediaStateChanged(QMediaPlayer::State)), SLOT(Slot_MediaStateChanged(QMediaPlayer::State)));
//	m_index = 0;
//	m_fullscreenindex = -1;
//	m_playergroup->Play(m_index);
//	m_audiobtngroup->button(m_index)->setChecked(true);
//	m_xmlfilepath = xmlfilename;

//}

void MainWindow::Play()
{
	QString strpath = QApplication::applicationDirPath();
	QDir curdir(strpath);
	QStringList filelist = curdir.entryList(QDir::Files);

	m_audiolist.clear();
	m_videolist.clear();
	//获取音频文件
	QList<QRegExp> audioreglist;
	audioreglist.append(QRegExp("^1_.+\\.mp3"));
	audioreglist.append(QRegExp("^2_.+\\.mp3"));
	audioreglist.append(QRegExp("^3_.+\\.mp3"));
	audioreglist.append(QRegExp("^4_.+\\.mp3"));

	foreach (QRegExp rx, audioreglist) {
		int i;
		for(i = 0; i < filelist.size(); i++) {
			QString filename = filelist.at(i);
			if(filename.contains(rx)){
				m_audiolist.append(strpath + "/" + filename);
				break;
			}
		}
		if(i >= filelist.size()){
			m_audiolist.append("");//if not found , add null string to it
		}
	}
	qDebug()<<"Audio list:"<<m_audiolist;

	//获取视频文件
	QList<QRegExp> videoreglist;
	videoreglist.append(QRegExp("^Court_1-1-.+\\.asf"));
	videoreglist.append(QRegExp("^Court_1-2-.+\\.asf"));
	videoreglist.append(QRegExp("^Court_1-3-.+\\.asf"));
	videoreglist.append(QRegExp("^Court_1-4-.+\\.asf"));
	videoreglist.append(QRegExp("^Court_1-5-.+\\.asf"));
	videoreglist.append(QRegExp("^Court_1-20-.+\\.asf"));

	foreach (QRegExp rx, videoreglist) {
		int i;
		for(i = 0; i < filelist.size(); i++) {
			QString filename = filelist.at(i);
			if(filename.contains(rx)){
				m_videolist.append(strpath + "/" + filename);
				break;
			}
		}
		if(i >= filelist.size()){
			m_videolist.append("");//if not found , add null string to it
		}
	}
	qDebug()<<"Video List:"<<m_videolist;

	if(m_playergroup) {
		m_playergroup->Stop();
		delete m_playergroup;
	}
	QList<QtAV::VideoOutput *> output;
	foreach(sVideoWindow *vo, m_videoout) {
		output.append(vo->output);
	}

	SetPlayState(m_audiolist);
	m_playergroup = new PlayerGroup(m_audiolist, m_videolist, output);

	connect(m_playergroup, SIGNAL(Signal_PositionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playergroup, SIGNAL(Signal_Started()), SLOT(updateSlider()));
	connect(m_playergroup, SIGNAL(Signal_UpdateSliderUnit()), SLOT(updateSliderUnit()));
	connect(m_playergroup, SIGNAL(Signal_mediaStateChanged(QMediaPlayer::State)), SLOT(Slot_MediaStateChanged(QMediaPlayer::State)));
	m_index = 0;
	m_fullscreenindex = -1;
	m_playergroup->Play(m_index);
	m_audiobtngroup->button(m_index)->setChecked(true);
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
		//m_audiobtngroup->buttons().at(i)->setText(QString::number(i));
	}

	ui->btnPlay->setEnabled(false);
	ui->btnPause->setEnabled(false);
	ui->btnStop->setEnabled(false);
	ui->sliProcess->setEnabled(false);
	ui->stackedWidget->setCurrentIndex(0);
	ExitFullScreen(m_fullscreenindex);
}

//void MainWindow::SetPlayState(QList<sAudioInfo> audiolist)
//{
//	for(int i = 0; i < MAX_AUDIO_FILE; i++) {
//		if(i < audiolist.size()) {
//			m_audiobtngroup->buttons().at(i)->setEnabled(true);
//			m_audiobtngroup->buttons().at(i)->setText(audiolist.at(i).desc);
//		} else {
//			m_audiobtngroup->buttons().at(i)->setEnabled(false);
//			m_audiobtngroup->buttons().at(i)->setText(QString::number(i+1));
//		}
//	}
//	ui->btnPlayPause->setEnabled(true);
//	ui->btnStop->setEnabled(true);
//	ui->sliProcess->setEnabled(true);
//}

void MainWindow::SetPlayState(QStringList audiolist)
{
	for(int i = 0; i < MAX_AUDIO_FILE; i++) {
		if(m_audiolist.at(i) == ""){
			m_audiobtngroup->buttons().at(i)->setEnabled(false);
		} else {
			m_audiobtngroup->buttons().at(i)->setEnabled(true);
		}
	}
	ui->btnPlay->setEnabled(true);
	ui->btnPlay->setVisible(false);
	ui->btnPause->setEnabled(true);
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

void MainWindow::Slot_MediaStateChanged(QMediaPlayer::State state)
{
	if(QMediaPlayer::StoppedState == state) {
		m_playergroup->Stop();
		delete m_playergroup;
		m_playergroup = NULL;
	}
	SetStopState();
}

