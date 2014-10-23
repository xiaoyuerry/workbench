
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __VOLUME_FILE_CREATE_DIALOG_DECLARE__
#include "VolumeFileCreateDialog.h"
#undef __VOLUME_FILE_CREATE_DIALOG_DECLARE__

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "GuiManager.h"
#include "VolumeFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::VolumeFileCreateDialog 
 * \brief Dialog for creating volume file or adding map to volume file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
VolumeFileCreateDialog::VolumeFileCreateDialog(QWidget* parent)
: WuQDialogModal("Create Volume",
                 parent)
{
    m_volumeFile = NULL;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(createNewVolumeFileWidget());
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
VolumeFileCreateDialog::~VolumeFileCreateDialog()
{
}

/**
 * @return Create and return the add map to volume file widget
 */
QWidget*
VolumeFileCreateDialog::addMapToVolumeFileWidget()
{
    QWidget* widget = new QWidget();
    return widget;
}

/**
 * @return Create and return the create new volume file widget
 */
QWidget*
VolumeFileCreateDialog::createNewVolumeFileWidget()
{
    QLabel* newFileNameLabel = new QLabel("Name:");
    m_newFileNameLineEdit = new QLineEdit();
    QPushButton* newFileNamePushButton = new QPushButton("Select...");
    QObject::connect(newFileNamePushButton, SIGNAL(clicked()),
                     this, SLOT(newFileNamePushButtonClicked()));
    
    QLabel* newFileTypeLabel = new QLabel("Type:");
    m_newFileTypeComboBox = new QComboBox();
    m_newFileTypeComboBox->addItem("Functional (Scalar)",
                                   SubvolumeAttributes::FUNCTIONAL);
    m_newFileTypeComboBox->addItem("Label",
                                   SubvolumeAttributes::LABEL);
    
    QLabel* newFileNumberOfMapsLabel = new QLabel("Maps:");
    m_newFileNumberOfMapsSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 1000, 1);
    m_newFileNumberOfMapsSpinBox->setFixedWidth(50);
    
    QGridLayout* nameTypeLayout = new QGridLayout();
    nameTypeLayout->setColumnStretch(0, 0);
    nameTypeLayout->setColumnStretch(1, 0);
    nameTypeLayout->setColumnStretch(2, 100);
    nameTypeLayout->setColumnStretch(3, 0);
    nameTypeLayout->addWidget(newFileNameLabel,      0, 0);
    nameTypeLayout->addWidget(m_newFileNameLineEdit, 0, 1, 1, 2);
    nameTypeLayout->addWidget(newFileNamePushButton, 0, 4);
    nameTypeLayout->addWidget(newFileTypeLabel,      1, 0);
    nameTypeLayout->addWidget(m_newFileTypeComboBox, 1, 1);
    nameTypeLayout->addWidget(newFileNumberOfMapsLabel,     2, 0);
    nameTypeLayout->addWidget(m_newFileNumberOfMapsSpinBox, 2, 1, Qt::AlignLeft);
    
    const int SPIN_BOX_WIDTH = 80;
    
    QLabel* dimensionsLabel = new QLabel("Dimensions:");
    m_newDimXSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_newDimYSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_newDimZSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(1, 100000, 1);
    m_newDimXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newDimYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newDimZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    
    const double bigDouble = 100000000.0;
    
    QLabel* originLabel = new QLabel("Origin:");
    m_newOriginXSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newOriginYSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newOriginZSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newOriginXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newOriginYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newOriginZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    
    QLabel* spacingLabel = new QLabel("Spacing:");
    m_newSpacingXSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newSpacingYSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newSpacingZSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigDouble, bigDouble, 1.0, 1);
    m_newSpacingXSpinBox->setValue(1.0);
    m_newSpacingYSpinBox->setValue(1.0);
    m_newSpacingZSpinBox->setValue(1.0);
    m_newSpacingXSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newSpacingYSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    m_newSpacingZSpinBox->setFixedWidth(SPIN_BOX_WIDTH);
    
    QLabel* xLabel = new QLabel("X");
    QLabel* yLabel = new QLabel("Y");
    QLabel* zLabel = new QLabel("Z");
    
    QLabel* loadFromLabel = new QLabel("Load From");
    m_paramFromFilePushButton = new QPushButton("File...");
    QObject::connect(m_paramFromFilePushButton, SIGNAL(clicked()),
                     this, SLOT(loadVolumeParametersFromFilePushButtonClicked()));
    
    const int COL_LABEL = 0;
    const int COL_X = COL_LABEL + 1;
    const int COL_Y = COL_X + 1;
    const int COL_Z = COL_Y + 1;
    const int COL_LOAD = COL_Z + 1;
    QGridLayout* paramsLayout = new QGridLayout();
    
    int row = 0;
    paramsLayout->addWidget(xLabel, row, COL_X, Qt::AlignHCenter);
    paramsLayout->addWidget(yLabel, row, COL_Y, Qt::AlignHCenter);
    paramsLayout->addWidget(zLabel, row, COL_Z, Qt::AlignHCenter);
    paramsLayout->addWidget(loadFromLabel, row, COL_LOAD, Qt::AlignHCenter);
    row++;
    
    paramsLayout->addWidget(dimensionsLabel, row, COL_LABEL);
    paramsLayout->addWidget(m_newDimXSpinBox, row, COL_X);
    paramsLayout->addWidget(m_newDimYSpinBox, row, COL_Y);
    paramsLayout->addWidget(m_newDimZSpinBox, row, COL_Z);
    paramsLayout->addWidget(m_paramFromFilePushButton, row, COL_LOAD);
    row++;
    
    paramsLayout->addWidget(originLabel, row, COL_LABEL);
    paramsLayout->addWidget(m_newOriginXSpinBox, row, COL_X);
    paramsLayout->addWidget(m_newOriginYSpinBox, row, COL_Y);
    paramsLayout->addWidget(m_newOriginZSpinBox, row, COL_Z);
    row++;
    
    paramsLayout->addWidget(spacingLabel, row, COL_LABEL);
    paramsLayout->addWidget(m_newSpacingXSpinBox, row, COL_X);
    paramsLayout->addWidget(m_newSpacingYSpinBox, row, COL_Y);
    paramsLayout->addWidget(m_newSpacingZSpinBox, row, COL_Z);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(nameTypeLayout);
    layout->addLayout(paramsLayout);
    
    if (s_previousVolumeSettingsValid) {
        m_newDimXSpinBox->setValue(s_previousVolumeSettings.m_dimensions[0]);
        m_newDimYSpinBox->setValue(s_previousVolumeSettings.m_dimensions[1]);
        m_newDimZSpinBox->setValue(s_previousVolumeSettings.m_dimensions[2]);
        m_newFileNumberOfMapsSpinBox->setValue(s_previousVolumeSettings.m_dimensions[3]);

        const int typeIndex = m_newFileTypeComboBox->findData(static_cast<int>(s_previousVolumeSettings.m_volumeType));
        if (typeIndex >= 0) {
            m_newFileTypeComboBox->setCurrentIndex(typeIndex);
        }
        
        std::vector<std::vector<float> > m = s_previousVolumeSettings.m_indexToSpace;
        m_newSpacingXSpinBox->setValue(m[0][0]);
        m_newSpacingYSpinBox->setValue(m[1][1]);
        m_newSpacingZSpinBox->setValue(m[2][2]);

        m_newOriginXSpinBox->setValue(m[0][3]);
        m_newOriginYSpinBox->setValue(m[1][3]);
        m_newOriginZSpinBox->setValue(m[2][3]);
    }
    
    return widget;
}

