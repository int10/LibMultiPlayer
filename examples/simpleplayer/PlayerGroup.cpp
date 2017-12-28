#include "PlayerGroup.h"
#include <QDebug>

PlayerGroup::PlayerGroup(QStringList audiolist, QStringList videolist)
{
	m_audiolist = audiolist;
	m_videolist = videolist;

	for(int i = 0; i < videolist.size(); i++) {
		QtAV::AVPlayer *player = new (QtAV::AVPlayer);
		m_playerlist.append(player);
		QtAV::VideoOutput * output = new (QtAV::VideoOutput);
		m_volist.append(output);
		player->setRenderer(output);
	}
	m_isplaying = false;
	m_synctimer = NULL;

#ifdef USE_QMEDIAPLAYER
	m_audioplayer = new QMediaPlayer;
	connect(m_audioplayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	//	connect(m_audioplayer, SIGNAL(started()), SLOT(updateSlider()));
	connect(m_audioplayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_audioplayer, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QtAV::AVPlayer::State)));

#else
	m_audioplayer = new QtAV::AVPlayer;
	connect(m_playerlist.at(0), SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playerlist.at(0), SIGNAL(started()), SLOT(updateSlider()));
	connect(m_playerlist.at(0), SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_playerlist.at(0), SIGNAL(stateChanged(QtAV::AVPlayer::State)), SLOT(stateChanged(QtAV::AVPlayer::State)));

	m_synctimer = new QTimer(this);
	connect(m_synctimer, SIGNAL(timeout()), SLOT(timeoutHandle()));
	m_synctimer->start(1000);
#endif
}

PlayerGroup::PlayerGroup(QStringList audiolist, QStringList videolist, QList<QtAV::VideoOutput *> videooutput)
{
	m_audiolist = audiolist;
	m_videolist = videolist;

	for(int i = 0; i < videolist.size(); i++) {
		QtAV::AVPlayer *player = new (QtAV::AVPlayer);
		m_playerlist.append(player);
		player->setRenderer(videooutput.at(i));
	}
	m_isplaying = false;
	m_synctimer = NULL;

#ifdef USE_QMEDIAPLAYER
	m_audioplayer = new QMediaPlayer;
	connect(m_audioplayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	//	connect(m_audioplayer, SIGNAL(started()), SLOT(updateSlider()));
	connect(m_audioplayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_audioplayer, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QtAV::AVPlayer::State)));

#else
	m_audioplayer = new QtAV::AVPlayer;
	connect(m_playerlist.at(0), SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_playerlist.at(0), SIGNAL(started()), SLOT(updateSlider()));
	connect(m_playerlist.at(0), SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));
	connect(m_playerlist.at(0), SIGNAL(stateChanged(QtAV::AVPlayer::State)), SLOT(stateChanged(QtAV::AVPlayer::State)));

	m_synctimer = new QTimer(this);
	connect(m_synctimer, SIGNAL(timeout()), SLOT(timeoutHandle()));
	m_synctimer->start(1000);
#endif
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
	if(m_audioplayer){
		delete m_audioplayer;
	}
	if(m_synctimer){
		m_synctimer->stop();
		delete m_synctimer;
	}
}

