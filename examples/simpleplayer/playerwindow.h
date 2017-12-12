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

#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QtAV>
#include <QGridLayout>
#include "PlayerGroup.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
QT_END_NAMESPACE
class PlayerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = 0);
public Q_SLOTS:
    void openMedia();
    void seekBySlider(int value);
    void seekBySlider();
    void playPause();
private Q_SLOTS:
    void updateSlider(qint64 value);
    void updateSlider();
    void updateSliderUnit();
	void Slot_Audio1();
	void Slot_Audio2();
	void Slot_Audio3();
	void Slot_Audio4();


private:
//    QtAV::VideoOutput *m_vo;
//    QtAV::AVPlayer *m_player;
	QGridLayout *vmain;
	PlayerGroup *m_playergroup;
    QSlider *m_slider;
    QPushButton *m_openBtn;
    QPushButton *m_playBtn;
    QPushButton *m_stopBtn;
	QPushButton *m_audio1;
	QPushButton *m_audio2;
	QPushButton *m_audio3;
	QPushButton *m_audio4;
    int m_unit;
	int m_index;
	qint64 m_position;
};

#endif // PLAYERWINDOW_H
