#include <iostream>   // For input/output operations
#include <string>     // For using string operations
#include <iomanip>    // For std::setw and std::setfill
#include <vector>     // For using the vector container
#include <algorithm>  // For algorithms like sort, find, etc.
#include <cmath>      // For mathematical functions
#include <sstream>
#include <cstdlib>
using namespace std;

struct Process{
    string process_name;
    int arrival_time;
    int start_time;
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
        // finish time is found out after running
        struct_Processes[i] = curr_process;
    }

    return struct_Processes;
}

Process select_process(vector<Process> all_processes,int ps_in_bag,int *current_index){
    int ret_index = *current_index;
    // ps_in_bag are the processes that have arrived
    // ps_in_bag max is num_processes
    *current_index = (*current_index + 1) % ps_in_bag;
    return all_processes[ret_index];
}

vector<Process> run_rr(vector<string> string_processes,int num_processes,int last_instant){
    // we need the vector to have the n processes, with info on where they ran?
    vector<Process> all_processes = get_Processes_as_struct(string_processes,num_processes);

    // Sort the processes based on arrival_time
    sort(all_processes.begin(), all_processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

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
        cerr << "Usage: ./fcfs <command> <policy_code> <last_instant> <num_processes> <processes>\n";
        return 1;
    }
    
    // Convert arguments to std::string
    string command = argv[1];
    string policy_code = argv[2];
    int last_instant = stoi(argv[3]);
    int num_processes = stoi(argv[4]);
    string process_string = argv[5];
    
    cout << "command: "  << command << endl;

    cout << "policy code: " << policy_code << endl;
    
    int polciy_number;
    int quantum;
    get_policy_number(policy_code,&polciy_number,&quantum);

    cout << "policy Number: " << polciy_number << endl;

    cout << "quantum: " << quantum << endl;

    cout << "last_instant: "  << last_instant << endl;

    cout << "num_processes: "  << num_processes << endl;
        
    cout << "process_string: " << process_string << endl;

    vector<string> process_string_vector = delimit(process_string,'-');
    for(int i =0;i<(int)process_string_vector.size();i++){
         cout << process_string_vector[i] << endl;
    }
    
    vector<Process> rr_processes = run_rr(process_string_vector,num_processes,last_instant); // we dont need last_instant?


    // cout << "Processes struct: " << endl;
    // for(int i = 0;i<num_processes;i++){
    //     cout << fcfs_processes[i].process_name << endl;
    //     cout << fcfs_processes[i].arrival_time << endl;
    //     cout << fcfs_processes[i].service_time << endl;
    // }
    
    // if(command == "trace"){
    //     // cout << "We tracing bois" << endl;
    //     trace_ft(last_instant,num_processes,fcfs_processes);
    // }else if(command == "stats"){
    //     // cout << "gimme them stats" << endl;
    //     stats_ft(num_processes,fcfs_processes);
    // }
    

    return 0;
}
