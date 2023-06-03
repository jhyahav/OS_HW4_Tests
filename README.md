## About
The ```test.c``` file in this repo tests the functions defined in your ```queue.c``` implementation. Most of the test functions were generated using ChatGPT 3.5 and manually fine-tuned to suit the assignment specifications.
These tests are subject to change in the near future. Please make sure you are working with the most recent version of them. It is not recommended to clone this repo before you are actually ready to carry out the tests.
**Please note that passing the tests in their current form does not guarantee that your code meets all of the requirements! Similarly, unexpected behavior or failed tests do not necessarily mean that your code does not meet the requirements! These tests are a work in progress.**
## How to use the automated tests
1. Clone this repository and copy your ```queue.c``` file into the resulting folder.
2. Compile your code along with the test file using ```gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 -pthread test.c -o test -g```.
3. Run ```./test```. 
4. *Optional*: For more detailed logs, particularly information about threads, install gdb (run ```sudo apt-get update```, then ```sudo apt-get install gdb```). After installing gdb, run ```gdb ./test```, hit the <kbd>Enter</kbd> key, then run your code by typing "run" and hitting the <kbd>Enter</kbd> key again.
5. Part of the function ```test_edge_cases()``` is commented out. After running the tests without it, comment it in and make sure it block execution (this is the expected behavior). You can, of course, comment it back out if you wish to run the other tests again.

## Contributing
These tests are by no means comprehensive. There are definitely edge cases that I haven't thought of or have yet to add tests for. Contributions would be very much appreciated. Feel free to submit a PR or to reach out to me.
### To-do:
- Verify correctness of ```test_multiconcurrent_enqueue_dequeue()``` or make adjustments
- Concurrent random sequences of queue operations
- Automatically verify correct thread ordering 
