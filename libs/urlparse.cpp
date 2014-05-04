#include <iostream>
#include "urlparse.hpp"

using namespace std;

// Parses the passed URL into an ParseResult object and returns it.
ParseResult UrlParse::parse(string url)
{
    ParseResult parsed_url;
    if(url.empty())
    {
        return parsed_url;
    }

    if(url.find("://") != string::npos)
    {
        parsed_url.scheme = getScheme(url);
    }

    if(!url.empty())
    {
        parsed_url.netloc = getNetloc(url);
    }

    if(!url.empty() && url.substr(0, 1).compare("/") == 0)
    {
        parsed_url.path = getPath(url);
    }

    if(!url.empty() && url.substr(0, 1).compare("?") == 0)
    {
        parsed_url.query = getQuery(url);
    }

    if(!url.empty() && url.substr(0, 1).compare("#") == 0)
    {
        parsed_url.fragment = getFragment(url);
    }

    return parsed_url;
}

string UrlParse::getUrl(ParseResult parsed_url)
{
    string url;
    if(!parsed_url.scheme.empty())
    {
        url += parsed_url.scheme;
        url += "://";
    }
    if(!parsed_url.netloc.empty())
    {
        url += parsed_url.netloc;
    }

    if(!parsed_url.path.empty())
    {
        url += parsed_url.path;
    }

    if(!parsed_url.query.empty())
    {
        url += "?";
        url += parsed_url.query;
    }

    if(!parsed_url.fragment.empty())
    {
        url += "#";
        url += parsed_url.fragment;
    }
    return url;
}

/*  Joins an URL, this method handles links just like in your webbrowser.
    This means a relative path will get translated correctly to it's absolute
    location. */
string UrlParse::join(string url, string location)
{
    // Skip invalid strings
    if(location.empty() && !url.empty())
    {
        return url;
    }
    else if(location.empty() && url.empty())
    {
        return "";
    }

    /* Remove a trailing slash from the passed URL, this is required to
       be able to step correctly, if we do not remove it we are off by one
       step, if the slash exists that is. */
    bool ends_with_slash = url.substr(url.length()).compare("/") == 0;
    bool contains_step = location.substr(0, 2).compare("..") == 0;
    if(ends_with_slash && contains_step)
    {
        url = url.substr(0, url.length() - 1);
    }

    /* Parse the URL and remove query and fragment, the reason for that is that
       we no longer will need the two values, the new URL will not have the same
       attributes as the current ones and thus delete them. */
    ParseResult parsed = parse(url);
    parsed.query = "";
    parsed.fragment = "";

    if(location.substr(0, 1).compare("/") == 0)
    {
        /* When the passed location starts with '/' the destination will always
           be relative to the root URL, thus a simple removal of the old path
           will suffice. */
        parsed.path = location;
    }
    else if(location.substr(0, 2).compare("./") == 0)
    {
        // For locations starting with "./"
        parsed.path = location.substr(1);
    }
    else if(location.substr(0, 2).compare("..") != 0)
    {
        // For locations starting without step, or any of the root paths
        normalJoin(parsed, location);
    }
    else if(location.substr(0, 2).compare("..") == 0)
    {
        // For locations which are relative, ergo starting with "../"
        stepRelative(parsed, location);
    }

    return getUrl(parsed);
}

// Splits a passed URL at the fragment identifier into two strings.
void UrlParse::defrag(string url, string &new_url, string &fragment)
{
    ParseResult url_object;
    url_object.scheme = getScheme(url);
    url_object.netloc = getNetloc(url);
    url_object.path = getPath(url);
    url_object.query = getQuery(url);
    new_url = getUrl(url_object);
    fragment = getFragment(url);
}

/*
   ============================================================================
   ======================= End of public methods ==============================
   ============================================================================
*/

// Joins the referenced location with the parsed object's path
void UrlParse::normalJoin(ParseResult &parsed, string &location)
{
    int pos = parsed.path.rfind("/");
    // When the passed URL's path is empty
    if(pos == string::npos)
    {
        /* Since the passed URL's path is empty we need to check if the
           referenced location contains a leading slash or not and add it if
           needed. */
        if(location.substr(0, 1).compare("/") != 0)
        {
            location = "/" + location;
        }
        parsed.path = parsed.path.substr(0, pos) + location;
    }
    else
    {
        // TODO: Figure out what the fuck I was doing here
        string new_path = parsed.path.substr(0, pos);
        if(new_path.substr(new_path.length()).compare("/") != 0)
        {
            new_path += "/";
        }

        parsed.path = new_path + location;
    }
}

