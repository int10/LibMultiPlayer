/******************************************************************************
    Simple Player:  this file is part of QtAV examples
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "playerwindow.h"
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QMessageBox>
#include <QFileDialog>

using namespace QtAV;

PlayerWindow::PlayerWindow(QWidget *parent) : QWidget(parent)
{
    m_unit = 1000;
    setWindowTitle(QString::fromLatin1("QtAV simple player example"));
//	m_player = new AVPlayer(this);
    QVBoxLayout *vl = new QVBoxLayout();
    setLayout(vl);
	vmain = new QGridLayout();
//    m_vo = new VideoOutput(this);
//    if (!m_vo->widget()) {
//        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
//        return;
//    }
//	m_player->setRenderer(m_vo);
	//vl->addWidget(m_vo->widget());
	vl->addLayout(vmain);
//	vmain->addWidget(m_vo->widget());
    m_slider = new QSlider();
    m_slider->setOrientation(Qt::Horizontal);
    connect(m_slider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(m_slider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));
//    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
//    connect(m_player, SIGNAL(started()), SLOT(updateSlider()));
//    connect(m_player, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));

    vl->addWidget(m_slider);
    QHBoxLayout *hb = new QHBoxLayout();
    vl->addLayout(hb);
    m_openBtn = new QPushButton(tr("Open"));
    m_playBtn = new QPushButton(tr("Play/Pause"));
    m_stopBtn = new QPushButton(tr("Stop"));
    hb->addWidget(m_openBtn);
    hb->addWidget(m_playBtn);
    hb->addWidget(m_stopBtn);
    connect(m_openBtn, SIGNAL(clicked()), SLOT(openMedia()));
    connect(m_playBtn, SIGNAL(clicked()), SLOT(playPause()));
//    connect(m_stopBtn, SIGNAL(clicked()), m_player, SLOT(stop()));
}

void PlayerWindow::openMedia()
{

//    QString file = QFileDialog::getOpenFileName(0, tr("Open a video"));
//    if (file.isEmpty())
//        return;
//    m_player->play(file);
	QStringList audiolist;
	audiolist.append("E:/SysFolder/Desktop/Player/part/1_20170615171110884.mp3");
	audiolist.append("E:/SysFolder/Desktop/Player/part/2_20170615171110905.WAV");
	QStringList videolist;
	videolist.append("E:/SysFolder/Desktop/Player/part/Court_1-1-20170615171110757.asf");
	videolist.append("E:/SysFolder/Desktop/Player/part/Court_1-4-20170615171110880.asf");
	videolist.append("E:/SysFolder/Desktop/Player/part/Court_1-5-20170615171110753.asf");
	m_playergroup = new PlayerGroup(audiolist, videolist);
	QList<QtAV::VideoOutput *> output = m_playergroup->GetVideoOutput();
	for(int i = 0; i < output.size(); i++){
		vmain->addWidget(output.at(i)->widget(), (i < 2)?0:1, i%2);
	}

	connect(m_playergroup, SIGNAL(Signal_PositionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playergroup, SIGNAL(Signal_Started()), SLOT(updateSlider()));
	connect(m_playergroup, SIGNAL(Signal_UpdateSliderUnit()), SLOT(updateSliderUnit()));
	m_playergroup->Play();
}

void PlayerWindow::seekBySlider(int value)
{
//    if (!m_player->isPlaying())
//        return;
//    m_player->seek(qint64(value*m_unit));

	if(m_playergroup->IsPlaying()){
		m_playergroup->Seek(qint64(value * m_unit));
	}

}

void PlayerWindow::seekBySlider()
{
	seekBySlider(m_slider->value());
}

void PlayerWindow::playPause()
{
//    if (!m_player->isPlaying()) {
//        m_player->play();
//        return;
//    }
//    m_player->pause(!m_player->isPaused());
}

void PlayerWindow::updateSlider(qint64 value)
{
	m_slider->setRange(0, int(m_playergroup->duration()/m_unit));
	m_slider->setValue(int(value/m_unit));
}

void PlayerWindow::updateSlider()
{
	updateSlider(m_playergroup->position());
}

void PlayerWindow::updateSliderUnit()
{
	m_unit = m_playergroup->notifyInterval();
	updateSlider();
}
