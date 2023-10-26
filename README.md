# README

这个小工具使用ptrace捕获另一个程序的所有系统调用，目前只有这个功能，后续会持续进行优化，当做一个练手的小项目。
其中sysacall_number_convert.py文件是用来读取linux系统的系统调用号和系统调用名称的对应表，根据自己的机器来修改映射表的路径。target只是一个测试的目标

This little tool uses ptrace to capture all the system calls of another program, which is currently the only function, and will be continuously optimized as a small project to practice.
The sysacall_number_convert.py file is used to read the linux system's table of syscall numbers and syscall names and change the path to the map table according to your machine. target is just a test target
