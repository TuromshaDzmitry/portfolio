#include "releaseclient.h"

ReleaseClient::ReleaseClient(const QString& strHost, int nPort, QWidget *parent)
    : QWidget(parent)
    , m_nNextBlockSize(0)
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    m_pTcpSocket = new QTcpSocket(this);
/*
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
*/
    m_ptxtInfo = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    l_ptxtLoginInfo = new QTextEdit;
    l_ptxtLoginInput = new QLineEdit;
    l_ptxtPasswordInput = new QLineEdit;
    //IP
    textIp = new QLineEdit;
    indCommand = 0;

    m_ptxtInfo->setReadOnly(true);

    //интерфейс
    QPushButton* loginSendBut = new QPushButton("&Войти");
    QPushButton* loginPageBut1 = new QPushButton("&Вернуться на страницу авторизации");
    QPushButton* loginPageBut2 = new QPushButton("&Попробовать снова");
    QPushButton* contactPageBut = new QPushButton("&Вернуться в контакты");
    QPushButton* messageBut = new QPushButton("&Написать");
    connect(loginSendBut, SIGNAL(clicked()), SLOT(slotLoginSendToServer()));
    connect(loginPageBut1, SIGNAL(clicked()), SLOT(slotToPage()));
    connect(loginPageBut2, SIGNAL(clicked()), SLOT(slotToPage()));
    connect(contactPageBut, SIGNAL(clicked()), SLOT(slotToPage()));
    connect(messageBut, SIGNAL(clicked()), SLOT(slotSendToServer()));

    QWidget *loginPageWidget = new QWidget;
    QWidget *secondPageWidget = new QWidget;
    QWidget *thirdPageWidget = new QWidget;
    QWidget *fourPageWidget = new QWidget;

    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(loginPageWidget);
    stackedWidget->addWidget(secondPageWidget);
    stackedWidget->addWidget(thirdPageWidget);
    stackedWidget->addWidget(fourPageWidget);

    stackedWidgetContacts = new QStackedWidget;
    stackedWidgetContacts->addWidget(new QPushButton("&Нет контактов"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);
    setLayout(layout);

    stackedWidget->setCurrentIndex(0);

    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Авторизация</H1>"));
    pvbxLayout->addWidget(l_ptxtLoginInfo);
    //IP
    pvbxLayout->addWidget(new QLabel("IP"));
    pvbxLayout->addWidget(textIp);

    pvbxLayout->addWidget(new QLabel("Логин"));
    pvbxLayout->addWidget(l_ptxtLoginInput);
    pvbxLayout->addWidget(new QLabel("Пароль"));
    pvbxLayout->addWidget(l_ptxtPasswordInput);
    pvbxLayout->addWidget(loginSendBut);
    loginPageWidget->setLayout(pvbxLayout);

    QVBoxLayout* pvbxLayout2 = new QVBoxLayout;
    pvbxLayout2->addWidget(new QLabel("<H1>Чат</H1>"));
    pvbxLayout2->addWidget(m_ptxtInfo);
    pvbxLayout2->addWidget(m_ptxtInput);
    pvbxLayout2->addWidget(messageBut);
    pvbxLayout2->addWidget(contactPageBut);
    fourPageWidget->setLayout(pvbxLayout2);

    QVBoxLayout* pvbxLayout3 = new QVBoxLayout;
    pvbxLayout3->addWidget(new QLabel("<H1>Ошибка авторизации</H1>"));
    pvbxLayout3->addWidget(loginPageBut2);
    thirdPageWidget->setLayout(pvbxLayout3);

    pvbxLayout4 = new QVBoxLayout;
    pvbxLayout4->addWidget(new QLabel("<H1>Контакты</H1>"));
    contactsLayout();
    pvbxLayout4->addWidget(loginPageBut1);
    secondPageWidget->setLayout(pvbxLayout4);
}

void ReleaseClient::slotConnected()
{
    l_ptxtLoginInfo->append("Подключено к серверу!");
    l_ptxtLoginInfo->setAlignment(Qt::AlignLeft);
}

void ReleaseClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                    "Хост не найден." :
                                    err == QAbstractSocket::RemoteHostClosedError ?
                                    "Удаленный хост закрыт." :
                                    err == QAbstractSocket::ConnectionRefusedError ?
                                    "В соединении было отказано." :
                                    QString(m_pTcpSocket->errorString()));
    l_ptxtLoginInfo->append(strError + "\nПерезапустите приложение!");
    l_ptxtLoginInfo->setAlignment(Qt::AlignLeft);
}

