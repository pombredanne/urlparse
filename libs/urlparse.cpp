#include <iostream>
#include "urlparse.hpp"

using namespace std;

// Parses the passed URL into an ParseResult object and returns it.
ParseResult UrlParse::parse(string url)
{
    ParseResult parsed_url;
    /* Grab *part* from URL, store it, remove it from the passed URL and then
       return the extracted part - repeat. */
    parsed_url.scheme = getScheme(url);
    parsed_url.netloc = getNetloc(url);
    parsed_url.path = getPath(url);
    parsed_url.query = getQuery(url);
    parsed_url.fragment = getFragment(url);
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

    url += parsed_url.netloc;
    url += parsed_url.path;

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

    ParseResult parsed = parse(url);

    // Handles root path URLs, either locations starting with ./ or /
    if(location.substr(0, 1).compare("/") == 0)
    {
        /* When the passed location starts with '/' the destination will always
           be relative to the root URL. */
        string new_url = parsed.scheme + "://" + parsed.netloc + location;
        return new_url;
    }
    else if(location.substr(0, 2).compare("./") == 0)
    {
        /* For some reason some webdevelopers use the current directory indicator
           (.) in front of the root path, this parses the path accordingly. */
        string new_url = parsed.scheme + "://" + parsed.netloc + location.substr(1);
        return new_url;
    }

    // Handle locations which do not start with root and do not start with step
    if(location.substr(0, 2).compare("..") != 0)
    {
        int pos = parsed.path.rfind("/");
        // WHen the passed URL's path is empty
        if(pos == -1 || pos > parsed.path.length())
        {
            parsed.fragment = "";
            parsed.query = "";
            if(location.substr(0, 1).compare("/") != 0)
            {
                location = "/" + location;
            }
            parsed.path = parsed.path.substr(0, pos) + location;
            return parsed.getUrl();
        }

        string new_path = parsed.path.substr(0, pos);
        if(new_path.substr(new_path.length()).compare("/") != 0)
        {
            new_path += "/";
        }

        new_path += location;

        parsed.fragment = "";
        parsed.query = "";
        parsed.path = new_path;
        return parsed.getUrl();
    }


    /* Handle relative stepping locations:
       For each step (../ or ..\) we remove one level inside the URL, repeat
       the process of removing one step after the other will leave us with
       the level we want to access with the passed path. */
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
            if(pos == -1 || parsed.path.empty())
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
    parsed.fragment = "";
    parsed.query = "";
    return getUrl(parsed) + location;
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


// Returns the scheme of the passed URL if any
string UrlParse::getScheme(string &url)
{
    int pos = url.find("://");
    /* When the returned position is bigger than the string itself then the sub-
       string was not found. */
    if(pos > url.length() || pos == -1)
        return "";
    string scheme = url.substr(0, pos);
    // Remove the extracted scheme from the passed URL (:// will be discarded)
    url = url.substr(pos + 3);
    return scheme;
}

// Returns the netloc of the passed URL if any
string UrlParse::getNetloc(string &url)
{
    int pos = url.find("/");
    if(pos > url.length() || pos == -1)
    {
        if(!url.empty())
        {
            string netloc = url;
            url = "";
            return netloc;
        }
        return "";
    }
    string netloc = url.substr(0, pos);
    // remove the netloc from the passed URL
    url = url.substr(pos);
    return netloc;
}

// Returns the path of the passed URL if any
string UrlParse::getPath(string &url)
{
    if(url.compare("/") == 0)
    {
        url = "";
        return "/";
    }

    /* Find the position of the query marker, the query marker will be the end
       of the path. */
    int pos = url.find("?");
    if(pos == -1)
    {
        string path = url;
        url = "";
        return path;
    }
    string path = url.substr(0, pos);
    url = url.substr(pos);
    return path;
}

// Returns the query part of the passed URL if any
string UrlParse::getQuery(string &url)
{
    // Find the index of a fragment
    int pos = url.find("#");
    if(url.empty())
    {
        return "";
    }
    else if(pos == -1)
    {
        /* When the URL doesn't contain the fragment marker (#) the url only
           consists of the query. */

        // Remove the query marker (?)
        if(url.substr(0, 1).compare("?") == 0)
        {
            url = url.substr(1);
        }
        return url;
    }
    // Remove the query marker (?)
    if(url.substr(0, 1).compare("?") == 0)
    {
        url = url.substr(1);
    }
    string query = url.substr(0, pos - 1);
    url = url.substr(pos - 1);
    return query;
}

// Returns the fragment of the passed URL if any
string UrlParse::getFragment(string &url)
{
    int pos = url.find("#");
    if(url.empty() || pos == -1)
    {
        return "";
    }
    string fragment = url.substr(pos + 1);
    return fragment;
}

