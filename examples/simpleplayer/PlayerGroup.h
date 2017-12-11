#ifndef PLAYERGROUP_H
#define PLAYERGROUP_H
#include <QStringList>
#include <QList>
#include "QtAV.h"

class PlayerGroup : public QObject
{
	Q_OBJECT
public:
	PlayerGroup(QStringList audiolist, QStringList videolist);
	~PlayerGroup();

	void Play();
	void Pause();
	void Stop();
	void Seek(int value);
	bool IsPlaying();
	int notifyInterval();
	qint64 position();
	qint64 duration();
	QList<QtAV::VideoOutput *> GetVideoOutput();

signals:
	void Signal_UpdateSliderUnit();
	void Signal_PositionChanged(qint64 value);
	void Signal_Started();
private slots:
	void updateSliderUnit();
	void updateSlider(qint64 value);
	void updateSlider();

private:
	QStringList m_audiolist;
	QStringList m_videolist;
	QList<QtAV::VideoOutput *> m_volist;
	QList<QtAV::AVPlayer *> m_playerlist;
	QtAV::AVPlayer *m_audioplayer;
	QtAV::AVClock m_mainclock;
	bool m_isplaying;
};

#endif // PLAYERGROUP_H
