#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <queue>
#include <map>
using namespace std;

struct Process
{
    string process_name;
    int arrival_time;
    int service_time;
    int remaining_time;
    int start_time = -1;
    int finish_time = -1;
    vector<pair<int, int>> running_intervals; // pairs of (start, end) times when process was running
};

struct InputArgs
{
    string operation;
    string schedulingPolicy;
    int simulationEndTime;
    int processCount;
    string rawProcessDetails;
};

string fix_string(string mainStr, int totalWidth)
{
    int left = (totalWidth - mainStr.size()) / 2;
    int right = totalWidth - mainStr.size() - left;
    return string(left, ' ') + mainStr + string(right, ' ');
}

string fix_string_float(double value, int totalWidth, int precision)
{
    ostringstream oss;
    oss << setw(totalWidth) << fixed << setprecision(precision) << value;
    return oss.str();
}

vector<Process> string_to_process(vector<string> string_processes, int processCount)
{
    vector<Process> processes(processCount);

    for (int i = 0; i < processCount; i++)
    {
        vector<string> process_parameters;
        stringstream ss(string_processes[i]);
        string token;
        while (getline(ss, token, ','))
        {
            process_parameters.push_back(token);
        }

        Process curr_process;
        curr_process.process_name = process_parameters[0];
        curr_process.arrival_time = stoi(process_parameters[1]);
        curr_process.service_time = stoi(process_parameters[2]);
        curr_process.remaining_time = curr_process.service_time;
        processes[i] = curr_process;
    }

    return processes;
}

vector<Process> run_fb_1(vector<string> string_processes, int processCount, int simulationEndTime)
{
    vector<Process> processes = string_to_process(string_processes, processCount);
    vector<Process*> completed_processes;
    vector<queue<Process*>> queues(10);
    int current_time = 0;
    Process* running_process = nullptr;
    Process* next_process = nullptr;
    int current_quantum = 0;
    size_t current_queue_level = 0;  // Changed to size_t
    size_t next_queue_level = 0;     // Changed to size_t
    int completed_count = 0;

    while (current_time <= simulationEndTime && completed_count < processCount) 
    {
        // Check for new arrivals first
        for (auto& p : processes) 
        {
            if (p.arrival_time == current_time && p.remaining_time > 0) 
            {
                queues[0].push(&p);
                if (p.start_time == -1) {
                    p.start_time = current_time;
                }
            }
        }

        // If we are about to finish a quantum, select next process before preempting current one
        if (running_process && current_quantum >= 1) 
        {
            next_process = nullptr;
            for (size_t i = 0; i < queues.size(); i++) 
            {
                if (!queues[i].empty()) 
                {
                    next_process = queues[i].front();
                    queues[i].pop();
                    next_queue_level = i;
                    break;
                }
            }

            // Move current process to next queue level
            if (running_process->remaining_time > 0) 
            {
                if (current_queue_level + 1 < queues.size())
                    queues[current_queue_level + 1].push(running_process);
                else
                    queues[current_queue_level].push(running_process);
            }

            running_process = next_process;
            current_queue_level = next_queue_level;
            current_quantum = 0;
        }

        // If no running process, get one from highest priority non-empty queue
        if (!running_process) 
        {
            for (size_t i = 0; i < queues.size(); i++) 
            {
                if (!queues[i].empty()) 
                {
                    running_process = queues[i].front();
                    queues[i].pop();
                    current_queue_level = i;
                    current_quantum = 0;
                    break;
                }
            }
        }

        // Execute current process for one time unit
        if (running_process) 
        {
            if (running_process->running_intervals.empty() || 
                running_process->running_intervals.back().second != current_time) {
                running_process->running_intervals.push_back({current_time, current_time + 1});
            } else {
                running_process->running_intervals.back().second = current_time + 1;
            }

            running_process->remaining_time--;
            current_quantum++;

            if (running_process->remaining_time == 0) 
            {
                running_process->finish_time = current_time + 1;
                completed_count++;
                running_process = nullptr;
                current_quantum = 0;
            }
        }

        current_time++;
    }

    // Collect completed processes for return
    vector<Process> result;
    for (const auto& p : processes) 
    {
        if (p.finish_time != -1) {
            result.push_back(p);
        }
    }

    return result;
}

