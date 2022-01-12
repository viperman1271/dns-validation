#include <dns.h>

#include <chrono>
#include <iostream>

#ifdef _WINDOWS
#include <windows.h>
#include <windns.h>

#pragma comment(lib, "ws2_32.lib")
#endif

#if defined(__linux__)
#include <arpa/nameser.h>
#include <cstring>
#include <netinet/in.h>
#include <resolv.h>
#endif // __linux__

int dns_validation(const std::string& server, const std::string& domain)
{
#ifdef _WINDOWS
    PIP4_ARRAY pSrvList = reinterpret_cast<PIP4_ARRAY>(LocalAlloc(LPTR, sizeof(IP4_ARRAY)));
    PDNS_RECORD ppQueryResultsSet;

    pSrvList->AddrArray[0] = inet_addr(server.c_str());
    pSrvList->AddrCount = 1; 

    DNS_STATUS dnsQueryResult = DnsQuery(domain.c_str(), DNS_TYPE_A, DNS_QUERY_BYPASS_CACHE, pSrvList, &ppQueryResultsSet, nullptr);
    if (dnsQueryResult != 0)
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_t = std::chrono::system_clock::to_time_t(now);
        std::cerr << "[" << std::ctime(&now_t) << "] Unable to resolve " << domain << " using " << server << "[err: " << dnsQueryResult << "]" << std::endl;

        LocalFree(pSrvList);

        return -1;
    }
    else
    {
        std::cout << "Value for " << domain << " was successfully retrieved" << std::endl;
    }

    LocalFree(pSrvList);
    pSrvList = nullptr;
#elif defined(__linux__)
    unsigned char nsbuf[1024];
    memset(nsbuf, 0, sizeof(nsbuf));

    int rc = res_query(domain.c_str(), ns_c_any, ns_t_a, nsbuf, sizeof(nsbuf));
    if (rc == -1)
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_t = std::chrono::system_clock::to_time_t(now);
        std::cerr << "[" << std::ctime(&now_t) << "] Unable to resolve " << domain << " using " << server << std::endl;

        return -1;
    }
    else
    {
        std::cout << "Value for " << domain << " was successfully retrieved" /*<< " (" << nsbuf << ")"*/ << std::endl;
    }
#endif // _WINDOWS

    return 0;
}
