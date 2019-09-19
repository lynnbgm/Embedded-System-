# Embedded-System-
Project for Embedded System Development 

Wrote a kernel module to control a software (kernel) timer for Linux 2.6 running on the XScale processor

Writing and debugging a standalone kernel module and a user-level program to communicate with the kernel module. In addition to the basic read and write file operations implemented in the previous lab, I implemented an asynchronous notification mechanism so that the kernel is able to wake up the user program. A procfs entry for the kernel module is implemented so that users can check the status of your timer by reading that file.