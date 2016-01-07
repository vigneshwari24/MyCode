#Blast1.py

# author: Vigneshwari Chandrasekaran

# This program does the BLAST of a gene, invokes a web search for the BLAST result

import subprocess
import urllib2
import simplejson
import cStringIO
import PIL
import sqlalchemy
import Tkinter
import bs4
import re
#Retrieves the nucleotide sequence for the gene mentioned by the user
def getnucleotideseq(name):
    engine = sqlalchemy.create_engine('mysql+pymysql://vikki:2408@localhost/sys')
    conn = engine.connect()
    conn.begin()
    Query = "select Fasta from FASTA where Name = '" + name + "'"
    #print Query
    result = conn.execute(Query)
    resultset = result.fetchone()
    nucseq = resultset[0]
    conn.close()
    f = open("D:\\test_query.txt","w")
    f.write(nucseq)
    f.close()    
    
def fn(name):
    getnucleotideseq(name)
    subprocess.Popen("Blast.bat",shell= True).communicate()
    ff = open("Blast.bat")
    r = ff.readlines()
    st = r[len(r) -1].strip().split(" ")
    f = open(st[len(st) - 1])
    readline = f.readlines()
    j = ""
    for i in readline:
        if i[0][0] == '>':
            #print i
            j = i
            break
    if len(j.strip()) > 0:
      google(j)
    else:
        if changeblast() == True:
            fn(name) 
        else: 
            print  "No match found!!"

def changeblast():
    #print "in change blast"
    f =  open("Blast.bat")
    f1 = f.readlines()
    a = f1[2].split(" ")[4]
    if int(a[len(a) - 2: ]) > 1:
        #print "Looking in db " +a[len(a) - 2:]
        dec = int(a[len(a) - 2:]) - 1
        f1 = open("Blast.bat" , "w")
        l = ['C:\n',
 'cd C:\\Program Files\\NCBI\\blast-2.2.31+\n',
 'blastn -query D:\\test_query.txt -db est_others.0' + str(dec)+ ' -out D:\\output.txt\n']
        for i in l:
            f1.write(i)
        f1.close()
        return True
    else: return False
   
def google(j):
    arr = j.split(" ")
    name = arr[3] + " " + arr[4]
    url = "https://www.google.com/search?hl=en&authuser=0&site=imghp&tbm=isch&source=hp&biw=1304&bih=663&q="+urllib2.quote(name)+"&oq="+urllib2.quote(name)
    header = {'User-Agent': 'Mozilla/5.0'} 
    soup = bs4.BeautifulSoup(urllib2.urlopen(urllib2.Request(url,headers=header)))
    images = [a['src'] for a in soup.find_all("img", {"src": re.compile("gstatic.com")})]
    imageUrl = images[0]
    file = cStringIO.StringIO(urllib2.urlopen(imageUrl).read())
    img = PIL.Image.open(file)
    img.save('Img.jpg')
    img.show()
  

# Displays the image to the user        
def show():
    output = Tkinter.Tk()
    image = PIL.Image.open("Img.jpg")
    #image.load()
    image.show()
    #pic = PIL.ImageTk.PhotoImage(image)
    #Tkinter.Label(output, image=pic).pack()
    #Tkinter.mainloop()
    
    
    
    
    
    
    
    
    