# create.py

# author: Nishnat Goel

# program to create tables in the MySQL database

from sqlalchemy import *
import csv
import warnings

# function to create tables
def create():
    #creating the coonection with the database
    db = create_engine('mysql+pymysql://vikki:2408@localhost/sys')
    conn = db.connect()
    conn.begin()

    db.echo = False  # Try changing this to True and see what happens

    metadata = MetaData(db)
    
    # Drop the table if they already exists
    query1 = "DROP TABLE IF EXISTS A_THALIANA;"
    query2 = "DROP TABLE IF EXISTS FASTA;"
    
    #filtering the warnings
    warnings.filterwarnings('ignore', 'unknown table')
    
    # executing the queries
    db.engine.execute(query2)
    db.engine.execute(query1)
    
    
    # defining the table structure    
    AT = Table("A_THALIANA",metadata,Column('Name',String(16),primary_key=True),
    Column('Type',String(16)),
    Column('Description',String(16)),
    Column('Length',Integer),
    Column('Strand',String(16)),
    Column('Start',Integer),
    Column('Stop',Integer))
    AT.create(checkfirst=True)

    fasta = Table("FASTA",metadata,Column('Name',String(16),ForeignKey('A_THALIANA.Name')), 
    Column('FASTA',String(200)))
    fasta.create(checkfirst=True)

    
    # populating the database using the csv files
    i = AT.insert()
    j = fasta.insert()

    with open('A_Thaliana.csv', 'r') as csvfile: 
                #sniff to find the format 
        fileDialect = csv.Sniffer().sniff(csvfile.read())
        csvfile.seek(0)
                #read the CSV file into a dictionary
        dictReader = csv.DictReader(csvfile, dialect=fileDialect)
    
        for row in dictReader:
                    
            i.execute(row)
            
                 
    with open('fasta.csv', 'r') as csvfile: 
                #sniff to find the format 
        fileDialect = csv.Sniffer().sniff(csvfile.read())
        csvfile.seek(0)
                #read the CSV file into a dictionary
        dictReader = csv.DictReader(csvfile, dialect=fileDialect)
    
        for row in dictReader:
                    
            j.execute(row)            
                                

    print "Tables are Created and data is populated"
    
    conn.close() # closing the connection

