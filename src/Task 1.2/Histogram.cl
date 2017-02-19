

__kernel void calcStatistic( __global uchar* pPixelBuffer, int nPixelCount, __global int* pResult )
{
	const int lid = get_local_id( 0 );
	const int groupid = get_group_id( 0 );


	const int nPixelOffset = ( groupid * 8192 );


	for( int r = 0; r < 8; r++ )
	{
		for( int k = 0; k < 32; k++ )
		{
			const int off = nPixelOffset + ( ( lid + k * 32 ) + ( r * 1024 ) );

			if( off < nPixelCount )
			{
				//> endian remapping (see SPixel struct)
				const uchar R = as_uchar( pPixelBuffer[ ( off * 4 ) + 2 ] );
				const uchar G = as_uchar( pPixelBuffer[ ( off * 4 ) + 1 ] );
				const uchar B = as_uchar( pPixelBuffer[ ( off * 4 ) + 0 ] );
				const uchar A = as_uchar( pPixelBuffer[ ( off * 4 ) + 3 ] );

				float I = 0.299 * R + 0.587 * G + 0.114 * B;

				atomic_add( &pResult[ ( int )I ], 1 );
			}
		}
	}
}