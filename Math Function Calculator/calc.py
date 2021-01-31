#!/usr/bin/env python3

import sys #module for command line arguments
import re  #module for pattern matching

# Written by Brendan Wadey, for Python.
#
# This program uses regular expressions to identify functions from the command line, using a stack-based approach to replace a string with it's number result.
# The initial goal was to use regex for EVERYTHING, as a bit of a self-imposed challange. This method also would have used recursion, to continously apple the regex.
# But unfortunately Regex does not do both multiple functions like (add (add 1 1) (add 2 2)) while also doing nested functions like (add 1 (add 1 (add 1 1))).
# This is because of a quirk in greedy/non-greedy regular expressions, in how it grabbed or didn't grab parentheses.


#There are two steps to modifying the code to add new functions:
#       1. Edit "regex2" in parseExpression() to recognize the new function
#       2. Edit parseNumbers() to implement the new function


def main():
    
    if (len(sys.argv) == 1):
        print(" --- ! --- Command argument missing.")
        return 1

    if (sys.argv[1].isdigit()):
        print(sys.argv[1])
        return 0
        
    #This ignores any garbage preceding the valid expression, and after it.
    regex1 = re.compile(r'(?<=\()(.*)(?=\))') #Match if begins with ( and ends with ). Does not capture ( or ).
    info = re.findall(regex1, sys.argv[1]) #Returns a list of the groups matched. 

    if (len(info) != 1):
        print(" --- ! --- Invalid expression.")
        return 1
    else:
        string = info[0]
        string = replaceAllNested(string) #replaces all nested functions with their numerical result.

        if (string == None): #replaceAllNested() returns None and prints a message when an error occurs.
            return 1

        string = parseExpression(string) #One last parse (at this point, there should only be one function left)

        if (string == None):
            print("Failed to compute.")
            return 1
        else:
            print(string)
            return 0
        #         
    #
#


#Function to determine add/multiply, and setup numbers for parseNumbers(). Returns the integer parseNumbers() returns.        
def parseExpression(expression):
    
    if (len(expression) == 0):
        print(" --- ! --- Length zero expression.")
        return None

    #////////////////////////////////////////////////////////////////////////////////////////////////////#
    # To show that it is easy to extend the functionality to involve more expressions per function...    #
    # I just did it. Because it was easy.                                                                #
    # Uncomment the first "regex2" to allow more than two expressions per function.                      #
    # Uncomment the second "regex2" to only allow two expressions per function.                          #
    #////////////////////////////////////////////////////////////////////////////////////////////////////#
    
    #regex2 = re.compile(r'^(multiply|add) (\d+) (\d+)((?: \d+)+)?$') #regex to match a string begining with multiply or add, space, number, space, number, ect
    regex2 = re.compile(r'^(multiply|add) (\d+) (\d+)$') #regex to match a string begining with multiply or add, then a space, number, space, ending with num
    
    parsed = re.findall(regex2, expression)
    
    if (len(parsed) == 0): #parsed is a list of every match (and every match is a tuple, since there are multiple capture groups), so if its length is zero, there were no matches.
        print(" --- ! --- Invalid function or function arguments given: \"", expression, "\"", sep="")
        return None

    parsed = parsed[0]      #There should only be one tuple, so we need to get to the tuple...
    parsed = list(parsed)   #   ...and turn the tuple into a list, so that reassignment is possible.
                            #I wouldn't use re.findall, if not for how it grabs the capture groups and puts them neatly into a tuple.
    mode = 0
    if (parsed[0] == "multiply"):
        mode = 1
    elif (parsed[0] == "add"):
        mode = 2

    numbers = [parsed[1], parsed[2]] #Makes a new list, without the operator
    
    #Runs if you enable extra numbers for the function, since the tuple will have an extra, 4th possible group
    if (len(parsed) == 4):
        if (parsed[3] != ''):
            regex7 = re.compile(r'\d+') #regex to match integer of any length
            parsed[3] = re.findall(regex7, parsed[3]) #find all that match the regex, and put them into a list
            numbers += parsed[3]
                  
    return parseNumbers(mode, numbers)
#
 

#Depending on the specified mode (multiply or add), given a list of string numbers this function returns a number result.
def parseNumbers(mode, num_list):
    
    #print("HEY?", num_list, mode)

    result = 0
    first_flag = True

    for snum in num_list:
        
        if (first_flag):
            result = int(snum)
            first_flag = False
        else:
            if mode == 1:
                result *= int(snum)
            elif mode == 2:
                result += int(snum)
            else:
                return None
            
    return result
#

  
#This function uses a stack approach to replacing substrings with their result when put into parseExpression().
def replaceAllNested(string):

    current = 0
    loc = []
 
    for c in string: #Iterates through every character in the string
        if c == '(':
            loc.append(current)
        elif c == ')':
            
            if (len(loc) == 0):
                print(" --- ! --- Parenthesis mismatch - Too many \")\".")
                return None

            start = loc.pop()
             
            substring = string[(start+1):current] #Slice from after ( to before )
 
            result = parseExpression(substring) #Get a number result from the substring
            
            if (result == None):
                #No error message needed - parseExpression will do it for us
                return None
           
            string = string[:start] + str(result) + string[(current+1):]  #Replace the space from ( to ) with the result
            
            current = current - len(substring) - 2 + len(str(result)) #Adjust the counter, so that everything remains consistent.
                                                                      #We are removing the substring, removing the () hence the 2, but adding a number 
        current += 1

    if len(loc) != 0:
        print(" --- ! --- Parenthesis mismatch - Unclosed \"(\".")
        return None
    else:
        return string



if __name__ == "__main__":
    main()
