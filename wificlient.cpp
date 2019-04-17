/**
 ** This file is part of the WifiClient project.
 ** Copyright 2019 张作深 <zhangzuoshen@hangsheng.com.cn>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "wificlient.h"

#include <QDebug>

#include "station_interface.h"

wifi::helper::Station *station = NULL;

WifiClient::WifiClient(QObject *parent) : QObject(parent)
{
    station = new wifi::helper::Station("wifi.helper.service",
                                        "/Station",
                                        QDBusConnection::systemBus());

    connect(station, SIGNAL(AccessPointAdded(QString)),
            SIGNAL(accessPointAdded(QString)));
    connect(station, SIGNAL(AccessPointRemoved(QString)),
            SIGNAL(accessPointRemoved(QString)));
    connect(station, SIGNAL(AccessPointUpdated(QString)),
            SLOT(onAccessPointUpdated(QString)));
    connect(station, SIGNAL(AccessPointUpdated(QString)),
            SLOT(onAccessPointUpdated(QString)));
    connect(station, SIGNAL(StatusChanged(QString)),
            SLOT(onStatusChanged(QString)));
}

WifiClient *WifiClient::instance()
{
    static WifiClient *client = new WifiClient;
    return client;
}

QVariantMap WifiClient::status() const
{
    QJsonDocument doc = QJsonDocument::fromJson(station->status().toUtf8());
    return doc.toVariant().toMap();
}

QVariantList WifiClient::accessPoints() const
{
    QJsonDocument doc = QJsonDocument::fromJson(station->accessPoints().toUtf8());
    QVariantList list;
    for(QVariant var : doc.array().toVariantList()) {
        QVariantMap map = var.toMap();
        int rssi = map.value("rssi").toInt();
        int level = 0;
        if(rssi < -85) {
            level = 0;
        } else if(rssi < -70) {
            level = 1;
        } else if(rssi < -55) {
            level = 2;
        } else {
            level = 3;
        }
        map.insert("signalLevel", level);
        list << map;
    }
    return list;
}

void WifiClient::addNetwork(const QString &ssid, const QString &password)
{
    station->AddNetwork(ssid, password);
}

void WifiClient::onAccessPointUpdated(const QString &point)
{
    Q_EMIT accessPointUpdated(point);
}

void WifiClient::onStatusChanged(const QString &status)
{
    Q_EMIT statusChanged(status);
}
