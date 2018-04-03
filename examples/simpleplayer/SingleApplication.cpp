#include "SingleApplication.h"
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include <QDebug>

#define TIME_OUT                (500)    // 500ms

SingleApplication::SingleApplication(int &argc, char **argv): QApplication(argc, argv)
{

	// 取应用程序名作为LocalServer的名字
	w = NULL;
	m_isRunning = false;
	m_localServer = NULL;
	m_serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();

	InitLocalConnection();
}


bool SingleApplication::IsRunning() 
{
	return m_isRunning;
}



void SingleApplication::NewLocalConnection() 
{
	QLocalSocket *socket = m_localServer->nextPendingConnection();
	if(socket) {
		socket->waitForReadyRead(2*TIME_OUT);
		delete socket;
		// 其他处理，如：读取启动参数

		ActivateWindow();
	}
}

void SingleApplication::InitLocalConnection() 
{
	m_isRunning = false;
	QLocalSocket socket;
	socket.connectToServer(m_serverName);
	if(socket.waitForConnected(TIME_OUT)) {
		qDebug()<<m_serverName<<" already running!";
		m_isRunning = true;
		// 其他处理，如：将启动参数发送到服务端
		return;
	}
	//连接不上服务器，就创建一个
	NewLocalServer();
}

void SingleApplication::NewLocalServer() 
{
	m_localServer = new QLocalServer(this);
	connect(m_localServer, SIGNAL(newConnection()), this, SLOT(NewLocalConnection()));
	if(!m_localServer->listen(m_serverName)) {
		// 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
		if(m_localServer->serverError() == QAbstractSocket::AddressInUseError) {
			QLocalServer::removeServer(m_serverName);
			m_localServer->listen(m_serverName); // 再次监听
		}
	}
}

void SingleApplication::ActivateWindow() 
{
	if(w) 
	{
		w->showNormal();
		w->raise();
		w->activateWindow(); // 激活窗口
	}
}
