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
    int remaining_time; // i think i will leave remaining time for the reusability of the code
};

struct CompareServiceTime {
    bool operator()(const Process* a, const Process* b) const {
        return a->service_time > b->service_time; // Min-heap based on remaining_time
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

vector<Process> get_Processes_as_struct(vector<string> processes,int num_processes){
    vector<Process> struct_Processes(num_processes);
    
    for(int i = 0;i<num_processes;i++){
        vector<string> curr_process_params(3);
        curr_process_params = delimit(processes[i],',');
        Process curr_process;
        curr_process.process_name = curr_process_params[0];
        curr_process.arrival_time = stoi(curr_process_params[1]);
        curr_process.service_time = stoi(curr_process_params[2]);
        curr_process.remaining_time = curr_process.service_time;
        // finish time is found out after running
        struct_Processes[i] = curr_process;
    }

    return struct_Processes;
}

Process* select_process(priority_queue<Process*, vector<Process*>, CompareServiceTime> * pq){
    if (pq->empty()) { // no process is ready
        // shouldnt stop execution tho
        // should not come here if ready queue is empty
        throw std::runtime_error("Error: Priority Queue is empty, no process to select.");
    }

    // Get the process at the front of the queue
    Process*  selected_process = pq->top();
    
    // Remove the selected process from the queue
    pq->pop();
    
    return selected_process;
}

void serveProcess(Process* current_process_running, int time) {
    // Increment time served so far
    current_process_running->remaining_time--;

    // Print serving message
    //cout << "SERVING : " << current_process_running.process_name << endl;

    // Check if the process has finished
    if (current_process_running->remaining_time == 0) {
        // Process is done
        current_process_running->finish_time = time + 1;
        //cout << "Process " << current_process_running->process_name << " just finished at time " << current_process_running->finish_time << std::endl;
    }
}


// i think we can remove has process?
bool hasProcess(priority_queue<Process*, vector<Process*>, CompareServiceTime>& pq, const Process& target) {
    // Temporary storage for elements
    std::vector<Process*> tempQueue;
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


void updateTrace(vector<vector<char>>& trace, 
                 const vector<Process>& all_processes, 
                 const Process& current_process_running, 
                 priority_queue<Process*, vector<Process*>, CompareServiceTime> pq, 
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


vector<vector<char>> run_spn(vector<Process>& all_processes,int num_processes,int last_instant){
    int next_process_to_be_ready = 0; // because we got the processes sorted with arrival time
    // Create a 2D vector
    vector<vector<char>> trace(num_processes, vector<char>(last_instant, ' '));

    // Min-heap of Process, ordered by remaining_time
    // we use the pq to get the srt if ready processes
    priority_queue<Process*, vector<Process*>, CompareServiceTime> pq; 

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
        
        // at each time quantum, we get the srt
        if(current_process_running->process_name != "null" && (current_process_running->remaining_time > 0)){
            // there is currently running process
            
            serveProcess(current_process_running,time);
        }
        else { // no currently running process
            if(!pq.empty()){
                // smth ready
                current_process_running = select_process(&pq);
                serveProcess(current_process_running,time);
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

void trace_ft(vector<vector<char>> &trace,vector<Process> processes ,int num_processes, int last_instant){
    string instants_string ="SPN   ";

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


void stats_ft(int num_processes,vector<Process> processes){
    double avg_taround = 0;
    double avg_normturn = 0;
    cout << "SPN" << endl;
    
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


int main(int argc, char const *argv[])
{
    if(argc != 6){
        cerr << "Usage: ./spn <command> <policy_code> <last_instant> <num_processes> <processes>\n";
        return 1;
    }
    
    // Convert arguments to std::string
    string command = argv[1];
    string policy_code = argv[2]; // wont make use of this here
    int last_instant = stoi(argv[3]);
    int num_processes = stoi(argv[4]);
    string process_string = argv[5];

    vector<string> process_string_vector = delimit(process_string,'-');
    // for(int i =0;i<(int)process_string_vector.size();i++){
    //     cout << process_string_vector[i] << endl;
    // }

    vector<Process> all_processes = get_Processes_as_struct(process_string_vector,num_processes);
    
    // Sort the processes based on arrival_time
    //idk if we sorting here or whatttt

    vector<vector<char>> trace = run_spn(all_processes,num_processes,last_instant); // we dont need last_instant?
    
    if(command == "trace"){
        // cout << "We tracing bois" << endl;
        trace_ft(trace,all_processes,num_processes,last_instant);
    }else if(command == "stats"){
        // cout << "gimme them stats" << endl;
        stats_ft(num_processes,all_processes);
    }
    
    
    return 0;
}
