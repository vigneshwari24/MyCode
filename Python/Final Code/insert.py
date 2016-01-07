#insert.py

# authors : Nishant Goel, Vigneshwari Chandrasekaran

#program to insert a gene record in both the database tables in MySQL using SQLAlchemy
from sqlalchemy import *

# creating the connection with the database

db = create_engine('mysql+pymysql://vikki:2408@localhost/sys')
conn = db.connect()
conn.begin()
db.echo = False  # Try changing this to True and see what happens


# function to ask information to be inserted from the user
def ask():
    
   
    flag = 0
    name = raw_input("Enter the name of the Gene: ")

    type = raw_input("Enter the type of the Gene: ")

    desc = raw_input("Enter the description of the Gene: ")

    length = raw_input("Enter the length of the Gene: ")

    strand = raw_input("Enter the strand of the Gene (Forward or Reverse): ")

    start = raw_input("Enter the start codon of the Gene: ")
    stop = raw_input("Enter the stop codon of the Gene: ")

    fasta = raw_input("Enter the nucleotide sequence of the Gene: ")
    
    data = [name,type,desc,length,strand,start,stop,fasta]
    
    # queries to check validations on the data entered by the user
    query1 = "select Name from A_THALIANA where Name = '"+name+"'";
    query2 = "select length from A_THALIANA";
    query3 = "select start from A_THALIANA";
    
    result = conn.execute(query1)
    result2 = conn.execute(query2)
    result3 = conn.execute(query3)
    resultset = result.fetchone()
    resultset2 = result2.fetchall()
    resultset2 = [int(i[0]) for i in resultset2] # converting the string into int
    resultset3 = result3.fetchall()
    resultset3 = [int(i[0]) for i in resultset3] # converting the string into int
    
    
    # check if the record with the entered name already exists    
    if(resultset != None):
        genename= resultset[0]
        
        if((genename == name)):
            print("Sorry.. The name already exists!! please enter the correct details")
            flag = 1
            
    # check if the length, start and stop parameters are numeric or not
    elif(((length.isalpha()) == True) or (start.isalpha() == True) or (stop.isalpha() == True)):
        
        print("Sorry.. Data is not correct!! please enter the correct details")
        flag = 1
    
    # check if value of strand is Forward or Reverse  
    elif(strand != "Forward" and strand != "Reverse"):
        print("Please check the Strand!! please enter the correct details")
        flag = 1
        
    # check if the range of start and stop codon is equal to the entered length     
    elif(((int(stop)-int(start))+1) != int(length)):
        print("Sorry.. Data is not correct!! please enter the correct details")
        flag = 1
    
    # check if the new values of start codon and length conflict with all the other records present in the database. 
    elif(codon_validation(resultset3,resultset2,int(start),int(length)) == False):
        print("Please check the value of length, start and stop codon!! please enter the correct details")
        flag = 1
                                               
    # if any of the above validations comes to false, we recursively call the ask method again
    if (flag == 1):
        return ask()
        
    # if not, return the parameter values
    else:
        return data
    
# function to check if the new values of start codon and length conflict with all the other records present in the database. 
def codon_validation(start, length, new_gene_Start, new_gene_len):
   
    final = []
    i = 0
    # Making a list of the all the start codon from minimum to maximum
    while i < len(length):
        st = start[i]
        le = length[i]
        
        for ii in range(le):
            final.append(st + ii)
        i += 1
    
    # Comparison of new range of start and stop codon with the all the existing values in the database
    new_Data_range = []
    for ni in range(new_gene_len):
        new_Data_range.append(new_gene_Start + ni)
    
    for c in new_Data_range:
        for b in final:
            # if there is any conflict, return false
            if c == b:
                return False
    
    # if not, return true                
    return True


# function to insert a new record to a Gene into the database
def insert():
    
    metadata = MetaData(db)
    
    # loading the tables
    AT = Table('A_THALIANA', metadata, autoload=True)
    fasta = Table('FASTA', metadata, autoload=True)

    details=ask()
    
    
    list1 = ['Name', 'Type', 'Description', 'Length', 'Strand', 'Start', 'Stop']
    list2 = []
    list3 = ['Name', 'FASTA']
    list4 = []
    
    list2.append(details[0])
    list2.append(details[1])
    list2.append(details[2])
    list2.append(int(details[3]))
    list2.append(details[4])
    list2.append(int(details[5]))
    list2.append(int(details[6]))
    list4.append(details[0])
    list4.append(details[7])
    
    # converting list of values into a planned dictionary to insert
    dict1 = {}
    map(lambda k, v: dict1.update({k: v}), list1, list2)
    dict2 = {}
    map(lambda k, v: dict2.update({k: v}), list3, list4)
    ins= AT.insert()
    ins1= fasta.insert()
    ins.execute(dict1)
    ins1.execute(dict2)
    
    print "New Gene is inserted successfully"
    

    
