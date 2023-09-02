# SimpleEvents

This library contains simple event classes useful in multithreading development.

## SimpleEvent class

Encapsulate a simple abstraction "conditional variable with state".

## ManualResetEvent class

Provide additional functionality to wait for multiple events.

## SynchronizedQueue class

A queue which could be stopped.
pop() method returns optional which is empty after stop.
push() method return its argument back if the queue is already stopped.
