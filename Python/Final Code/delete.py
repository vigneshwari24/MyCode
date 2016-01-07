# delete.py

# author: Nishant Goel
# program to implement deletion of a Gene record from both the tables

from sqlalchemy import *


#function to implement deletion
def delete():
    
    # creating the connection with the MySQL database using SQLAlchemy
    db = create_engine('mysql+pymysql://vikki:2408@localhost/sys')
    conn = db.connect()
    conn.begin()
    db.echo = False  
    
    
    name = raw_input("Enter the name of the Gene to be deleted: ")

    query1 = "select Name from A_THALIANA where Name = '"+name+"'";
    query2 = "delete from A_THALIANA where Name = '"+name+"'";
    query3 = "delete from FASTA where Name = '"+name+"'";    
   
    
    result = conn.execute(query1)   
    resultset = result.fetchone() # fetching the result of the query 
    
    # checking if the gene record exists in the database or not
    if(resultset == None):
        print("This Gene is not there in the database!! Kindly enter the correct name!!")
        delete() #calling the delete function recursively
    
    #delete if the record is present in the database
    else:
       db.engine.execute(query3)
       
       db.engine.execute(query2)
       result1 = db.engine.execute(query1)
            
       if(result1.fetchone() == None):
           print(name+" Gene is deleted successfully!!")
            
   
    conn.close() # closing the connection
        


