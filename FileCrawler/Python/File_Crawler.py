import os
import sys
import getopt

# Current working directory
cwd = os.getcwd()

# Global boolean
recursive = False
sensitive = False
display_line = False

# Relative directory
search_path = os.path.join(cwd, '..', 'TestData')

# Search for word that the user specified
def search_for_word(path, word):
    current_path = None
    folders = []
    # Loop through files in the directory with listdir function
    for filename in os.listdir(path):
        # Set current path with path and filename
        current_path = os.path.join(path, filename)
        # Check if it is a readable file
        if os.path.isfile(current_path):
            # Open the file to read the contents
            with open(current_path, 'r', encoding='utf-8') as f:
                found = False
                line = 0
                # Loop through the lines in the file
                for l in f:
                    line_str = ' '
                    line += 1
                    if display_line is True:
                        line_str += "Line - " + str(line)
                    # Loop through the words in the line
                    for w in l.split():
                        # If it is not case sensitive, change everything to lower case
                        if sensitive is False:
                            word = word.lower()
                            w = w.lower()
                        # If the word is a match print to console
                        if word == w:
                            print('.' + current_path.split("\.")[1] + line_str)
                            # If the option display line is not selected only write out the path once, otherwise write out with each line the word is found
                            if display_line is False:
                                found = True
                                break
                    if found is True:
                        break
        # If the file is a directory, save into the folders list
        elif os.path.isdir(current_path):
            folders.append(current_path)
        # If the file can not be read, write out Error message with the path
        else:
            print("!!ERROR!! Could not read file:" + current_path)

    # If recursive search is on, search through with help of folders list
    if recursive is True:
        for x in folders:
            search_for_word(x, word)


def Usage():
    print("-----Usage-----\nDefault setting:\n\tRecursive - off\n\tCase sensitive - off\n\tDisplay line - off")
    print("Options:\n\t-r {Recursive}\n\t-c {Case sensitive}\n\t-l {Display line}")


if __name__ == '__main__':
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "rcl?")
    except getopt.GetoptError as err:
        print(err)
        Usage()
        exit(-1)

    for opt, arg in opts:
        if opt == '-r':
            recursive = True
        if opt == '-c':
            sensitive = True
        if opt == '-l':
            display_line = True
        if opt == '-?':
            Usage()
            exit(-1)

    word = input("Please enter word to search for: ")
    search_for_word(search_path, word)
