#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H


#include <QObject>
#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QWidget>

class SingleApplication : public QApplication {
	Q_OBJECT
public:
		SingleApplication(int &argc, char **argv);

		bool IsRunning();                // 是否已經有实例在运行 true:有实例在运行 false:没实例在运行
		QWidget *w;                        // MainWindow指针

	private slots:
		void NewLocalConnection();			// 有新连接时触发
	private:
		void InitLocalConnection();			// 初始化本地连接
		void NewLocalServer();				// 创建服务端
		void ActivateWindow();				// 激活窗口

		bool m_isRunning;                // 是否已經有实例在运行
		QLocalServer *m_localServer;     // 本地socket Server
		QString m_serverName;            // 服务名称
};

#endif // SINGLEAPPLICATION_H
