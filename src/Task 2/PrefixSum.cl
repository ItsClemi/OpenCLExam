



#define GX get_global_id(0)
#define LX get_local_id(0)

//> Something in here is broken:
//> I'll get random failures above 4k elements
__kernel void calcPrefix256_Broken( __global int* in, __global int* out, __global int* pHelper )
{
	__local int localArray[ 256 ];

	int k = 8;	// depth of tree: log2(256)
	int d, i;

	// copy to local memory
	localArray[ LX ] = in[ GX ];
	barrier( CLK_LOCAL_MEM_FENCE );

	// Up-Sweep
	int noItemsThatWork = 128;
	int offset = 1;
	for( d = 0; d<k; d++, noItemsThatWork >>= 1, offset <<= 1 )
	{
		if( LX < noItemsThatWork )
		{
			i = LX*( offset << 1 ) + offset - 1;
			localArray[ i + offset ] = localArray[ i ] + localArray[ i + offset ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}


	// Down-Sweep
	if( LX == 255 )
		localArray[ 255 ] = 0;
	noItemsThatWork = 1;
	offset = 128;
	for( d = 0; d<k; d++, noItemsThatWork <<= 1, offset >>= 1 )
	{
		if( LX < noItemsThatWork )
		{
			i = LX*( offset << 1 ) + offset - 1;
			int tmp = localArray[ i ];
			localArray[ i ] = localArray[ i + offset ];
			localArray[ i + offset ] = tmp + localArray[ i + offset ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	// write result to global memory
	out[ GX ] = localArray[ LX ];

	if( pHelper && LX == 255 )
	{
		const int groupid = get_group_id( 0 );

		pHelper[ groupid ] = in[ GX ] + localArray[ LX ];
	}
}



__kernel void calcPrefix256( const __global int* in, __global int* out, __global int* pHelper )
{
	int n = 256;

	int gid = get_global_id( 0 );
	int lid = get_local_id( 0 );
	int groupid = get_group_id( 0 );

	__local int localArray[ 256 ];

	localArray[ lid ] = in[ gid ];
	barrier( CLK_LOCAL_MEM_FENCE );

	int offset = 1;
	for( int i = n >> 1; i > 0; i >>= 1 )
	{
		if( lid < i )
		{
			int ai = offset * ( ( lid << 1 ) + 1 ) - 1;
			int bi = offset * ( ( lid << 1 ) + 2 ) - 1;

			localArray[ bi ] += localArray[ ai ];
		}

		offset *= 2;

		barrier( CLK_LOCAL_MEM_FENCE );
	}

	if( lid == 0 )
	{
		localArray[ n - 1 ] = 0;
	}

	for( int i = 1; i < n; i *= 2 )
	{
		offset >>= 1;

		if( lid < i )
		{
			int ai = offset * ( ( lid << 1 ) + 1 ) - 1;
			int bi = offset * ( ( lid << 1 ) + 2 ) - 1;

			int t = localArray[ ai ];
			localArray[ ai ] = localArray[ bi ];
			localArray[ bi ] += t;
		}

		barrier( CLK_LOCAL_MEM_FENCE );
	}

	out[ gid ] = localArray[ lid ];


	if( pHelper && LX == 255 )
	{
		const int groupid = get_group_id( 0 );

		pHelper[ groupid ] = in[ GX ] + localArray[ LX ];
	}
	//else 
	//{
	//	if( gid > 0 )
	//	{
	//		out[ gid ] = in[ GX ] + localArray[ LX ];
	//	}
	//}

	//if( pHelper && LX == 255 )
	//{
	//	const int groupid = get_group_id( 0 );

	//	pHelper[ groupid ] = in[ GX ] + localArray[ LX ];
	//}
}


__kernel void finalize( __global int* pResult, __global int* pHelper )
{
	const int gid = get_global_id( 0 );
	const int groupid = get_group_id( 0 );

	pResult[ gid ] += pHelper[ groupid ];
}



/*
	//Base Algortihm (PrefixSumBasisalgorithmus.zip)


#define GX get_global_id(0)
#define LX get_local_id(0)

__kernel void calcPrefix256( __global int* in, __global int* out )	// needs exactly 256 elements
{
	__local int localArray[ 256 ];

	int k = 8;	// depth of tree: log2(256)
	int d, i;

	// copy to local memory
	localArray[ LX ] = in[ GX ];
	barrier( CLK_LOCAL_MEM_FENCE );

	// Up-Sweep
	int noItemsThatWork = 128;
	int offset = 1;
	for( d = 0; d<k; d++, noItemsThatWork >>= 1, offset <<= 1 )
	{
		if( LX < noItemsThatWork )
		{
			i = LX*( offset << 1 ) + offset - 1;
			localArray[ i + offset ] = localArray[ i ] + localArray[ i + offset ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}


	// Down-Sweep
	if( LX == 255 )
		localArray[ 255 ] = 0;
	noItemsThatWork = 1;
	offset = 128;
	for( d = 0; d<k; d++, noItemsThatWork <<= 1, offset >>= 1 )
	{
		if( LX < noItemsThatWork )
		{
			i = LX*( offset << 1 ) + offset - 1;
			int tmp = localArray[ i ];
			localArray[ i ] = localArray[ i + offset ];
			localArray[ i + offset ] = tmp + localArray[ i + offset ];
		}
		barrier( CLK_LOCAL_MEM_FENCE );
	}

	// write result to global memory
	out[ GX ] = localArray[ LX ];
}
*/