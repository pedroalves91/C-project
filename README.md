# C-project

## Control and Monitoring of Processes and Communication

### The service should allow a user to submit successive tasks, each one being a sequence of commands linked anonymous pipes. In addition to starting the execution of the tasks, the service must be able to identify the tasks in execution, as well as the conclusion of their execution. The service must terminate tasks in execution, if there is no communication through pipes after a specified time.

Communication inactivity time => Command: -i n (n=time)<br>
Execution time => Command: -m n (n=time)<br>
Run a task => Command: -e p1|p2...|pn\n<br>
Listing tasks => Command: -l\n<br>
Listing history => Command: -r\n<br>
Exiting a task => Command: -t n (n == task id)
