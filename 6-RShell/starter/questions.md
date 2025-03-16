1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client know's when a commands full output has been recieved when it sees an EOF character at the end of a message. This way, we can repeatedly recieve messages in a loop until finding that EOF to avoid printing out partial output from each command, because the entire thing almost never comes in one call from recieve. 

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A shell working over the network needs to decide some convention for itself to understand when messages are complete. I think our shell uses good conventions, since commands will be 1 line of input, treating them as c strings and having the server wait for a null byte to find the end of the message works very well. Since many commands wll have longer output, they may at times contain multiple null bytes, so it was again a good dea to pick the EOF in this situation. Different problems can arise on both sides if these conditions aren't handeled. First of all if you somehow pick a bad condition to stop your messages, you could create infinite loops looking for a character that will never occur. Also, on the server side of things if you ignored trying to find a way to read complete messages and just treated each recv() call as a message you would probably attempt to execute incomplete commands and just recieve a bunch of errors back, and on the client side if you made the same mistake those errors would not print properly because you would again be handling incomplete messages. 

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols have state or in other words have a record of the past interactions or commands from a client, and therefore will have to act accordingly to that state of past interactions. Stateless protocols hold no data about past interactions, and therefore always behave the same way for any given interaction made in the same environment. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Part of the reason TCP is more reliable than UDP is because it makes more security checks as it passes messages along, and therefore it is slower. So, UDP is a good idea to use in situations where you need really fast, low latency network connections, and especially when these situations can afford rare but not zero package losses, like video streaming and similar areas. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Linux provides sockets as a file abstraction to allow network communication between user programs. 