# FlexTimer
A simple library (for Arduino, but not only) to handle interrupts

Basically, the goal of this project is:
  To provide a very simple way to handle repeating actions (ex: a flashing light on an Arduino board), 
  especially on systems where you don't have elaborate interrupt options (think Arduino vs Linux)
  
I also had the following constraints in mind:
  - use as little memory as possible, yet provide real functionalities
  - be flexible on the following points:
     - memory handling scheme (malloc, fixed array, mixed)
     - time handling: you can choose the time units you want to use (ms, us, s) with little effort
     - loop handling: you have a few options here
     - modify the delay between 2 repeats is possible
  - being able to handle time/integer overflow, so you don't have to! 
    (ex: microseconds in a 32 bits environment start over at 0 after about 70 minutes: you don't want to care about that: the library does it for you)
  - real added value: it will free you of thinking of a lot of problems
  - being easy to use on modern systems with little effort (ex: on Unix systems)
  - it can run on modern systems too, and be quite efficient
  - putting processor in wait mode to prevent electric consumption 
  - it is not meant to be multi-threaded (and will probably never be), or you need just one thread to handle the timers...
  - the granularity of the calls is the same than the precision of time function (ex: if your unit is the ms, your code will not do anything more precise that 1ms)
  - reasonably easy customization (using #defines)
  - Important: it has to be executed in one thread
  - it is easy to adapt to other systems (ex: Unix)

Examples:
- Repeating an action every n ms
- You want to flash a light 10 times, then stop.
- Do one thing after some time!
- Repeating many different actions at different speeds without caring about what comes next

It is meant to be extremely simple, but also efficient.

Implementations details:
------------------------
The FlexTimer library uses a chained list to neatly handle the future events. It always knows what are the next events to come and when they'll come. 

The precision of the ticks is absolutely not guaranteed, but it is not that bad, after all, and coding it by hand is not going to be any better anyway!

If several ticks happen on the exact same time (ex: on the same millisecond), they will be ordered based on the delay from smallest to biggest.

Version 1
---------
I want 'verion 1' to stay as it is: the set of tools you have is minimalistic, but it makes sense to not add anything: it is meant for simplicity and you don't want it to use too much space (ex: on Arduino with 30kb of RAM)...

Maybe what's needed is a few examples of memory handling and time handling (with different granularities)

There may be some bugs, even if I couldn't find any, but feel free to let me know of any problems!

The code uses maybe 5kb, but if you deactivate FT_PARANOIA (that will remove the "asserts"), you can probably go down to 3...

Summary of functionalities:
- repeat some action (once, a certain number of times, forever)
- handle 32-bit overflow automatically
- millisecond granularity is default (but you can change this by recompiling with different parameters)
- meant for Arduino (but works fine with Unix too: you need to recompile)
- some control over alignment of tasks (you can desynchronize tasks)
- some control over modification of timers
- monothread-oriented: everything has to be made from one thread (but you can use other threads to do heavy calculation for example)

Limitation:
- biggest delay is theoretically limited to 2^31-1, but this is a dangerous limit: miss one ms and it is broken: that's why we use the value (2^31-1)/3

Ideas to improve it:
- Making it simpler to choose a granularity (say: I want 1 unit = 15ms, with as long as is possible values)
- Error handling (better than just returning NULL? Do we need that? No)
- Desynchronization: force start of timers to be non-aligned so ticks never happen exactly at the same time: the algorithms to do this feature are non-trivial and probably quite big: use of randomize_all_timers() or spread_all_timers() is probably as effective, and much more lighter!

Version 2
---------
That's the place where you could implement something more elaborate.

Ideas to be implemented:
- Activation/deactivation of a timer
- Priorities: if two timers fire at very same time, the one whith the biggest priority fires first
- More options to control the behaviour of timers, like:
  - Choose between catch up when you missed a tick or no catch up
  - Choose between remove a timer when finished or deactivate
- C++ interface
- Synchronizing two timers (ex: A happens 10ms after B, always)
- 64-bit precision for delays (having big and small granularity at the same time)
- Make it more precise (it's already not bad at all with a fine granularity)
- Modify a timer: already possible
  - Being able to start at some time, stop at some time...
- Make it suitable for multithreading: no: not needed: you don't want to
- have "system" timers for special/internal use: not really needed

Some of these features sound like something really advanced: I would implement them in some kind of "Event" library and keep the FlexTimer simple!

I think 64-bit timers on 32-bit time handling system is a really good idea, but a little complicated for version 1.

Other version:
--------------
A C++ version is needed and believe me, it's going to be great!

I've started working on a C++ version, which is promissing! Check https://github.com/lapalisse/LTimer

PS: I've got an even newer library on that subject which will be made available in github... Interested? contact me at ludovic.bertsch@gmail.com
--
Copyright 2015 Ludovic BERTSCH, ludovic.bertsch@gmail.com
