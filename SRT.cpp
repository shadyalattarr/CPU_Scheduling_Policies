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



struct Process{
    string process_name;
    int arrival_time;
    int start_time; // start time is not useful here?
    int service_time;
    int finish_time;
    int time_served_so_far; // starts with 0 and when it reaches service time it is done
};

int main(int argc, char const *argv[])
{
    if(argc != 6){
        cerr << "Usage: ./rr <command> <policy_code> <last_instant> <num_processes> <processes>\n";
        return 1;
    }
    
    // Convert arguments to std::string
    string command = argv[1];
    string policy_code = argv[2]; // wont make use of this here
    int last_instant = stoi(argv[3]);
    int num_processes = stoi(argv[4]);
    string process_string = argv[5];
    
    cout << "command: "  << command << endl;

    cout << "policy code: " << policy_code << endl;

    cout << "last_instant: "  << last_instant << endl;

    cout << "num_processes: "  << num_processes << endl;
        
    cout << "process_string: " << process_string << endl;

    vector<string> process_string_vector = delimit(process_string,'-');
    for(int i =0;i<(int)process_string_vector.size();i++){
         cout << process_string_vector[i] << endl;
    }
    
    return 0;
}
