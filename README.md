Urlparse
========
This is a lib for C++ inspired from Python's urlparse lib which you can check
out [here](https://docs.python.org/2/library/urlparse.html). The lib contains
multiple methods to parse and modify various types of URLs, see a complete
overview of the methods which are available to you under the [usage section](#usage).
If you are familiar with the urlparse lib from python this shouldn't take you
long to comprehend.

Just in case you don't know how to run this example program, read the
[compile](#compile) section and you are ready to see the library in action.

If you encounter any bugs or have feedback to give feel free to do so, contact
information can be found in the [feedback](#feedback) section.

Usage
=====

Here's a quick overview of the methods available to you:

| Method |
| :---------------|
| [UrlParse::parse](#parseresult-urlparseparsestring-url) |
| [UrlParse::getUrl](#string-urlparsegeturlparseresult-parsed_url) |
| [ParseResult::getUrl](#string-urlparsegeturlparseresult-parsed_url) |
| [UrlParse::join](#string-urlparsejoinstring-url-string-location)|
| [UrlParse::defrag](#void-urlparsedefragstring-url-string-new_url-string-fragment)|



### **ParseResult UrlParse::parse(string url)**

Returns a ParseResult object, it contains multiple parts, the names of these
parts are: scheme, netloc, path, query and fragment.
An example program:


```C++
#include <iostream>
#include "libs/urlparse.hpp"

using namespace std;

int main(void)
{
    // The URL we want to split into pieces
    string url = "https://evilzone.org/le/path/page.php?id=1&evil=1#important";
    UrlParse up;
    ParseResult parsed_url = up.parse(url);

    // Print out all the parts
    cout << "Scheme: " << parsed_url.scheme << endl
         << "Netloc: " << parsed_url.netloc << endl
         << "Path: " << parsed_url.path << endl
         << "Query: " << parsed_url.query << endl
         << "Fragment: " << parsed_url.fragment << endl << endl;

    return 0;
}

```

The output:

```
Scheme: https
Netloc: evilzone.org
Path: /le/path/page.php
Query: id=1&evil=1
Fragment: important
```

--

### **string UrlParse::getUrl(ParseResult parsed_url)**
(and **string ParseResult::getUrl()**)

Expands the parsed URL, an example of it's usage:

``` C++
#include <iostream>
#include "libs/urlparse.hpp"

using namespace std;

int main(void)
{
    string url = "https://evilzone.org/le/path/page.php?id=1&evil=1#important";
    UrlParse up;

    // Our parsed URL we want to expand
    ParseResult parsed_url = up.parse(url);

    // Obtain the URL from the passed ParseResult object
    // UrlParse::getUrl(ParseResult parsed_url)
    cout << up.getUrl(parsed_url) << endl;

    // Or obtain it directly from the object itself
    // ParseResult::getUrl()
    cout << parsed_url.getUrl() << endl;

    return 0;
}
```

The output:

```
https://evilzone.org/le/path/page.php?id=1&evil=1#important
https://evilzone.org/le/path/page.php?id=1&evil=1#important
```

--

### **string UrlParse::join(string url, string location)**
This method joins an URL with the specified location, an example of it looks like this:

``` C++
#include <iostream>
#include "libs/urlparse.hpp"

using namespace std;

int main(void)
{
    // Our URL and location we want to join
    string url = "https://evilzone.org/level1/level2/page.php";
    string location = "../level3/page.php";

    UrlParse up;
    cout << up.join(url, location) << endl;

    return 0;
}
```

Output:

```
https://evilzone.org/level1/level3/page.php
```

--


### **void UrlParse::defrag(string url, string &new_url, string &fragment)**

Splits the URL at the fragment indicator (#) into two strings,
new_url and fragment.
Example program:

```C++
#include <iostream>
#include "libs/urlparse.hpp"

using namespace std;

int main(void)
{
    string url = "https://evilzone.org/page.php?evil=1#overview";
    UrlParse up;

    string new_url;
    string fragment;
    up.defrag(url, new_url, fragment);

    cout << "Our URL without fragment: " << new_url << endl;
    cout << "The removed fragment: " << fragment << endl;

    return 0;
}

```

Output of the program:

```
    Our URL without fragment: https://evilzone.org/page.php?evil=1
    The removed fragment: overview
```
--


Compile
=======
To compile the example program simply run the included build.sh script.



Feedback
========
Feedback and critique is always welcome, find my email address on my github
profile located at https://github.com/Daapii
