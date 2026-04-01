Assignment 1 — Parallel Log Processing


Implement a program that processes 20000 log entries in parallel.

Each log entry contains:

    request_id
    user_id
    response_time_ms

Inside a parallel region with 4 threads:

    Use single to initialize all logs.
    Use a barrier to ensure all threads wait until logs are fully initialized
    Use for to process logs in parallel:
        classify each log as:
            FAST (<100 ms)
            MEDIUM (100–300 ms)
            SLOW (>300 ms)
    Use another barrier to ensure all threads finish processing before the summary
    Use single to compute and print:
        number of FAST / MEDIUM / SLOW logs
        (sequentially, no reduction)

Assignment 2: Delivery Priority Update


Implement a program for 10000 delivery orders.

Each order has:

    order_id
    distance_km
    priority

Inside a parallel region with 4 threads:

    Use single to set one common rule for all orders:
    for example, a distance threshold such as 20 km.
    Use for to process orders in parallel:
        if distance_km < threshold set priority to HIGH
        otherwise set priority to NORMAL
    Use a barrier to ensure all priorities are assigned
    Use single to print a message that the priority assignment is finished.
    Use another for to process orders again in parallel:
        count how many orders in each thread have HIGH priority
        store the result in an array thread_high_count[4]
    Use another barrier ensures all per-thread counts are ready
    Use single to print the values from thread_high_count[ ] and the total number of HIGH priority orders.
