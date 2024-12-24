#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <map>
using namespace std;

struct Process {
    string process_name;
    int arrival_time;
    int start_time = -1;
    int service_time;
    int finish_time = -1;

    // Comparator for priority_queue (min-heap by service_time)
    bool operator>(const Process& other) const {
        return service_time > other.service_time;
    }
};

struct InputArgs {
    string operation;
    string schedulingPolicy;
    int simulationEndTime;
    int processCount;
    string rawProcessDetails;
};

string fix_string(string mainStr, int totalWidth) {
    int left = (totalWidth - mainStr.size()) / 2;
    int right = totalWidth - mainStr.size() - left;
    return string(left, ' ') + mainStr + string(right, ' ');
}

string fix_string_float(double value, int totalWidth, int precision) {
    ostringstream oss;
    oss << setw(totalWidth) << fixed << setprecision(precision) << value;
    return oss.str();
}

vector<Process> string_to_process(vector<string> string_processes, int processCount) {
    vector<Process> processes(processCount);

    for (int i = 0; i < processCount; i++) {
        vector<string> process_parameters;
        stringstream ss(string_processes[i]);
        string token;
        while (getline(ss, token, ',')) {
            process_parameters.push_back(token);
        }

        Process curr_process;
        curr_process.process_name = process_parameters[0];
        curr_process.arrival_time = stoi(process_parameters[1]);
        curr_process.service_time = stoi(process_parameters[2]);
        processes[i] = curr_process;
    }

    return processes;
}

vector<Process> run_spn(vector<string> string_processes, int processCount) {
    vector<Process> processes = string_to_process(string_processes, processCount);
    vector<Process> completed_processes;
    priority_queue<Process, vector<Process>, greater<Process>> ready_queue;
    int current_time = 0;

    while (!processes.empty() || !ready_queue.empty()) {
        // Add all processes that have arrived to the priority queue
        for (auto it = processes.begin(); it != processes.end();) {
            if (it->arrival_time <= current_time) {
                ready_queue.push(*it);
                it = processes.erase(it);
            } else {
                ++it;
            }
        }

        if (!ready_queue.empty()) {
            Process shortest_process = ready_queue.top();
            ready_queue.pop();

            shortest_process.start_time = current_time;
            shortest_process.finish_time = current_time + shortest_process.service_time;
            current_time = shortest_process.finish_time;

            completed_processes.push_back(shortest_process);
        } else {
            current_time++; // Advance time if no process is ready
        }
    }

    return completed_processes;
}

void trace_spn(int last_instant, const vector<Process>& originalProcesses, const vector<Process>& scheduledProcesses) {
    string instants_string = "SPN   ";
    string dashes = "------";
    for (int i = 0; i <= last_instant; i++) {
        instants_string += to_string(i % 10) + " ";
        dashes += "--";
    }
    cout << instants_string << endl;
    cout << dashes << endl;

    // Create a map to look up scheduled process details by name
    map<string, Process> processMap;
    for (const auto& process : scheduledProcesses) {
        processMap[process.process_name] = process;
    }

    // Generate trace output in the order of originalProcesses
    for (const auto& process : originalProcesses) {
        string process_line = process.process_name + "     ";
        if (processMap.find(process.process_name) == processMap.end()) {
            continue; // Skip processes not scheduled
        }

        const Process& scheduledProcess = processMap[process.process_name];
        for (int t = 0; t <= last_instant; t++) {
            process_line += "|";
            if (t < scheduledProcess.arrival_time) {
                process_line += " "; // Process has not arrived
            } else if (t >= scheduledProcess.start_time && t < scheduledProcess.finish_time) {
                process_line += "*"; // Process is executing
            } else if (t >= scheduledProcess.arrival_time && t < scheduledProcess.finish_time) {
                process_line += "."; // Process is waiting
            } else {
                process_line += " "; // Process is not active
            }
        }
        cout << process_line << endl;
    }

    cout << dashes << endl;
    cout << endl;
}

void stats_spn(int processCount, const vector<Process>& originalProcesses, const vector<Process>& scheduledProcesses) {
    double avg_taround = 0;
    double avg_normturn = 0;
    cout << "SPN" << endl;

    string process_line = "Process    |";
    string arrival_line = "Arrival    |";
    string service_line = "Service    |";
    string finish_line = "Finish     |";
    string turnaround_line = "Turnaround |";
    string normturn_line = "NormTurn   |";

    // Create a map for quick access to scheduled process details by name
    map<string, Process> processMap;
    for (const auto& process : scheduledProcesses) {
        processMap[process.process_name] = process;
    }

    // Use originalProcesses to iterate in input order
    for (const auto& process : originalProcesses) {
        if (processMap.find(process.process_name) == processMap.end()) {
            continue; // Skip processes not scheduled
        }

        const Process& scheduledProcess = processMap[process.process_name];
        int arrival = scheduledProcess.arrival_time;
        int service = scheduledProcess.service_time;
        int finish = scheduledProcess.finish_time;
        process_line += fix_string(scheduledProcess.process_name, 5) + "|";
        arrival_line += fix_string(to_string(arrival), 5) + "|";
        service_line += fix_string(to_string(service), 5) + "|";
        finish_line += fix_string(to_string(finish), 5) + "|";
        int turnaround = finish - arrival;
        avg_taround += turnaround;
        turnaround_line += fix_string(to_string(turnaround), 5) + "|";
        double normturn = (double)turnaround / service;
        avg_normturn += normturn;
        normturn_line += fix_string_float(normturn, 5, 2) + "|";
    }

    avg_taround /= processCount;
    avg_normturn /= processCount;

    cout << process_line << endl;
    cout << arrival_line << endl;
    cout << service_line << " Mean|" << endl;
    cout << finish_line << "-----|" << endl;
    cout << turnaround_line << fix_string_float(avg_taround, 5, 2) << "|" << endl;
    cout << normturn_line << fix_string_float(avg_normturn, 5, 2) << "|\n" << endl;
}


vector<string> splitProcessString(const string& rawProcessString, char delimiter = '-') {
    vector<string> process_vector;
    stringstream ss(rawProcessString);
    string processEntry;
    while (getline(ss, processEntry, delimiter)) {
        process_vector.push_back(processEntry);
    }
    return process_vector;
}

InputArgs parse_args(int argc, char const* argv[]) {
    if (argc != 6) {
        cerr << "AN ERROR HAS OCCURED!!\n";
        exit(1);
    }

    InputArgs args;
    args.operation = argv[1];
    args.schedulingPolicy = argv[2];
    args.simulationEndTime = stoi(argv[3]);
    args.processCount = stoi(argv[4]);
    args.rawProcessDetails = argv[5];

    return args;
}

void run_algo(vector<string> process_vector, InputArgs args) {
    vector<Process> originalProcesses = string_to_process(process_vector, args.processCount);
    vector<Process> scheduledProcesses = run_spn(process_vector, args.processCount);

    if (args.operation == "trace") {
        trace_spn(args.simulationEndTime, originalProcesses, scheduledProcesses);
    } else if (args.operation == "stats") {
        stats_spn(args.processCount, originalProcesses, scheduledProcesses);
    } else {
        cerr << "Invalid operation. Use 'trace' or 'stats'.\n";
        exit(1);
    }
}

int main(int argc, char const* argv[]) {
    InputArgs args = parse_args(argc, argv);
    vector<string> process_vector = splitProcessString(args.rawProcessDetails);
    run_algo(process_vector, args);
    return 0;
}
