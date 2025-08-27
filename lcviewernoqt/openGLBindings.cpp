#include "openGLBindings.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace lc
{
namespace viewer
{

OpenGLBindings::OpenGLBindings()
{
    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
        LOG_ERROR << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    if (!GLEW_VERSION_2_1) 
    {
        LOG_ERROR << "OpenGL version 2.1 is not available" << std::endl;
        exit(1);
    }
}

}
}
