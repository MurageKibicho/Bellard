# Zero Knowledge ZIP - MVP

This is our submission for the Neuralink compression challenge.

It has been tested on a linux PC running Ubuntu. 

We assume your Linux system has Bzip and GCC pre-installed.

## Introduction
We attempt to showcase an application of Zero Knowledge proofs -  making files smaller in realtime.
We have day jobs so we're yet to fully document our work.


This sections are documented:
1. [Ditching Shannon entropy and building a Custom Entropy measure](https://kibicho.substack.com/p/the-uniformity-measure?r=2at73k)
2. [Building an Data Compressor using our Custom measure]()
3. [Coming soon. Adding polynomial proofs]

## Results


## How to Run
1. Clone the repository.
   
   ```git clone https://github.com/MurageKibicho/Bellard.git```

2. Compile the build script
   ``` gcc main.c -o -lm main.o```

3. Run the build scrip
   ```./main.o```
 
