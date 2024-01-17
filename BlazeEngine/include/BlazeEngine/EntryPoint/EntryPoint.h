#pragma once

#ifdef BLAZE_STATIC
extern void Setup();
extern void AddLoggerOutputFiles();
extern "C" BLAZE_API int main(int argc, char* argv[]);
#else
BLAZE_API int main(int argc, char* argv[]);
#endif