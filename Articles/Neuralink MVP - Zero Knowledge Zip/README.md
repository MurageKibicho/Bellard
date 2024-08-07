# Zero Knowledge ZIP - MVP

This is our submission for the Neuralink compression challenge.

We assume you have a Linux system with Bzip2 and GCC pre-installed.

This is not an archiver. It works on individual files.
We compare the total size of **data** folder and **Output** folder.

## Introduction
We attempt to showcase an application of Zero Knowledge proofs -  making files smaller in realtime.
We have day jobs so we're yet to fully document our work.


These sections are documented:
1. [Ditching Shannon entropy and building a Custom Entropy measure](https://kibicho.substack.com/p/the-uniformity-measure?r=2at73k)
2. [Building a Data Compressor using our Custom measure](https://open.substack.com/pub/kibicho/p/decreasing-uniformity-neuralink-compression?r=2at73k&utm_campaign=post&utm_medium=web&showWelcomeOnShare=true)
3. [Coming soon. Adding polynomial proofs]

## Results
Complete discussion [here](https://open.substack.com/pub/kibicho/p/decreasing-uniformity-neuralink-compression?r=2at73k&utm_campaign=post&utm_medium=web&showWelcomeOnShare=true)

### File size
![File size comparison](https://substackcdn.com/image/fetch/f_auto,q_auto:good,fl_progressive:steep/https%3A%2F%2Fsubstack-post-media.s3.amazonaws.com%2Fpublic%2Fimages%2Fd8759947-bb8b-4b58-8259-440ad08ef943_2334x1314.png)
Total size of directory **Data** after Bzip:   **47191022 (bytes)**

Total size of directory **Output** after Bzip + Custom Compress:   **47083938 (bytes)**

Total saved  size:     **107084 (bytes)**

### Average running time.
**0.018055 seconds** per file

Complete discussion [here](https://open.substack.com/pub/kibicho/p/decreasing-uniformity-neuralink-compression?r=2at73k&utm_campaign=post&utm_medium=web&showWelcomeOnShare=true)


## How to Run 
[YouTube tutorial](https://youtu.be/u6zg2sSKIig)

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
   - Performs our [custom transform](https://open.substack.com/pub/kibicho/p/decreasing-uniformity-neuralink-compression?r=2at73k&utm_campaign=post&utm_medium=web&showWelcomeOnShare=true)
   - Runs a test to compare files.
 
   
## Disclaimer
Our encoder has a failure rate of 10 / 743.

This means 10 in 743 files won't decompress correctly.

We didn't have time to debug these edge cases.

## Further direction
[Follow us on Twitter](https://x.com/murage_kibicho)

[Take our compression course : Using FFmpeg in C](https://www.udemy.com/course/part-1-video-coding-with-ffmpeg-and-c-in-replit-online-ide/)


