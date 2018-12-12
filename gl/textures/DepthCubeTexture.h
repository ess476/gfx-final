#ifndef DEPTHCUBETEXTURE_H
#define DEPTHCUBETEXTURE_H

#include "DepthTexture.h"

class DepthCubeTexture : public DepthTexture
{
public:

    void bind() const override {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
    }

    void unbind() const override {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    DepthCubeTexture(size_t width, size_t height)
    {
        bind();

        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        unbind();
    }
};

#endif // DEPTHCUBETEXTURE_H
