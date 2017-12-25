#include "PlayerGroup.h"
#include <QDebug>

PlayerGroup::PlayerGroup(QStringList audiolist, QStringList videolist)
{
	m_audiolist = audiolist;
	m_videolist = videolist;
	m_audioplayer = new QtAV::AVPlayer;

	for(int i = 0; i < videolist.size(); i++) {
		QtAV::AVPlayer *player = new (QtAV::AVPlayer);
		m_playerlist.append(player);
		QtAV::VideoOutput * output = new (QtAV::VideoOutput);
		m_volist.append(output);
		player->setRenderer(output);
	}
	m_isplaying = false;

//	connect(m_audioplayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
//	connect(m_audioplayer, SIGNAL(started()), SLOT(updateSlider()));
//	connect(m_audioplayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_playerlist.at(0), SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playerlist.at(0), SIGNAL(started()), SLOT(updateSlider()));
	connect(m_playerlist.at(0), SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	m_synctimer = new QTimer(this);
	connect(m_synctimer, SIGNAL(timeout()), SLOT(timeoutHandle()));
	m_synctimer->start(1000);
}

PlayerGroup::PlayerGroup(QStringList audiolist, QStringList videolist, QList<QtAV::VideoOutput *> videooutput)
{
	m_audiolist = audiolist;
	m_videolist = videolist;
	m_audioplayer = new QtAV::AVPlayer;
	for(int i = 0; i < videolist.size(); i++) {
		QtAV::AVPlayer *player = new (QtAV::AVPlayer);
		m_playerlist.append(player);
		player->setRenderer(videooutput.at(i));
	}

	m_isplaying = false;

//	connect(m_audioplayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
//	connect(m_audioplayer, SIGNAL(started()), SLOT(updateSlider()));
//	connect(m_audioplayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_playerlist.at(0), SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playerlist.at(0), SIGNAL(started()), SLOT(updateSlider()));
	connect(m_playerlist.at(0), SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	m_synctimer = new QTimer(this);
	connect(m_synctimer, SIGNAL(timeout()), SLOT(timeoutHandle()));
	m_synctimer->start(1000);
}

PlayerGroup::~PlayerGroup()
{
	Stop();
	foreach(QtAV::AVPlayer *player, m_playerlist) {
		delete player;
	}
	m_playerlist.clear();
	foreach(QtAV::VideoOutput *output, m_volist) {
		delete output;
	}
	m_volist.clear();
	if(m_synctimer){
		m_synctimer->stop();
		delete m_synctimer;
	}
}

void PlayerGroup::Play(int index)
{
	//m_audioplayer->play(m_audiolist.at(index), &m_mainclock, index);
	m_curaudioindex = 0;
	for(int i = 0; i < m_videolist.size(); i++) {
		if(0 == i) {
			m_playerlist.at(i)->setAudioStream(m_audiolist.at(index));
//			m_playerlist.at(i)->play(m_videolist.at(i), &m_mainclock, 0);
			m_playerlist.at(i)->play(m_videolist.at(i));

		} else {
//			m_playerlist.at(i)->play(m_videolist.at(i), &m_mainclock, 1);
			m_playerlist.at(i)->play(m_videolist.at(i));
		}
	}
	m_isplaying = true;
}

bool PlayerGroup::IsPlaying()
{
	return m_isplaying;
}

void PlayerGroup::PlayPause()
{
	if (!m_playerlist.at(0)->isPlaying()) {
		foreach(QtAV::AVPlayer *player, m_playerlist) {
			player->play();
		}
		return;
	}
	foreach(QtAV::AVPlayer *player, m_playerlist) {
		player->pause(!player->isPaused());
	}
}

void PlayerGroup::SwitchAudio(int index)
{
	if(index < 0 || index >= m_audiolist.size() || index == m_curaudioindex){
		return;
	}

	m_curaudioindex = index;
	m_playerlist.at(0)->setAudioStream(m_audiolist.at(index));
//	qint64 pos = m_audioplayer->position();
//	m_audioplayer->stop();

//	m_audioplayer->play(m_audiolist.at(index), &m_mainclock, 0);
//	Seek(pos);
}

void PlayerGroup::Stop()
{
	foreach(QtAV::AVPlayer * player, m_playerlist){
		player->stop();
	}
	m_audioplayer->stop();
}

void PlayerGroup::Seek(qint64 value)
{
	m_audioplayer->seek(value);
	foreach(QtAV::AVPlayer * player, m_playerlist){
		player->seek(value);
	}
}

void PlayerGroup::updateSliderUnit()
{
	emit Signal_UpdateSliderUnit();
}

void PlayerGroup::updateSlider(qint64 value)
{
	emit Signal_PositionChanged(value);
}

void PlayerGroup::updateSlider()
{
	emit Signal_Started();
}

int PlayerGroup::notifyInterval()
{
//	return m_audioplayer->notifyInterval();
	return m_playerlist.at(0)->notifyInterval();
}

qint64 PlayerGroup::position()
{
//	return m_audioplayer->position();
	return m_playerlist.at(0)->position();
}

qint64 PlayerGroup::duration()
{
//	return m_audioplayer->duration();
	return m_playerlist.at(0)->duration();
}

QList<QtAV::VideoOutput *> PlayerGroup::GetVideoOutput()
{
	return m_volist;
}

void PlayerGroup::AddVideoOutput(int index, QtAV::VideoOutput * output)
{
	if(index >= m_playerlist.size()) return;
	if(!m_playerlist.at(index)->isPlaying()) return;
	m_playerlist.at(index)->addVideoRenderer(output);
}

void PlayerGroup::RemoveVideoOutput(int index, QtAV::VideoOutput * output)
{
	if(index >= m_playerlist.size()) return;
	if(!m_playerlist.at(index)->isPlaying()) return;
	m_playerlist.at(index)->removeVideoRenderer(output);
}

void PlayerGroup::timeoutHandle()
{
	QList<double> poslist;
	if(m_playerlist.at(0)->isPlaying()) {
		double pos = m_playerlist.at(0)->masterClock()->value();
		for(int i = 1; i < m_playerlist.size(); i++){
			if(m_playerlist.at(i)->isPlaying()){
				double ppos = m_playerlist.at(i)->masterClock()->value();
				if(qAbs(pos - ppos) > 0.1){		//> 0.1ms update clock;
					m_playerlist.at(i)->updateClock(qint64(pos * 1000.0));
				}
			}
		}
	}

	foreach(QtAV::AVPlayer * player, m_playerlist){
		poslist.append(player->masterClock()->value());
	}

	qDebug()<<poslist;
}
