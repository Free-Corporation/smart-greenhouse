#include <QLabel>
#include <QPushButton>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QtEndian>

#include "receiver.h"

Receiver::Receiver(QWidget *parent)
    : QWidget(parent)
{
    statusLabel = new QLabel(tr("Listening for broadcasted messages"));
    statusLabel->setWordWrap(true);

    auto quitButton = new QPushButton(tr("&Quit"));

//! [0]
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(3333, QUdpSocket::ShareAddress);
//! [0]

//! [1]
    connect(udpSocket, &QUdpSocket::readyRead,
            this, &Receiver::processPendingDatagrams);
//! [1]
    connect(quitButton, &QPushButton::clicked,
            this, &Receiver::close);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Broadcast Receiver"));
}

void Receiver::processPendingDatagrams()
{
    QByteArray datagram;
//! [2]
    while (udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size());
        statusLabel->setNum(qFromLittleEndian<qint16>(datagram.data()));

        qDebug() << "Message from: " << qFromLittleEndian<qint16>(datagram.data());

    }
//! [2]
}
