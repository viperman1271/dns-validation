#include <CLI/CLI.hpp>

#include <chrono>

#ifdef _WINDOWS
#include <windows.h>
#include <windns.h>

#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
    CLI::App app("Command line application for querying dns records from a specific server");

    std::string server;
    std::string domain;
    app.add_option("-s,--server", server, "DNS server to use");
    app.add_option("-d,--domain", domain, "The domain to query");

    CLI11_PARSE(app, argc, argv);

    if (domain.empty())
    {
        std::cout << app.help() << std::endl;
        return -1;
    }
    else
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

        LocalFree(pSrvList);
        pSrvList = nullptr;
#endif // _WINDOWS
    }

    return 0;
}