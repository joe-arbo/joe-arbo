# Concepts: What is sockfuzz?

AFL and AFL++ are excellent at what they do, but they have limitations. One such limitation is how AFL feeds input to the target program: it only works with programs that read from STDIN or from a file. In many cases, this is sufficient; lots of C programs take their input from STDIN or a file.
  
However, this project is about creating a HTTP server. Servers don't read input through a file or STDIN - they read from network sockets. So, the question becomes: how can we force a HTTP server to read input from STDIN, so we can fuzz it with AFL? Additionally, how can we do this without modifying your source code?
  
Sockstorm is a C library I developed to solve this problem. It works by "overloading" the `accept` system call and running some extra code to establish an internal connection to your server. Using the special `LD_PRELOAD` environment variable, it can convince your server to use sockfuzz's copy of `accept`, rather than the actual system call.

## Connection Multiplexing

Once called, sockstorm's version of the `accept` system call spawns a controller thread. This controller threads reads input via stdin, expecting a specific file format (dubbed the **comux** file format). These comux files are designed to specify the data to be sent to the target server across multiple connections. The controller thread parses the input file, then spawns individual threads to send "chunks" of data to the target server across specific connections.

This approach allows for multiple internal client connections to be made to your server, increasing the probability of finding multithreading-related bugs. As a bonus, it requires *zero* modification to your source code. All you have to do is prepend `LD_PRELOAD=/path/to/sockstorm-preload.so` to your command-line invocation of your server, then pipe one of these comux files to your process via stdin.

## AFL++ Custom Mutator

The other half of sockstorm is an AFL++ custom mutator. AFL++ does great when fuzzing many programs on its own, but for more complex file formats (such as the **comux** files being used here), a custom mutator can be implemented to ensure the file's structure doesn't get overwritten during fuzzing. Sockstorm's mutator (`sockstorm-mutator.so`) does just that; it maintains the structure of each comux file while also randomly modifying (fuzzing) the connection data to be sent to the target server.
