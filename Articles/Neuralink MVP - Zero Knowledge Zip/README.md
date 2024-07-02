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
1. Open a terminal and clone the repository.
   
   ```git clone https://github.com/MurageKibicho/Bellard.git```
2. Navigate to the **Neuralink MVP - Zero Knowledge Zip** folder 
   ```cd "Bellard/Articles/Neuralink MVP - Zero Knowledge Zip" ```

4. Compile the build script
   ``` gcc main.c -lm -o main.o```

5. Run the build script
   ```./main.o```

   The build script
   - Downloads the neuralink dataset
   - Unzips the dataset
   - Performs our [custom transform]()
   - Runs a test to compare files.
 