/**
 * @return Volume file that was created (NULL if error).
 */
VolumeFile*
VolumeFileCreateDialog::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * Called to create a new file name via the file selection dialog.
 */
void
VolumeFileCreateDialog::newFileNamePushButtonClicked()
{
    const AString filename = CaretFileDialog::getChooseFileNameDialog(DataFileTypeEnum::VOLUME,
                                                                      "");
    if ( ! filename.isEmpty()) {
        m_newFileNameLineEdit->setText(filename);
    }
}

/**
 * Gets called when the ok button is clicked.
 */
void
VolumeFileCreateDialog::okButtonClicked()
{
    const AString filename = m_newFileNameLineEdit->text().trimmed();
    if (filename.isEmpty()) {
        WuQMessageBox::errorOk(this, "Filename is empty.");
        return;
    }
    if (! DataFileTypeEnum::isValidFileExtension(filename,
                                               DataFileTypeEnum::VOLUME)) {
        AString validExtensions;
        const std::vector<AString> allExts = DataFileTypeEnum::getAllFileExtensions(DataFileTypeEnum::VOLUME);
        
        AString allExtsString;
        for (std::vector<AString>::const_iterator iter = allExts.begin();
             iter != allExts.end();
             iter++) {
            allExtsString += (*iter + " ");
        }
        WuQMessageBox::errorOk(this,
                               ("Filename must end with "
                                + allExtsString));
        return;
    }
    
    const int typeIndex = m_newFileTypeComboBox->currentIndex();
    const SubvolumeAttributes::VolumeType volumeType = static_cast<SubvolumeAttributes::VolumeType>(m_newFileTypeComboBox->itemData(typeIndex).toInt());
    
    std::vector<int64_t> dimensions;
    dimensions.push_back(m_newDimXSpinBox->value());
    dimensions.push_back(m_newDimYSpinBox->value());
    dimensions.push_back(m_newDimZSpinBox->value());
    dimensions.push_back(m_newFileNumberOfMapsSpinBox->value());
    
    const float xOrigin = m_newOriginXSpinBox->value();
    const float yOrigin = m_newOriginYSpinBox->value();
    const float zOrigin = m_newOriginZSpinBox->value();
    
    const float xSpacing = m_newSpacingXSpinBox->value();
    const float ySpacing = m_newSpacingYSpinBox->value();
    const float zSpacing = m_newSpacingZSpinBox->value();
    if ((xSpacing == 0.0)
        || (ySpacing == 0.0)
        || (zSpacing == 0.0)) {
        WuQMessageBox::errorOk(this, "Spacing values must be non-zero.");
        return;
    }
    
    std::vector<float> rowOne(4, 0.0);
    rowOne[0] = xSpacing;
    rowOne[3] = xOrigin;

    std::vector<float> rowTwo(4, 0.0);
    rowTwo[1] = ySpacing;
    rowTwo[3] = yOrigin;
    
    std::vector<float> rowThree(4, 0.0);
    rowThree[2] = zSpacing;
    rowThree[3] = zOrigin;
    
    std::vector<float> rowFour(4, 0.0);
    rowFour[3] = 1.0;
    
    std::vector<std::vector<float> > indexToSpace;
    indexToSpace.push_back(rowOne);
    indexToSpace.push_back(rowTwo);
    indexToSpace.push_back(rowThree);
    indexToSpace.push_back(rowFour);
    
    m_volumeFile = new VolumeFile(dimensions,
                                  indexToSpace,
                                  1,
                                  volumeType);
    m_volumeFile->setFileName(filename);
    m_volumeFile->setType(volumeType);
    
    s_previousVolumeSettings.m_dimensions = dimensions;
    s_previousVolumeSettings.m_indexToSpace = indexToSpace;
    s_previousVolumeSettings.m_volumeType = volumeType;
    s_previousVolumeSettingsValid = true;
    
    WuQDialog::okButtonClicked();
}

