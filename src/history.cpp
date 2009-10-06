/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2007-2008 Trolltech ASA. All rights reserved
* Copyright (C) 2008 Benjamin C. Meyer <ben@meyerhome.net>
* Copyright (C) 2008-2009 by Andrea Diamantini <adjam7 at gmail dot com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy 
* defined in Section 14 of version 3 of the license.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */


// Self Includes
#include "history.h"
#include "history.moc"

// Auto Includes
#include "rekonq.h"

// Local Includes
#include "historymodels.h"
#include "autosaver.h"
#include "application.h"

// KDE Includes
#include <KDebug>
#include <KStandardDirs>
#include <KLocale>
#include <KCompletion>

// Qt Includes
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QBuffer>

#include <QtGui/QClipboard>

// generic algorithms
#include <QtAlgorithms>


static const unsigned int HISTORY_VERSION = 23;


HistoryManager::HistoryManager(QObject *parent)
    : QWebHistoryInterface(parent)
    , m_saveTimer(new AutoSaver(this))
    , m_historyLimit(30)
    , m_historyModel(0)
    , m_historyFilterModel(0)
    , m_historyTreeModel(0)
    , m_completion(0)
{
    // take care of the completion object
    m_completion = new KCompletion;
    m_completion->setOrder( KCompletion::Weighted );
    
    m_expiredTimer.setSingleShot(true);
    connect(&m_expiredTimer, SIGNAL(timeout()), this, SLOT(checkForExpired()));
    connect(this, SIGNAL(entryAdded(const HistoryItem &)), m_saveTimer, SLOT(changeOccurred()));
    connect(this, SIGNAL(entryRemoved(const HistoryItem &)), m_saveTimer, SLOT(changeOccurred()));

    load();

    m_historyModel = new HistoryModel(this, this);
    m_historyFilterModel = new HistoryFilterModel(m_historyModel, this);
    m_historyTreeModel = new HistoryTreeModel(m_historyFilterModel, this);

    // QWebHistoryInterface will delete the history manager
    QWebHistoryInterface::setDefaultInterface(this);
}


HistoryManager::~HistoryManager()
{
    m_saveTimer->saveIfNeccessary();
}


QList<HistoryItem> HistoryManager::history() const
{
    return m_history;
}


bool HistoryManager::historyContains(const QString &url) const
{
    return m_historyFilterModel->historyContains(url);
}


void HistoryManager::addHistoryEntry(const QString &url)
{
    QUrl cleanUrl(url);
    
    // don't store rekonq: urls (home page related)
    if(cleanUrl.scheme() == QString("rekonq"))
        return;
    
    cleanUrl.setPassword(QString());
    cleanUrl.setHost(cleanUrl.host().toLower());
    HistoryItem item(cleanUrl.toString(), QDateTime::currentDateTime());
    addHistoryEntry(item);

    // Add item to completion object
    QString _url(url);
    _url.remove(QRegExp("^http://|/$"));
    m_completion->addItem(_url);
}


void HistoryManager::setHistory(const QList<HistoryItem> &history, bool loadedAndSorted)
{
    m_history = history;

    // verify that it is sorted by date
    if (!loadedAndSorted)
        qSort(m_history.begin(), m_history.end());

    checkForExpired();

    if (loadedAndSorted)
    {
        m_lastSavedUrl = m_history.value(0).url;
    }
    else
    {
        m_lastSavedUrl.clear();
        m_saveTimer->changeOccurred();
    }
    emit historyReset();
}


HistoryModel *HistoryManager::historyModel() const
{
    return m_historyModel;
}


HistoryFilterModel *HistoryManager::historyFilterModel() const
{
    return m_historyFilterModel;
}


HistoryTreeModel *HistoryManager::historyTreeModel() const
{
    return m_historyTreeModel;
}


void HistoryManager::checkForExpired()
{
    if (m_historyLimit < 0 || m_history.isEmpty())
        return;

    QDateTime now = QDateTime::currentDateTime();
    int nextTimeout = 0;

    while (!m_history.isEmpty())
    {
        QDateTime checkForExpired = m_history.last().dateTime;
        checkForExpired.setDate(checkForExpired.date().addDays(m_historyLimit));
        if (now.daysTo(checkForExpired) > 7)
        {
            // check at most in a week to prevent int overflows on the timer
            nextTimeout = 7 * 86400;
        }
        else
        {
            nextTimeout = now.secsTo(checkForExpired);
        }
        if (nextTimeout > 0)
            break;
        HistoryItem item = m_history.takeLast();
        // remove from saved file also
        m_lastSavedUrl.clear();
        emit entryRemoved(item);
    }

    if (nextTimeout > 0)
        m_expiredTimer.start(nextTimeout * 1000);
}


void HistoryManager::addHistoryEntry(const HistoryItem &item)
{
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;

    m_history.prepend(item);
    emit entryAdded(item);
    
    if (m_history.count() == 1)
        checkForExpired();
}


void HistoryManager::updateHistoryEntry(const KUrl &url, const QString &title)
{
    for (int i = 0; i < m_history.count(); ++i)
    {
        if (url == m_history.at(i).url)
        {
            m_history[i].title = title;
            m_saveTimer->changeOccurred();
            if (m_lastSavedUrl.isEmpty())
                m_lastSavedUrl = m_history.at(i).url;
            emit entryUpdated(i);
            break;
        }
    }
}


