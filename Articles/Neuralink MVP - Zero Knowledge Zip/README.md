# Zero Knowledge ZIP - MVP

This is our submission for the Neuralink compression challenge.

We assume you have a Linux system with Bzip2 and GCC pre-installed.

This is not an archiver. It works on individual files.
We compare the total size of **data** folder and **Output** folder.

## Introduction
We attempt to showcase an application of Zero Knowledge proofs -  making files smaller in realtime.
We have day jobs so we're yet to fully document our work.


This sections are documented:
1. [Ditching Shannon entropy and building a Custom Entropy measure](https://kibicho.substack.com/p/the-uniformity-measure?r=2at73k)
2. [Building a Data Compressor using our Custom measure]()
3. [Coming soon. Adding polynomial proofs]

## Results

### File size
Total size of directory **Data** after Bzip:   **47191022 (bytes)**

Total size of directory **Output** after Bzip + Custom Compress:   **47083938 (bytes)**
Total saved  size:     **107084 (bytes)**

### Average running time.
**0.018055 seconds** per file

## How to Run
1. Open a terminal and clone the repository.
   
   ```git clone https://github.com/MurageKibicho/Bellard.git```
2. Navigate to the **Neuralink MVP - Zero Knowledge Zip** folder
   
   ```cd "Bellard/Articles/Neuralink MVP - Zero Knowledge Zip" ```

3. Compile the build script
   
   ``` gcc main.c -lm -o main.o```

4. Run the build script
   
   ```./main.o```

 The build script
   - Downloads the neuralink dataset
   - Unzips the dataset
   - Performs our [custom transform]()
   - Runs a test to compare files.
 
   
## Disclaimer
Our encoder has a failure rate of 10 / 743.

This means 10 in 743s files won't decompress correctly.

We didn't have time to debug these edge cases.

## Further direction
[Follow us on Twitter](https://x.com/murage_kibicho)
[Take our compression course : Using FFmpeg in C](https://www.udemy.com/course/part-1-video-coding-with-ffmpeg-and-c-in-replit-online-ide/)


