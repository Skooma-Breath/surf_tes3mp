#ifndef MWGUI_VELOCITYWINDOW_H
#define MWGUI_VELOCITYWINDOW_H

#include "windowpinnablebase.hpp"

namespace MWGui
{
    class VelocityWindow : public WindowPinnableBase, public NoDrop
    {
    public:
        VelocityWindow(DragAndDrop* drag);
        virtual ~VelocityWindow();

        void setVelocityText(const std::string& text);
        void toggleWindowFrame();  // F6 toggles this
        bool isFrameVisible() const { return mFrameVisible; }

        void onResChange(int width, int height) override;
        void onOpen() override;
        void updateTextSize();

        MyGUI::TextBox* getFloatingText() { return mFloatingText; }

    protected:
        void onPinToggled() override;

    private:
        MyGUI::TextBox* mFloatingText;       // Text shown when frame is hidden
        MyGUI::TextBox* mVelocityText;       // Text inside window
        bool mFrameVisible;

        void onWindowResize(MyGUI::Window* sender);
        void updateFloatingTextPosition();
        void syncTextWidgets();
    };
}

#endif
