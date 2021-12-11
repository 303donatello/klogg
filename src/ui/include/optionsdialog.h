/*
 * Copyright (C) 2009, 2010, 2013 Nicolas Bonnefon and other contributors
 *
 * This file is part of glogg.
 *
 * glogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glogg.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Copyright (C) 2016 -- 2019 Anton Filimonov and other contributors
 *
 * This file is part of klogg.
 *
 * klogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * klogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klogg.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QHBoxLayout>
#include <QColor>

#include "configuration.h"

#include "ui_optionsdialog.h"

class KeySequencePresenter : public QWidget {
    Q_OBJECT
  public:
    explicit KeySequencePresenter( const QString& keySequence );

  QString keySequence() const;

  private slots:
    void showEditor();

  private:
    QLabel* keySequenceLabel_;
};

// Implements the main option dialog box
class OptionsDialog : public QDialog, public Ui::OptionsDialog {
    Q_OBJECT

  public:
    explicit OptionsDialog( QWidget* parent = nullptr );

  signals:
    // Is emitted when new settings must be used
    void optionsChanged();

  private slots:
    // Clears and updates the font size box with the sizes allowed
    // by the passed font family.
    void updateFontSize( const QString& fontFamily );
    // Update the content of the global Config() using parameters
    // from the dialog box.
    void updateConfigFromDialog();
    // Called when a ok/cancel/apply button is clicked.
    void onButtonBoxClicked( QAbstractButton* button );

    void changeMainColor();
    void changeQfColor();

  private:
    void setupTabs();
    void setupFontList();
    void setupRegexp();
    void setupPolling();
    void setupSearchResultsCache();
    void setupLogging();
    void setupArchives();
    void setupStyles();
    void setupEncodings();

    void buildShortcutsTable();

    int getRegexpTypeIndex( SearchRegexpType syntax ) const;
    SearchRegexpType getRegexpTypeFromIndex( int index ) const;

    int getRegexpEngineIndex( RegexpEngine engine ) const;
    RegexpEngine getRegexpEngineFromIndex( int index ) const;

    void updateDialogFromConfig();

    QColor mainSearchColor_;
    QColor qfSearchColor_;
};

#endif
