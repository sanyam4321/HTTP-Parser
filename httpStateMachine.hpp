#ifndef HTTP_DFA
#define HTTP_DFA

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

enum ParserStates{
    A,
    B,
    C,
    D,
    E,
    Z
};

enum HttpElements{
    METHOD,
    PATH,
    PROTOCOL_VERSION,
    HOST,
    USER_AGENT,
    ACCEPT_ENCODING,
    ACCEPT_CHARSET,
    CONTENT_LENGTH,
    AUTHORIZATION,
    REFERER,
    COOKIE,
    CONNECTION,
    CACHE_CONTROL,
    ACCEPT,
    ACCEPT_LANGUAGE,
    CONTENT_LANGUAGE,
    CONTENT_TYPE,
    RANGE
};

enum TokenizationStates{
    KEY,
    VALUE
};

class HttpRequest{
    public:
    HttpRequest(){
        this->tempBuffer = "";
        this->keyOrValue = KEY;
        this->lineIndex = 0;
        this->currentElement = METHOD;
        this->contentLength = 0;
        this->Body = "";
        this->BodySize = 0;
    }
    ~HttpRequest(){

    }

    void appendToHeaders(const char ch){
        switch(this->lineIndex){
            case 0:
                // std::cout<<"append to request line"<<std::endl;
                break;
            default:
                if(ch == ' '){
                    if(this->tempBuffer == "Host"){
                        this->currentElement = HOST;
                    }
                    else if(this->tempBuffer == "User-Agent"){
                        this->currentElement = USER_AGENT;
                    }
                    else if(this->tempBuffer == "Accept"){
                        this->currentElement = ACCEPT;
                    }
                    else if(this->tempBuffer == "Accept-Encoding"){
                        this->currentElement = ACCEPT_ENCODING;
                    }
                    else if(this->tempBuffer == "Authorization"){
                        this->currentElement = AUTHORIZATION;
                    }
                    else if(this->tempBuffer == "Referer"){
                        this->currentElement = REFERER;
                    }
                    else if(this->tempBuffer == "Cookie"){
                        this->currentElement = COOKIE;
                    }
                    else if(this->tempBuffer == "Content-Type"){
                        this->currentElement = CONTENT_TYPE;
                    }
                    else if(this->tempBuffer == "Connection"){
                        this->currentElement = CONNECTION;
                    }
                    else if(this->tempBuffer == "Cache-Control"){
                        this->currentElement = CACHE_CONTROL;
                    }
                    else if(this->tempBuffer == "Accept-Charset"){
                        this->currentElement = ACCEPT_CHARSET;
                    }
                    else if(this->tempBuffer == "Accept-Language"){
                        this->currentElement = ACCEPT_LANGUAGE;
                    }
                    else if(this->tempBuffer == "Content-Length"){
                        this->currentElement = CONTENT_LENGTH;
                    }
                    else if(this->tempBuffer == "Content-Language"){
                        this->currentElement = CONTENT_LANGUAGE;
                    }
                    else if(this->tempBuffer == "Range"){
                        this->currentElement = RANGE;
                    }
                    else{
                        std::cout<<"Error In Header"<<std::endl;
                    }
                    // CLEAR THE BUFFER
                    this->tempBuffer = "";
                }
                else if(ch == ':'){
                    this->keyOrValue = VALUE;
                }
                else{
                    this->tempBuffer += ch;
                }
                break;
        }

    }
    void processHeader(){
        this->request[this->currentElement] = this->tempBuffer;
        if(this->currentElement == CONTENT_LENGTH){
            this->contentLength = stoi(this->tempBuffer);
        }
        // setup for next headers
        this->tempBuffer = "";
        this->keyOrValue = KEY;
        this->lineIndex++;
        return;
    }

    void IterateRequest(){
        std::cout<<"\nHEADERS\n";
        for(const auto &it: this->request){
            std::cout<<it.second<<std::endl;
        }
        std::cout<<"BODY\n";
        for(const auto &it: this->Body){
            std::cout<<it;
        }
    }

    bool searchForHeaders(HttpElements header){
        if(this->request.find(header) == this->request.end()){
            return false;
        }
        return true;
    }
    bool appendBody(const char ch){
        this->Body += ch;
        this->BodySize += sizeof(ch);
        if(this->BodySize < this->contentLength){
            return false;
        }
        return true;
    }

    private:
    int lineIndex;
    std::unordered_map<HttpElements, std::string> request;
    std::string tempBuffer;
    std::string Body;
    long long BodySize;
    TokenizationStates keyOrValue;
    HttpElements currentElement;
    long long contentLength;
};


class HttpStateMachine{
    public:
    HttpStateMachine(){
        this->currentState = A;
        this->isComplete = false;
        this->requestMap = new HttpRequest();
    }
    ~HttpStateMachine(){
        delete this->requestMap;
    }
    void stateMachine(const char ch){
        switch (this->currentState){
        case A:
            if(ch == '\n'){
                this->currentState = Z;
            }
            else if(ch == '\r'){
                this->currentState = B;
            }
            else{
                this->requestMap->appendToHeaders(ch);
            }
            break;
        case B:
            if(ch == '\n'){
                this->currentState = C;
                this->requestMap->processHeader();
            }
            else{
                this->currentState = Z;
            }
            break;
        case C:
            if(ch == '\r'){
                this->currentState = D;
            }
            else if(ch == '\n'){
                this->currentState = Z;
            }
            else{
                this->currentState = A;
                this->requestMap->appendToHeaders(ch);
            }
            break;
        case D:
            if(ch == '\n'){
                this->currentState = E;  /*To final State*/
            }
            else{
                this->currentState = Z;
            }
            break;
        case E:
            if(this->isComplete == false){
                this->isComplete = this->requestMap->appendBody(ch);
            }
            break;
        case Z:
            std::cout<<"incorrect HTTP message"<<std::endl;
            break;
        default:
            std::cout<<"Invalid state"<<std::endl;
            this->currentState = Z;
            break;
        }
    }
    int parseHttp(std::string request){
        for(const char ch: request){
            std::cout<<ch;
            stateMachine(ch);
            if(this->currentState == Z){
                return -1;
            }
            else if(this->currentState == E){
                if(this->requestMap->searchForHeaders(CONTENT_LENGTH) == false || this->isComplete == true){
                    this->requestMap->IterateRequest();
                    return 1;
                }
            }
        }
        return 0;
    }

    private:
    ParserStates currentState;
    bool isComplete;
    HttpRequest *requestMap;
    std::string Body;
};



#endif