

__kernel void calcStatistic( __global uchar4* pPixelBuffer, int nPixelCount, __global int* pResult )
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
	for( int k = 0; k < 32; k++ )
	{
		for( int i = 0; i < 8; i++ )
		{
			const int nActualPixel = ( groupid * 8192 ) + ( lid + i * 32 );

			if( nActualPixel < nPixelCount )
			{
				const uchar4 pixel = pPixelBuffer[ nActualPixel ];

				const float R = ( float )pixel.x;
				const float G = ( float )pixel.y;
				const float B = ( float )pixel.z;

				float I = 0.299 * R + 0.587 * G + 0.114 * B;

				counts[ lid ][ ( ( int )I ) & 0xFF ]++;
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

