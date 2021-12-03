#ifndef VERSION_HXX_
#define VERSION_HXX_

extern "C"
{
    extern const char* GIT_TAG;
    extern const char* GIT_REV;
    extern const char* GIT_BRANCH;
    extern const char* GIT_VERSION;
}

static inline const char* getGitVersion()
{
    return GIT_VERSION;
}

#endif