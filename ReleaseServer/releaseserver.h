#ifndef RELEASESERVER_H
#define RELEASESERVER_H

#include <QWidget>
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QDataStream>
#include <QtSql>

class QTcpServer;
class QTextEdit;
class QTcpSocket;

class ReleaseServer : public QWidget
{
    Q_OBJECT

public:
    ReleaseServer(int nPort, QWidget *parent = nullptr);
    ~ReleaseServer();
    QSqlDatabase db;

private:
    QTcpServer* m_ptcpServer;
    QTextEdit* m_ptxt;
    quint16 m_nNextBlockSize;
    QVector <QTcpSocket*> vecSocket;
    int indCommand = 0;
    //QString numberContact;

private:
    void sendToClient(QTcpSocket* pSocket, const QTime& timeFunction, const QString& str);
    void readClientIndex0(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction);
    void readClientIndex2(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction);
    void readClientIndex4(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction);
    void disconnectedAuthorization(QTcpSocket* pSocket);

public slots:
    virtual void slotNewConnection();
            void slotReadClient();
            void slotDisconnectedClient();
};
#endif // RELEASESERVER_H
