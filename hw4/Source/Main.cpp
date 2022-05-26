/*
  ==============================================================================

    Main.cpp
    Created: May, 2022
    Author:  Minsuk Choi and Jaekwon Im 

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Synth.h"

class Application   : public juce::JUCEApplication
{
public:
    Application() = default;

    const juce::String getApplicationName() override        { return "GCT535_Homework4_DelayBasedAudioEffects"; }
    const juce::String getApplicationVersion() override     { return "1.0.0"; }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow ("GCT535_Homework4_DelayBasedAudioEffects", new MainContentComponent, *this));
    }

    void shutdown() override                                { mainWindow = nullptr; }

private:
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name, juce::Component* c, JUCEApplication& a)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                .findColour (ResizableWindow::backgroundColourId),
                              juce::DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (false, false);
            centreWithSize (getWidth(), getHeight());
           #endif
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (Application)
