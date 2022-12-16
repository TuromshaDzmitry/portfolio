#ifndef RELEASECLIENT_H
#define RELEASECLIENT_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QApplication>

class QTextEdit;
class QLineEdit;

class ReleaseClient : public QWidget
{
    Q_OBJECT

public:
    ReleaseClient(const QString& strHost, int nPort, QWidget *parent = nullptr);
    ~ReleaseClient();

private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    QTextEdit* l_ptxtLoginInfo;
    QLineEdit* l_ptxtLoginInput;
    QLineEdit* l_ptxtPasswordInput;
    QLineEdit* textIp;
    quint16 m_nNextBlockSize;
    QStackedWidget *stackedWidget;
    QStackedWidget *stackedWidgetContacts;
    int indCommand;
    QVBoxLayout* pvbxLayout4;
    QVector <QPushButton*> vecButtonContact;
    QString nameContact;

private:
    void disconnectedServerAuthorization();
    void SendContactToServer();
    void contactsLayout(const QString& strLayout = "");

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotLoginSendToServer();
    void slotConnected();
    void slotToPage();
    void slotChatPage();
};
#endif // RELEASECLIENT_H
