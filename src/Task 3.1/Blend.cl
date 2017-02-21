

const float calcMask( float x, float fWidth )
{
	return fabs( ( ( -2.0f * x ) / ( fWidth - 1.0f ) ) + 1.0f );
}

const float calcMaskI( float x, float fWidth )
{
	return 1 - calcMask( x, fWidth );
}


int GetPixelPosV_Translated( int nStride, int x, int y )
{
	const int nStrideHalf = ( nStride / 2 ) + ( nStride % 2 );

	return ( y * ( nStride ) ) + ( ( x + nStrideHalf ) % nStride );
}

int GetPixelPos_Normal( int nStride, int x, int y )
{
	return ( y * ( nStride ) ) + x;
}

int GetPixelPosH_Translated( int x, int y, int nWidth, int nHeight )
{
	if( x > nWidth || y > nHeight )
	{
		//> map back to padding
		return ( y * ( nWidth ) ) + x;
	}

	const int nHeightHalf = ( nHeight / 2 ) + ( nHeight % 2 );

	return ( ( ( y + nHeightHalf ) % nHeight ) * ( nWidth ) ) + x;
}



uchar4 applyMask( uchar4 pixel, float fMask )
{
	uchar4 _pixel = pixel;

	_pixel.x = ( uchar )( ( ( float )pixel.x ) * fMask );	//B
	_pixel.y = ( uchar )( ( ( float )pixel.y ) * fMask );	//G
	_pixel.z = ( uchar )( ( ( float )pixel.z ) * fMask );	//R

	return _pixel;
}


/*
	nStride is not 4 bytes alligned
*/
__kernel void xyblend( __global uchar4* in, __global uchar4* out, const int nWidth, const int nHeight )
{
	const int lid = get_local_id( 0 );
	const int gid = get_global_id( 0 );
	const int groupid = get_group_id( 0 );

	const int2 pos = ( int2 )(
		( ( groupid * 256 ) + lid ) % nWidth,
		( ( groupid * 256 ) + lid ) / nWidth
		);

	{
		//> Normal pos
		const int itN = GetPixelPos_Normal( nWidth, pos.x, pos.y );
		const int itTV = GetPixelPosV_Translated( nWidth, pos.x, pos.y );
		const int itTH = GetPixelPosH_Translated( pos.x, pos.y, nWidth, nHeight );


		//> transform
		uchar4 pixelA = in[ itN ];
		uchar4 pixelB = in[ itTV ];
		barrier( CLK_LOCAL_MEM_FENCE );

		//> First transform
		pixelA = applyMask( pixelA, calcMaskI( pos.x, ( float )nWidth ) );
		pixelB = applyMask( pixelB, calcMask( pos.x, ( float )nWidth ) );

		//> Reduce transformation
		uchar4 _tmpPixel = ( uchar4 )(
			pixelA.x + pixelB.x,
			pixelA.y + pixelB.y,
			pixelA.z + pixelB.z,
			0xFF
			);
	
		_tmpPixel = applyMask( _tmpPixel, calcMaskI( pos.y, ( float )nHeight ) );

		
		uchar4 pixelC = itTV == itTH ? pixelA : in[ itTH ];
		barrier( CLK_LOCAL_MEM_FENCE );

		pixelC = applyMask( pixelC, calcMask( pos.y, ( float )nHeight ) );

		out[ gid ] = ( uchar4 )(
			_tmpPixel.x + pixelC.x,
			_tmpPixel.y + pixelC.y,
			_tmpPixel.z + pixelC.z,
			0xFF
			);
	}
}
