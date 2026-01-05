#include "licenseChecker.h"

int checkLicense(std::string license, std::string email) {
    if (license == "" || email == "") {
        return 0;
    }
    // add regex to make sure valid email (and maybe license)
    std::string json =
        "{\n"
        " \"license\": \"" + license + "\",\n"
        " \"email\": \"" + email + "\"\n"
        "}\n";

    CURL* curl;
    CURLcode result;
    long response = 0;

    result = curl_global_init(CURL_GLOBAL_ALL);
    if (result) {
        std::cout << result << "\n";
        return 0;
    }

    curl = curl_easy_init();
    curl_slist* header = NULL;
    if (curl) {
        header = curl_slist_append(header, "Content-Type: text/plain");

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.255/licenses");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        result = curl_easy_perform(curl);

        if (result == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response);
        }
        else {
            std::cout << result << "\n";
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    
    return static_cast<int>(response);
}