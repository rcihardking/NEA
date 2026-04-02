#include "licenseChecker.h"

int checkLicense(std::string license, std::string email) {
    if (license == "" || email == "") { //ensure license or email isnt empty
        return 0;
    }

    //format license and email into json
    std::string json =
        "{\n"
        " \"license\": \"" + license + "\",\n"
        " \"email\": \"" + email + "\"\n"
        "}\n";


    //setup libcurl to send json
    CURL* curl;
    CURLcode result;
    long response = 0;

    result = curl_global_init(CURL_GLOBAL_ALL);
    if (result) {
        return 0;
    }

    curl = curl_easy_init();
    curl_slist* header = NULL;
    if (curl) {
        header = curl_slist_append(header, "Content-Type: text/plain");

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.255/licenses"); //ip and application the webserver is hosted on
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        result = curl_easy_perform(curl);

        if (result == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response); //get response code
        }
        else {
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    
    return static_cast<int>(response);  //return response code from server
}

void licenseProcess() {
    while (true) {
        MessageBox(NULL, (LPCWSTR)L"Enter your license key into the console.", (LPCWSTR)L"License", NULL);
        std::string license;
        std::cin >> license;

        MessageBox(NULL, (LPCWSTR)L"Now enter your email into the console.", (LPCWSTR)L"License", NULL);
        std::string email;
        std::cin >> email;

        int respone = checkLicense(license, email);
        if (respone == 200) {   //server will send back 200 OK if response is valid
            MessageBox(NULL, (LPCWSTR)L"Valid license key and email", (LPCWSTR)L"License", NULL);
            return;
        }

        int msgboxID = MessageBox(NULL, (LPCWSTR)L"Invalid license key or email, try again?", (LPCWSTR)L"License", MB_ICONWARNING | MB_RETRYCANCEL | MB_DEFBUTTON2);
        if (msgboxID == 2) {    //user clicked cancel with invalid license, so stop program
            exit(0);
        }
    }
}