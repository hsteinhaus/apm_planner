/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2011 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Brief Description
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef SERIALLINK_H
#define SERIALLINK_H

#include "SerialLinkInterface.h"
#include "configuration.h"
#include <QPointer>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QtSerialPort/qserialport.h>
#include <configuration.h>
#include <QMetaType>
Q_DECLARE_METATYPE(QSerialPort::SerialPortError)
class UASInterface;
class LinkManager;

/**
 * @brief The SerialLink class provides cross-platform access to serial links.
 * It takes care of the link management and provides a common API to higher
 * level communication layers. It is implemented as a wrapper class for a thread
 * that handles the serial communication. All methods have therefore to be thread-
 * safe.
 *
 */
class SerialLink : public SerialLinkInterface
{
    Q_OBJECT
    //Q_INTERFACES(SerialLinkInterface:LinkInterface)

public:
    SerialLink();
    ~SerialLink();

    void disableTimeouts();
    void enableTimeouts();

    static const int poll_interval = 100; ///< Polling interval, defined in configuration.h

    /** @brief Get a list of the currently available ports */
    QList<QString> getCurrentPorts();

    void requestReset();

    bool isConnected() const;
    qint64 bytesAvailable();

    /**
     * @brief The port handle
     */
    QString getPortName() const;
    /**
     * @brief The human readable port name
     */
    QString getName() const;
    int getBaudRate() const;
    int getDataBits() const;
    int getStopBits() const;

    // ENUM values
    int getBaudRateType() const;
    int getFlowType() const;
    int getParityType() const;
    int getDataBitsType() const;
    int getStopBitsType() const;

    qint64 getConnectionSpeed() const;
    qint64 getCurrentInDataRate() const;
    qint64 getCurrentOutDataRate() const;

    void loadSettings();
    void writeSettings();

    void run();
    void run2();

    int getId() const;

    /** @brief Returns a list of Serial Ports known to the LinkManager */
    static const QList<SerialLink*> getSerialLinks(LinkManager* linkManager);

    /** @brief Returns a list of Serial Ports known to a UAS */
    static const QList<SerialLink*> getSerialLinks(UASInterface* uas);

signals: //[TODO] Refactor to Linkinterface
    void updateLink(LinkInterface*);

public slots:
    bool setPortName(QString portName);
    bool setBaudRate(int rate);
    bool setDataBits(int dataBits);
    bool setStopBits(int stopBits);

    // Set string rate
    bool setBaudRateString(const QString& rate);

    // Set ENUM values
    bool setBaudRateType(int rateIndex);
    bool setFlowType(int flow);
    bool setParityType(int parity);
    bool setDataBitsType(int dataBits);
    bool setStopBitsType(int stopBits);

    void readBytes();
    /**
     * @brief Write a number of bytes to the interface.
     *
     * @param data Pointer to the data byte array
     * @param size The size of the bytes array
     **/
    void writeBytes(const char* data, qint64 length);
    bool connect();
    bool disconnect();
    void portReadyRead();

    void linkError(QSerialPort::SerialPortError error);
    void timeoutTimerTimeout();

protected:


    quint64 m_bytesRead;
    QPointer<QSerialPort> m_port;
    bool m_isConnected;
    int m_baud;
    int m_dataBits;
    int m_flowControl;
    int m_stopBits;
    int m_parity;
    QString m_portName;
    int m_timeout;
    int m_id;
    QMutex m_dataMutex;       // Mutex for reading data from m_port
    QMutex m_writeMutex;      // Mutex for accessing the m_transmitBuffer.
    QList<QString> m_ports;

private:
    bool connectNoThreaded();
    bool connectPartialThreaded();
    bool connectPureThreaded();
    bool disconnectNoThreaded();
    bool disconnectPartialThreaded();
    bool disconnectPureThreaded();
    QString findTypeFromPort(QString portname);
    /**
     * @brief Wait for a port "name" to either exist, or not exist
     *
     * @param name Name of the port to look for
     * @param size timeoutmilliseconds timeout in milliseconds before returning false
     * @param toexist True means it scans for the port to appear, false means it scans for it to disappear.
     **/
    bool waitForPort(QString name,int timeoutmilliseconds,bool toexist);
    bool m_triedDtrReset;
    bool m_triedRebootReset;
    bool m_useEventLoop;

    volatile bool m_stopp;
    volatile bool m_reqReset;
	QMutex m_stoppMutex;
    QByteArray m_transmitBuffer;
    QMap<QString,int> m_portBaudMap;
    QTimer *m_timeoutTimer;
    int m_timeoutCounter;
    int m_timeoutExtendCounter;
    QString m_connectedType;

    bool hardwareConnect(QString type);
    bool m_timeoutsEnabled;

signals:
    void aboutToCloseFlag();

};

#endif // SERIALLINK_H
