/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2010 by Yoann Laissus <yoann dot laissus at gmail dot com>
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


#ifndef PANELTREEVIEW_H
#define PANELTREEVIEW_H

// Local Includes
#include "application.h"

// Qt Includes
#include <QTreeView>


class PanelTreeView : public QTreeView
{
    Q_OBJECT

public:
    PanelTreeView(QWidget *parent = 0);
    ~PanelTreeView();

signals:
    void openUrl(const KUrl &, const Rekonq::OpenType &);
    void itemHovered(const QString &);
    void delKeyPressed();
    void contextMenuItemRequested(const QPoint &pos);
    void contextMenuGroupRequested(const QPoint &pos);
    void contextMenuEmptyRequested(const QPoint &pos);

public slots:
    void copyToClipboard();
    void openInCurrentTab();
    void openInNewTab();
    void openInNewWindow();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void validOpenUrl(const KUrl &url, Rekonq::OpenType openType = Rekonq::CurrentTab);
};

#endif // PANELTREEVIEW_H