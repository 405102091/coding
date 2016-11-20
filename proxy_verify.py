# coding:gbk
# 验证最新可用代理 For http://www.5uproxy.net 多线程版
# by redice 2010.12.05

import sys
reload(sys)
sys.setdefaultencoding('gbk')


import urllib
import urllib2
from urllib2 import URLError, HTTPError

DEBUG = True

#html页面下载函数
def getHtml(url,post_data=None,cookie=None):
        """Fetch the target html
        url - URL to fetch
        post_data - POST Entity
        cookie - Cookie Header
        """
        if DEBUG:
            print "getHtml: ",url

        result =''
        
        try:
            #create a request
            request = urllib2.Request(url)

            #change User-Agent
            request.add_header('User-Agent','Mozilla/5.0')
            
            #change Referrer
            request.add_header('Referrer',url)
            
            #if has cookie,add cookie header
            if cookie:
               request.add_header('Cookie',cookie)

            #create a opener
            opener = urllib2.build_opener()            
           
            #if has post entity
            if post_data:
                #encode post data
                post_data = urllib.urlencode(post_data)
                
                response = opener.open(request,post_data)
            else:
                response = opener.open(request)
            
            result = response.read()
                
            response.close()

            #no content,don't save
            if not result or len(result)==0:
                return ''
            
            return  result
        except HTTPError, e:
            if DEBUG:
                print 'Error retrieving data:',e
                print 'Server error document follows:\n'
                #print e.read()
            return ''
        except URLError, e:
            if hasattr(e, 'reason'):
                if DEBUG:
                    print 'Failed to reach a server.'
                    print 'Reason: ', e.reason
                return ''
            elif hasattr(e, 'code'):
                if DEBUG:
                    print 'The server couldn\'t fulfill the request.'
                    print 'Error code: ', e.code
                return ''
        except Exception, e:
            if DEBUG:
                print e
            return ''
        

#需要验证的代理列表
proxy_urls = []
proxy_urls.append({'url':'http://www.5uproxy.net/http_fast.html','type':'http_fast'})
proxy_urls.append({'url':'http://www.5uproxy.net/http_anonymous.html','type':'http_anonymous'})
proxy_urls.append({'url':'http://www.5uproxy.net/http_non_anonymous.html','type':'http_transparent'})
proxy_urls.append({'url':'http://www.5uproxy.net/socks5.html','type':'socks5'})


import re
import socket
import time
import threading

result =[]


#线程同步锁
lock = threading.Lock() 

def synchronous(f):
    def call(*args, **kwargs):
        lock.acquire()
        try:
            return f(*args, **kwargs)
        finally:
            lock.release()
    return call



#先获取所有待验证的代理
proxies = []

for proxy_url in proxy_urls:
    html = getHtml(proxy_url['url'])
    
    #正则匹配获取每一代理
    rs = re.compile(r'''<tr .*?>[\s\S]*?<td .*?>\d+?</td>[\s\S]*?<td>(\S+?)</td>[\s\S]*?<td .*?>(\S+?)</td>[\s\S]*?<td .*?>(\S+?)</td>[\s\S]*?</tr>''',re.DOTALL).findall(html)
    
    for r in rs:
        proxy = {}
        
        #代理域名
        proxy['domain'] = r[0]
        #代理端口
        proxy['port'] = r[1]
        #代理国家
        proxy['state'] = r[2]
        #代理类型
        proxy['type'] = proxy_url['type']
        #响应时间
        proxy['time'] = 0
        
        if not (proxy in proxies):
            proxies.append(proxy)



#获取一个待验证代理
@synchronous
def getproxy():
    global proxies
    if len(proxies)>0:
        return proxies.pop()
    else:
        return ''


    
#保存验证结果
@synchronous
def saveresult(proxy):
    global result
    
    if not(proxy in result):
        result.append(proxy)


#线程函数
def verify():
    
    while 1:
        proxy = getproxy()
        #所有代理均已验证完毕
        if len(proxy)==0:
            return
        
        print "正在验证：%s,%s" % (proxy['domain'],proxy['port'])
        
        #验证代理的可用性
        #创建一个TCP连接套接字
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #设置10超时
        sock.settimeout(10)
        try:
            start = time.clock()
            
            #连接代理服务器
            sock.connect((proxy['domain'], int(proxy['port'])))
            proxy['time'] = int((time.clock() - start) * 1000) 
            sock.close()
            
            saveresult(proxy)
            print "%s,%s 验证通过，响应时间：%d ms." % (proxy['domain'],proxy['port'],proxy['time'])
        except Exception, e:
            if DEBUG:
                print e
            
            print "%s,%s 验证失败." % (proxy['domain'],proxy['port'])




#init thread_pool 
thread_pool = []

for i in range(20): 
    th = threading.Thread(target=verify,args=()) ; 
    thread_pool.append(th)

# start threads one by one         
for thread in thread_pool: 
    thread.start()

#collect all threads 
for thread in thread_pool: 
    threading.Thread.join(thread)


#结果按响应时间从小到大排序

result.sort(lambda x,y: cmp(x['time'], y['time']))  

fname = 'proxy_'+ time.strftime('%Y-%m-%d-%H-%M-%S',time.localtime(time.time())) +'.txt'
file = open(fname,'w')

print "验证结果如下："
for item in result:
    str = '%s,%s,%s,%s,%d' % (item['type'],item['domain'],item['port'],item['state'],item['time'])
    print str
    file.write(str+'\n')
    
file.close()

print "所有代理已验证完毕，共计%d个验证通过。验证通过的代理已存入%s" % (len(result),fname)
    
    
     