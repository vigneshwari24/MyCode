# main.py

# authors: Nishant Goel, Vigneshwari Chandrasekaran
# program to create a menu and enable the user to interact with the Arabidopsis Thaliana Database using SQLAlchemy

from prettytable import PrettyTable
import create as create
import display as display
import insert as insert
import delete as delete
import Blast1 as Blast
import subprocess
import urllib2
import simplejson
import cStringIO
import PIL

import Image
def main():

    quit = "y"
    while (quit == 'y' or quit == 'Y'):
        
        x = PrettyTable(["A R A B I D O P S I S  T H A L I A N A  D A T A B A S E"])
        x.align["A R A B I D O P S I S  T H A L I A N A  D A T A B A S E"] = "l" 
        x.padding_width = 1
        x.add_row(["1. Create the Genes table"]) 
        x.add_row(["2. Display the Genes"])
        x.add_row(["3. Insert a new Gene"])
        x.add_row(["4. Delete a Gene"])
        x.add_row(["5. Blast a Gene"])
        print x 
    
        ## Get input ###
        choice = raw_input('Enter your choice [1-5] : ')
    
        ### Convert string to int type ##
        choice = int(choice)
 
        ### Take action as per selected menu-option ###
        if choice == 1:
            create.create() #this will create the tables and popululate the data
            
        elif choice == 2:
            display.display() # display both the tables from the database
            
        elif choice == 3:
            insert.insert() # insert a new gene record in the database
            
        elif choice == 4:
           delete.delete() # delete a gene record from the database
           
        elif choice == 5:
            name = raw_input("Please enter the name of the gene to perform blast: ")
            Blast.fn(name) # Calls the BLAST module
            Blast.show()   # Calls the module that prints image downloaded from Google Search
            
           
               
        else:    ## default ##
            print ("Invalid input. Please enter the correct input...")
        quit = raw_input('Do you wanna Continue : ')
    
    
# call the main function
main()