#ifndef __SESSION_INTERFACE
#define __SESSION_INTERFACE

#ifdef __cplusplus
extern "C" {
#endif

   void *createComputator(int heigth, int width, float angle, int objectsNumber, float *objects);
   void traceRays(void *computator, long long startIndex, long long endIndex, float **results);
   void destroyComputator(void *computator);

#ifdef __cplusplus
}
#endif

#endif // __SESSION_INTERFACE
