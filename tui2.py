# -*- coding:utf-8 -*-

import urllib2
import re
import os 
import time
import copy
def get_html(url):
    request = urllib2.Request(url=url)
    response = urllib2.urlopen(request, data=None, timeout=3)
    html = response.read()
    return html	
def get_pic_list(html):
    pattern = re.compile(r'<img src="(.*?)" alt="')
    list = pattern.findall(html)
    return list	
def save_pic(url):
    pattern = re.compile(r'<img src="(.*?)" alt="')
    list = pattern.findall(html)
    return list
def get_pages_list(html):
    pages_pattern = re.compile(r'<li class="thisclass">([\s\S]*?)</div>')
    pages_body = pages_pattern.findall(html)
    pages_list_pattern = re.compile(r'<a href="(.*?)">')
    pages_list = pages_list_pattern.findall(pages_body[0])
    return pages_list	        
def mk_dir(i):    
    bash_pash= "f:/my_spider/picture/"
    path_str =bash_pash+"%d"%i
    try:
        os.makedirs(path_str)
        return path_str
    except :
        print path_str ," is already exists."
        return None
def save_as_jpg(pash_num,jpg_list):          
    a=mk_dir(pash_num)
    if a is None:
        return save_as_jpg(pash_num+1,jpg_list)
    print "start saving pictures"
    for url in jpg_list:
        pic = get_html(url)
        file_path = a + "/%s.jpg"%time.time()
        f = open(file_path,'wb')  
        f.write(pic)  
        f.close()
        print "save "+url+" succeed"
def save_a_page(item_url,num):
    bash_url = "http://www.aitaotu.com"    
    html = get_html(bash_url+item_url)
    pic_list = get_pic_list(html)
    print "start get pic_list"
    pages_list = get_pages_list(html)	
    print "get pic_list"
    for page in pages_list:
        if '#' in page:
            continue
        page_html = get_html(bash_url+page)
        page_pic_list = get_pic_list(page_html)
        for i in page_pic_list:
            pic_list.append(i)
    print "start saving"
    save_as_jpg(num,pic_list)
#def save_pages(url):
def get_pages(url):
    bash_url = "http://www.aitaotu.com"
    html = get_html(bash_url+url)
    pages_pattern = re.compile(r'<a href="(.*?)" target="_blank">')
    pages_body = pages_pattern.findall(html)
    pages_body.sort()
    pages_lst = []
    for i in range(len(pages_body)):
        if i==0 or pages_body[i]!=pages_body[i-1]:
            if "guonei" in pages_body[i]:
                pages_lst.append(pages_body[i])               
    return pages_lst
def get_whole_page(url,num):
    lst=[]
    pages_body = get_pages(url)   
    for page_url in pages_body:
        if "html" in page_url and "guonei" in page_url:
            try:
                print page_url,num
                print page_url
                save_a_page(page_url,num)
                this_page = get_pages(page_url)
                print "get list"
                #print this_page
                lst = lst + this_page
                num=num+1
            except:
                print "error in %s"%page_url
    return lst,num
    
#lst = ["/guonei/"]
lst = ["/guonei/21066.html"]

num=0
hash_map = set([]) 
while 1:
    temp_lst = []
    for url in lst:
        try:
            lst_temp,num = get_whole_page(url,num)
            num=num+1
            temp_lst = temp_lst + lst_temp
        except:
            print "error happens"
        time.sleep(1)
    temp = copy.copy(hash_map)
    hash_map.update(temp_lst)
    lst = hash_map - temp
    #print lst
    

        

