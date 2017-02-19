

__kernel void calcStatistic( __global uchar* pPixelBuffer, int nPixelCount, __global int* pResult )
{
	const int lid = get_local_id( 0 );
	const int groupid = get_group_id( 0 );		

	__local int counts[ 32 ][ 256 ];

	//clear mem
	for( int i = 0; i < 256; i++ )
	{
		counts[ lid ][ i ] = 0;
	}

	barrier( CLK_LOCAL_MEM_FENCE );


	//> 8(rounds) * 32(pixel) * 32(workitems) = 8192 pixel

	const int nPixelOffset = ( groupid * 8192 );
	for( int r = 0; r < 8; r++ )
	{
		for( int k = 0; k < 32; k++ )
		{
			//> pixel memory offset
			const int off = nPixelOffset + ( ( lid + k * 32 ) + ( r * 1024 ) );

			if( off < nPixelCount )
			{
				//> endian remapping (see SPixel struct)
				const uchar R = as_uchar( pPixelBuffer[ ( off * 4 ) + 2 ] );
				const uchar G = as_uchar( pPixelBuffer[ ( off * 4 ) + 1 ] );
				const uchar B = as_uchar( pPixelBuffer[ ( off * 4 ) + 0 ] );
				const uchar A = as_uchar( pPixelBuffer[ ( off * 4 ) + 3 ] );

				float I = 0.299 * R + 0.587 * G + 0.114 * B;

				counts[ lid ][ ( int )I ]++;
			}
		}
	}

	barrier( CLK_LOCAL_MEM_FENCE );
	

	//> Reduce results
	for( int k = 0; k < 32; k++ )
	{
		for( int i = 0; i < 8; i++ )
		{
			pResult[ ( groupid * 256 ) + ( lid + i * 32 ) ] += counts[ k ][ lid + i * 32 ];
		}
	}
}


__kernel void reduceStatistic( __global int* pResult, int nWorkGroups )
{
	const int lid = get_local_id( 0 );

	__local int counts[ 256 ];

	//> Clear Array
	counts[ lid ] = 0;
	barrier( CLK_LOCAL_MEM_FENCE );

	//> Reduce results
	for( int i = 0; i < nWorkGroups; i++ )
	{
		counts[ lid ] += pResult[ lid + ( i * 256 ) ];
	}
	barrier( CLK_LOCAL_MEM_FENCE );


	pResult[ lid ] = counts[ lid ];

}