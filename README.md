# ELF-Loader
Homework assignment for the Operating Systems Course.

Basically, an ELF Loader implemented as a dynamic library for Linux.

## Implementation
The segv_handler function is a routine for handling the SIGSEGV signal.

First, here's a picture to help you visualise what i'm trying to do.

![image](https://user-images.githubusercontent.com/95059633/219495497-0ef56954-ad5d-4521-9682-ce517cbd3c76.png)

We're talking about a demand paging mechanism, so first we verify if the page is within a segment or not. If not, or info->si_code = SEGV_ACCER (invalid permissions), we run the default handler, using the signal function. Otherwise, if the address is not mapped, we find out what's the offset where we need to map.

For mapping, we have 3 cases:
* If the whole page is located beyond file_size, we map using MAP_ANONYMOUS (which initializes with 0), the last 2 parameters being ignored.
* If the page doesn't go over the file_size, we map setting the file descriptor and offset field.
* If our page is "intermediate", meaning it starts before file_size ends but part of it goes beyond, we map like the previous case, but we will have to initialize with 0 what is beyond file_size. In the end, we call mprotect to change the permissions of the segments to the right ones.

## Useful resources

 - [Signal](https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-04)
 - [Managing Memory](https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-05)
 - [Virtual Memory](https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-06)
