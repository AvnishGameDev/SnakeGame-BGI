# Snake Game - BGI

Basic Snake Game made with Borland Graphics Interface and C

The full game source is in main.c

## How to Run
### Windows
1. Make sure you have VS Code installed.
2. Download and Install suitable compiler version from [here](https://jmeubank.github.io/tdm-gcc/download).
3. Copy ```graphics.h``` and ```winbgim.h``` to ```C:\TDM-GCC-32\include```
4. Copy ```libbgi.a``` to ```C:\TDM-GCC-32\lib```
5. Open the folder in VS Code then open main.c
6. In the top search bar, search for ```>C/C++: Edit Configurations (UI)``` and press enter
7. There, set the compiler path to ```C:/TDM-GCC-32/bin/g++.exe```
8. Open ```.vscode/tasks.json``` and add the following to args
    ```json
        "-lbgi",
        "-lgdi32", 
        "-lcomdlg32", 
        "-luuid", 
        "-loleaut32", 
        "-lole32"
    ```
9. Done, now you should be able to compile.

----
## LICENSE
This repository is published under the MIT License.
