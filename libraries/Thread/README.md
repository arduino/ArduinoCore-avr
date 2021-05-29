# Thread

It is simplest implementation of iterative multithredaing for AVR8 microcontrollers.
      You can install this library by the library manager Arduino also. 

Using
-----

See example for how to.

#### functions

`spawn(..)`	create new thread with function and unique stack resources

`yield()`	switch execution to next thread immediately

`hold()`	disable yield() and time division functionality

`schedule()`	enable yield() functionality only (switching points)

`quantize()`	enable time division of the execution flows and yield()

`grab(..)`	implement mutual exclusion barrier lock (semaphore)

`loose(.)`	unlock barrier what has been locked by `grab(..)` call

#### constant

`MAIN`		pointer to dummy stack of the main root thread

#### variables

`thread`	dynamic pointer on the process stack structure
		what can be used for reentrant and identification