/**
 * Set the volume parameters from a loaded file.
 */
void
VolumeFileCreateDialog::loadVolumeParametersFromFilePushButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numberOfVolumeFiles = brain->getNumberOfVolumeFiles();
    if (numberOfVolumeFiles <= 0) {
        WuQMessageBox::errorOk(m_paramFromFilePushButton,
                               "No volume files are loaded.");
    }
    
    std::vector<QRadioButton*> volumeRadioButtons;
    WuQDataEntryDialog dialog("Select Volume File For Parameters",
                              m_paramFromFilePushButton);
    for (int32_t i = 0; i < numberOfVolumeFiles; i++) {
        VolumeFile* vf = brain->getVolumeFile(i);
        QRadioButton* rb = dialog.addRadioButton(vf->getFileNameNoPath());
        volumeRadioButtons.push_back(rb);
        if (i == 0) {
            rb->setChecked(true);
        }
    }
    
    
    if (dialog.exec() == WuQDataEntryDialog::Accepted) {
        for (int32_t i = 0; i < numberOfVolumeFiles; i++) {
            if (volumeRadioButtons[i]->isChecked()) {
                VolumeFile* vf = brain->getVolumeFile(i);
                
                const bool includeTypeAndNumberOfMapsFlag = false;
                if (includeTypeAndNumberOfMapsFlag) {
                    const int typeIndex = m_newFileTypeComboBox->findData(static_cast<int>(vf->getType()));
                    if (typeIndex >= 0) {
                        m_newFileTypeComboBox->setCurrentIndex(typeIndex);
                    }
                    m_newFileNumberOfMapsSpinBox->setValue(vf->getNumberOfMaps());
                }
                
                const VolumeSpace volumeSpace = vf->getVolumeSpace();
                
                const int64_t* dimensions = volumeSpace.getDims();
                m_newDimXSpinBox->setValue(dimensions[0]);
                m_newDimYSpinBox->setValue(dimensions[1]);
                m_newDimZSpinBox->setValue(dimensions[2]);
                
                VolumeSpace::OrientTypes orientation[3];
                float spacing[3];
                float origin[3];
                volumeSpace.getOrientAndSpacingForPlumb(orientation,
                                                        spacing,
                                                        origin);
                
                m_newOriginXSpinBox->setValue(origin[0]);
                m_newOriginYSpinBox->setValue(origin[1]);
                m_newOriginZSpinBox->setValue(origin[2]);
                
                m_newSpacingXSpinBox->setValue(spacing[0]);
                m_newSpacingYSpinBox->setValue(spacing[1]);
                m_newSpacingZSpinBox->setValue(spacing[2]);
                
                break;
            }
        }
    }
}


