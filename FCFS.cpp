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


// vector<Process> get_Processes_as_struct(vector<string> processes,int num_processes){
//     vector<Process> struct_Processes(num_processes);
    
//     for(int i = 0;i<num_processes;i++){
//         vector<string> curr_process_params(3);
//         curr_process_params = delimit(processes[i],',');
//         Process curr_process;
//         curr_process.process_name = curr_process_params[0];
//         curr_process.arrival_time = stoi(curr_process_params[1]);
//         curr_process.service_time = stoi(curr_process_params[2]);
//         struct_Processes[i] = curr_process;
//     }

//     return struct_Processes;
// }



string pad(string printme,int totalWidth){
    // Calculate padding
    int leftPadding = (totalWidth - printme.size()) / 2;
    int rightPadding = totalWidth - printme.size() - leftPadding;
    string padded;
    // Print formatted output
    padded = string(leftPadding, ' ') + printme + string(rightPadding, ' ');  // Add right padding
    return padded;
}

string getPaddedfloat(double value,int totalWidth, int precision) {
    ostringstream oss;
    oss << setw(totalWidth) // Total width for the number (including decimal places)
        << fixed << setprecision(precision) // Fixed-point notation with 2 decimal places
        << value;
    return oss.str();
}


vector<Process> run_fcfs(vector<string> string_processes,int num_processes){
    vector<Process> processes(num_processes);
    
    for(int i = 0;i<num_processes;i++){
        vector<string> curr_process_params(3);
        curr_process_params = delimit(string_processes[i],',');
        Process curr_process;
        curr_process.process_name = curr_process_params[0];
        curr_process.arrival_time = stoi(curr_process_params[1]);
        curr_process.service_time = stoi(curr_process_params[2]);
        processes[i] = curr_process;
    }
    // need start and finish time now
    
    
    // need to get out of this function with each process' 
    // START and FINISH and we also want arrival
    // FINISH = START + SERVICE
    // so we just need to find out the START for eaach
    // START >= ARRIVAL
    // DEPENDS ON THE FINISH OF THE PREVIOUS process

    // Sort the processes based on arrival_time
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    //process 1 start time is arrival time
    processes[0].start_time = processes[0].arrival_time;
    processes[0].finish_time = processes[0].start_time + processes[0].service_time;
    for(int i =1;i<num_processes;i++){
        // start time of next is its arrival OR finish of previous
        if(processes[i-1].finish_time <= processes[i].arrival_time) // finished before it came then start at arrival
        {
            processes[i].start_time = processes[i].arrival_time;
            processes[i].finish_time = processes[i].start_time + processes[i].service_time;
        }
        else{
            processes[i].start_time = processes[i-1].finish_time;
            processes[i].finish_time = processes[i].start_time + processes[i].service_time;
        }
        
    }

    return processes;
}


void trace_ft(int last_instant,int num_processes,vector<Process> processes){
    string instants_string = "FCFS  ";
    string dashes = "------"; // 6 till forst instant
    for(int i = 0;i<=last_instant;i++){
        instants_string += to_string(i % 10) + " ";
        dashes += "--";
    }
    cout << instants_string << endl;
    cout << dashes << endl;

    for(int i =0;i<num_processes;i++){
        // for each process
        string process_line = processes[i].process_name + "     "; // 5 spaces
        int arrival = processes[i].arrival_time;
        //int service = processes[i].service_time;
        int start = processes[i].start_time;
        int finish = processes[i].finish_time;
        
        for(int j=0;j<=last_instant;j++){ // for each we add a | and smth
            process_line += "|";
            // we add a 
            // * if running
            // . if waiting
            // space if non existent
            if(j < arrival) // we still don't exist
            {
                process_line+=" ";
            }else if(j < start){ // we exist -- j >= arrival
                process_line+=".";
            } else if(j < finish){ // j >= arrival and j >=start
                process_line+="*";
            }else{
                process_line+=" ";
            }
        }
        cout << process_line << endl;
    }
    cout << dashes << endl;
    cout << endl;
    // do we need another endl?
}

void stats_ft(int num_processes,vector<Process> processes){
    double avg_taround = 0;
    double avg_normturn = 0;
    cout << "FCFS" << endl;
    
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
        cerr << "Usage: ./fcfs <command> <policy_code> <last_instant> <num_processes> <processes>\n";
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

    // cout << "last_instant: "  << last_instant << endl;

    // cout << "num_processes: "  << num_processes << endl;
        
    // cout << "process_string: " << process_string << endl;

    vector<string> process_string_vector = delimit(process_string,'-');
    // for(int i =0;i<(int)process_string_vector.size();i++){
    //     cout << process_string_vector[i] << endl;
    // }
    vector<Process> fcfs_processes = run_fcfs(process_string_vector,num_processes); // we dont need last_instant?


    // cout << "Processes struct: " << endl;
    // for(int i = 0;i<num_processes;i++){
    //     cout << fcfs_processes[i].process_name << endl;
    //     cout << fcfs_processes[i].arrival_time << endl;
    //     cout << fcfs_processes[i].service_time << endl;
    // }
    
    if(command == "trace"){
        // cout << "We tracing bois" << endl;
        trace_ft(last_instant,num_processes,fcfs_processes);
    }else if(command == "stats"){
        // cout << "gimme them stats" << endl;
        stats_ft(num_processes,fcfs_processes);
    }
    

    return 0;
}
