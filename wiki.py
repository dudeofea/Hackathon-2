import urllib2, optparse
from bs4 import BeautifulSoup, SoupStrainer
from random import randint

parser = optparse.OptionParser()
parser.add_option("-l", "--link", dest="link", help="link to wiki page")
parser.add_option("-m", "--max", action="store", type="int", dest="max", default=0, help="max number of links to display")

(options, args) = parser.parse_args()

page = urllib2.urlopen('http://en.wikipedia.org/wiki/'+options.link).read()

url_list = []
#get all wiki links on page
for link in BeautifulSoup(page, parse_only=SoupStrainer('a')):
	if link.has_attr('href'):
		tmp = link['href']
		if tmp.startswith('/wiki/') and tmp.find(':') == -1:
			tmp = tmp[6:]
			if tmp not in url_list:
				url_list.append(tmp)
#cut down links randomly
if options.max > 0:
	length = min(options.max, len(url_list) - 1)
else:
	length = len(url_list) - 1
tmp = len(url_list) - 1
for i in xrange(0, length):
	print url_list[randint(0, tmp)]