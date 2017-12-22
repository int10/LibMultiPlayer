#ifndef PLAYERGROUP_H
#define PLAYERGROUP_H
#include <QStringList>
#include <QList>
#include <QTimer>
#include "QtAV.h"

#define MAX_VIDEO_OUT		(6)
class PlayerGroup : public QObject
{
	Q_OBJECT
public:
	PlayerGroup(QStringList audiolist, QStringList videolist);
	~PlayerGroup();

	void Play(int index);
	void Play();
	void PlayPause();
	void Stop();
	void Seek(qint64 value);
	bool IsPlaying();
	int notifyInterval();
	qint64 position();
	qint64 duration();
	void SwitchAudio(int index);
	QList<QtAV::VideoOutput *> GetVideoOutput();

signals:
	void Signal_UpdateSliderUnit();
	void Signal_PositionChanged(qint64 value);
	void Signal_Started();
private slots:
	void updateSliderUnit();
	void updateSlider(qint64 value);
	void updateSlider();
	void timeoutHandle();

private:
	QStringList m_audiolist;
	QStringList m_videolist;
	QList<QtAV::VideoOutput *> m_volist;
	QList<QtAV::AVPlayer *> m_playerlist;
	QtAV::AVPlayer *m_audioplayer;
	QtAV::AVClock m_mainclock;
	QTimer * m_synctimer;
	bool m_isplaying;
	int m_curaudioindex;
};

#endif // PLAYERGROUP_H
