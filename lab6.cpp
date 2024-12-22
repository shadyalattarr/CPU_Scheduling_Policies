#include <iostream>   // For input/output operations
#include <string>     // For using string operations
#include <vector>     // For using the vector container
#include <algorithm>  // For algorithms like sort, find, etc.
#include <cmath>      // For mathematical functions
#include <sstream>


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

int main() {
    string command,policies;
    int last_instant,num_processes;
    

    cin >> command;
    cin >> policies;

    vector<string> policy_vector = delimit(policies,',');
    cout << "policy_vector.size(): " << policy_vector.size() << endl;
    // Print the separated tokens
    for (const auto& item : policy_vector) {
        cout << item << endl;
    }

    cin >> last_instant;
    cin >> num_processes;

    vector<string> string_processes(num_processes);

    for(int i = 0;i<num_processes;i++){
        cin >> string_processes[i];
    }

    vector<Process> struct_Processes = get_Processes_as_struct(string_processes,num_processes);

    // cout << "command: "  << command << endl;

    // cout << "policy: "  << policies << endl;

    // cout << "last_instant: "  << last_instant << endl;

    // cout << "num_processes: "  << num_processes << endl;

    cout << "Processes: " << endl;
    for(int i = 0;i<num_processes;i++){
        cout << string_processes[i] << endl;
        cout << struct_Processes[i].process_name << endl;
        cout << struct_Processes[i].arrival_time << endl;
        cout << struct_Processes[i].att2 << endl;
    }
    //will need to delimit the processes into smth
    

    for(int i =0;i<policy_vector.size();i++){
        // for each policy we got
        // run it

    }


    return 0;
}


// each policy needs:
// policy code -string-
// processes -vector<Process>-
// num_processes -int-
// command -string-
