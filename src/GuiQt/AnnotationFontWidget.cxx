
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#define __ANNOTATION_FONT_WIDGET_DECLARE__
#include "AnnotationFontWidget.h"
#undef __ANNOTATION_FONT_WIDGET_DECLARE__

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationFontNameEnum.h"
#include "AnnotationFontSizeEnum.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::AnnotationFontWidget
 * \brief Widget for annotation font selection
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of window in which this instance is displayed
 * @param parent
 *     Parent for this widget.
 */
AnnotationFontWidget::AnnotationFontWidget(const int32_t browserWindowIndex,
                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    /*
     * "Font" label
     */
    QLabel* fontLabel = new QLabel("Font ");
    
    /*
     * Combo box for font name selection
     */
    m_fontNameComboBox = new EnumComboBoxTemplate(this);
    m_fontNameComboBox->setup<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>();
    QObject::connect(m_fontNameComboBox, SIGNAL(itemActivated()),
                     this, SLOT(fontNameChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontNameComboBox->getWidget(),
                                          "Change font");
    
    /*
     * Combo box for font size
     */
    m_fontSizeComboBox = new EnumComboBoxTemplate(this);
    m_fontSizeComboBox->setup<AnnotationFontSizeEnum,AnnotationFontSizeEnum::Enum>();
    QObject::connect(m_fontSizeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(fontSizeChanged()));
    WuQtUtilities::setToolTipAndStatusTip(m_fontSizeComboBox->getWidget(),
                                          "Change font size (height)");
    
    /*
     * Bold Font
     */
    m_boldFontAction = WuQtUtilities::createAction("B", //boldFontText.toStringWithHtmlBody(),
                                                   "Enable/disable bold styling",
                                                   this, this, SLOT(fontBoldChanged()));
    m_boldFontAction->setCheckable(true);
    QToolButton* boldFontToolButton = new QToolButton();
    boldFontToolButton->setDefaultAction(m_boldFontAction);
    
    /*
     * Change the bold toolbutton's font to bold.
     */
    QFont boldFont = boldFontToolButton->font();
    boldFont.setBold(true);
    boldFontToolButton->setFont(boldFont);
    
    /*
     * Italic font toolbutton
     */
    m_italicFontAction = WuQtUtilities::createAction("i", "Enable/disable italic styling",
                                                     this, this, SLOT(fontItalicChanged()));
    m_italicFontAction->setCheckable(true);
    QToolButton* italicFontToolButton = new QToolButton();
    italicFontToolButton->setDefaultAction(m_italicFontAction);
    
    /*
     * Change the italic toolbutton's font to italic.
     */
    QFont italicFont = italicFontToolButton->font();
    italicFont.setItalic(true);
    italicFontToolButton->setFont(italicFont);
    
    /*
     * Underline font toolbutton
     */
    m_underlineFontAction =  WuQtUtilities::createAction("U", "Enable/disable font underlining",
                                                         this, this, SLOT(fontUnderlineChanged()));
    m_underlineFontAction->setCheckable(true);
    QToolButton* underlineFontToolButton = new QToolButton();
    underlineFontToolButton->setDefaultAction(m_underlineFontAction);
    
    /*
     * Change the underline toolbutton's font to underline.
     */
    QFont underlineFont = underlineFontToolButton->font();
    underlineFont.setUnderline(true);
    underlineFontToolButton->setFont(underlineFont);
    
    /*
     * Layout the widgets
     */
    QHBoxLayout* topRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(topRowLayout, 2, 0);
    topRowLayout->addWidget(fontLabel, 0);
    topRowLayout->addWidget(m_fontNameComboBox->getWidget(), 100);
    
    
    
    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 0);
    bottomRowLayout->addWidget(boldFontToolButton);
    bottomRowLayout->addWidget(italicFontToolButton);
    bottomRowLayout->addWidget(underlineFontToolButton);
    bottomRowLayout->addStretch();
    bottomRowLayout->addWidget(m_fontSizeComboBox->getWidget());
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(topRowLayout);
    layout->addLayout(bottomRowLayout);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationFontWidget::~AnnotationFontWidget()
{
}

/**
 * Update the content of this widget with the given text annotation.
 *
 * @param annotationTexts
 *     Text annotations for display (may be NULL).
 */
void
AnnotationFontWidget::updateContent(std::vector<AnnotationText*>& annotationTexts)
{
    if ( ! annotationTexts.empty()) {
        bool boldOnFlag      = true;
        bool italicOnFlag    = true;
        bool underlineOnFlag = true;
        
        AnnotationFontNameEnum::Enum fontName = AnnotationFontNameEnum::VERA;
        int32_t fontSizeIntegerCode = 100000;
        bool fontNameValid = true;
        bool haveMultipleFontSizeValues = false;
        
        const int32_t numAnn = static_cast<int32_t>(annotationTexts.size());
        for (int32_t i = 0; i < numAnn; i++) {
            CaretAssertVectorIndex(annotationTexts, i);
            const AnnotationText* annText = annotationTexts[i];
            
            const AnnotationFontSizeEnum::Enum fontSize = annText->getFontSize();
            const int32_t fontIntCode = AnnotationFontSizeEnum::toIntegerCode(fontSize);
            if (i == 0) {
                fontName = annText->getFont();
                fontSizeIntegerCode = fontIntCode;
            }
            else {
                if (annText->getFont() != fontName) {
                    fontNameValid = false;
                }
                if (fontSizeIntegerCode != fontIntCode) {
                    haveMultipleFontSizeValues = true;
                    fontSizeIntegerCode = std::min(fontSizeIntegerCode,
                                                   fontIntCode);
                }
            }
            
            if ( ! annText->isBoldEnabled()) {
                boldOnFlag = false;
            }
            if ( ! annText->isItalicEnabled()) {
                italicOnFlag = false;
            }
            if ( ! annText->isUnderlineEnabled()) {
                underlineOnFlag = false;
            }
        }
        
        m_fontNameComboBox->setSelectedItem<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>(fontName);
        
        bool validFontSizeFlag = false;
        const AnnotationFontSizeEnum::Enum fontSize = AnnotationFontSizeEnum::fromIntegerCode(fontSizeIntegerCode,
                                                                                              &validFontSizeFlag);
        if (validFontSizeFlag) {
            m_fontSizeComboBox->setSelectedItem<AnnotationFontSizeEnum,AnnotationFontSizeEnum::Enum>(fontSize);
            if (haveMultipleFontSizeValues) {
                //m_fontSizeComboBox->setSuffix("+");
            }
        }
        
        /*
         * Font styles are ON only if all selected
         * text annotations have the style enabled
         */
        m_boldFontAction->setChecked(boldOnFlag);
        m_italicFontAction->setChecked(italicOnFlag);
        m_underlineFontAction->setChecked(underlineOnFlag);
    }
    
    setEnabled( ! annotationTexts.empty());
}

/**
 * Gets called when font bold changed.
 */
void
AnnotationFontWidget::fontBoldChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontBold(m_boldFontAction->isChecked(),
                                 annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when font italic changed.
 */
void
AnnotationFontWidget::fontItalicChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontItalic(m_italicFontAction->isChecked(),
                                   annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when font name changed.
 */
void
AnnotationFontWidget::fontNameChanged()
{
    const AnnotationFontNameEnum::Enum fontName = m_fontNameComboBox->getSelectedItem<AnnotationFontNameEnum,AnnotationFontNameEnum::Enum>();
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontName(fontName,
                                 annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when font size changed.
 */
void
AnnotationFontWidget::fontSizeChanged()
{
    const AnnotationFontSizeEnum::Enum fontSize = m_fontSizeComboBox->getSelectedItem<AnnotationFontSizeEnum, AnnotationFontSizeEnum::Enum>();
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontSize(fontSize,
                                 annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when font underline changed.
 */
void
AnnotationFontWidget::fontUnderlineChanged()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeTextFontUnderline(m_underlineFontAction->isChecked(),
                                      annMan->getSelectedAnnotations());
    annMan->applyCommand(command);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}