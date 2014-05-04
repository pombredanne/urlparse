#include <string>

#ifndef PARSERESULT_H
#define PARSERESULT_H

class ParseResult
{
public:
    ParseResult();
    std::string getUrl(); // Reassembles the parsed URL
    // Example URL: http://evilzone.org/example.php?page=1#important
    std::string scheme; // http
    std::string netloc; // evilzone.org
    std::string path; // /example.php
    std::string query; // page=1
    std::string fragment; // important
};

#endif

#ifndef URLPARSE_H
#define URLPARSE_H

class UrlParse
{
public:
    ParseResult parse(std::string url);
    static std::string getUrl(ParseResult parsed_url);
    std::string join(std::string url, std::string path);
    void defrag(std::string url, std::string &new_url, std::string &fragment);

private:
    void normalJoin(ParseResult &parsed, std::string &location);
    void stepRelative(ParseResult &parsed, std::string &location);
    std::string getScheme(std::string &url);
    std::string getNetloc(std::string &url);
    std::string getPath(std::string &url);
    std::string getQuery(std::string &url);
    std::string getFragment(std::string &url);


};

#endif


