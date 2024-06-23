/*
  ==============================================================================

    OpenGLComponent.h
    Created: 28 May 2024 9:49:33pm
    Author:  Alden Braverman

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OpenGLComponent  : public juce::Component, public juce::OpenGLRenderer, public juce::AsyncUpdater
{
public:
    OpenGLComponent();
    ~OpenGLComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void handleAsyncUpdate() override;
    
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    
    float openGLtimeFloat;

private:
    juce::OpenGLContext openGLContext;
    GLuint VBO, VAO, EBO;
    // juce::String()
    void start();
    void stop();
    void createShaders();

    const char* vertexShader;
    const char* fragmentShader;

    struct Uniforms;
    std::unique_ptr<Uniforms> uniforms;
    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    
    juce::Point<float> lastMousePosition;

    float xPadMousePosition;
    float yPadMousePosition;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};
