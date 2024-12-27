#include <iostream>   // For input/output operations
#include <string>     // For using string operations
#include <vector>     // For using the vector container
#include <algorithm>  // For algorithms like sort, find, etc.
#include <cmath>      // For mathematical functions
#include <sstream>
#include <cstdlib>

using namespace std;

struct Process{
    string process_name;
    int arrival_time;
    int att2;
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


string get_policy_name(int policy_number) {
    vector<string> policy_names = {"fcfs","rr","spn","srt","hrrn","fb_1","fb_2i","aging"};

    if(policy_number < 1 or policy_number > 8){
        cerr << "Invalid Policy number";
        exit(1);
    }

    return policy_names[policy_number-1]; 
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
        curr_process.att2 = stoi(curr_process_params[2]);
        struct_Processes[i] = curr_process;
    }

    return struct_Processes;
}

string concat_strings(vector<string> processes, int num_processes){
    string result = "";
    for(int i =0;i<num_processes-1;i++){
        result += processes[i] + "-";
    }
    result+= processes[num_processes-1];
    return result;
}

int main() {
    string command,policies;
    int last_instant,num_processes;
    

    cin >> command;
    cin >> policies;

    vector<string> policy_vector = delimit(policies,',');
    //cout << "policy_vector.size(): " << policy_vector.size() << endl;
    // Print the separated tokens
    // for (const auto& item : policy_vector) {
    //     cout << item << endl;
    // }

    cin >> last_instant;
    cin >> num_processes;

    vector<string> string_processes(num_processes);

    for(int i = 0;i<num_processes;i++){
        cin >> string_processes[i];
    }

    

    
    vector<Process> struct_Processes = get_Processes_as_struct(string_processes,num_processes);

    // cout << "command: "  << command << endl;

    //cout << "policy: "  << policies << endl;

    // cout << "last_instant: "  << last_instant << endl;

    // cout << "num_processes: "  << num_processes << endl;

    // cout << "Processes: " << endl;
    // for(int i = 0;i<num_processes;i++){
    //     cout << string_processes[i] << endl;
    //     cout << struct_Processes[i].process_name << endl;
    //     cout << struct_Processes[i].arrival_time << endl;
    //     cout << struct_Processes[i].att2 << endl;
    // }
    //will need to delimit the processes into smth
    






    
    // loop through policies
    int policy_number;
    int q;
    string process_string = concat_strings(string_processes,num_processes);

    //cout << "Policies: " << endl;
    for(int i = 0;i<(int)policy_vector.size();i++){
        // wanna get all
        
        // each
        get_policy_number(policy_vector[i],&policy_number,&q); 
        // cout << "Policy Number: " << policy_number << endl;
        // cout << "q: " << q << endl;

        // run the policy

        // cout << "command: "  << command << endl;

        // cout << "policy code: " << policy_vector[i] << endl;

        // cout << "last_instant: "  << last_instant << endl;

        // cout << "num_processes: "  << num_processes << endl;
        
        // cout << "process_string: " << process_string << endl;
        
        //cout << "Running another program..." << endl; // policy vectotr is policy code
        string run = "./" + get_policy_name(policy_number) + " " + command + " " + policy_vector[i] + " " + to_string(last_instant) + " " + to_string(num_processes) + " " + process_string;
        //cout << run << endl;
        int result = system(run.c_str()); // Run the other program
        //cout << "The program exited with code: " << result << endl;


    }


    return 0;
}

// each policy needs:
// command -string-
// policy code -string-
// last_instatn thing
// num_processes -int-
// processes -vector<Process>-
// cant send a vector so will send a string of processes 
// and make it as a vector here
