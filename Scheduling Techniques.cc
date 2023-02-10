#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <math.h>

using namespace std;
#include <queue>

//Created a struct of process with all needed attributes
struct process
{
    char name;
    int arrival_time;
    int service_time;
    int time_interval;
    int remaining_time;
    int waiting_time=0;
    int finish;
    int turnaround;
    float normturn;
    bool ready=false;
    bool running=false;
    bool finished=false;
    int current_priority;
    int original_priority;
    int ran=0;
    char arr[60];

    bool operator<(const process& p) const
    {
        return time_interval > p.time_interval;
    }

};

//Created compare struct for SPN and SRT in which we need a priority queue for ordered with the time interval and remaining time
//respectively where this function returns minimum process 
struct compare_time
{
    bool operator()(process const& p1, process const& p2)
    {
        if(p1.time_interval != p2.time_interval)
            return p1.time_interval > p2.time_interval;
        else
            return p1.name > p2.name;

    }
};

//This compares the response ratio needed for HRRN function to return max ratio process 

struct compare_RR
{
    bool operator()(process const& p1, process const& p2)
    {

        float responseRatio_p1=(p1.waiting_time+p1.time_interval)/p1.time_interval;
        float responseRatio_p2=(p2.waiting_time+p2.time_interval)/p2.time_interval;
        if(responseRatio_p1 != responseRatio_p2)
            return responseRatio_p1 < responseRatio_p2;
        else
            return p1.name < p2.name;

    }
};


//This is used in aging where it returns highest priority, if equal then highest waiting time, if not returns process which hadn't ran

struct compare_waiting
{
    bool operator()(process const& p1, process const& p2)
    {
        if(p1.current_priority != p2.current_priority)
            return p1.current_priority < p2.current_priority;
        else if(p1.waiting_time != p2.waiting_time)
            return p1.waiting_time < p2.waiting_time;
        else
            return p1.ran > p2.ran;

    }
};

//This function prints trace for each algorithm as we pass the array of processes in which each process
//has an array indicating character ('.' '*' ' ') to be printed in each time slot
void print_trace(string alg_name, struct process p[], int time_slots, int nprocesses, int quantum)
{
    cout<<alg_name;
    if(quantum > 0)
    	cout<<"-"<<quantum<<"  ";

    int ctr=0;
    for(int i=0; i<=time_slots;i++){
    	cout<<ctr<<" ";
    	ctr++;
    	if(ctr == 10)
    		ctr=0;
    }
    cout<<"\n";
    cout<<"------------------------------------------------\n";
    for(int i=0; i<nprocesses; i++)
    {
        cout<<p[i].name<<"     ";
        for(int j=0; j<time_slots; j++)
            cout<<"|"<<p[i].arr[j];
        cout<<"| ";
        cout<<"\n";
    }
    cout<<"------------------------------------------------\n\n";
}

//This function prints stats of each algorithm according to the values associated with each process in the array
void print_stats(string alg_name, struct process p[], int time_slots, int nprocesses, int quantum)
{
    float mean=0;
    cout<<alg_name;
    if (quantum > 0)
    	cout<<"-"<<quantum;
    cout<<"\n";

    cout<<"Process    ";
    for(int i=0; i<nprocesses; i++)
    {
        cout<<"|  "<<p[i].name<<"  ";
    }
    cout<<"|";
    cout<<"\n";

    cout<<"Arrival    ";
    for(int i=0; i<nprocesses; i++)
    {
        cout<<"|  "<<p[i].arrival_time<<"  ";
    }
    cout<<"|";
    cout<<"\n";


    cout<<"Service    ";
    for(int i=0; i<nprocesses; i++)
    {
        cout<<"|  "<<p[i].service_time<<"  ";
    }
    cout<<"| Mean|";
    cout<<"\n";

    cout<<"Finish     ";
    for(int i=0; i<nprocesses; i++)
    {
        if (p[i].finish<=9)
            cout<<"|  "<<p[i].finish<<"  ";
        else
            cout<<"| "<<p[i].finish<<"  ";
    }
    cout<<"|-----|";
    cout<<"\n";

    cout<<"Turnaround ";
    for(int i=0; i<nprocesses; i++)
    {
    	p[i].turnaround = p[i].finish - p[i].arrival_time;
        mean+=p[i].turnaround;
        if(p[i].turnaround<=9)
            cout<<"|  "<<p[i].turnaround<<"  ";
        else
            cout<<"| "<<p[i].turnaround<<"  ";

    }
    if((mean/nprocesses)<10.00)
    	cout<<"| ";
    else
    	cout<<"|";
    printf("%.2f", (mean/nprocesses));
    cout<< "|";
    cout<<"\n";

    cout<<"NormTurn   ";
    mean=0;
    for(int i=0; i<nprocesses; i++)
    {
    	p[i].normturn = (float) p[i].turnaround / (float)p[i].service_time;
        mean+=p[i].normturn;
        cout<<"| ";
        printf("%.2f", p[i].normturn);
    }
    if((mean/nprocesses)<10.00)
    	cout<<"| ";
    else
    	cout<<"|";
    printf("%.2f", (mean/nprocesses));
    cout<<"|";
    cout<<"\n";
    cout<<"\n";

}


