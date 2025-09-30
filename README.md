# Basic SSA IR 

Using for counting factorial.  
Example in main.cpp, creating this code, but in IR level:
```
int factorial(int n) {
    int result = 1;
    for (int k = 2; k <= n; k++) {
        result *= k;
    }
    return result;
}
```

## How to build:
```
mkdir build && cd build
cmake ..
./Basic_IR
```