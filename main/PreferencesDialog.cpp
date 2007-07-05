/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Sonic Visualiser
    An audio file viewer and annotation editor.
    Centre for Digital Music, Queen Mary, University of London.
    This file copyright 2006 Chris Cannam.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "PreferencesDialog.h"

#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QString>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "widgets/WindowTypeSelector.h"
#include "base/Preferences.h"

PreferencesDialog::PreferencesDialog(QWidget *parent, Qt::WFlags flags) :
    QDialog(parent, flags)
{
    setWindowTitle(tr("Sonic Visualiser: Application Preferences"));

    Preferences *prefs = Preferences::getInstance();

    QGridLayout *grid = new QGridLayout;
    setLayout(grid);
    
    QGroupBox *groupBox = new QGroupBox;
    groupBox->setTitle(tr("Application Preferences"));
    grid->addWidget(groupBox, 0, 0);
    
    QGridLayout *subgrid = new QGridLayout;
    groupBox->setLayout(subgrid);

    // Create this first, as slots that get called from the ctor will
    // refer to it
    m_applyButton = new QPushButton(tr("Apply"));

    int min, max, deflt, i;

    m_windowType = WindowType(prefs->getPropertyRangeAndValue
                              ("Window Type", &min, &max, &deflt));
    m_windowTypeSelector = new WindowTypeSelector(m_windowType);

    connect(m_windowTypeSelector, SIGNAL(windowTypeChanged(WindowType)),
            this, SLOT(windowTypeChanged(WindowType)));

    QComboBox *smoothing = new QComboBox;
    
    int sm = prefs->getPropertyRangeAndValue("Spectrogram Smoothing", &min, &max,
                                             &deflt);
    m_spectrogramSmoothing = sm;

    for (i = min; i <= max; ++i) {
        smoothing->addItem(prefs->getPropertyValueLabel("Spectrogram Smoothing", i));
    }

    smoothing->setCurrentIndex(sm);

    connect(smoothing, SIGNAL(currentIndexChanged(int)),
            this, SLOT(spectrogramSmoothingChanged(int)));

    QComboBox *propertyLayout = new QComboBox;
    int pl = prefs->getPropertyRangeAndValue("Property Box Layout", &min, &max,
                                         &deflt);
    m_propertyLayout = pl;

    for (i = min; i <= max; ++i) {
        propertyLayout->addItem(prefs->getPropertyValueLabel("Property Box Layout", i));
    }

    propertyLayout->setCurrentIndex(pl);

    connect(propertyLayout, SIGNAL(currentIndexChanged(int)),
            this, SLOT(propertyLayoutChanged(int)));

    m_tuningFrequency = prefs->getTuningFrequency();

    QDoubleSpinBox *frequency = new QDoubleSpinBox;
    frequency->setMinimum(100.0);
    frequency->setMaximum(5000.0);
    frequency->setSuffix(" Hz");
    frequency->setSingleStep(1);
    frequency->setValue(m_tuningFrequency);
    frequency->setDecimals(2);

    connect(frequency, SIGNAL(valueChanged(double)),
            this, SLOT(tuningFrequencyChanged(double)));

    QComboBox *resampleQuality = new QComboBox;

    int rsq = prefs->getPropertyRangeAndValue("Resample Quality", &min, &max,
                                              &deflt);
    m_resampleQuality = rsq;

    for (i = min; i <= max; ++i) {
        resampleQuality->addItem(prefs->getPropertyValueLabel("Resample Quality", i));
    }

    resampleQuality->setCurrentIndex(rsq);

    connect(resampleQuality, SIGNAL(currentIndexChanged(int)),
            this, SLOT(resampleQualityChanged(int)));

    int row = 0;

    subgrid->addWidget(new QLabel(tr("%1:").arg(prefs->getPropertyLabel
                                                ("Property Box Layout"))),
                       row, 0);
    subgrid->addWidget(propertyLayout, row++, 1, 1, 2);

    subgrid->addWidget(new QLabel(tr("%1:").arg(prefs->getPropertyLabel
                                                ("Tuning Frequency"))),
                       row, 0);
    subgrid->addWidget(frequency, row++, 1, 1, 2);

    subgrid->addWidget(new QLabel(tr("%1:").arg(prefs->getPropertyLabel
                                                ("Resample Quality"))),
                       row, 0);
    subgrid->addWidget(resampleQuality, row++, 1, 1, 2);

    subgrid->addWidget(new QLabel(prefs->getPropertyLabel
                                  ("Spectrogram Smoothing")),
                       row, 0);
    subgrid->addWidget(smoothing, row++, 1, 1, 2);

    subgrid->addWidget(new QLabel(tr("%1:").arg(prefs->getPropertyLabel
                                                ("Window Type"))),
                       row, 0);
    subgrid->addWidget(m_windowTypeSelector, row++, 1, 2, 2);
    subgrid->setRowStretch(row, 10);
    row++;
    
    QDialogButtonBox *bb = new QDialogButtonBox(Qt::Horizontal);
    grid->addWidget(bb, 1, 0);
    
    QPushButton *ok = new QPushButton(tr("OK"));
    QPushButton *cancel = new QPushButton(tr("Cancel"));
    bb->addButton(ok, QDialogButtonBox::AcceptRole);
    bb->addButton(m_applyButton, QDialogButtonBox::ApplyRole);
    bb->addButton(cancel, QDialogButtonBox::RejectRole);
    connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    m_applyButton->setEnabled(false);
}