void FCFS(struct process p[],int time_slots, int nprocesses)
{
    queue<struct process> q;
    int flag=0, i=0, j=0;
    struct process running;

    //push all the processes in the queue
    for(i = 0; i<nprocesses; i++)
        q.emplace(p[i]);

    for(i=0; i<time_slots; i++)
    {
        if (flag == 0)
        {
            for(int k=0; k<nprocesses; k++)
            {
                if(q.front().name == p[k].name)
                {
                    p[k].ready = false;
                    p[k].running = true;
                }
            }
            //The process that has been in the ready queue the longest is selected for running.
            running = q.front();
            q.pop();
            flag=1;
        }

        if (flag == 1)
        {
            running.time_interval--;
            for(j=0; j<nprocesses; j++)
            {
            	//If a proccess arrives while another process is running, it becomes ready.
                if(p[j].arrival_time == i)
                    p[j].ready=true;
                    
	//We assign values to each process in its array in the current time slot according to if it is ready, running, or not arrived
                if(p[j].ready == true && p[j].running != true)
                    p[j].arr[i]='.';

                else if(p[j].name == running.name)
                    p[j].arr[i]='*';

                else
                    p[j].arr[i]=' ';

            }
        }

	//If a process finishes its service time, update its finish time, turnaround time and normalized turnaround time.
        if(running.time_interval == 0)
        {
            flag=0;
            for(j = 0; j<nprocesses; j++)
            {
                if(running.name == p[j].name)
                    p[j].finish=i+1;
            }
        }
    }

}


void RR(struct process p[],int time_slots, int nprocesses, int quantum)
{
    queue<struct process> q;

    int flag=0, i=0, j=0, quantum_ctr=quantum;

    struct process running;
    running.time_interval=0;


    for(i=0; i < time_slots; i++)
    {
    	//If a process finishes its service time or the time quantum
        if(quantum_ctr == 0 || running.time_interval == 0)
        {
            for(j=0; j<nprocesses; j++)
            {
            	//Any process that arrives, enters the ready queue
                if(p[j].arrival_time == i && p[j].time_interval>0)
                {
                    q.emplace(p[j]);
                }
            }

	    //If the currently running process is preempted, it is pushed in the ready queue again.
	    if(running.time_interval > 0)
		q.emplace(running);

	    //The next ready process is dispatched.
	    running = q.front();
	    running.time_interval--;
	    q.pop();
	    quantum_ctr = quantum;
	    quantum_ctr--;

        }
        else
        {
            //Decrement the service time of the currently running process.
            running.time_interval--;
            quantum_ctr--;
            for(j=0; j<nprocesses; j++)
            {
                if(p[j].arrival_time == i && p[j].time_interval>0)
                {
                    q.emplace(p[j]);
                }
            }
             if(running.time_interval == 0)
	    	{
	    		for(int j=0; j<nprocesses; j++)
			{
				if(p[j].name == running.name)
					p[j].finish=i+1;
			}
	    	}

        }
	//We decrement running time in the processes array too
        for(j=0; j<nprocesses; j++)
        {
            if(p[j].name == running.name){
                p[j].time_interval--;
             }
        }


        for(j=0; j<nprocesses; j++)
        {
        	//We assign the suitable character to each process in this time slot 
		if(p[j].name==running.name)
			p[j].arr[i]='*';
		else if(p[j].arrival_time <= i && p[j].time_interval > 0 && p[j].name != running.name)
			p[j].arr[i]='.';
		else
			p[j].arr[i]=' ';
	}
	for(j = 0; j<nprocesses; j++)
            {
                if(running.name == p[j].name)
                    p[j].finish=i+1;
            }
    }
}


