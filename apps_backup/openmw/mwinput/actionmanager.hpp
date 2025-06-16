#ifndef MWINPUT_ACTIONMANAGER_H
#define MWINPUT_ACTIONMANAGER_H

#include <osg/ref_ptr>
#include <osgViewer/ViewerEventHandlers>

namespace osgViewer
{
    class Viewer;
    class ScreenCaptureHandler;
}

namespace MWRender
{
    class RenderingManager; // Forward declaration to avoid unnecessary includes
}

namespace MWInput
{
    class BindingsManager;

    class ActionManager
    {
    public:
        ActionManager(BindingsManager* bindingsManager,
            osgViewer::ScreenCaptureHandler::CaptureOperation* screenCaptureOperation,
            osg::ref_ptr<osgViewer::Viewer> viewer,
            osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler
            //MWRender::RenderingManager* renderingManager
    );

        void update(float dt, bool triedToMove);

        void executeAction(int action);

        bool checkAllowedToUseItems() const;

        void toggleMainMenu();
        void toggleSpell();
        void toggleWeapon();
        void toggleInventory();
        void toggleConsole();
        void screenshot();
        void toggleJournal();
        void activate();
        void toggleWalking();
        void toggleSneaking();
        void toggleAutoMove();
        void rest();
        void quickLoad();
        void quickSave();

        void quickKey (int index);
        void showQuickKeysMenu();

        void resetIdleTime();

        bool isAlwaysRunActive() const { return mAlwaysRunActive; };
        bool isSneaking() const { return mSneaking; };

        void setAttemptJump(bool enabled) { mAttemptJump = enabled; }

        bool isPreviewModeEnabled();

        /// New method for camera zoom functionality
        void updateCameraZoom(float zoomLevel);
        void resetCameraZoom();



    private:
        void handleGuiArrowKey(int action);

        void updateIdleTime(float dt);

        BindingsManager* mBindingsManager;
        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osgViewer::ScreenCaptureHandler> mScreenCaptureHandler;
        osgViewer::ScreenCaptureHandler::CaptureOperation* mScreenCaptureOperation;

        //MWRender::RenderingManager* mRenderingManager; // Add member to store RenderingManager pointer

        bool mAlwaysRunActive;
        bool mSneaking;
        bool mAttemptJump;

        float mOverencumberedMessageDelay;
        float mPreviewPOVDelay;
        float mTimeIdle;
    };
}
#endif
