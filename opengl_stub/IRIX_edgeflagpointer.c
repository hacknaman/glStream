#include "cr_glwrapper.h"

// This is necessary because IRIX disagrees with Windows about arg 2.

extern void *__glim_EdgeFlagPointer;
typedef void (*glEdgeFlagPointer_ptr) ( GLsizei stride, const GLboolean *pointer ) ;

void glEdgeFlagPointer( GLsizei stride, const GLboolean *pointer )
{
	((glEdgeFlagPointer_ptr) __glim_EdgeFlagPointer)( stride, pointer );
}
