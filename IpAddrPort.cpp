#include "IpAddrPort.h"
#include "Log.h"
#include "Util.h"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

using namespace std;

shared_ptr<addrinfo> getAddrInfo ( const string& addr, uint16_t port, bool isV4, bool passive )
{
    addrinfo addrConf, *addrRes = 0;
    ZeroMemory ( &addrConf, sizeof ( addrConf ) );

    addrConf.ai_family = ( isV4 ? AF_INET : AF_INET6 );

    if ( passive )
        addrConf.ai_flags = AI_PASSIVE;

    int error = getaddrinfo ( addr.empty() ? 0 : addr.c_str(), TO_C_STR ( port ), &addrConf, &addrRes );

    if ( error != 0 )
    {
        LOG ( "getaddrinfo failed: %s", getWindowsErrorAsString ( error ).c_str() );
        throw "something"; // TODO
    }

    return shared_ptr<addrinfo> ( addrRes, freeaddrinfo );
}

string getAddrFromSockAddr ( const sockaddr_storage& sa )
{
    char addr[INET6_ADDRSTRLEN];

    if ( sa.ss_family == AF_INET )
        inet_ntop ( sa.ss_family, & ( ( ( sockaddr_in * ) &sa )->sin_addr ), addr, sizeof ( addr ) );
    else
        inet_ntop ( sa.ss_family, & ( ( ( sockaddr_in6 * ) &sa )->sin6_addr ), addr, sizeof ( addr ) );

    return addr;
}

uint16_t getPortFromSockAddr ( const sockaddr_storage& sa )
{
    if ( sa.ss_family == AF_INET )
        return ntohs ( ( ( sockaddr_in * ) &sa )->sin_port );
    else
        return ntohs ( ( ( sockaddr_in6 * ) &sa )->sin6_port );
}

const char *inet_ntop ( int af, const void *src, char *dst, size_t size )
{
    if ( af == AF_INET )
    {
        sockaddr_in in;
        memset ( &in, 0, sizeof ( in ) );
        in.sin_family = AF_INET;
        memcpy ( &in.sin_addr, src, sizeof ( in_addr ) );
        getnameinfo ( ( sockaddr * ) &in, sizeof ( sockaddr_in ), dst, size, 0, 0, NI_NUMERICHOST );
        return dst;
    }
    else if ( af == AF_INET6 )
    {
        sockaddr_in6 in;
        memset ( &in, 0, sizeof ( in ) );
        in.sin6_family = AF_INET6;
        memcpy ( &in.sin6_addr, src, sizeof ( in_addr6 ) );
        getnameinfo ( ( sockaddr * ) &in, sizeof ( sockaddr_in6 ), dst, size, 0, 0, NI_NUMERICHOST );
        return dst;
    }

    return 0;
}

IpAddrPort::IpAddrPort ( bool isV4 )
    : port ( 0 ), addrInfo ( getAddrInfo( "", 0, isV4 ) ) {}

IpAddrPort::IpAddrPort ( const string& addr, uint16_t port, bool isV4 )
    : addr ( addr ), port ( port ), addrInfo ( getAddrInfo ( addr, port, isV4 ) ) {}

IpAddrPort::IpAddrPort ( const sockaddr_storage& sa )
    : addr ( getAddrFromSockAddr ( sa ) ), port ( getPortFromSockAddr ( sa ) )
{
    updateAddrInfo ( sa.ss_family == AF_INET );
}

shared_ptr<addrinfo>& IpAddrPort::updateAddrInfo ( bool isV4 )
{
    return ( addrInfo = getAddrInfo ( addr, port, isV4 ) );
}