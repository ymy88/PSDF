#include "StdAfx.h"

#include <PSDFCore/Common/Common.h>

bool IsIntel()
{
    unsigned char buf[ 2 ];
    unsigned short n;

    n = 0x1234;
    memcpy( ( void * )&buf[0], &n, 2 );
    n = buf[ 1 ];
    n *= 256;
    n += buf[ 0 ];
    if( n == 0x1234 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MemReverse( void * pStart, unsigned int len )
{
    unsigned char buf[ 128 ];
    if( len == 0 )
    {
        return;
    }
    if( len > 128 )
    {
        return;
    }
    if( pStart == NULL )
    {
        return;
    }
    for( unsigned int i = 0; i < len; i++ )
    {
        memcpy( ( void * )( buf + len - i - 1 ), ( void * )( ( char * )pStart + i ), 1 );
    }
    memcpy( pStart, &buf[0], len );
}

void Intel2Sparc( unsigned int data )
{
    if( !IsIntel() )
    {
        MemReverse( &( data ), sizeof( ( data ) ) );
    }
}

wstring stringToWString( const string& str )
{
	if( str == "" )
	{
		return wstring( L"" );
	}
	int size = str.length() + 1;

	wchar_t * buf = new wchar_t[ size ];
	
	memset( buf, 0, size );
	string curLocal = setlocale( LC_CTYPE, "" );
#ifdef _WIN32
	unsigned tmp;
	mbstowcs_s( &tmp, buf, size, str.c_str(), str.length() );
#else
	mbstowcs( buf, str.c_str(), size );
#endif
	wstring strW( buf );
	delete [] buf;
	setlocale( LC_CTYPE, curLocal.c_str() );
	return strW;
}

string wstringToString( const wstring& wstr )
{
	if (wstr == L"")
	{
		return string("");
	}

	int size = 2 * wstr.length() + 1;
	char* buf = new char[size];
	memset(buf, 0, size);
	string curLocal = setlocale(LC_CTYPE, "");
#ifdef _WIN32
	unsigned tmp;
	wcstombs_s(&tmp, buf, size, wstr.c_str(), 2 * wstr.length());
#else
	wcstombs(buf, wstr.c_str(), size);
#endif
	string strA(buf);
	delete [] buf;

	setlocale(LC_CTYPE, curLocal.c_str());
	return strA;
}

void readXML( const string& filename, boost::property_tree::wptree& xml )
{
	locale oldLocale;
	locale utf8Locale(oldLocale, new boost::program_options::detail::utf8_codecvt_facet);

	try
	{
		boost::property_tree::xml_parser::read_xml(filename, xml, 0, utf8Locale);
	}
	catch(...)
	{
		throw 1;
	}
}