void PlayerGroup::Play(int index)
{
	m_curaudioindex = index;
	for(int i = 0; i < m_videolist.size(); i++) {
		if(0 == i) {
		#ifdef USE_QMEDIAPLAYER
			m_audioplayer->setMedia(QUrl::fromLocalFile(m_audiolist.at(index)));
			m_audioplayer->play();
		#else
			m_playerlist.at(i)->setAudioStream(m_audiolist.at(index));
		#endif
			m_playerlist.at(i)->play(m_videolist.at(i));

		} else {
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
#ifdef USE_QMEDIAPLAYER
	if(QMediaPlayer::PausedState == m_audioplayer->state()) {
		m_audioplayer->play();
		foreach(QtAV::AVPlayer *player, m_playerlist) {
			player->play();
		}
		m_isplaying = true;
		return;
	}
	foreach(QtAV::AVPlayer *player, m_playerlist) {
		player->pause(!player->isPaused());
	}
	m_audioplayer->pause();
	m_isplaying = false;

#else
	if (!m_playerlist.at(0)->isPlaying()) {
		foreach(QtAV::AVPlayer *player, m_playerlist) {
			player->play();
		}
		m_isplaying = true;
		return;
	}

	foreach(QtAV::AVPlayer *player, m_playerlist) {
		player->pause(!player->isPaused());
	}
	m_isplaying = false;
#endif
}

void PlayerGroup::SwitchAudio(int index)
{
	if(index < 0 || index >= m_audiolist.size() || index == m_curaudioindex){
		return;
	}

	m_curaudioindex = index;
#ifdef USE_QMEDIAPLAYER
	qint64 pos = m_audioplayer->position();
	m_audioplayer->stop();
	m_audioplayer->setMedia(QUrl::fromLocalFile(m_audiolist.at(index)));
	m_audioplayer->setPosition(pos);
	m_audioplayer->play();

	if(m_isplaying == false) {
		foreach(QtAV::AVPlayer *player, m_playerlist) {
			player->play();
		}
		m_isplaying = true;
	}
#else
	m_playerlist.at(0)->setAudioStream(m_audiolist.at(index));
	if(m_isplaying == false) {
		foreach(QtAV::AVPlayer *player, m_playerlist) {
			player->play();
		}
		m_isplaying = true;
	}
#endif
}

void PlayerGroup::Stop()
{
	foreach(QtAV::AVPlayer * player, m_playerlist){
		player->stop();
	}
	m_audioplayer->stop();
	m_isplaying = false;
}

void PlayerGroup::Seek(qint64 value)
{
#ifdef USE_QMEDIAPLAYER
	m_audioplayer->setPosition(value);
	foreach(QtAV::AVPlayer * player, m_playerlist){
		player->setPosition(value);
	}
#else
	m_audioplayer->seek(value);
	foreach(QtAV::AVPlayer * player, m_playerlist){
		player->seek(value);
	}
#endif

}

void PlayerGroup::updateSliderUnit()
{
	emit Signal_UpdateSliderUnit();
}

void PlayerGroup::updateSlider(qint64 value)
{
	static qint64 tinysync = -3000;
	qint64 pos = m_audioplayer->position();
	foreach(QtAV::AVPlayer * player, m_playerlist){
		if(player->isPlaying()) {
			if(pos > tinysync) {
				if(qAbs(pos - tinysync - player->position()) > 200)	//sync in cast a-v bigger than 200ms;
					player->updateClock(pos - tinysync);
				if(qAbs(pos - tinysync - player->position()) > 1000)	//seek in cast a-v bigger than 1s;
					player->setPosition(pos - tinysync);
			}
		}
	}

	QList<int> poslist;
	poslist.append(m_audioplayer->position());
	foreach(QtAV::AVPlayer * player, m_playerlist){
		poslist.append(player->position());
	}
	qDebug()<<poslist;



	emit Signal_PositionChanged(value);
}

void PlayerGroup::updateSlider()
{
	emit Signal_Started();
}

int PlayerGroup::notifyInterval()
{
#ifdef USE_QMEDIAPLAYER
	return 1000;
#else
	return m_playerlist.at(0)->notifyInterval();
#endif
}

qint64 PlayerGroup::position()
{
#ifdef USE_QMEDIAPLAYER
	return m_audioplayer->position();
#else
	return m_playerlist.at(0)->position();
#endif
}

qint64 PlayerGroup::duration()
{
#ifdef USE_QMEDIAPLAYER
	return m_audioplayer->duration();
#else
	return m_playerlist.at(0)->duration();
#endif
}

void PlayerGroup::stateChanged(QtAV::AVPlayer::State state)
{
	emit Signal_StateChanged(state);
}

void PlayerGroup::mediaStateChanged(QMediaPlayer::State state)
{
	emit Signal_mediaStateChanged(state);
}

QList<QtAV::VideoOutput *> PlayerGroup::GetVideoOutput()
{
	return m_volist;
}

bool PlayerGroup::AddVideoOutput(int index, QtAV::VideoOutput * output)
{
	if(index >= m_playerlist.size()) return false;
	if(!m_playerlist.at(index)->isPlaying()) return false;
	m_playerlist.at(index)->addVideoRenderer(output);
	return true;
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