void SPN(struct process p[],int time_slots, int nprocesses)
{
    //We use a priority queue which depends on shortest time interval of each process and returns the minimum
    priority_queue<process, vector<process>, compare_time> q;

    int flag=0, i=0, j=0, ctr=0;
    struct process running;

    //If a process's arrival time has come it enters our priority queue and assign true to ready in order to print '.' in this process array
    //as it entered the queue
    for(i=0; i<time_slots; i++)
    {
        for(int k=0; k<nprocesses; k++)
        {
            if(p[k].arrival_time == i)
            {
                q.push(p[k]);
                p[k].ready = true;
            }
        }
        //We assign running to be top of queue if it is the first iteration
        if(i==0)
        {
            running = q.top();
            q.pop();
        }

        //Each iteration we check if the currently running process's time interval has finished we assign a new process to processor
        if(running.time_interval==0)
        {
            running = q.top();
            q.pop();
        }


        for(j=0; j<nprocesses; j++)
        {
            //If the process is ready (entered the queue) and not the running process and its time interval hasn't finished
            //we assign '.' in this index ( referring to time slot number ) in its array which will be printed in fn printtrace
            if(p[j].ready == true && p[j].name != running.name&&p[j].time_interval>0)
                p[j].arr[i]='.';
            //If the process is running process and its running time has not finished yet we assign '*'
            else if(p[j].name == running.name && running.time_interval>0)
                p[j].arr[i]='*';
            //Else we assign ' ' as it is neither running nor ready
            else
                p[j].arr[i]=' ';

        }

        //we decrement the time interval of the running process
        running.time_interval--;
        for(int k=0; k<nprocesses; k++)
        {
            if(p[k].name == running.name)
                p[k].time_interval--;

        }
        //We set the finish time for running process to be next time slot
        for(j = 0; j<nprocesses; j++)
            {
                if(running.name == p[j].name)
                    p[j].finish=i+1;
            }
    }
}

void SRT(struct process p[],int time_slots, int nprocesses)
{
    //We use a priority queue which depends on shortest remaining time of each process and returns the minimum
    priority_queue<process, vector<process>, compare_time> q;

    int flag=0, i=0, j=0, ctr=0;
    struct process running;

    //If a process's arrival time has come it enters our priority queue and assign true to ready in order to print '.' in this process array
    //as it entered the queue
    for(i=0; i<time_slots; i++)
    {
        for(int k=0; k < nprocesses; k++)
        {
            if(p[k].arrival_time == i)
            {
                q.push(p[k]);
                p[k].ready = true;
            }
        }
        //We assign the ready process to be top and change its ready to be false not to conflict with ready processes, and decrement its time_interval
        running = q.top();
        q.pop();
        running.ready=false;
        running.time_interval--;
        //After decrementing running process we push it back to queue if it still has not finished

        if(running.time_interval>0)
        {
            q.push(running);
        }

        //We do the same thing for our array of processes that we had done to our running process
        for(int k=0; k < nprocesses; k++)
        {
            if(p[k].name == running.name)
            {
                p[k].ready = false;
                p[k].time_interval--;
            }
        }


        for(j=0; j<nprocesses; j++)
        {
            //We set the process's character to '.' if it had arrived and still has not finished and it is not running process
            if(p[j].arrival_time <= i && p[j].time_interval>0&&p[j].name != running.name)
                p[j].arr[i]='.';

            //We assign '*' to running process
            else if(p[j].name == running.name)
                p[j].arr[i]='*';
            //We assign ' ' to any other process
            else
                p[j].arr[i]=' ';

        }

    //We set the finish time of our running process
	if(running.time_interval == 0)
    	{
    		for(int j=0; j<nprocesses; j++)
		{
			if(p[j].name == running.name)
				p[j].finish=i+1;
		}
    	}
    }
}


