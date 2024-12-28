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
    int initial_priority;
    int curr_priority;
    int remaining_time;
    int time_served_so_far;
    int wait_time; // i dont think that's needed?
};

// struct ComparePriority {
//     bool operator()(const Process* a, const Process* b) const {
//         return a->curr_priority < b->curr_priority; // max heap
//     }
// };

struct ComparePriority {
    bool operator()(const Process* a, const Process* b) const {
        if (a->curr_priority == b->curr_priority) {
            return a->wait_time < b->wait_time; // Prefer larger start_time
        }
        return a->curr_priority < b->curr_priority; // Max heap based on curr_priority
    }
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
        curr_process.initial_priority = stoi(curr_process_params[2]);
        curr_process.curr_priority = curr_process.initial_priority;
        curr_process.time_served_so_far = 0;
        curr_process.wait_time = 0;
        // no remaining time cuz no service time
        // finish time is found out after running
        struct_Processes[i] = curr_process;
    }

    return struct_Processes;
}

Process* select_process(priority_queue<Process*, vector<Process*>, ComparePriority> * pq){
    if (pq->empty()) { // no process is ready
        // shouldnt stop execution tho
        // should not come here if ready queue is empty
        throw runtime_error("Error: Priority Queue is empty, no process to select.");
    }

    // Get the process at the front of the queue
    Process*  selected_process = pq->top();
    
    // Remove the selected process from the queue
    pq->pop();
    
    return selected_process;
}

void update_priority(priority_queue<Process*, vector<Process*>, ComparePriority>& pq){ // i do not think we have to send the reference of the pq
    // everything in priority queue is waiting
    vector<Process*> tempQueue;

    // Extract elements from the priority queue
    while (!pq.empty()) {
        Process* current = pq.top();
        pq.pop();

        current->curr_priority++; 
        current->wait_time++;
        //cout << "PRIORITY OF " << current->process_name << " in ready is " << current->curr_priority << endl;
        // Store the process in the temporary container
        tempQueue.push_back(current);
    }

    // Reinsert elements back into the priority queue
    for (Process* p : tempQueue) {
        pq.push(p);
    }

    // if we send value of pq then np
    // we can remove this loop above ^

}

// i think we can remove has process?
bool hasProcess(priority_queue<Process*, vector<Process*>, ComparePriority>& pq, const Process& target) {
    // Temporary storage for elements
    vector<Process*> tempQueue;
    bool found = false;

    // Extract elements from the priority queue
    while (!pq.empty()) {
        Process* current = pq.top();
        pq.pop();

        // Check if the current process matches the target
        if (current->process_name == target.process_name) {
            found = true;
        }

        // Store the process in the temporary container
        tempQueue.push_back(current);
    }

    // Reinsert elements back into the priority queue
    for (Process* p : tempQueue) {
        pq.push(p);
    }

    return found;
}

void serveProcess(Process* current_process_running) {
    // Increment time served so far
    current_process_running->time_served_so_far++;
    current_process_running->wait_time = 0;
}



void updateTrace(vector<vector<char>>& trace, 
                 const vector<Process>& all_processes, 
                 const Process& current_process_running, 
                 priority_queue<Process*, vector<Process*>, ComparePriority> pq, 
                 int time, 
                 int num_processes) {
    //cout << std::endl;
    //cout << "-------------------------------------------" << std::endl;

    // Iterate through all processes
    for (int p = 0; p < num_processes; ++p) {
        // Check if process is in the ready queue
        if (hasProcess(pq, all_processes[p])) {
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


vector<vector<char>> run_aging(vector<Process>& all_processes,int num_processes,int last_instant, int quantum){
    int next_process_to_be_ready = 0; // because we got the processes sorted with arrival time
    // Create a 2D vector
    vector<vector<char>> trace(num_processes, vector<char>(last_instant, ' '));

    // Min-heap of Process, ordered by remaining_time
    // we use the pq to get the srt if ready processes
    priority_queue<Process*, vector<Process*>, ComparePriority> pq; 

    Process null_process;
    null_process.process_name = "null";
    Process* current_process_running = &null_process;


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
            pq.push(&all_processes[next_process_to_be_ready]);
            next_process_to_be_ready++;
        }
        
        
        // at each time quantum, we get the one with highest ratio
        if(current_process_running->process_name != "null"){
            // there is currently running process
            // process doesnt end so nn to check if it is done or not
            
            if(current_process_running->time_served_so_far%quantum == 0){ // can start at awkward time so better use timeservedsofar
                // it's time is done
                // quantum finished
                // scheduler called






                //set priority of currently to old
                current_process_running->curr_priority = current_process_running->initial_priority;
                // increment ready
                //cout << "Time: " << time << endl;
                //cout << "current process: " << current_process_running->process_name << " is " << current_process_running->curr_priority << endl;
                
                update_priority(pq);
                //cout << endl;
                // choose from eligibles
                pq.push(current_process_running); // preempt process
                current_process_running = select_process(&pq); // if it is the only one then np
                //cout << "AND TOOK " << current_process_running.process_name << endl;
            }
            // serve in both cases, differs on who served only
            serveProcess(current_process_running);
        }
        else { // no currently running process
            if(!pq.empty()){
                // smth ready
                //set old
                //mfeesh
                //update all ready
                // cout << "Time: " << time << endl;
                // update_priority(pq);
                // cout << endl;
                //choose from eligibles
                current_process_running = select_process(&pq);
                serveProcess(current_process_running);
            }else{
                current_process_running = &null_process; // lazem 3ashan f update trace
            }
        }
        
        updateTrace(trace,all_processes,*current_process_running,pq,time,num_processes);       
        
    }

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

void trace_ft(vector<vector<char>> &trace,vector<Process> processes ,int num_processes, int last_instant){ // nn quantum here
    string instants_string ="Aging ";

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

int main(int argc, char const *argv[])
{
if(argc != 6){
        cerr << "Usage: ./aging <command> <policy_code> <last_instant> <num_processes> <processes>\n";
        return 1;
    }
    
    // Convert arguments to std::string
    string command = argv[1];
    string policy_code = argv[2];
    int last_instant = stoi(argv[3]);
    int num_processes = stoi(argv[4]);
    string process_string = argv[5];
    
    // cout << "command: "  << command << endl;

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
    

    vector<vector<char>> trace = run_aging(all_processes,num_processes,last_instant,quantum); // we dont need last_instant?
    
    if(command == "trace"){
        // cout << "We tracing bois" << endl;
        trace_ft(trace,all_processes,num_processes,last_instant);
    }

    return 0;
}