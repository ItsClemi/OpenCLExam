


__kernel calcStatistic( uchar3* pixel, int nLength, int* erg )
{

	__local int counts[ 32 ][ 256 ];

	for( int i = 0; i < 256; i++ )
	{

	}


	for( int k = 0; k < 32; k++ )
	{
		for( int i = 0; i < 8; i++ )
		{
			// I = 0 , ... 7;
			counts[ k ][ LX + i * 32 ];
		}


	}

}

__kernel reduceStatistic( )
{

}
//(LX ist dabei die local_id(0))