void HRRN(struct process p[],int time_slots, int nprocesses)
{
    priority_queue<process, vector<process>, compare_RR> q;
    queue<struct process> temp_q;
    int i=0;
    struct process running;
    struct process temp;

	//We push each process whose arrival time = i and increase its waiting time by 1 and set it as ready
    for(i=0; i<time_slots; i++)
    {
        for(int k=0; k<nprocesses; k++)
        {
            if(p[k].arrival_time == i)
            {
                q.push(p[k]);
                p[k].waiting_time++;
                p[k].ready = true;
            }
        }

	//We set running as q.top in first iteration as we wont have a running process in the beginning
        if(i==0)
        {
            running = q.top();
            q.pop();
        }
        //If running process has finished we set assign new process to run

        if(running.time_interval==0)
        {
            running = q.top();
            q.pop();
        }

	//set values of charachters for each process
        for(int j=0; j<nprocesses; j++)
        {

            if(p[j].ready == true && p[j].name != running.name&&p[j].time_interval>0)
                p[j].arr[i]='.';

            else if(p[j].name == running.name && running.time_interval>0)
                p[j].arr[i]='*';

            else
                p[j].arr[i]=' ';

        }
        //Decrement running time interval each iteration 
        running.time_interval--;
        for(int k=0; k<nprocesses; k++)
        {
            if(p[k].name == running.name)
            {
                p[k].time_interval--;
                p[k].finish=i+1;
                p[k].turnaround=p[k].finish - p[k].arrival_time;
            }

        }
        int n = q.size();
        //If the queue is not empty we pop each process, update its value then push it back in queue q
        if (!q.empty())
        {
            for(int k=0; k<n; k++)
            {
                temp = q.top();
                q.pop();
                temp.waiting_time++;
                temp_q.emplace(temp);

                for(int j=0; j<nprocesses; j++)
                {
                    if(p[j].name == temp.name)
                    {
                        p[j].waiting_time++;
                    }
                }
            }
            for(int k=0; k<n; k++)
            {
                temp = temp_q.front();
                temp_q.pop();
                q.push(temp);
            }
        }
        //We set finishing time for the running process to be used in stats

         if(running.time_interval == 0)
    	{
    		for(int j=0; j<nprocesses; j++)
		{
			if(p[j].name == running.name)
				p[j].finish=i+1;
		}
    	}

    }
}


void FB_1(struct process p[],int time_slots, int nprocesses)
{
    queue<struct process> queues[9];
    queue<struct process> temp;

    for(int i=0; i<9; i++)
    {
        queues[i]=temp;
    }

    int flag=0, i=0, j=0, l=0;
    int flag2=0;

    struct process running;
    running.time_interval=0;
    
    //emplace all processes in queue 
    for(l=0; l<time_slots; l++)
    {
        for(j=0; j<nprocesses; j++)
        {
            if (p[j].arrival_time == l && p[j].ready == false)
            {
                queues[0].emplace(p[j]);
            }
        }
        //We loop on all queues to find the first process that we face starting from highest priority

        for(int i=0; i<9; i++)
        {
            if(queues[i].empty())
                continue;
	
            running=queues[i].front();
            queues[i].pop();
            running.time_interval--;

            for(j=0; j<nprocesses; j++)
            {
                if (p[j].name == running.name)
                {
                    p[j].time_interval--;
                }
            }
            if(running.time_interval>0)
            {
            //We check if all queues are empty
                for(j=0; j<9; j++)
                {
                    if(!queues[j].empty())
                    {
                        flag=1;
                        break;
                    }
                }
                if(flag == 0)
                {
                    for(j=0; j<nprocesses; j++)
                    {
                        if (p[j].arrival_time == l+1)
                        {
                            p[j].ready = true;
                            queues[0].emplace(p[j]);
                            flag2=1;
                        }
                    }
                    if (flag2== 1)
                        queues[i+1].emplace(running);
                    else
                        queues[i].emplace(running);
                }
                else
                    queues[i+1].emplace(running);
            }
            break;
        }

        for(j=0; j<nprocesses; j++)
        {
            if(p[j].name==running.name)
                p[j].arr[l]='*';
            else if(p[j].arrival_time <= l && p[j].time_interval > 0 && p[j].name != running.name)
                p[j].arr[l]='.';
            else
                p[j].arr[l]=' ';
        }

	if(running.time_interval == 0)
	{
		for(int j=0; j<nprocesses; j++)
		{
			if(p[j].name == running.name)
				p[j].finish=l+1;
		}
	}

    }
}