void UrlParse::stepRelative(ParseResult &parsed, string &location)
{
    while(location.substr(0, 2).compare("..") == 0)
    {
        if(parsed.path.compare("/") == 0)
        {
            /* When the parsed path (the passed URL's path) is either empty or
               consists of the root path, we remove the "overflowing" steps
               which remain in the passed location. */
            while(location.substr(0, 2).compare("..") == 0)
            {
                location = location.substr(3);
            }
            break;
        }

        /* Find the two last occuring slashes and remove it from the URL,
           for example:
               http://evilzone.org/level1/level2/level3/page.html
               will result in, http://evilzone.org/level1/level2       */
        for(int i = 0; i < 2; i++)
        {
            int pos = parsed.path.rfind("/");
            if(pos == string::npos)
            {
                break;
            }
            parsed.path = parsed.path.substr(0, pos);
        }

        // Remove the step (../) from the location
        location = location.substr(3);
    }

    if(location.substr(0, 1) != "/")
    {
        location = "/" + location;
    }

    // The fragment and query of the URL are now obsolete, thus they are removed
    parsed.path = location;
}

// Returns the scheme of the passed URL if any
string UrlParse::getScheme(string &url)
{
    int pos = url.find("://");
    /* When the returned position is bigger than the string itself then the sub-
       string was not found. */
    if(pos == string::npos)
        return "";
    string scheme = url.substr(0, pos);
    // Remove the extracted scheme from the passed URL (:// will be discarded)
    url = url.substr(pos + 3);
    return scheme;
}

// Returns the netloc of the passed URL if any
string UrlParse::getNetloc(string &url)
{
    // <scheme> :// <netloc>     <path>             <query>  <fragment>
    // http     :// evilzone.org /path/to/page.html ?query=1 #fragment
    // http://evilzone.org/path/to/page.html?query=1#fragment

    // i = index of ...
    int ipath = url.find("/");
    int iquery = url.find("?");
    int ifragment = url.find("#");
    string netloc = "";
    if(ipath == string::npos && ifragment == string::npos && iquery == string::npos)
    {
        /*  When the passed URL isn't empty yet and the above markers aren't
            present and we assume the rest of the URL is the netloc. */
        if(!url.empty())
        {
            netloc = url;
            url = "";
        }
    }
    else if(ipath != string::npos)
    {
        // When the fragment marker is present but no query marker could be found
        netloc = url.substr(0, ipath);
        url = url.substr(ipath);
    }
    else if(ifragment != string::npos)
    {
        netloc = url.substr(0, ifragment);
        url = url.substr(ifragment);
    }

    return netloc;
}

// Returns the path of the passed URL if any
string UrlParse::getPath(string &url)
{
    if(url.empty())
    {
        return "";
    }
    else if(url.compare("/") == 0)
    {
        url = "";
        return "/";
    }

    /* Find the position of the query marker, the query marker will be the end
       of the path. */
    int iquery = url.find("?");
    int ifragment = url.find("#");
    string path = "";
    if(iquery == string::npos && ifragment == string::npos)
    {
        path = url;
        url = "";
    }
    else if(ifragment != string::npos && iquery == string::npos)
    {
        // When the fragment marker is present but the query marker is missing
        path = url.substr(0, ifragment);
        url = url.substr(ifragment);
    }
    else
    {
        path = url.substr(0, iquery);
        url = url.substr(iquery);
    }

    /* Removes possible duplicates of slashes, this occurs sometimes when a
       passed link had them already in them, this loop gets rid of them. */
    int dslash;
    while((dslash = path.find("//")) != string::npos)
    {
        string sub1 = path.substr(0, dslash);
        string sub2 = path.substr(dslash + 1);
        path = sub1 + sub2;
    }

    return path;
}

// Returns the query part of the passed URL if any
string UrlParse::getQuery(string &url)
{
    int ifragment = url.find("#");
    string query = "";
    if(ifragment == string::npos)
    {
        /*  When the URL doesn't contain the fragment marker (#) it consists only
            of the query. */
        query = url;
        url = "";
    }
    else
    {
        query = url.substr(0, ifragment);
        url = url.substr(ifragment);
    }

    // Remove the query marker (?)
    if(url.substr(0, 1).compare("?") == 0)
    {
        url = url.substr(1);
    }
    else if(query.substr(0, 1).compare("?") == 0)
    {
        query = query.substr(1);
    }

    return query;
}

// Returns the fragment of the passed URL if any
string UrlParse::getFragment(string &url)
{
    int pos = url.find("#");
    if(url.empty() || pos == string::npos)
    {
        return "";
    }
    string fragment = url.substr(pos + 1);
    return fragment;
}

