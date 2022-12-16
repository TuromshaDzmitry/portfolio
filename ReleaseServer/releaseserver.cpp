#include "releaseserver.h"

ReleaseServer::ReleaseServer(int nPort, QWidget *parent)
    : QWidget(parent),
      m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort))
    {
        QMessageBox::critical(0, "Ошибка сервера", "Невозможно запустить сервер: " + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }

    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel{"<H1>Server</H1>"});
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("messenger_thesis");

    db.setUserName("root");
    db.setHostName("localhost");
    db.setPassword("newrootpassword");
    if (!db.open()) {
         m_ptxt->append(db.lastError().text());
    }
    else
    {
        m_ptxt->append("Подключение к базе данных осуществленна успешно!");
    }

}

void ReleaseServer::slotNewConnection()
{
    if (m_ptcpServer->hasPendingConnections())
    {
        QString strMessage = "Подключение нового клиента...";
        m_ptxt->append(strMessage);
    }
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnectedClient()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    sendToClient(pClientSocket, QTime::currentTime(), "Ответ сервера: Подключено!");
}

void ReleaseServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_15);
    for(;;)
    {
        if(!m_nNextBlockSize)
        {
            if(pClientSocket->bytesAvailable()<sizeof(quint16))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (pClientSocket->bytesAvailable()<m_nNextBlockSize)
        {
            break;
        }
        QTime time;
        QString str;
        in >> time >> str >> indCommand;

        if(indCommand == 2) {
            readClientIndex2(pClientSocket, time, str);
        }
        else if (indCommand == 0) {
            readClientIndex0(pClientSocket, time, str);
        }
        else if (indCommand == 3){
            disconnectedAuthorization(pClientSocket);
            m_nNextBlockSize = 0;
        }
        else if(indCommand == 4)
        {
            readClientIndex4(pClientSocket, time, str);
            m_nNextBlockSize = 0;
        }
    }
}

