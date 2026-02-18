### Tasks:

## 1. Basic Thread Creation

    Objective: Familiarize yourself with creating threads.
    Instructions:
        Write a C program that creates three threads.
        Each thread should print a message, including its thread ID, to indicate it is running (e.g., "Thread X is running").
        Ensure the main thread waits for each of the threads to complete using pthread_join.
    Expected Output: Each thread prints a message, and the program exits only after all threads are complete.

## 2. Parallel Array Sum

    Objective: Create threads, split work across threads, and avoid shared writes
    Instructions:
        Write a C program that creates a large array of integers (e.g., size = 50 million).
        Fill the array with random numbers.
        Compute the total sum:
            sequentially
            using N threads (No global shared counter, No mutex, No atomic variables. Each thread works on a separate index range)
        Each thread should:
            Process a separate chunk of the array
            Store its partial sum in a thread-local variable
            Return the result to the main thread
        Main threadshould:
            Collect all partial sums
            Compute the final result
            Print execution time for both versions
    Expected Output: Compute and print the final result, and print execution time for both versions

## 3. Parallel Maximum Search

    Objective: Parallel reduction without synchronization.
    Instructions:
        Write a C program that creates a large array of integers (e.g., size = 50 million).
        Fill the array with random numbers.
        Find the maximum value sequentially.
        Find the maximum using 4 threads.
        Each thread:
            Searches maximum in its own chunk.
            Stores local max in a struct passed to the thread.
            Returns result to main.
        Main thread:
            Finds the global maximum from 4 local maximums.
    Expected Output: Compute and print the final result and print execution time for both versions (sequential and threads).

## 4. Parallel Prime Counting

    Objective: Parallel CPU-bound workload.
    Instructions:
        Count the number of prime numbers from 1 to 20 million.
        Sequential version first.
        Parallel version:
            Divide the numeric range into equal intervals
            Each thread counts primes in its interval
            Store result locally
        Combine in the main.

    Expected Output: Print the final result and execution time for both versions

## 5. CPU Core Exploration

    Objective: Understand scheduling behavior.
    Instructions:
        Create N threads.
        Each thread runs a heavy loop for several seconds. Just have some big number for iterations.
        Each thread prints:
            Thread ID
            CPU core ID (sched_getcpu())
        Observe:
            Do threads run on different CPUs?
            Does OS migrate threads?
        Try running with taskset.
            taskset -c 0 ./my_program
            If you are using your own laptop with multiple cores, you can also trydifferent combinations:
                taskset -c 0,1,2,3 ./my_program
                taskset -c 0,2 ./my_program
                taskset -c 0-3 ./my_program
                taskset -c 0-2,5,7 ./my_program
    Expected Output: Print the thread IDs and CPU numbers.
