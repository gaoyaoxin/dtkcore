/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qsettingbackend.h"

#include <QDebug>
#include <QMutex>
#include <QSettings>

DCORE_BEGIN_NAMESPACE

class QSettingBackendPrivate
{
public:
    QSettingBackendPrivate(QSettingBackend *parent) : q_ptr(parent) {}

    QSettings       *settings   = nullptr;
    QMutex          writeLock;

    QSettingBackend *q_ptr;
    Q_DECLARE_PUBLIC(QSettingBackend)
};

QSettingBackend::QSettingBackend(const QString &filepath, QObject *parent) :
    DSettingsBackend(parent), d_ptr(new QSettingBackendPrivate(this))
{
    Q_D(QSettingBackend);

    d->settings = new QSettings(filepath, QSettings::NativeFormat, this);
    qDebug() << "create config" <<  d->settings->fileName();
}

QSettingBackend::~QSettingBackend()
{

}

QStringList QSettingBackend::keys() const
{
    Q_D(const QSettingBackend);
    return d->settings->childGroups();
}

QVariant QSettingBackend::getOption(const QString &key) const
{
    Q_D(const QSettingBackend);
    d->settings->beginGroup(key);
    auto value = d->settings->value("value");
    d->settings->endGroup();
    return value;
}

void QSettingBackend::doSetOption(const QString &key, const QVariant &value)
{
    Q_D(QSettingBackend);
    d->writeLock.lock();
    d->settings->beginGroup(key);
    auto oldValue = d->settings->value("value");
    if (oldValue != value) {
        d->settings->setValue("value", value);
        Q_EMIT optionChanged(key, value);
    }
    d->settings->endGroup();
    d->settings->sync();
    d->writeLock.unlock();
}

void QSettingBackend::doSync()
{
    Q_D(QSettingBackend);
    d->settings->sync();
}


DCORE_END_NAMESPACE
