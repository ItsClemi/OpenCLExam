

__kernel void calcStatistic( __global int* pPixelBuffer, int nPixelCount, __global int* pResult, __global int* pDebug )
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


	const int nPixelOffset = ( groupid * 8192 );


	//> 8(rounds) * 32(pixel) * 32(workitems) = 8192 pixel
	
	for( int r = 0; r < 8; r++ )
	{
		for( int k = 0; k < 32; k++ )
		{
			//> pixel memory offset
			const int off = nPixelOffset + ( ( lid + k * 32 ) + ( r * 1024 ) );

			if( off < nPixelCount )
			{
				uchar4 pixel = as_uchar4( pPixelBuffer[ off * sizeof( int ) ] );

				const uchar R = pixel.x;
				const uchar G = pixel.y;
				const uchar B = pixel.z;
				const uchar A = pixel.w;

				float I = 25.0f;//0.299 * R + 0.587 * G + 0.114 * B;

				counts[ lid ][ ( int )I ]++;

				pDebug[ get_global_id( 0 ) ] = get_global_id( 0 );

			}
			else
			{
				//pDebug[ get_global_id( 0 ) ] = 0xFF;
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

/*

//const int nActualPixel = ( groupid * 8192 ) + ( lid + k * i * 32 );

if( nActualPixel < nPixelCount )
{
	//const uchar4 pixel = pPixelBuffer[ nActualPixel ];

	
	const float R = ( float )pixel.x;
	const float G = ( float )pixel.y;
	const float B = ( float )pixel.z;
	const float A = ( float )pixel.w;
	

	const uchar R = ( ( uchar )pPixelBuffer[ nActualPixel + 0 ] );
	const uchar G = ( ( uchar )pPixelBuffer[ nActualPixel + 1 ] );
	const uchar B = ( ( uchar )pPixelBuffer[ nActualPixel + 2 ] );
	const uchar A = ( ( uchar )pPixelBuffer[ nActualPixel + 3 ] );

	float I = A; //0.299 * R + 0.587 * G + 0.114 * B;
	
	//counts[ lid ][ ( ( int )I ) ]++;
	//counts[ lid ][ G ]++;


}
*/