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

	connect(m_audioplayer, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
	connect(m_audioplayer, SIGNAL(started()), SLOT(updateSlider()));
	connect(m_audioplayer, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));

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
}

void PlayerGroup::Play()
{
	m_audioplayer->play(m_audiolist.at(0), &m_mainclock, 0);
	for(int i = 0; i < m_videolist.size(); i++) {
		m_playerlist.at(i)->play(m_videolist.at(i), &m_mainclock, 1);
	}
	m_isplaying = true;
}

bool PlayerGroup::IsPlaying()
{
	return m_isplaying;
}

void PlayerGroup::Pause()
{

}

void PlayerGroup::Stop()
{

}

void PlayerGroup::Seek(int value)
{

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
	return m_audioplayer->notifyInterval();
}

qint64 PlayerGroup::position()
{
	return m_audioplayer->position();
}

qint64 PlayerGroup::duration()
{
	return m_audioplayer->duration();
}

QList<QtAV::VideoOutput *> PlayerGroup::GetVideoOutput()
{
	return m_volist;
}
