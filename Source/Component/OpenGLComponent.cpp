/*
  ==============================================================================

    OpenGLComponent.cpp
    Created: 28 May 2024 9:49:33pm
    Author:  Alden Braverman

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLComponent.h"

//==============================================================================
OpenGLComponent::OpenGLComponent()
{
    // DBG("OpenGLComponent Constructor Called");
    // Sets the OpenGL version to 3.2
    // openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
        // Indicates that no part of this Component is transparent.
    // setOpaque(true);

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);

}

OpenGLComponent::~OpenGLComponent()
{
    openGLContext.setContinuousRepainting(false);
    openGLContext.detach();
}

void OpenGLComponent::start()
{
    // DBG("OpenGLComponent start() Called");
    // openGLContext.setContinuousRepainting(true);
}

void OpenGLComponent::stop()
{
    openGLContext.setContinuousRepainting(false);
}



void OpenGLComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    /*
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("OpenGLComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
    */
}

void OpenGLComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void OpenGLComponent::newOpenGLContextCreated()
{

    DBG("GLSL: v" + juce::String(juce::OpenGLShaderProgram::getLanguageVersion(), 2));

    // Setup Shaders
    createShaders();
    
    // Setup Buffer Objects
    openGLContext.extensions.glGenBuffers(1, &VBO); // Vertex Buffer Object
    openGLContext.extensions.glGenBuffers(1, &EBO); // Element Buffer Object

    start();

}

struct OpenGLComponent::Uniforms
{
    Uniforms(juce::OpenGLContext& openGLContext, juce::OpenGLShaderProgram& shaderProgram)
    {
        resolution.reset(createUniform(openGLContext, shaderProgram, "resolution"));

        // xPadUniform.reset(createUniform(openGLContext, shaderProgram, "xPad"));
        // yPadUniform.reset(createUniform(openGLContext, shaderProgram, "yPad"));
    }

    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> resolution;
    
    // std::unique_ptr<juce::OpenGLShaderProgram::Uniform> xPadUniform;
    // std::unique_ptr<juce::OpenGLShaderProgram::Uniform> yPadUniform;

private:
    static juce::OpenGLShaderProgram::Uniform* createUniform(juce::OpenGLContext& openGLContext,
                                                             juce::OpenGLShaderProgram& shaderProgram,
                                                             const char* uniformName
                                                             )
    {
        if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
            return nullptr;
        return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
    }
};



void OpenGLComponent::createShaders()
{
    vertexShader =
        "attribute vec3 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    fragmentShader =
        "uniform vec2  resolution;\n"
        "uniform float  xPad;\n"
        "uniform float  yPad;\n"
        "void main()\n"
        "{\n"
        "gl_FragColor = vec4(yPad, xPad, 0.0, 1.0);\n"
        "}\n";

    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgramAttempt = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);

    // Sets up pipeline of shaders and compiles the program
    if (shaderProgramAttempt->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader))
        && shaderProgramAttempt->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
        && shaderProgramAttempt->link())
    {
        uniforms.release();
        shader = std::move(shaderProgramAttempt);
        uniforms.reset(new Uniforms(openGLContext, *shader));

    }
    else
    {
        // DBG(juce::String(shaderProgramAttempt->getLastError());
    }

    triggerAsyncUpdate();
}

void OpenGLComponent::renderOpenGL()
{
    jassert(juce::OpenGLHelpers::isContextActive());

    // Setup Viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    juce::gl::glViewport(0, 0, juce::roundToInt(renderingScale * getWidth()), juce::roundToInt(renderingScale * getHeight()));

    // Set background Color
    juce::OpenGLHelpers::clear(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Use Shader Program that's been defined
    shader->use();

    // Setup the Uniforms for use in the Shader
    // =======================================================================================================================
    if (uniforms->resolution != nullptr)
        uniforms->resolution->set((GLfloat)renderingScale * getWidth(), (GLfloat)renderingScale * getHeight());
    
    /*
    if (uniforms->xPadUniform != nullptr)
        uniforms->xPadUniform->set(xPadMousePosition / 640.0f);
        // DBG("xPadMousePosition: " + juce::String(xPadMousePosition));

    if (uniforms->yPadUniform != nullptr)
        uniforms->yPadUniform->set(yPadMousePosition / 310.0f);
    */
    // DBG("xPadMousePosition: " + juce::String(xPadMousePosition));
    // =======================================================================================================================
    
    // Define Vertices for a Square (the view plane)
    GLfloat vertices[] = {
        1.0f,   1.0f,  0.0f,  // Top Right
        1.0f,  -1.0f,  0.0f,  // Bottom Right
        -1.0f, -1.0f,  0.0f,  // Bottom Left
        -1.0f,  1.0f,  0.0f   // Top Left
    };
    // Define Which Vertex Indexes Make the Square
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };

    // VBO (Vertex Buffer Object) - Bind and Write to Buffer
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(vertices), vertices, juce::gl::GL_STREAM_DRAW);

    // EBO (Element Buffer Object) - Bind and Write to Buffer
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, EBO);
    openGLContext.extensions.glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, juce::gl::GL_STREAM_DRAW);

    // Setup Vertex Attributes
    openGLContext.extensions.glVertexAttribPointer(0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    openGLContext.extensions.glEnableVertexAttribArray(0);

    // Draw Vertices
    juce::gl::glDrawElements(juce::gl::GL_TRIANGLES, 6, juce::gl::GL_UNSIGNED_INT, 0); // For EBO's (Element Buffer Objects) (Indices)

    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLComponent::openGLContextClosing()
{
    shader.release();
    uniforms.release();
}

void::OpenGLComponent::handleAsyncUpdate()
{

}
