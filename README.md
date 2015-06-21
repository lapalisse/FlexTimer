# FlexTimer
A simple library (for Arduino, but not only) to handle interrupts

Basically, the goal of this project is:
  To provide a very simple way to handle repeating actions (ex: a flashing light on an Arduino board), 
  especially on systems where you don't have elaborate interrupt options (think Arduino vs Linux)
  
I also had the following constraints in mind:
  - use as little memory as possible
  - be flexible on the following points:
     - memory handling scheme (malloc, fixed array, etc.)
     - time handling: you can choose the time units you want to use (ms, us, s) with little effort
     - loop handling: you have a few options here
  - being able to handle time overflow, so you don't have to! 
    (ex: microseconds in a 32 bits environment start over in 70 minutes: you don't want to care about that)
  - real added value: it will free you of thinking of a lot of problems
  - being easy to use on modern systems with little effort (ex: on Unix systems)
  - it can run on modern systems too, and be quite efficient
  - putting processor in wait mode to prevent electric consumption 
  - it is not multi-threaded, and will probably never be



Examples:
1/ 
Repeating an action every ms
2/
You want to flash a light 10 times, then stop.

It is meant to be extremely simple, but also efficient.

Implementations details:
------------------------
The FlexTimer library uses a chained list to neatly handle the future events. It always knows what are the next events to come and when they'll come. 

The precision of the ticks is absolutely not guaranteed, but it is not that bad, after all, and coding it by hand is not going to be any better anyway!

If several ticks happen on the exact same time (ex: on the same millisecond), they will be ordered based on the delay from smallest to biggest.

Version 1
---------
I want 'verion 1' to stay as it is: the set of tools you have is minimalistic, but it makes sense to not add anything: it is meant for simplicity and you don't want it to use to much space (ex: on Arduino)...

Maybe what's needed is a few examples of memory handling and time handling (with different granularities)

There may be some bugs, even if I couldn't find any, but feel free to let me know of any problems!

The code uses maybe 5kb, but if you remove the "asserts", you can probably go down to 3...

Ideas to improve it:
- Making it simple to choose a granularity (say: I want 1 unit = 100ms, with as long as is possible values)
- ?

Version 2
---------
That's the place where you could implement something more elaborate.

Ideas to be implemented:
- Activation/deactivation of a timer
- More options to control the behaviour of timers, like:
  - Choose between catch up when you missed a tick or no catch up
  - Choose between remove a timer when finished or deactivate
- Make it more precise, when possible
- Modify a timer
- Make it suitable for multithreading
- 64-bit precision for delays (having big and small granularity at the same time)
- Synchronizing two timers (ex: A happens 10ms after B)
- etc.

Some of these features sound like something really advanced: I would implement them in some kind of "Event" library and keep the FlexTimer simple!

--
Copyright 2015 Ludovic BERTSCH, ludovic.bertsch@gmail.com
