#include <dns.h>

#include <CLI/CLI.hpp>

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
        return dns_validation(server, domain);
    }

    return 0;
}