void FB_2i(struct process p[],int time_slots, int nprocesses)
{
    queue<struct process> queues[9];
    queue<struct process> temp;
    int q_ctr[9];

    for(int i=0; i<9; i++)
    {
        queues[i]=temp;
        q_ctr[i]=pow(2,i);
    }

    int flag=0, i=0, j=0, l=0;
    int flag2=0;
    int ctr=0, quantum=0, quantum_ctr;


    struct process running;
    running.time_interval=0;

    for(l=0; l<time_slots; l++)
    {
        if(quantum_ctr == 0 || running.time_interval == 0)
        {
            for(j=0; j<nprocesses; j++)
            {
                if (p[j].arrival_time == l && p[j].ready == false)
                    queues[0].emplace(p[j]);
            }
	//We loop on all queues to find the first process that we face starting from highest priority
            for(int i=0; i<9; i++)
            {
            //If this queue is empty continue to next queue
                if(queues[i].empty())
                    continue;

		
                running=queues[i].front();
                queues[i].pop();
                quantum_ctr=q_ctr[i];
                quantum_ctr--;
                running.time_interval--;

                if(running.time_interval>0)
                {
                    for(j=0; j<nprocesses+1; j++)
                    {
                    //We check if all queues are empty
                        if(!queues[j].empty())
                        {
                            flag=1;
                            break;
                        }
                    }
                    //If all queues are empty emplace process in same queue
                    if(flag == 0)
                    {
                    //We check first if there is a process coming next iteration we place it first and then the current will be placed
                    //in next queue as normal
                        for(j=0; j<nprocesses; j++)
                        {
                            if (p[j].arrival_time == l+1)
                            {
                                p[j].ready = true;
                                queues[0].emplace(p[j]);
                                flag2=1;
                            }
                        }
                        //Emplace running in next queue as normal as we found a new process coming
                        if (flag2== 1)
                            queues[i+1].emplace(running);
                            //else place it in the same queue 
                        else
                            queues[i].emplace(running);
                    }
                    else
                        queues[i+1].emplace(running);
                }
                break;
            }
            //decrement time interval of process
            for(j=0; j<nprocesses; j++)
            {
                if (p[j].name == running.name)
                {
                    p[j].time_interval--;
                }
            }

        }
        else
        {
        	//decrement time interval of running process
            quantum_ctr--;
            running.time_interval--;
            for(j=0; j<nprocesses; j++)
            {
                if (p[j].name == running.name)
                {
                    p[j].time_interval--;
                }
            }

            for(int i=0; i<9; i++)
            {
                int n=queues[i].size();
                for(int index=0; index<n; index++)
                {
                    for(j=0; j<nprocesses; j++)
                    {
                        if(queues[i].front().name == p[j].name)
                        {
                            queues[i].pop();
                            queues[i].emplace(p[j]);
                        }
                    }
                }

            }

            for(j=0; j<nprocesses; j++)
            {
                if (p[j].arrival_time == l && p[j].ready == false)
                    queues[0].emplace(p[j]);
            }

        }


        for(j=0; j<nprocesses; j++)
        {
            if(p[j].name==running.name)
                p[j].arr[l]='*';
            else if(p[j].arrival_time <= l && p[j].time_interval > 0 && p[j].name != running.name)
                p[j].arr[l]='.';
            else
                p[j].arr[l]=' ';
        }
        if(running.time_interval == 0)
	{
		for(int j=0; j<nprocesses; j++)
		{
			if(p[j].name == running.name)
				p[j].finish=l+1;
		}
	}

    }
}

void Aging(struct process p[],int time_slots, int nprocesses, int quantum)
{
    //We create a priority queue depending on priorities of processes (returns max priority)
    priority_queue<process, vector<process>, compare_waiting> q;
    int flag=0, i=0, j=0, quantum_ctr=0, highest_priority=0;

    struct process running;


    for(i=0; i < time_slots; i++)
    {
        //We empty our queue every time we loop as we won't need any old value
        while(!q.empty())
            q.pop();

        if(quantum_ctr == 0)
        {
            for(j=0; j<nprocesses; j++)
            {
                //We push processes whose arrival time has come and not the running process and increment its priority
                  if(p[j].arrival_time <= i && p[j].name != running.name)
                  {
                      p[j].current_priority++;
                      p[j].waiting_time++;
                      q.push(p[j]);
                  }
                  //Then after that we push the running process
                  else if(p[j].arrival_time <= i && p[j].name == running.name)
                  {
                      p[j].current_priority = p[j].original_priority;
                      p[j].waiting_time++;
                      q.push(p[j]);
                  }
            }

            //We assign top of queue (highest priority) to be running process and set its waiting time 0 as it will run now
            running = q.top();
            running.waiting_time=0;
            //We set the value of process which already ran to 1, so that if two processes in queue have same priority the new process would run first
            running.ran = 1;
            //Set the running priority back to its original priority
            running.current_priority = running.original_priority;
            q.pop();

            //We set the same things for the array of processes
            for(j=0; j<nprocesses; j++)
            {
                if(p[j].name == running.name)
                {
                    p[j].ran=1;
                    p[j].waiting_time=0;
                    p[j].current_priority = p[j].original_priority;
                }
            }
            //We set the quantum_ctr to be first = quantum after every preemption and decrement it by 1
            quantum_ctr = quantum;
            quantum_ctr--;
        }
        else
        {
            //Else if the quantum has not finished yet we decrement the quantum_ctr by 1
            quantum_ctr--;
        }

        for(j=0; j<nprocesses; j++)
        {
            //If a process is running we set its charachter to '*'
            if(p[j].name==running.name)
                p[j].arr[i]='*';
            //else if it had arrived, set its character to '.', as it will remain forever wanting to run
            else if(p[j].arrival_time <= i)
                p[j].arr[i]='.';
            //else set it to ' '
            else
                p[j].arr[i]=' ';
        }

    }
}