PreferencesDialog::~PreferencesDialog()
{
    std::cerr << "PreferencesDialog::~PreferencesDialog()" << std::endl;
}

void
PreferencesDialog::windowTypeChanged(WindowType type)
{
    m_windowType = type;
    m_applyButton->setEnabled(true);
}

void
PreferencesDialog::spectrogramSmoothingChanged(int smoothing)
{
    m_spectrogramSmoothing = smoothing;
    m_applyButton->setEnabled(true);
}

void
PreferencesDialog::propertyLayoutChanged(int layout)
{
    m_propertyLayout = layout;
    m_applyButton->setEnabled(true);
}

void
PreferencesDialog::tuningFrequencyChanged(double freq)
{
    m_tuningFrequency = freq;
    m_applyButton->setEnabled(true);
}

void
PreferencesDialog::resampleQualityChanged(int q)
{
    m_resampleQuality = q;
    m_applyButton->setEnabled(true);
}

void
PreferencesDialog::okClicked()
{
    applyClicked();
    accept();
}

void
PreferencesDialog::applyClicked()
{
    Preferences *prefs = Preferences::getInstance();
    prefs->setWindowType(WindowType(m_windowType));
    prefs->setSpectrogramSmoothing(Preferences::SpectrogramSmoothing
                                   (m_spectrogramSmoothing));
    prefs->setPropertyBoxLayout(Preferences::PropertyBoxLayout
                                (m_propertyLayout));
    prefs->setTuningFrequency(m_tuningFrequency);
    prefs->setResampleQuality(m_resampleQuality);
    m_applyButton->setEnabled(false);
}    

void
PreferencesDialog::cancelClicked()
{
    reject();
}

void
PreferencesDialog::applicationClosing(bool quickly)
{
    if (quickly) {
        reject();
        return;
    }

    if (m_applyButton->isEnabled()) {
        int rv = QMessageBox::warning
            (this, tr("Preferences Changed"),
             tr("Some preferences have been changed but not applied.\n"
                "Apply them before closing?"),
             QMessageBox::Apply | QMessageBox::Discard,
             QMessageBox::Discard);
        if (rv == QMessageBox::Apply) {
            applyClicked();
            accept();
        } else {
            reject();
        }
    } else {
        accept();
    }
}

