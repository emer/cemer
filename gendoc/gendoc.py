#!/usr/bin/env python
from cgi import FieldStorage
from BeautifulSoup import BeautifulSoup
from urllib import urlopen, quote_plus, unquote
import urllib2 
import re

# OpenGrok transparently redirects us to the correct line using a 302 when using ?def=  etc...
class CustomRedirectHandler(urllib2.HTTPRedirectHandler):
    def http_error_301(self, req, fp, code, msg, headers):
        result = urllib2.HTTPRedirectHandler.http_error_301(self, req, fp, code, msg, headers)
        result.status = code
        return result
    def http_error_302(self, req, fp, code, msg, headers):
        result = urllib2.HTTPRedirectHandler.http_error_302(self, req, fp, code, msg, headers)
        result.status = code
        return result

opener = urllib2.build_opener(CustomRedirectHandler())

# This intercepts content and rewrites hrefs so that they point back into the proxy
def intercept(url, prefix):
    f=open('/tmp/url.log','w');f.write(prefix + unquote(url));f.close()
    try:
        html = opener.open(urllib2.Request(prefix + unquote(url))).read()
    # What happens here is that it redirects the 302 but then gets a 404. But the url
    # that it got a 404 on is exactly the one we want to send the user to, so lets
    # snag it!
    except urllib2.HTTPError as gotcha:
        url = gotcha.geturl()
        output = redirect(url.split(':8080')[1])
        return output

    soup = BeautifulSoup(html)

    links = soup.findAll(re.compile('.*'), {'href':re.compile('.*')})
    if links is not None:
        for tag in links:
            if not 'http' in tag['href'] and not tag['href'][0] == '#':
                tag['href'] = 'http://grey.colorado.edu/gendoc.py?proxy=' + quote_plus(tag['href'].replace('?','QUESTION_MARK'))


    # There are various features of OpenGrok that this proxy does not support. Nuke them.
    form = soup.find('form',{'name':'sbox'})
    if form:
        form.extract()

    bar = soup.find('div', {'id':'bar'})
    if bar:
        bar.extract()

    header = soup.find('div',{'id':'header'})
    if header:
        header.extract()

    masthead = soup.find('div', {'id':'Masthead'})
    if masthead:
        masthead.extract()

    return str(soup)

# Urls that contain a # component need to be redirected back to this script
# so that the user gets sent to the correct line
def redirect(url):
    if '#' in url:
        url, rel = url.split('#')
        output = '<html><head><meta http-equiv="refresh" content="0; url=' + \
                 'http://grey.colorado.edu/gendoc.py?url=' + url + '#' + rel + '"/>'\
                 '</head><body></body></html>'
    return output

def application(environ, start_response):
    output = ''
    content_type = "text/html"
    prefix = 'http://grey.colorado.edu:8080'
    finished = False

    form = FieldStorage(fp=environ['wsgi.input'],
                        environ=environ)
    query = form.getfirst('q')
    url = form.getfirst('url')
    proxy = form.getfirst('proxy')

    # A query is a search, for example, for "void Blah::Blerg"
    if query is not None and url is None:

        """
        First we will try with +"query"
        """
        try:
            # Try the full signature, e.g., void BaseSpec::UpdateSubSpecs
            url = prefix + '/source/search?q=' + quote_plus('+"' + query + '"')
            soup = BeautifulSoup(urlopen(url).read())
            url = soup.find('tt',{'class':'con'}).a['href']
            if '#' in url:
                output = redirect(url)
            else:
                soup = BeautifulSoup(urlopen(prefix + url).read())
                url = soup.find('a',{'class':'s'})['href']
                output = redirect(url)
        except:
            try:
                """
                Then we will try to find an exact match by searching for the query without quotes but
                getting an exact string match
                """
                url = prefix + '/source/search?q=' + quote_plus(query)
                soup = BeautifulSoup(urlopen(url).read())        
                results = soup.findAll('tt',{'class':'con'})
                if results is not None:
                    for result in results:
                        if finished:
                            break
                        this_url = prefix + result.a['href']
                        this_soup = BeautifulSoup(urlopen(this_url).read())
                        possible_results = this_soup.findAll('a',{'class':'s'})
                        for this_result in possible_results:
                            this_sig = str(this_result).replace('<b>','').\
                                       replace('</b>','').split('</span>')[1].split('</a>')[0].strip()
                            #print this_soup.findAll('a',{'class':'s'})[0]
                            #print query, '\t', this_sig
                            print this_sig
                            if query in this_sig:
                                output = redirect(this_result['href'])
                                finished = True
                                break
                if not finished:
                    # hack to get it to try the next exception code
                    raise
            except:
                try:
                    """
                    Now we will try within quotes but without the return type
                    """
                    query = query.split(' ')[1]
                    url = prefix + '/source/search?q=' + quote_plus('+"' + query + '"')
                    soup = BeautifulSoup(urlopen(url).read())
                    url = soup.find('tt',{'class':'con'}).a['href']
                    if '#' in url:
                        output = redirect(url)
                    else:
                        soup = BeautifulSoup(urlopen(prefix + url).read())
                        url = soup.find('a',{'class':'s'})['href']
                        output = redirect(url)
                except:
                    try:
                        """
                        Finally, just the method name
                        """
                        query = query.split('::')[-1]
                        url = prefix + '/source/search?q=' + quote_plus('+"' + query + '"')
                        soup = BeautifulSoup(urlopen(url).read())
                        url = soup.find('tt',{'class':'con'}).a['href']
                        if '#' in url:
                            print 'if guy'
                            output = redirect(url)
                        else:
                            soup = BeautifulSoup(urlopen(prefix + url).read())
                            url = soup.find('a',{'class':'s'})['href']
                            output = redirect(url)
                    except:
                        output = 'Sorry, I was unable to find the source code for this guy'
                
    # This gets called recursively so we can take the user directly to the line
    if url is not None and query is None:
        output = intercept(url, prefix)

    # Intercepting proxy mode
    if proxy is not None and query is None and url is None:
        url = proxy.replace('QUESTION_MARK','?')

        if '#' in url:
            output = redirect(url)
        else:
            if 'png' in url:
                content_type = "image/png"
            elif 'css' in url:
                content_type = "text/css"
            if not 'source' in url:
                url = '/source/' + url
            
            output = intercept(url, prefix)


    status = '200 OK'
    response_headers = [('Content-type',content_type),
                        ('Content-Length',str(len(output)))]

    start_response(status, response_headers)
    return [str(output)]
