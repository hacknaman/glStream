#include "packer.h"
#include "cr_opcodes.h"
#include "cr_glwrapper.h"

void PACK_APIENTRY crPackCreateContext( void *arg1, void *arg2 )
{
	unsigned char *data_ptr;
	(void) arg1;
	(void) arg2;

	GET_BUFFERED_POINTER_NO_ARGS();
	WRITE_OPCODE( CR_CREATECONTEXT_OPCODE );
}