int main()
{

    int time_slots;
    int nprocesses;
    int alg_number;
    string operation;
    char pr[10];
    char alg_type[10];
    char arr1[4];
    int quantum;

    cin>>operation;
    cin>>alg_type;
    cin>>time_slots;
    char *token = strtok(alg_type, "-");
    int ctr=0;

    while (token != NULL)
    {
        arr1[ctr] = *token;
        ctr++;
        token = strtok(NULL, "-");
    }

    if(ctr > 1)
    {
        alg_number=int(arr1[0] - '0');
        quantum=int(arr1[1] - '0');
    }
    else
        alg_number=int(arr1[0] - '0');

    cin>>nprocesses;
    struct process p[nprocesses];


    char pname, arr[4];
    int arrive, interval;

    struct process temp;;

    for(int i =0; i< nprocesses; i++)
    {

        cin>>pr;

        char *token = strtok(pr, ",");

        for (int j = 0; j<3; j++)
        {
            arr[j] = *token;
            token = strtok(NULL, ",");
        }

        temp.name = arr[0];
        temp.arrival_time = int(arr[1] - '0');
        if (alg_number == 8){
		temp.original_priority = int(arr[2] - '0');
		temp.current_priority = int(arr[2] - '0');
	}
	else{
		temp.service_time = int(arr[2] - '0');
		temp.time_interval = int(arr[2] - '0');
	}
	
        p[i] = temp;
    }

    switch(alg_number)
    {
        case 1:
		FCFS(p, time_slots, nprocesses);
		if (operation =="trace")
			print_trace("FCFS  ", p, time_slots, nprocesses, 0);
		else
			print_stats("FCFS", p, time_slots, nprocesses, 0);
            	break;
        case 2:
		RR(p, time_slots, nprocesses, quantum);
		if (operation =="trace")
			print_trace("RR", p, time_slots, nprocesses, quantum);
		else
			print_stats("RR", p, time_slots, nprocesses, quantum);
            	break;
        case 3:
            	SPN(p, time_slots, nprocesses);
            	if (operation =="trace")
            		print_trace("SPN   ", p, time_slots, nprocesses, 0);
            	else
            		print_stats("SPN", p, time_slots, nprocesses, 0);
            	break;
        case 4:
		SRT(p, time_slots, nprocesses);
		if (operation =="trace")
			print_trace("SRT   ", p, time_slots, nprocesses, 0);
		else
			print_stats("SRT", p, time_slots, nprocesses, 0);
           	break;
        case 5:
		HRRN(p, time_slots, nprocesses);
		if (operation =="trace")
			print_trace("HRRN  ", p, time_slots, nprocesses, 0);
		else
			print_stats("HRRN", p, time_slots, nprocesses, 0);
            	break;
        case 6:
    		FB_1(p, time_slots, nprocesses);
    		if (operation =="trace")
			print_trace("FB-1  ", p, time_slots, nprocesses, 0);
		else
			print_stats("FB-1", p, time_slots, nprocesses, 0);
          	break;
        case 7:
		FB_2i(p, time_slots, nprocesses);
		if (operation =="trace")
			print_trace("FB-2i ", p, time_slots, nprocesses, 0);
		else
			print_stats("FB-2i", p, time_slots, nprocesses, 0);
            	break;
        case 8:
           	Aging(p, time_slots, nprocesses,1);
          	print_trace("Aging ", p, time_slots, nprocesses, 0);
            	break;
        default:
            break;
    }

    return 0;
}

