#include "cr_spu.h"
#include <stdio.h>
#include "cr_glwrapper.h"

#define ANGLE_STEP .050f

int main(int argc, char *argv[])
{
	SPU *spu;
	char *spuname = "renderspu";
	float angle = 0;
	int frame = 0;

	if (argc > 1)
	{
		spuname = argv[1];
	}
	spu = crSPULoad( NULL, 0, spuname, NULL );

	spu->dispatch_table.Viewport( 0, 0, 256, 256 );
	spu->dispatch_table.ClearColor( 0,0,0,1 );
	for (;;)
	{
		frame++;
		//if (frame == 3)
		//{
			//break;
		//}
		angle += ANGLE_STEP;
		spu->dispatch_table.Clear( GL_COLOR_BUFFER_BIT );
		spu->dispatch_table.MatrixMode( GL_MODELVIEW );
		spu->dispatch_table.LoadIdentity();
		spu->dispatch_table.Rotatef( angle, 0, 0, 1 );
		spu->dispatch_table.Begin( GL_TRIANGLES );
		spu->dispatch_table.Vertex3f( .5, .5, 0 );
		spu->dispatch_table.Vertex3f( .5,1,0 );
		spu->dispatch_table.Vertex3f( 1,.5,0 );
		spu->dispatch_table.End( );
		spu->dispatch_table.SwapBuffers();
	}
}
