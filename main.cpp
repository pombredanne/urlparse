#include <iostream>
#include "libs/urlparse.hpp"

using namespace std;

int main(void)
{
    string url = "https://evilzone.org/page.php?id=1#important";
    UrlParse up;
    ParseResult parsed_url;
    parsed_url = up.parse(url);
    // Showing all the parts of the splitted URL
    cout << "All elements of the parsed URL: " << endl;
    cout << "Scheme: " << parsed_url.scheme << endl
         << "Netloc: " << parsed_url.netloc << endl
         << "Path: " << parsed_url.path << endl
         << "Query: " << parsed_url.query << endl
         << "Fragment: " << parsed_url.fragment << endl << endl;

    // Testing url joining
    string location = "../level2/page.html";
    cout << "Urljoining: " << url << " and " << location << endl;
    cout << up.join(url, location) << endl << endl;

    // Testing the defrag function, this seperates the url from the fragment
    string actual_url;
    string fragment;
    up.defrag(url, actual_url, fragment);
    cout << "Defrag method" << endl;
    cout << actual_url << " -> " << fragment << endl << endl;

    // Testing the reassembling (.getUrl) methods
    cout << "Get the full URL from the object by itself: " << endl
         << parsed_url.getUrl() << endl << endl;

    cout << "Full URL with the UrlParse::getUrl method:"
         << endl << up.getUrl(parsed_url) << endl << endl;
    return 0;
}
