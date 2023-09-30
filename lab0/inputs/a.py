import os
filename = "memtest0"
lines = ''
with open(filename,'r') as file_to_read:
    while True:
        lines = file_to_read.readline()
        if not lines:
            break
        else:
            this_lines = lines.split()
            print(this_lines[1])