
/*
	Note: The only constraint is the width of the image so we can put our padding into the height

	|------|
	|------|
	|------|
	|------|
	________
	|------| << padding
*/

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

int GetPixelPosV_Normal( int nStride, int x, int y )
{
	return ( y * ( nStride ) ) + x;
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
__kernel void xblend( const __global uchar4* in, __global uchar4* out, const int nStride )
{
	const int lid = get_local_id( 0 );
	const int gid = get_global_id( 0 );
	const int groupid = get_group_id( 0 );
	
	const int2 pos = ( int2 )(
		( ( groupid * 256 ) + lid ) % nStride,
		( ( groupid * 256 ) + lid ) / nStride
		);

	{
		uchar4 pixelA = in[ GetPixelPosV_Normal( nStride, pos.x, pos.y ) ];
		uchar4 pixelB = in[ GetPixelPosV_Translated( nStride, pos.x, pos.y ) ];
		barrier( CLK_LOCAL_MEM_FENCE );
		
		pixelA = applyMask( pixelA, calcMaskI( pos.x, ( float )nStride ) );
		pixelB = applyMask( pixelB, calcMask( pos.x, ( float )nStride ) );


		out[ gid ] = (uchar4)( 
			pixelA.x + pixelB.x, 
			pixelA.y + pixelB.y, 
			pixelA.z + pixelB.z, 
			0xFF 
		);
	}
}


__kernel void yblend( __global uchar4* in, __global uchar4* out )
{
	/*
	Nun verfährt man ausgehend von Bild B in vertikaler Richtung analog, indem man
	das Bild in y-Richtung um die halbe Höhe zyklisch verschiebt und mit dem Originalbild
	B überblendet.

	Die Blendmasken a und a‘ sind dabei um 90° gedreht.
	*/


}


/*
auto fMapV = [ ]( size_t nStride, int x, int y ) {
const auto nStrideHalf = ( nStride / 2 ) + ( nStride % 2 );

return ( y * ( nStride ) ) + ( ( x + nStrideHalf ) % nStride );
};

auto fMapNormal = [ ]( size_t nStride, int x, int y ) {
return ( y * ( nStride ) ) + x;
};
*/