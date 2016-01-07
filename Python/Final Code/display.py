# display.py



from prettytable import PrettyTable
from sqlalchemy import *

def display():
    
    
    db = create_engine('mysql+pymysql://vikki:2408@localhost/sys')
    conn = db.connect()
    conn.begin()
    db.echo = False  # Try changing this to True and see what happens
    
    
    

    x = PrettyTable(['Name', 'Type', 'Description', 'Length', 'Strand', 'Start', 'Stop'])
    x.align["Name"] = "l" 
    x.padding_width = 1 

    y = PrettyTable(['Name', 'FASTA'])
    y.align["Name"] = "l" 
    y.align["FASTA"] = "l" 
    y.padding_width = 1 

    query1 = "select * from A_THALIANA"
    query2 = "select * from FASTA"
    result = conn.execute(query1)
    result2 = conn.execute(query2)
    resultset = get_list_of_lists(result.fetchall())
    resultset2 = get_list_of_lists(result2.fetchall())
    
    for i in resultset:
        x.add_row(i)
       
    for j in resultset2:
        y.add_row(j)
    
    print x
    print "\n"
    print y

def get_list_of_lists(list_of_tuples):
    list_of_lists = []                                                          
    for tuple in list_of_tuples:
        list_of_lists.append(list(tuple))

    return list_of_lists

