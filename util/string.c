/* Copyright (c) 2001, Stanford University
 * All rights reserved
 *
 * See the file LICENSE.txt for information on redistributing this software.
 */

#include "cr_mem.h"
#include "cr_string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int crStrlen( const char *str )
{
	const char *temp;
	for (temp = str ; *temp ; temp++);
	return temp-str;
}

char *crStrdup( const char *str )
{
	int len;
	char *ret;
	
	// Allow strdup'ing of NULL strings -- this makes the __fillin functions
	// much cleaner.
	
	if (str == NULL) return NULL;
	
	len = crStrlen(str);
	ret = crAlloc( len+1 );
	memcpy( ret, str, len );
	ret[len] = '\0';
	return ret;
}

char *crStrndup( const char *str, unsigned int len )
{
	char *ret = crAlloc( len+1 );
	memcpy( ret, str, len );
	ret[len] = '\0';
	return ret;
}

int crStrcmp( const char *str1, const char *str2 )
{
	while (*str1 && *str2)
	{
		if (*str1 != *str2)
		{
			break;
		}
		str1++; str2++;
	}
	return (*str1 - *str2);
}

int crStrncmp( const char *str1, const char *str2, int n )
{
	int i = 0;
	while (*str1 && *str2 && i < n)
	{
		if (*str1 != *str2)
		{
			break;
		}
		str1++; str2++; i++;
	}
	if (i == n) return 0;
	return (*str1 - *str2);
}

char lowercase[256] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007', 
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017', 
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027', 
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037', 
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047', 
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057', 
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067', 
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077', 
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147', 
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157', 
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167', 
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137', 
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147', 
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157', 
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167', 
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177', 
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207', 
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217', 
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227', 
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237', 
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247', 
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257', 
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267', 
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277', 
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307', 
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317', 
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327', 
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337', 
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347', 
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357', 
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367', 
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377' 
};

int crStrcasecmp( const char *str1, const char *str2 )
{
	while (*str1 && *str2)
	{
		if (lowercase[(int) *str1] != lowercase[(int) *str2])
		{
			break;
		}
		str1++; str2++;
	}
	return (lowercase[(int) *str1] - lowercase[(int) *str2]);
}

void crStrcpy( char *dest, const char *src )
{
	int len = crStrlen(src);
	memcpy( dest, src, len );
	dest[len] = '\0';
}

void crStrncpy( char *dest, const char *src, unsigned int len )
{
	unsigned int str_len = crStrlen(src);
	unsigned int copy_len = str_len;
	if (copy_len > len - 1)
		copy_len = len;
	memcpy( dest, src, copy_len );
	dest[len] = '\0';
}

void crStrcat( char *dest, const char *src )
{
	crStrcpy( dest + crStrlen(dest), src );
}

char *crStrstr( const char *str, const char *pat )
{
	int pat_len = crStrlen( pat );
	const char *end = str + crStrlen(str) - pat_len;
	char first_char = *pat;
	for (; str <= end ; str++)
	{
		if (*str == first_char && !memcmp( str, pat, pat_len ))
			return (char *) str;
	}
	return NULL;
}

char *crStrchr( const char *str, char c )
{
	for ( ; *str ; str++ )
	{
		if (*str == c)
			return (char *) str;
	}
	return NULL;
}

char *crStrrchr( const char *str, char c )
{
	const char *temp = str + crStrlen( str );
	for ( ; temp >= str ; temp-- )
	{
		if (*temp == c)
			return (char *) temp;
	}
	return NULL;
}

// These functions are from the old wiregl net.c -- hexdumps?  Not sure quite yet.
	
void crBytesToString( char *string, int nstring, void *data, int ndata )
{
	int i, offset;
	unsigned char *udata;

	offset = 0;
	udata = (unsigned char *) data;
	for ( i = 0; i < ndata && ( offset + 4 <= nstring ); i++ ) 
	{
		offset += sprintf( string + offset, "%02x ", udata[i] );
	}

	if ( i == ndata && offset > 0 )
		string[offset-1] = '\0';
	else
		strcpy( string + offset - 3, "..." );
}

void crWordsToString( char *string, int nstring, void *data, int ndata )
{
	int i, offset, ellipsis;
	unsigned int *udata;

	/* turn byte count into word count */
	ndata /= 4;

	/* we need an ellipsis if all the words won't fit in the string */
	ellipsis = ( ndata * 9 > nstring );
	if ( ellipsis )
	{
		ndata = nstring / 9;

		/* if the ellipsis won't fit then print one less word */
		if ( ndata * 9 + 3 > nstring )
			ndata--;
	}
		
	offset = 0;
	udata = (unsigned int *) data;
	for ( i = 0; i < ndata; i++ ) 
	{
		offset += sprintf( string + offset, "%08x ", udata[i] );
	}

	if ( ellipsis )
		strcpy( string + offset, "..." );
	else if ( offset > 0 )
		string[offset-1] = 0;
}

int crStrToInt( const char *str )
{
	return atoi(str);
}

double crStrToFloat( const char *str )
{
	return atof(str);
}

static int __numOccurrences( const char *str, const char *substr )
{
	int ret = 0;
	char *temp = (char *) str;
	while ((temp = crStrstr( temp, substr )) != NULL )
	{
		temp += crStrlen(substr);
		ret++;
	}
	return ret;
}

char **crStrSplit( const char *str, const char *splitstr )
{
	char  *temp = (char *) str;
	int num_args = __numOccurrences( str, splitstr ) + 1;
	char **faked_argv = (char **) crAlloc( (num_args + 1)*sizeof( *faked_argv ) );
	int i;

	for (i = 0 ; i < num_args ; i++)
	{
		char *end;
		end = crStrstr( temp, splitstr );
		if (!end)
			end = temp + crStrlen( temp );
		faked_argv[i] = crStrndup( temp, end-temp );
		temp = end + crStrlen(splitstr);
	}
	faked_argv[num_args] = NULL;
	return faked_argv;
}

char **crStrSplitn( const char *str, const char *splitstr, int n )
{
	char **faked_argv;
	int i;
	char *temp = (char *) str;
	int num_args = __numOccurrences( str, splitstr );

	if (num_args > n)
		num_args = n;
	num_args++;

	faked_argv = (char **) crAlloc( (num_args + 1) * sizeof( *faked_argv ) );
	for (i = 0 ; i < num_args ; i++)
	{
		char *end;
		end = crStrstr( temp, splitstr );
		if (!end || i == num_args - 1 )
			end = temp + crStrlen( temp );
		faked_argv[i] = crStrndup( temp, end-temp );
		temp = end + crStrlen(splitstr);
	}
	faked_argv[num_args] = NULL;
	return faked_argv;
}