void ReleaseClient::slotReadyRead() // получает данные от сервера
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_15);
    for(;;)
    {
        if (!m_nNextBlockSize)
        {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }

        QTime time;
        QString str;
        in >> time >> str >> indCommand;

        if (indCommand == 0) //сообщение
        {
            m_ptxtInfo->append(str + " " + time.toString());
            m_ptxtInfo->setAlignment(Qt::AlignRight);
        }
        else if (indCommand == 1) //ошибка авторизации
        {
            stackedWidget->setCurrentIndex(2);
        }
        else if (indCommand == 2)//авторизация прошла успешна
        {
            stackedWidget->setCurrentIndex(1);
            contactsLayout(str);
        }
        else if (indCommand == 4)
        {

            if (str[0] == "l")
            {
                m_ptxtInfo->append(time.toString() + " " + str.remove(0, 1));
                m_ptxtInfo->setAlignment(Qt::AlignLeft);
            }
            else if (str[0] == "r")
            {

                m_ptxtInfo->append(str.remove(0, 1) + " " + time.toString());
                m_ptxtInfo->setAlignment(Qt::AlignRight);
            }
            else
            {
                m_ptxtInfo->append(str);
                m_ptxtInfo->setAlignment(Qt::AlignCenter);
            }
            indCommand = 0;
        }

        m_nNextBlockSize = 0;
    }
}

void ReleaseClient::slotSendToServer()
{
    indCommand = 0;
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    if (m_ptxtInput->text() != "")
    {
        QString strMes = nameContact + "," + m_ptxtInput->text();
        out << quint16(0) << QTime::currentTime() << strMes << indCommand;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof (quint16));

        m_pTcpSocket->write(arrBlock);
        m_ptxtInfo->append(QTime::currentTime().toString() + " " + m_ptxtInput->text());
        m_ptxtInfo->setAlignment(Qt::AlignLeft);
        m_ptxtInput->setText("");
    }
}

void ReleaseClient::slotLoginSendToServer()
{
    //для комиссии
    m_pTcpSocket->connectToHost(textIp->text(), 2323);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    indCommand = 2;
    QString strLogPas;
    strLogPas = l_ptxtLoginInput->text() + "," + l_ptxtPasswordInput->text();
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << QTime::currentTime() << strLogPas << indCommand;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof (quint16));

    m_pTcpSocket->write(arrBlock);
    l_ptxtLoginInput->setText("");
    l_ptxtPasswordInput->setText("");
}

void ReleaseClient::contactsLayout(const QString& strLayout)
{
    QString strKolContact;
    QString strContact;
    int g = 0;

    for (int i = 0; i != vecButtonContact.size(); i++) // очистка вектора
    {
        vecButtonContact[i]->hide();
    }
    vecButtonContact.clear();

    if (strLayout == "")
    {
        vecButtonContact.push_back(new QPushButton("&Нет контактов"));
        pvbxLayout4->addWidget(vecButtonContact[0]);
    }
    else
    {
        for (int i = 0; i != strLayout.size(); i++)
        {
            if (strLayout[i] != ",")
            {
                strKolContact += strLayout[i];
            }
            else
            {
                for (int j = i+1; j != strLayout.size(); j++)
                {
                    if(strLayout[j] != ",")
                    {
                        strContact += strLayout[j];
                    }
                    else
                    {
                        vecButtonContact.push_back(new QPushButton("&" + strContact));
                        pvbxLayout4->addWidget(vecButtonContact[g]);
                        connect(vecButtonContact[g], SIGNAL(clicked()), SLOT(slotChatPage()));
                        strContact = "";
                        g++;
                    }
                }
                break;
            }
        }
    }
    g = 0;
}

void ReleaseClient::slotChatPage()
{
    stackedWidget->setCurrentIndex(3);
    QPushButton* pButtonContact = (QPushButton*)sender();
    QString tempStr = pButtonContact->text();
    nameContact = tempStr.remove(0, 1);
    SendContactToServer();
    m_ptxtInfo->clear();
}

void ReleaseClient::SendContactToServer()
{
    indCommand = 4;
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << QTime::currentTime() << nameContact << indCommand;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof (quint16));

    m_pTcpSocket->write(arrBlock);
}

void ReleaseClient::slotToPage()
{
    QPushButton* pButtonContact = (QPushButton*)sender();
    QString tempStr = pButtonContact->text();
    if (tempStr == "&Вернуться в контакты")
    {
        stackedWidget->setCurrentIndex(1);
        nameContact = "";
        m_ptxtInfo->clear();
    }
    else if (tempStr == "&Вернуться на страницу авторизации")
    {
        stackedWidget->setCurrentIndex(0);
        disconnectedServerAuthorization();
    }
    else
    {
        stackedWidget->setCurrentIndex(0);
    }
}

void ReleaseClient::disconnectedServerAuthorization()
{
    indCommand = 3;
    QByteArray arrBlock;
    QString strDisconnected = "Выход из профиля!";
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << QTime::currentTime() << strDisconnected << indCommand;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof (quint16));

    m_pTcpSocket->write(arrBlock);
}

ReleaseClient::~ReleaseClient()
{
}

