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


// Self Includes
#include "syncdatawidget.h"
#include "syncdatawidget.moc"

// Auto Includes
#include "rekonq.h"

// Local Includes
#include "syncmanager.h"
#include "syncassistant.h"


SyncDataWidget::SyncDataWidget(QWidget *parent)
    : QWizardPage(parent)
    , _changed(false)
{
    setupUi(this);

    kcfg_syncEnabled->setChecked(ReKonfig::syncEnabled());

    kcfg_syncBookmarks->setChecked(ReKonfig::syncBookmarks());
    kcfg_syncHistory->setChecked(ReKonfig::syncHistory());
    kcfg_syncPasswords->setChecked(ReKonfig::syncPasswords());

    bool isSyncEnabled = ReKonfig::syncEnabled();
    enablewidgets(isSyncEnabled);

    connect(kcfg_syncEnabled, SIGNAL(clicked()), this, SLOT(hasChanged()));
}


bool SyncDataWidget::changed()
{
    return _changed;
}


void SyncDataWidget::hasChanged()
{
    enablewidgets(kcfg_syncEnabled->isChecked());

    _changed = true;
    emit changed(true);
}


void SyncDataWidget::enablewidgets(bool b)
{
    syncGroupBox->setEnabled(b);
}


int SyncDataWidget::nextId() const
{
    // save
    ReKonfig::setSyncEnabled(kcfg_syncEnabled->isChecked());

    ReKonfig::setSyncBookmarks(kcfg_syncBookmarks->isChecked());
    ReKonfig::setSyncHistory(kcfg_syncHistory->isChecked());
    ReKonfig::setSyncPasswords(kcfg_syncPasswords->isChecked());

    if (ReKonfig::syncEnabled())
        return SyncAssistant::Page_Type;
    else
        return SyncAssistant::Page_Check;
}
