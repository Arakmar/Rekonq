/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
*
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* ============================================================ */



#ifndef FINDBAR_H
#define FINDBAR_H

// KDE Includes
#include <KLineEdit>
#include <KToolBar>
#include <KXmlGuiWindow>

// Qt Includes
#include <QtCore>
#include <QtGui>


class FindBar : public QWidget
{
    Q_OBJECT

public:
    FindBar(KXmlGuiWindow *mainwindow);
    ~FindBar();
    KLineEdit *lineEdit() const;
    bool matchCase() const;

public slots:
    void clear();
    void showFindBar();

protected Q_SLOTS:
    void keyPressEvent(QKeyEvent* event);

signals:
    void searchString(const QString &);

private:
    KLineEdit *m_lineEdit;
    QCheckBox *m_matchCase;
};


#endif
