#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "shader.h"

class cTexture{
    private:
        unsigned int texture{ 0 };

    public:
        cTexture();
        void Create();
        void load(char const* path, int type);
        unsigned int get();
};

#endif