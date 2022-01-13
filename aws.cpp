#include <dns.h>

#include <aws/lambda-runtime/runtime.h>
#include <json-c/json.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

aws::lambda_runtime::invocation_response aws_main(aws::lambda_runtime::invocation_request const& req);
std::vector<std::pair<std::string, std::string>> parse_json(const std::string& input);

aws::lambda_runtime::invocation_response aws_main(aws::lambda_runtime::invocation_request const& req)
{
     std::vector<std::pair<std::string, std::string>> serverDomainPairs = parse_json(req.payload);
 
     struct json_object* responseJsonArray = json_object_new_array();
     for (const std::pair<std::string, std::string>& serverDomainPair : serverDomainPairs)
     {
         const int result = dns_validation(serverDomainPair.first, serverDomainPair.second);
 
         struct json_object* responseJsonObject = json_object_new_object();
         json_object_object_add(responseJsonObject, "server", json_object_new_string(serverDomainPair.first.c_str()));
         json_object_object_add(responseJsonObject, "domain", json_object_new_string(serverDomainPair.second.c_str()));
         json_object_object_add(responseJsonObject, "result", json_object_new_boolean(result == 0));
 
         json_object_array_add(responseJsonArray, responseJsonObject);
     }
 
     return aws::lambda_runtime::invocation_response::success(json_object_to_json_string(responseJsonArray), "application/json");
}

std::vector<std::pair<std::string, std::string>> parse_json(const std::string& input)
{
    std::vector<std::pair<std::string, std::string>> returnVal;

    struct json_object* jobj = json_tokener_parse(input.c_str());

    if (json_object_is_type(jobj, json_type_array))
    {
        struct array_list* jarray = json_object_get_array(jobj);

        const size_t arraySize = array_list_length(jarray);
        for (int i = 0; i < arraySize; ++i)
        {
            struct json_object* jsonArrayObject = json_object_array_get_idx(jobj, i);

            std::string server;
            std::string domain;

            json_object_object_foreach(jsonArrayObject, key, val)
            {
                std::string keyStr = key;
                if (keyStr == "server")
                {
                    server = json_object_get_string(val);
                }
                else if (keyStr == "domain")
                {
                    domain = json_object_get_string(val);
                }
            }

            returnVal.push_back({ server, domain });
        }
    }

    return returnVal;
}

int main()
{
    aws::lambda_runtime::run_handler(aws_main);

    return 0;
}
