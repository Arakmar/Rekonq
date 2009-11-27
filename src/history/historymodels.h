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


#ifndef HISTORYMODELS_H
#define HISTORYMODELS_H


// Local Includes
#include "historymanager.h"

// KDE Includes
#include <KUrl>

// Qt Includes
#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QWebHistoryInterface>


class HistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public slots:
    void historyReset();
    void entryAdded();
    void entryUpdated(int offset);

public:
    enum Roles
    {
        DateRole = Qt::UserRole + 1,
        DateTimeRole = Qt::UserRole + 2,
        UrlRole = Qt::UserRole + 3,
        UrlStringRole = Qt::UserRole + 4
    };

    explicit HistoryModel(HistoryManager *history, QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:
    HistoryManager *m_history;
};


// ----------------------------------------------------------------------------------------------------

/**
 * Proxy model that will remove any duplicate entries.
 * Both m_sourceRow and m_historyHash store their offsets not from
 * the front of the list, but as offsets from the back.
 *
 */

class HistoryFilterModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit HistoryFilterModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

    inline bool historyContains(const QString &url) const
    {
        load(); return m_historyHash.contains(url);
    }
    int historyLocation(const QString &url) const;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    void setSourceModel(QAbstractItemModel *sourceModel);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int, int, const QModelIndex& = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void sourceReset();
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &, int, int);

private:
    void load() const;

    mutable QList<int> m_sourceRow;
    mutable QHash<QString, int> m_historyHash;
    mutable bool m_loaded;
};


// ----------------------------------------------------------------------------------------------------------------------

/**
 * The history menu
 * - Removes the first twenty entries and puts them as children of the top level.
 * - If there are less then twenty entries then the first folder is also removed.
 *
 * The mapping is done by knowing that HistoryTreeModel is over a table
 * We store that row offset in our index's private data.
 *
 */

class HistoryMenuModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit HistoryMenuModel(HistoryTreeModel *sourceModel, QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const;
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

    int bumpedRows() const;

private:
    HistoryTreeModel *m_treeModel;
};


// ----------------------------------------------------------------------------------------

/**
 * Proxy model for the history model that
 * exposes each url http://www.foo.com and
 * it url starting at the host www.foo.com
 *
 */

class HistoryCompletionModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    HistoryCompletionModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int, int, const QModelIndex& = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
    void setSourceModel(QAbstractItemModel *sourceModel);

private slots:
    void sourceReset();

};


// ---------------------------------------------------------------------------------------

/**
 * Proxy model for the history model that converts the list
 * into a tree, one top level node per day.
 * Used in the HistoryDialog.
 *
 */

class HistoryTreeModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    explicit HistoryTreeModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setSourceModel(QAbstractItemModel *sourceModel);

private slots:
    void sourceReset();
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &parent, int start, int end);

private:
    int sourceDateRow(int row) const;
    mutable QList<int> m_sourceRowCache;

};


// -----------------------------------------------------------------------------------------------------------------

/**
 * A modified QSortFilterProxyModel that always accepts
 * the root nodes in the tree
 * so filtering is only done on the children.
 *
 * Used in the HistoryDialog.
 *
 */

class TreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TreeProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};


#endif // HISTORYMODELS_H