void ReleaseServer::sendToClient(QTcpSocket *pSocket, const QTime& timeFunction, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << timeFunction << str << indCommand;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void ReleaseServer::readClientIndex2(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction)
{
    QSqlQuery queryAut = QSqlQuery(db);
    QSqlQuery queryCont = QSqlQuery(db);
    if (!queryAut.exec("SELECT * FROM authorization")){
       qDebug() << queryAut.lastError().text();
    return;
    }

    QString strLoginText;
    QString strPasswordText;
    QString strLoginFunction;
    QString strPasswordFunction;
    QString strLoginOnline;
    int loginIndex = 0;
    int sumIndex = 0;
    QString indexContacts;
    QString allIndexContacts;
    QSqlRecord recAut = queryAut.record();
    QString nameContacts;
    QString allNameContacts;

    for (int i = 0; i != strFunction.size(); i++)
    {
        if(strFunction[i] != ",")
        {
            strLoginFunction += strFunction[i];
        }
        else
        {
            for (int j = i+1; j != strFunction.size(); j++)
            {
                strPasswordFunction += strFunction[j];
            }
            break;
        }
    }

    indCommand = 1;
    while (queryAut.next()) {
        strLoginText = queryAut.value(recAut.indexOf("login")).toString();
        strPasswordText = queryAut.value(recAut.indexOf("password")).toString();
        if (strLoginText == strLoginFunction && strPasswordText == strPasswordFunction)
        {
            strLoginOnline = queryAut.value(recAut.indexOf("online_status")).toInt();
            if (strLoginOnline == 1)
            {
                break;
            }
            loginIndex = queryAut.value(recAut.indexOf("id_aut")).toInt();

            indCommand = 2;
            QString strMessage = timeFunction.toString() + " " + "Авторизация одобрена! Код: "
                 + QString::number(indCommand);
            m_ptxt->append(strMessage);
            m_nNextBlockSize = 0;

            //запрос в таблицу контактов
            if (!queryCont.exec("SELECT name FROM authorization WHERE id_aut IN"
                                " (SELECT id_recipient FROM contacts WHERE id_sender = "
                                + QString::number(loginIndex)+ " )"
                                )){
               m_ptxt->append(queryCont.lastError().text());
            return;
            }
            QSqlRecord recCont = queryCont.record();
            while (queryCont.next())
            {
                nameContacts = queryCont.value(recCont.indexOf("name")).toString();
                allNameContacts += nameContacts + ",";
                sumIndex++;
            }
            allNameContacts = QString::number(sumIndex) + "," + allNameContacts;
            sendToClient(pSocket, QTime::currentTime(), allNameContacts);
            indCommand = 0;
            break;
        }
    }

    if (indCommand == 1){
        QString strMessage = timeFunction.toString() + " " + "В авторизации отказано! Код: "
             + QString::number(indCommand);
        m_ptxt->append(strMessage);
        m_nNextBlockSize = 0;
        sendToClient(pSocket, QTime::currentTime(), "В авторизации отказано!");
        indCommand = 0;
    }
    else
    {
        vecSocket.push_back(pSocket);
        int indexSocket = vecSocket.indexOf(pSocket);
        QSqlQuery queryInexSocket = QSqlQuery(db);
        if (!queryInexSocket.exec("UPDATE authorization SET socket = "
                 + QString::number(indexSocket) + " WHERE login=\"" + strLoginFunction + "\"")){
           m_ptxt->append(queryInexSocket.lastError().text());
        return;
        }
        if (!queryInexSocket.exec("UPDATE authorization SET online_status = \"1\" WHERE login=\""
                                  + strLoginFunction + "\"")){
           m_ptxt->append(queryInexSocket.lastError().text());
        return;
        }
    }
}

void ReleaseServer::readClientIndex0(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction)
{
    QDate dateFunction = QDate::currentDate();
    QString nameContacts;
    QString strContacts;
    for (int i = 0; i != strFunction.size(); i++)
    {
        if(strFunction[i] != ",")
        {
            nameContacts += strFunction[i];
        }
        else
        {
            for (int j = i+1; j != strFunction.size(); j++)
            {
                strContacts += strFunction[j];
            }
            break;
        }
    }

    int indexSocket = vecSocket.indexOf(pSocket);
    QSqlQuery query = QSqlQuery(db);
    if (!query.exec("SELECT * FROM contacts WHERE id_sender IN "
                           "(SELECT id_aut FROM authorization WHERE socket = \"" + QString::number(indexSocket) +
                           "\") && id_recipient IN (SELECT id_aut FROM authorization WHERE name = \""
                           + nameContacts + "\");"
                                 )){
        m_ptxt->append(query.lastError().text());
    return;
    }

    int numberSender = 0;
    QSqlRecord rec = query.record();
    while (query.next())
    {
        numberSender = query.value(rec.indexOf("id_con")).toInt();
    }

    QSqlQuery queryMesBd = QSqlQuery(db);
    if (!queryMesBd.exec("INSERT INTO message (id_contacts, messages, dispatch_time, departure_date) VALUES "
                    "(\"" + QString::number(numberSender) + "\", \"" + strContacts + "\", "
                    "\"" + timeFunction.toString() + "\", \"" + dateFunction.toString("yyyy-MM-dd") + "\");"
                                 )){
       m_ptxt->append(queryMesBd.lastError().text());
    return;
    }

    QSqlQuery queryOnline = QSqlQuery(db);
    if (!queryOnline.exec("SELECT * FROM authorization WHERE name = \"" + nameContacts + "\";")){
       m_ptxt->append(queryOnline.lastError().text());
    return;
    }

    int socketNum = 0;
    int onlineInt = 0;
    QSqlRecord recOnline = queryOnline.record();
    while (queryOnline.next())
    {
        socketNum = queryOnline.value(recOnline.indexOf("socket")).toInt();
        onlineInt = queryOnline.value(recOnline.indexOf("online_status")).toInt();
    }
    m_ptxt->append(nameContacts + " статус = " + QString::number(onlineInt) +
                    ", номер сокета = " + QString::number(socketNum));

    if (onlineInt == 1)
    {
        sendToClient(vecSocket[socketNum], QTime::currentTime(), strContacts);
    }

    m_nNextBlockSize = 0;
}

void ReleaseServer::readClientIndex4(QTcpSocket* pSocket, const QTime& timeFunction, const QString& strFunction)
{
    int indexSocket = vecSocket.indexOf(pSocket);
    QSqlQuery queryContactSocket = QSqlQuery(db);
    if (!queryContactSocket.exec("SELECT * FROM message WHERE id_contacts IN "
                                 "(SELECT id_con FROM contacts WHERE id_sender IN "
                                 "(SELECT id_aut FROM authorization WHERE socket = \"" + QString::number(indexSocket) + "\" || name = \"" + strFunction + "\") && id_recipient IN "
                                 "(SELECT id_aut FROM authorization WHERE socket = \"" + QString::number(indexSocket) + "\" || name = \"" + strFunction + "\")) ORDER BY id_mes;"
                                 )){
       m_ptxt->append(queryContactSocket.lastError().text());
    return;
    }
    QSqlQuery queryContact = QSqlQuery(db);
    if (!queryContact.exec("SELECT * FROM contacts WHERE id_sender IN "
                           "(SELECT id_aut FROM authorization WHERE socket = \"" + QString::number(indexSocket) +
                           "\") && id_recipient IN (SELECT id_aut FROM authorization WHERE name = \""
                           + strFunction + "\");"
                                 )){
       m_ptxt->append(queryContact.lastError().text());
    return;
    }

    int numberSender = 0;
    QSqlRecord recContact = queryContact.record();
    while (queryContact.next())
    {
        numberSender = queryContact.value(recContact.indexOf("id_con")).toInt();
    }

    QString strMessage;
    int numberContact = 0;
    QTime timeMessage;
    QDate dateMessage, dateTemp;
    QSqlRecord recMessage = queryContactSocket.record();
    if (queryContactSocket.size() != 0)
    {
        while (queryContactSocket.next())
        {
            strMessage = queryContactSocket.value(recMessage.indexOf("messages")).toString();
            numberContact = queryContactSocket.value(recMessage.indexOf("id_contacts")).toInt();
            timeMessage = queryContactSocket.value(recMessage.indexOf("dispatch_time")).toTime();
            dateMessage = queryContactSocket.value(recMessage.indexOf("departure_date")).toDate();

            if (dateTemp != dateMessage)
            {
                sendToClient(pSocket, QTime::currentTime(), dateMessage.toString());
                sendToClient(pSocket, timeMessage, "r" + strMessage);
                dateTemp = dateMessage;
            }
            else
            {
                if (numberSender == numberContact)
                {
                    sendToClient(pSocket, timeMessage, "l" + strMessage);
                }
                else
                {
                    sendToClient(pSocket, timeMessage, "r" + strMessage);
                }
            }
        }
    }
    else
    {
        sendToClient(pSocket, QTime::currentTime(), "Нет сообщений!");
    }
    return;
}


void ReleaseServer::disconnectedAuthorization(QTcpSocket* pSocket)
{
    int indexSocket = vecSocket.indexOf(pSocket);
    if (indexSocket == -1)
    {
        m_ptxt->append("Неавторизованный клиент " + QString::number(indexSocket) + " вышел!");
        return;
    }
    m_ptxt->append("Клиент " + QString::number(indexSocket) + " вышел!");
    vecSocket.remove(indexSocket);

    QSqlQuery queryDeletSocket = QSqlQuery(db);
    if (!queryDeletSocket.exec("UPDATE authorization SET online_status = \"0\" WHERE socket=\""
                              + QString::number(indexSocket) + "\"")){
       m_ptxt->append(queryDeletSocket.lastError().text());
    return;
    }
    if (!queryDeletSocket.exec("UPDATE authorization SET socket = -1 WHERE socket=\""
                               + QString::number(indexSocket) + "\"")){
       m_ptxt->append(queryDeletSocket.lastError().text());
    return;
    }
    if (!queryDeletSocket.exec("UPDATE authorization SET socket = socket - 1 WHERE socket=\""
                               + QString::number(indexSocket) + "\" < socket")){
       m_ptxt->append(queryDeletSocket.lastError().text());
    return;
    }
    return;
}

void ReleaseServer::slotDisconnectedClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();

    if( !pClientSocket )
        return;

    disconnectedAuthorization(pClientSocket);
    pClientSocket->deleteLater();
}

ReleaseServer::~ReleaseServer()
{

}