void trace_fb_1(int last_instant, const vector<Process> &originalProcesses, const vector<Process> &scheduledProcesses)
{
    string instants_string = "FB-1  "; // Changed from FB-2i to FB-1
    string dashes = "------";
    for (int i = 0; i <= last_instant; i++)
    {
        instants_string += to_string(i % 10) + " ";
        dashes += "--";
    }
    cout << instants_string << endl;
    cout << dashes << endl;

    // Create a map to look up scheduled process details by name
    map<string, Process> processMap;
    for (const auto &process : scheduledProcesses)
    {
        processMap[process.process_name] = process;
    }

    // Generate trace output
    for (const auto &process : originalProcesses)
    {
        string process_line = process.process_name + "     ";
        if (processMap.find(process.process_name) == processMap.end())
        {
            continue; // Skip processes not scheduled
        }

        const Process &scheduledProcess = processMap[process.process_name];
        for (int t = 0; t <= last_instant; t++)
        {
            process_line += "|";
            if (t < scheduledProcess.arrival_time)
            {
                process_line += " "; // Process has not arrived
            }
            else if (t < scheduledProcess.finish_time)
            {
                bool is_running = false;
                // Check if the process was running at time t
                for (const auto &interval : scheduledProcess.running_intervals)
                {
                    if (t >= interval.first && t < interval.second)
                    {
                        is_running = true;
                        break;
                    }
                }
                process_line += (is_running ? "*" : "."); // * for running, . for ready
            }
            else
            {
                process_line += " "; // Process is not active
            }
        }
        cout << process_line << endl;
    }

    cout << dashes << endl;
    cout << endl;
}

void stats_fb_1(int processCount, const vector<Process> &originalProcesses, const vector<Process> &scheduledProcesses)
{
    double avg_taround = 0;
    double avg_normturn = 0;
    cout << "FB-1" << endl; // Changed from FB-2i to FB-1

    string process_line = "Process    |";
    string arrival_line = "Arrival    |";
    string service_line = "Service    |";
    string finish_line = "Finish     |";
    string turnaround_line = "Turnaround |";
    string normturn_line = "NormTurn   |";

    // Create a map for quick access to scheduled process details by name
    map<string, Process> processMap;
    for (const auto &process : scheduledProcesses)
    {
        processMap[process.process_name] = process;
    }

    for (const auto &process : originalProcesses)
    {
        if (processMap.find(process.process_name) == processMap.end())
        {
            continue; // Skip processes not scheduled
        }

        const Process &scheduledProcess = processMap[process.process_name];
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
    cout << normturn_line << fix_string_float(avg_normturn, 5, 2) << "|\n"
         << endl;
}

vector<string> splitProcessString(const string &rawProcessString, char delimiter = '-')
{
    vector<string> process_vector;
    stringstream ss(rawProcessString);
    string processEntry;
    while (getline(ss, processEntry, delimiter))
    {
        process_vector.push_back(processEntry);
    }
    return process_vector;
}

InputArgs parse_args(int argc, char const *argv[])
{
    if (argc != 6)
    {
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

void run_algo(vector<string> process_vector, InputArgs args)
{
    vector<Process> originalProcesses = string_to_process(process_vector, args.processCount);
    vector<Process> scheduledProcesses;

    scheduledProcesses = run_fb_1(process_vector, args.processCount, args.simulationEndTime);

    if (args.operation == "trace")
    {
        trace_fb_1(args.simulationEndTime, originalProcesses, scheduledProcesses);
    }
    else if (args.operation == "stats")
    {
        stats_fb_1(args.processCount, originalProcesses, scheduledProcesses);
    }
    else
    {
        cerr << "Invalid operation. Use 'trace' or 'stats'.\n";
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    InputArgs args = parse_args(argc, argv);
    vector<string> process_vector = splitProcessString(args.rawProcessDetails);
    run_algo(process_vector, args);
    return 0;
}
