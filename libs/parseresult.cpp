#include "urlparse.hpp"

using namespace std;

ParseResult::ParseResult() : scheme(""), netloc(""), path(""), query(""),
                             fragment("")
{
}

// Re-assembles the passed url_t object into it's original state, type string.
string ParseResult::getUrl()
{
    string original_url;
    if(!scheme.empty())
    {
        original_url += scheme;
        original_url += "://";
    }

    original_url += netloc;
    original_url += path;

    /* Need to check if the query is present in the parsed_url, because we need
       to insert a leading question mark before the query. */
    if(!query.empty())
    {
        original_url += "?";
        original_url += query;
    }

    // The same goes for the fragment..
    if(!fragment.empty())
    {
        original_url += "#";
        original_url += fragment;
    }
    return original_url;
}

