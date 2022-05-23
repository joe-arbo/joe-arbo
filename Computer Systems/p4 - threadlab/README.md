# Project 4

In Project 4 we created a "threadpool" which is a collection of workers that can grab, execute, and return from tasks in a task pool. It's a multi-threaded operation and exposed us to thread-safe operations (i.e., mutexes, locks, and semaphores). We were also tasked with implementing work stealing, meaning that each thread could grab remaining calculations from other threads to complete tasks quicker. Each task was not locked to one thread this way. 

---

### Student Information
Joseph Arbolino<br>
Rio Cross

---

### How to Execute the Shell
- Run 'make'
- Run './threadpool'