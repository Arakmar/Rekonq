/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2011-2012 by Andrea Diamantini <adjam7 at gmail dot com>
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


#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H


// Rekonq Includes
#include "rekonq_defines.h"

// Local Includes
#include "synchandler.h"

// Qt Includes
#include <QObject>
#include <QWeakPointer>


class REKONQ_TESTS_EXPORT SyncManager : public QObject
{
    Q_OBJECT

public:
    SyncManager(QObject *parent = 0);
    ~SyncManager();

    SyncHandler *handler() const
    {
        return _syncImplementation.data();
    };

public Q_SLOTS:
    void syncBookmarks();
    void syncHistory();
    void syncPasswords();

    void loadSettings();
    void showSettings();

private:
    QWeakPointer<SyncHandler> _syncImplementation;
};

#endif // SYNC_MANAGER_H
