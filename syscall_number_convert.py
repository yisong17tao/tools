import re

filename = '/usr/include/x86_64-linux-gnu/asm/unistd_64.h'
syscall_dict = {}

with open(filename, 'r') as file:
    lines = file.readlines()

for line in lines:
    match = re.search(r'__NR_(\w+)\s+(\d+)', line)
    if match:
        syscall_name = match.group(1)
        syscall_number = match.group(2)
        syscall_dict[syscall_name] = syscall_number

with open('syscall_list.txt', 'w') as file:
    for syscall_name, syscall_number in syscall_dict.items():
        file.write(f"{syscall_name}\n")
