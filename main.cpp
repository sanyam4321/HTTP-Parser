#include <iostream>
#include "httpStateMachine.hpp"

int main(){
    
    HttpStateMachine *parser = new HttpStateMachine();
    std::string sampleRequest = "POST /api/v1/resource HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/json\r";
    int statusCode = parser->parseHttp(sampleRequest);
    if(statusCode == -1){
        std::cout<<"Failed to parse Request Headers"<<std::endl;
    }
    else if(statusCode == 1){
        std::cout<<"Request Headers parsed successfully"<<std::endl;
    }

    sampleRequest = "\nContent-Length: 36";
    statusCode = parser->parseHttp(sampleRequest);
    if(statusCode == -1){
        std::cout<<"Failed to parse Request Headers"<<std::endl;
    }
    else if(statusCode == 1){
        std::cout<<"Request Headers parsed successfully"<<std::endl;
    }

    sampleRequest = "\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nReferer: https://developer.mozilla.org/testpage.html\r\nCache-Control: max-age=0\r";
    statusCode = parser->parseHttp(sampleRequest);
    if(statusCode == -1){
        std::cout<<"Failed to parse Request Headers"<<std::endl;
    }
    else if(statusCode == 1){
        std::cout<<"Request Headers parsed successfully"<<std::endl;
    }

    sampleRequest = "\n\r\n{'messa";
    statusCode = parser->parseHttp(sampleRequest);
    if(statusCode == -1){
        std::cout<<"Failed to parse Request Headers"<<std::endl;
    }
    else if(statusCode == 1){
        std::cout<<"\nRequest parsed successfully\n";
    }

    sampleRequest = "ge':'this is client request'}";
    statusCode = parser->parseHttp(sampleRequest);
    if(statusCode == -1){
        std::cout<<"Failed to parse Request Headers"<<std::endl;
    }
    else if(statusCode == 1){
        std::cout<<"\nRequest parsed successfully\n";
    }

    
    return 0;
}