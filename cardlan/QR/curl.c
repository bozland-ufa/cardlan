
#include "../include/curl/curl.h"


size_t writefunc(void* ptr, size_t size, size_t nmemb, char** s)
{
    *s = (char *)malloc(size * nmemb + 1);
    *s[0] = 0;
    strncat(*s,(char*)ptr, size * nmemb);
    
    return size * nmemb;
}


int curl(void)
{
    CURL* curl;
    CURLcode res;

    char device[255];
    char qrcode[255];
    char sign[255];
    char jsonpost[1024];
    char* s;

    curl_global_init(CURL_GLOBAL_ALL);
       
    curl = curl_easy_init();
    if (curl) {

        strcpy(device, "1158bfa576dcb0c31dcf883fe82b5bdf");
        strcpy(qrcode, "ticket-1-83fe82b5bdf1158bfa576dcb0c31dcf8");
        strcpy(sign, "d16bb090be5e4c13ece93793f173bfa4");
        sprintf(jsonpost, "{\"action\":\"hardware-controller\\/qr-code-check\",\"device\":\"%s\",\"qrcode\":\"%s\",\"signature\":\"%s\"}", device, qrcode, sign);

        curl_easy_setopt(curl, CURLOPT_URL, "http://tickets.icom.uz/api");
        
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"action\":\"hardware-controller\\/qr-code-check\",\"device\":\"1158bfa576dcb0c31dcf883fe82b5bdf\",\"qrcode\":\"ticket-1-83fe82b5bdf1158bfa576dcb0c31dcf8\",\"signature\":\"d16bb090be5e4c13ece93793f173bfa4\"}");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonpost);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        
        curl_easy_cleanup(curl);

        free(s);
    }

    curl_global_cleanup();
    
    return 0;
}