void HistoryManager::removeHistoryEntry(const HistoryItem &item)
{
    m_lastSavedUrl.clear();
    m_history.removeOne(item);
    emit entryRemoved(item);
}


void HistoryManager::removeHistoryEntry(const KUrl &url, const QString &title)
{
    for (int i = 0; i < m_history.count(); ++i)
    {
        if (url == m_history.at(i).url
                && (title.isEmpty() || title == m_history.at(i).title))
        {
            removeHistoryEntry(m_history.at(i));
            break;
        }
    }
    
    // Remove item from completion object
    QString _url = url.path();
    _url.remove(QRegExp("^http://|/$"));
    m_completion->removeItem(_url);
}


int HistoryManager::historyLimit() const
{
    return m_historyLimit;
}


void HistoryManager::setHistoryLimit(int limit)
{
    if (m_historyLimit == limit)
        return;
    m_historyLimit = limit;
    checkForExpired();
    m_saveTimer->changeOccurred();
}


void HistoryManager::clear()
{
    m_history.clear();
    m_lastSavedUrl.clear();
    m_saveTimer->changeOccurred();
    m_saveTimer->saveIfNeccessary();
    historyReset();
}


void HistoryManager::loadSettings()
{
    int historyExpire = ReKonfig::expireHistory();
    int days;
    switch (historyExpire)
    {
    case 0: days = 1; break;
    case 1: days = 7; break;
    case 2: days = 14; break;
    case 3: days = 30; break;
    case 4: days = 365; break;
    case 5: days = -1; break;
    default: days = -1;
    }
    m_historyLimit = days;
}


void HistoryManager::load()
{
    loadSettings();

    QString historyFilePath = KStandardDirs::locateLocal("appdata" , "history");
    QFile historyFile(historyFilePath);
    if (!historyFile.exists())
        return;
    if (!historyFile.open(QFile::ReadOnly))
    {
        kWarning() << "Unable to open history file" << historyFile.fileName();
        return;
    }

    QList<HistoryItem> list;
    QDataStream in(&historyFile);
    // Double check that the history file is sorted as it is read in
    bool needToSort = false;
    HistoryItem lastInsertedItem;
    QByteArray data;
    QDataStream stream;
    QBuffer buffer;
    stream.setDevice(&buffer);
    while (!historyFile.atEnd())
    {
        in >> data;
        buffer.close();
        buffer.setBuffer(&data);
        buffer.open(QIODevice::ReadOnly);
        quint32 ver;
        stream >> ver;
        if (ver != HISTORY_VERSION)
            continue;
        HistoryItem item;
        stream >> item.url;
        stream >> item.dateTime;
        stream >> item.title;

        if (!item.dateTime.isValid())
            continue;

        if (item == lastInsertedItem)
        {
            if (lastInsertedItem.title.isEmpty() && !list.isEmpty())
                list[0].title = item.title;
            continue;
        }

        if (!needToSort && !list.isEmpty() && lastInsertedItem < item)
            needToSort = true;

        list.prepend(item);
        lastInsertedItem = item;

        // Add item to completion object
        QString _url = item.url;
        _url.remove(QRegExp("^http://|/$"));
        m_completion->addItem(_url);
    }
    if (needToSort)
        qSort(list.begin(), list.end());

    setHistory(list, true);

    // If we had to sort re-write the whole history sorted
    if (needToSort)
    {
        m_lastSavedUrl.clear();
        m_saveTimer->changeOccurred();
    }
}


void HistoryManager::save()
{
    bool saveAll = m_lastSavedUrl.isEmpty();
    int first = m_history.count() - 1;
    if (!saveAll)
    {
        // find the first one to save
        for (int i = 0; i < m_history.count(); ++i)
        {
            if (m_history.at(i).url == m_lastSavedUrl)
            {
                first = i - 1;
                break;
            }
        }
    }
    if (first == m_history.count() - 1)
        saveAll = true;

    QString historyFilePath = KStandardDirs::locateLocal("appdata" , "history");
    QFile historyFile(historyFilePath);

    // When saving everything use a temporary file to prevent possible data loss.
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    bool open = false;
    if (saveAll)
    {
        open = tempFile.open();
    }
    else
    {
        open = historyFile.open(QFile::Append);
    }

    if (!open)
    {
        kWarning() << "Unable to open history file for saving"
        << (saveAll ? tempFile.fileName() : historyFile.fileName());
        return;
    }

    QDataStream out(saveAll ? &tempFile : &historyFile);
    for (int i = first; i >= 0; --i)
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        HistoryItem item = m_history.at(i);
        stream << HISTORY_VERSION << item.url << item.dateTime << item.title;
        out << data;
    }
    tempFile.close();

    if (saveAll)
    {
        if (historyFile.exists() && !historyFile.remove())
        {
            kWarning() << "History: error removing old history." << historyFile.errorString();
        }
        if (!tempFile.rename(historyFile.fileName()))
        {
            kWarning() << "History: error moving new history over old." << tempFile.errorString() << historyFile.fileName();
        }
    }
    m_lastSavedUrl = m_history.value(0).url;
}


KCompletion * HistoryManager::completionObject() const
{
    return m_completion;
}
