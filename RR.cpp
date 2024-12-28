#include <iostream>   // For input/output operations
#include <string>     // For using string operations
#include <iomanip>    // For std::setw and std::setfill
#include <vector>     // For using the vector container
#include <algorithm>  // For algorithms like sort, find, etc.
#include <cmath>      // For mathematical functions
#include <sstream>
#include <cstdlib>
#include <queue>
using namespace std;

struct Process{
    string process_name;
    int arrival_time;
    int start_time; // start time is not useful here?
    int service_time;
    int finish_time;
    int time_served_so_far; // starts with 0 and when it reaches service time it is done
};


vector<string> delimit(string to_delimit, char delimiter){
    vector<string> result;
    stringstream ss(to_delimit);
    string token;

    // getline with a delimiter -> gets you everything till delimeter,
    // and continues until you hit end of stringstream aka \0 ig
    // token is the string we got rn
    while (getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

void get_policy_number(string policy_code,int *policy_number,int* q){
    if(policy_code.length() > 1){
        vector<string> components = delimit(policy_code,'-');
        *policy_number = stoi(components[0]);
        *q = stoi(components[1]);
    }
    else{
        *policy_number = stoi(policy_code);
        *q = 0; // to know it isnt usable
    }
}

vector<Process> get_Processes_as_struct(vector<string> processes,int num_processes){
    vector<Process> struct_Processes(num_processes);
    
    for(int i = 0;i<num_processes;i++){
        vector<string> curr_process_params(3);
        curr_process_params = delimit(processes[i],',');
        Process curr_process;
        curr_process.process_name = curr_process_params[0];
        curr_process.arrival_time = stoi(curr_process_params[1]);
        curr_process.service_time = stoi(curr_process_params[2]);
        curr_process.time_served_so_far = 0;
        // finish time is found out after running
        struct_Processes[i] = curr_process;
    }

    return struct_Processes;
}

Process* select_process(queue<Process*> * ready_queue){
    if (ready_queue->empty()) { // no process is ready
        // shouldnt stop execution tho
        // should not come here if ready queue is empty
        throw std::runtime_error("Error: Ready queue is empty, no process to select.");
    }

    // Get the process at the front of the queue
    Process*  selected_process = ready_queue->front();
    
    // Remove the selected process from the queue
    ready_queue->pop();
    
    return selected_process;
}

bool hasProcess(queue<Process*>& processQueue, const Process& target) {
    // Temporary container to store queue elements for iteration
    deque<Process*> tempQueue;

    // Iterate through the queue
    bool found = false;
    while (!processQueue.empty()) {
        Process* current = processQueue.front();
        processQueue.pop();

        // Check if the current process matches the target
        if (current->process_name == target.process_name) { // we find if it is in queue using name
            found = true;
        }
        
        // Store the process in the temporary queue
        tempQueue.push_back(current);
    }

    // Restore the original queue from the temporary container
    for (const auto& p : tempQueue) {
        processQueue.push(p);
    }

    return found;
}

void serveProcess(Process* current_process_running, int time) {
    // Increment time served so far
    current_process_running->time_served_so_far++;

    // Print serving message
    //cout << "SERVING : " << current_process_running.process_name << endl;

    // Check if the process has finished
    if (current_process_running->time_served_so_far == current_process_running->service_time) {
        // Process is done
        current_process_running->finish_time = time + 1;
        //cout << "Process " << current_process_running->process_name << " just finished at time " << current_process_running->finish_time << std::endl;
    }
}

// Update the trace matrix based on the ready queue and the running process
void updateTrace(vector<vector<char>>& trace, 
                 const vector<Process>& all_processes, 
                 const Process& current_process_running, 
                 queue<Process*> ready_queue, 
                 int time, 
                 int num_processes) {
    //cout << std::endl;
    //cout << "-------------------------------------------" << std::endl;

    // Iterate through all processes
    for (int p = 0; p < num_processes; ++p) {
        // Check if process is in the ready queue
        if (hasProcess(ready_queue, all_processes[p])) {
            // Process is in ready queue
            //cout << all_processes[p].process_name << " is in ready queue" << std::endl;
            trace[p][time] = '.';  // Update trace to show waiting
            //cout << all_processes[p].process_name << " has been shown ." << std::endl;
        } 
        // Check if the process is currently running
        else if (current_process_running.process_name == all_processes[p].process_name) {
            trace[p][time] = '*';  // Update trace to show running
            //cout << current_process_running.process_name << " has been shown *" << std::endl;
        } 
        // Process is not in ready queue or running
        else {
            trace[p][time] = ' ';  // No change (space)
        }
    }

    //cout << "-------------------------------------------" << endl << endl;
}


vector<vector<char>> run_rr(vector<Process>& all_processes,int num_processes,int last_instant,int quantum){
    int next_process_to_be_ready = 0; // because we got the processes sorted with arrival time
    // Create a 2D vector
    vector<vector<char>> trace(num_processes, vector<char>(quantum, ' '));
    // ready queue
    queue<Process*> ready_queue;
    Process null_process;
    null_process.process_name = "null";
    Process* current_process_running =& null_process;
    // need to initialize current process somehow
    for(int time=0;time<=last_instant;time++){

        //cout << "time: " << time << endl;
        //cout << "current_process: " << current_process_running.process_name << endl;
        // for each time quantum we need to check things
        // anything to add to ready queue? anything arrived
        while(next_process_to_be_ready < num_processes && all_processes[next_process_to_be_ready].arrival_time == time){
            //cout << "TIME: " << time << endl;
            //cout << "adding to queue: " << endl;
            //cout << all_processes[next_process_to_be_ready].process_name << endl;
            //cout << "arrival at " << all_processes[next_process_to_be_ready].arrival_time << endl;
            ready_queue.push(&all_processes[next_process_to_be_ready]);
            next_process_to_be_ready++;
        }
        

        if(current_process_running->process_name != "null"){ // there is a currently running process
            // are we done with CURRENT process?

            if(current_process_running->time_served_so_far < current_process_running->service_time){
                // still not done
                // check quantum
                if(current_process_running->time_served_so_far%quantum == 0){
                    // it's time is done
                    // quantum finished
                    ready_queue.push(current_process_running); // preempt process
                    //cout << "We prempted " << current_process_running.process_name << endl;
                    // take another
                    
                    current_process_running = select_process(&ready_queue); // if it is the only one then np
                    //cout << "AND TOOK " << current_process_running.process_name << endl;
                    serveProcess(current_process_running,time); // serve the new process
                    

                } else{
                    // my quantum is not finished AND am not done
                    // then contune yourself 3adi - cant preempt
                    // continue ?
                    serveProcess(current_process_running,time); 

                }
            } else{
                //dome, give me next process --
                // what if there is no process? we go next 3adi
                if(ready_queue.empty()){
                    // no process?
                    current_process_running = &null_process;
                    //cout << "NOTHING IN READY QUQUUE THO" << endl;
                    
            // update trace?

                    updateTrace(trace,all_processes,*current_process_running,ready_queue,time,num_processes);
        
                    continue;
                    // no process to serve
                }
                //curr process done and next is up
                current_process_running = select_process(&ready_queue);
                serveProcess(current_process_running,time);
            }
        }else{
            // no process running rn
            if(!ready_queue.empty()){ // 
                // smth ready
                // this line is extra
                current_process_running = select_process(&ready_queue);
                //cout << "current_process: " << current_process_running.process_name << endl;
                // if first time, this is start time?
                serveProcess(current_process_running,time);
            }
            else{
                // nothing is ready and we have nothing currently running
                //cout << " // nothing is ready and we have nothing currently running" << endl;
                //continue;
            }
        }


        // dont do continue to update trace vector

        // we update trace
        // every process is space
        // except those in ready queue is .
        // and current_process_running is *
        
        updateTrace(trace,all_processes,*current_process_running,ready_queue,time,num_processes);       
    }

    // loop throught all_process, increment i when quantum reaached
    // at EACH quantum we have to check if curr process finished
    // if still we select a new one
    // which one to select?



    // can have struct as a parameter and update it accordingly -> stats ez
    // can return a list
    // each entry is a process name and start time and end time -> basically its size is 
    // last_instant/quantum   ?
    // in tracing we loop through each process
    // process A
    // then loop through tracing list and if rn is A we show it is running,
    // if it isnt we show it is waitng given it arrived
    return trace;
}

string pad(string printme,int totalWidth){
    // Calculate padding
    int leftPadding = (totalWidth - printme.size()) / 2;
    int rightPadding = totalWidth - printme.size() - leftPadding;
    string padded;
    // Print formatted output
    padded = string(leftPadding, ' ') + printme + string(rightPadding, ' ');  // Add right padding
    return padded;
}

string generatePaddedString(string printme,int totalwidth) {
    ostringstream oss;
    oss << printme;
    string base = oss.str();
    
    if ((int)base.length() < totalwidth) {
        // Add spaces to the right until the length is 6
        base.append(totalwidth - base.length(), ' ');
    }
    
    return base;
}

void trace_ft(vector<vector<char>> &trace,vector<Process> processes ,int num_processes, int last_instant,int quantum){
    string instants_string = generatePaddedString("RR-"+to_string(quantum),6);

    string dashes = "------"; // 6 till first instant
    for(int i = 0;i<=last_instant;i++){
        instants_string += to_string(i % 10) + " ";
        dashes += "--";
    }
    cout << instants_string << endl;
    cout << dashes << endl;

    for(int i =0;i<num_processes;i++){
        // for each process
        string process_line = processes[i].process_name + "     "; // 5 spaces
        trace[i][last_instant] = ' '; // end is space
        for(int j=0;j<=last_instant;j++){ // for each we add a | and smth
            process_line += "|";
            string character = string(1, trace[i][j]);

            process_line += character;
        }
        cout << process_line << endl;
    }
    cout << dashes << endl;
    cout << endl;
    // do we need another endl?
}

string getPaddedfloat(double value,int totalWidth, int precision) {
    ostringstream oss;
    oss << setw(totalWidth) // Total width for the number (including decimal places)
        << fixed << setprecision(precision) // Fixed-point notation with 2 decimal places
        << value;
    return oss.str();
}


void stats_ft(int num_processes,vector<Process> processes,int quantum){
    double avg_taround = 0;
    double avg_normturn = 0;
    cout << "RR-" << to_string(quantum) << endl;
    
    string process_line =    "Process    |";
    string arrival_line =    "Arrival    |";
    string service_line =    "Service    |";
    string finish_line =     "Finish     |";
    string turnaround_line = "Turnaround |"; 
    string normturn_line  =  "NormTurn   |";
    
    for(int i=0;i<num_processes;i++){
        int arrival = processes[i].arrival_time;
        int service = processes[i].service_time;
        //int start = processes[i].start_time;
        int finish = processes[i].finish_time;
        process_line += pad(processes[i].process_name,5) + "|";
        arrival_line += pad(to_string(arrival),5) + "|";
        service_line += pad(to_string(service),5) + "|";
        finish_line += pad(to_string(finish),5) + "|";
        int turnaround = finish-arrival;
        avg_taround += turnaround;
        turnaround_line += pad(to_string(turnaround),5) + "|";
        double normturn = (double)turnaround/service;
        avg_normturn += normturn;
        normturn_line += getPaddedfloat(normturn,5,2) + "|";
    }
    cout << process_line << endl;
    cout << arrival_line << endl;
    cout << service_line    << " Mean|" << endl;
    cout << finish_line     << "-----|" << endl;
    avg_taround /= num_processes;
    cout << turnaround_line << getPaddedfloat(avg_taround,5,2) << "|" << endl;
    avg_normturn /= num_processes;
    cout << normturn_line << getPaddedfloat(avg_normturn,5,2) << "|" << endl;
    cout << endl;
}   


// each policy needs:
// command -string-
// policy code -string-
// last_instatn thing
// num_processes -int-
// processes -vector<Process>-
// cant send a vector so will send a string of processes 
// and make it as a vector here
int main(int argc, char const *argv[]) {
    if(argc != 6){
        cerr << "Usage: ./rr <command> <policy_code> <last_instant> <num_processes> <processes>\n";
        return 1;
    }
    
    // Convert arguments to std::string
    string command = argv[1];
    string policy_code = argv[2];
    int last_instant = stoi(argv[3]);
    int num_processes = stoi(argv[4]);
    string process_string = argv[5];
    
    //cout << "command: "  << command << endl;

    // cout << "policy code: " << policy_code << endl;
    
    int polciy_number;
    int quantum;
    get_policy_number(policy_code,&polciy_number,&quantum);

    // cout << "policy Number: " << polciy_number << endl;

    // cout << "quantum: " << quantum << endl;

    // cout << "last_instant: "  << last_instant << endl;

    // cout << "num_processes: "  << num_processes << endl;
        
    // cout << "process_string: " << process_string << endl;

    vector<string> process_string_vector = delimit(process_string,'-');
    // for(int i =0;i<(int)process_string_vector.size();i++){
    //      cout << process_string_vector[i] << endl;
    // }
    
    vector<Process> all_processes = get_Processes_as_struct(process_string_vector,num_processes);
    // Sort the processes based on arrival_time
    sort(all_processes.begin(), all_processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<vector<char>> trace = run_rr(all_processes,num_processes,last_instant,quantum); // we dont need last_instant?
    //printTrace(trace,num_processes,last_instant);
    //cout << "Finish of first one "<< all_processes[0].finish_time << endl;
    //trace_ft(trace,all_processes,num_processes,last_instant,quantum);
    // cout << "Processes struct: " << endl;
    // for(int i = 0;i<num_processes;i++){
    //     cout << fcfs_processes[i].process_name << endl;
    //     cout << fcfs_processes[i].arrival_time << endl;
    //     cout << fcfs_processes[i].service_time << endl;
    // }
    
    if(command == "trace"){
        // cout << "We tracing bois" << endl;
        trace_ft(trace,all_processes,num_processes,last_instant,quantum);
    }else if(command == "stats"){
        // cout << "gimme them stats" << endl;
        stats_ft(num_processes,all_processes,quantum);
    }
    

    return 0;
}