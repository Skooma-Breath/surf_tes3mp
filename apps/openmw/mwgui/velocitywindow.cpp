#include "velocitywindow.hpp"

#include <MyGUI_TextBox.h>
#include <MyGUI_Window.h>
#include <MyGUI_Gui.h>

#include <components/settings/settings.hpp>

#include "draganddrop.hpp"

namespace MWGui
{
    VelocityWindow::VelocityWindow(DragAndDrop* drag)
        : WindowPinnableBase("openmw_velocity_window.layout")
        , NoDrop(drag, mMainWidget)
        , mFrameVisible(true)
        , mFloatingText(nullptr)
    {
        getWidget(mVelocityText, "VelocityText");

        // Create floating text widget manually (not from layout)
        mFloatingText = MyGUI::Gui::getInstance().createWidget<MyGUI::TextBox>(
            "SandText",
            0, 0, 200, 60,
            MyGUI::Align::Default,
            "HUD",
            "VelocityFloatingText"
        );

        mFloatingText->setTextAlign(MyGUI::Align::Center);
        mFloatingText->setTextShadow(true);
        mFloatingText->setTextShadowColour(MyGUI::Colour::Black);
        mFloatingText->setTextColour(MyGUI::Colour(0, 1, 0));
        mFloatingText->setNeedMouseFocus(false);
        mFloatingText->setVisible(false);

        setTitle("Speed");
        setPinned(false);

        // Hook into window resize events
        MyGUI::Window* window = mMainWidget->castType<MyGUI::Window>();
        if (window)
        {
            window->eventWindowChangeCoord += MyGUI::newDelegate(this, &VelocityWindow::onWindowResize);
        }

        updateTextSize();
    }

    VelocityWindow::~VelocityWindow()
    {
        // Clean up floating text widget
        if (mFloatingText)
        {
            MyGUI::Gui::getInstance().destroyWidget(mFloatingText);
            mFloatingText = nullptr;
        }
    }

    void VelocityWindow::setVelocityText(const std::string& text)
    {
        if (mVelocityText)
            mVelocityText->setCaption(text);
        if (mFloatingText)
            mFloatingText->setCaption(text);
    }

    void VelocityWindow::toggleWindowFrame()
    {
        mFrameVisible = !mFrameVisible;

        if (mFrameVisible)
        {
            // Show window frame, hide floating text
            mMainWidget->setVisible(true);
            if (mFloatingText)
                mFloatingText->setVisible(false);
        }
        else
        {
            // Hide window frame, show floating text
            updateFloatingTextPosition();
            syncTextWidgets();
            mMainWidget->setVisible(false);
            if (mFloatingText)
                mFloatingText->setVisible(true);
        }
    }

    void VelocityWindow::updateFloatingTextPosition()
    {
        if (!mFloatingText || !mMainWidget || !mVelocityText)
            return;

        // Get the absolute position of the text widget inside the window
        // This accounts for all window decorations automatically
        MyGUI::IntPoint textAbsPos = mVelocityText->getAbsolutePosition();
        MyGUI::IntSize textSize = mVelocityText->getSize();

        // Position floating text exactly where the window text is
        mFloatingText->setPosition(textAbsPos);
        mFloatingText->setSize(textSize);
    }

    void VelocityWindow::syncTextWidgets()
    {
        if (!mFloatingText || !mVelocityText)
            return;

        // Sync text content
        mFloatingText->setCaption(mVelocityText->getCaption());

        // Sync font size
        mFloatingText->setFontHeight(mVelocityText->getFontHeight());

        // Sync text alignment
        mFloatingText->setTextAlign(mVelocityText->getTextAlign());
    }

    void VelocityWindow::onResChange(int width, int height)
    {
        WindowPinnableBase::onResChange(width, height);
        updateTextSize();

        if (!mFrameVisible)
        {
            updateFloatingTextPosition();
            syncTextWidgets();
        }
    }

    void VelocityWindow::onOpen()
    {
        WindowPinnableBase::onOpen();
        updateTextSize();

        // Update floating text position when window opens (if frame is hidden)
        if (!mFrameVisible)
        {
            updateFloatingTextPosition();
            syncTextWidgets();
        }
    }

    void VelocityWindow::onWindowResize(MyGUI::Window* sender)
    {
        updateTextSize();

        // Update floating text position/size when window is resized
        if (!mFrameVisible)
        {
            updateFloatingTextPosition();
            syncTextWidgets();
        }
    }

    void VelocityWindow::updateTextSize()
    {
        if (!mMainWidget || !mVelocityText)
            return;

        int windowHeight = mMainWidget->getHeight();

        // Scale font between 14-32px as window grows from 50-100px
        int minHeight = 50;
        int maxHeight = 1000;
        int minFontSize = 10;
        int maxFontSize = 64;

        int clampedHeight = std::max(minHeight, std::min(maxHeight, windowHeight));
        float ratio = static_cast<float>(clampedHeight - minHeight) / (maxHeight - minHeight);
        int fontSize = minFontSize + static_cast<int>(ratio * (maxFontSize - minFontSize));

        mVelocityText->setFontHeight(fontSize);

        // Also update floating text font size
        if (mFloatingText)
            mFloatingText->setFontHeight(fontSize);
    }

    void VelocityWindow::onPinToggled()
    {
        Settings::Manager::setBool("velocity pin", "Windows", mPinned);
    }
}
