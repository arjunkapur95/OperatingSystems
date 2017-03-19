/*** NO CHANGES TO THIS FILE ARE ALLOWED EXCEPT WHERE INDICATED ***/
/* PCB management functions for HOSTD */

/* Include Files */
#include "pcb.h"

/*** CODE COMMENTS MAY BE ADDED BEYOND THIS POINT ***/

/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION PROTOTYPES MAY BE ADDED HERE ***/
PcbPtr deqHrrn(PcbPtr * ,int);
/*** END OF SECTION MARKER ***/

/*******************************************************
 * PcbPtr startPcb(PcbPtr process) - start (or restart)
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if start (restart) failed
 ******************************************************/
PcbPtr startPcb (PcbPtr p)
{
    if (p->pid == 0)
    {
        switch (p->pid = fork())
        {
            case -1:
                fprintf(stderr, "FATAL: Could not fork process!\n");
                exit(EXIT_FAILURE);
            case 0:
                p->pid = getpid();
                p->status = PCB_RUNNING;
                printPcbHdr();
                printPcb(p);
                fflush(stdout);
                execv(p->args[0], p->args);
                fprintf(stderr, "Could not run process!\n");
                exit(EXIT_FAILURE);
        }
    }
    else
    {
        kill(p->pid, SIGCONT);
    }
    p->status = PCB_RUNNING;

    return p;
}

/*******************************************************
 * PcbPtr suspendPcb(PcbPtr process) - suspend
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if suspension failed
 ******************************************************/
PcbPtr suspendPcb(PcbPtr p)
{
    int status;
     
     kill(p->pid, SIGTSTP);
     waitpid(p->pid, &status, WUNTRACED);
     p->status = PCB_SUSPENDED;
     return p;
}


/*******************************************************
 * PcbPtr terminatePcb(PcbPtr process) - terminate
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if termination failed
 ******************************************************/
PcbPtr terminatePcb(PcbPtr p)
{
     int status;//need the variable inorder to use waitPid
    kill(p->pid, SIGINT);
    waitpid(p->pid, &status, WUNTRACED);
    p->status = PCB_TERMINATED;//setting the job to suspended
    return p;
}

/*******************************************************
 * PcbPtr printPcb(PcbPtr process)
 *  - print process attributes on stdout
 *  returns:
 *    PcbPtr of process
 ******************************************************/
PcbPtr printPcb(PcbPtr p)
{
    if (!p)
    {
        fprintf(stderr, "ERROR: Could not print process control block parameters\n");
        return NULL;
    }
    else
    {
        printf("%ld              ",p->pid);
		printf("%d               ",p->arrival_time);
		printf("%d               ",p->priority);
		printf("%d               ",p->scheduled_service_time);
		printf("%d               ",p->mbytes);
		printf("%d           \n",p->mem_block->offset);

        return p;
    }
}

/*******************************************************
 * void printPcbHdr() - print header for printPcb
 *  returns:
 *    void
 ******************************************************/
//printing a header
 void printPcbHdr()
{
		printf("%s      ","PID");
		printf("%s      ","Arrival");
		printf("%s      ","Priority");
		printf("%s      ","Allocated Time");
		printf("%s      ","Allocated Memory");
		printf("%s   \n","Offset");
}

/*******************************************************
 * PcbPtr createnullPcb() - create inactive Pcb.
 *
 * returns:
 *    PcbPtr of newly initialised Pcb
 *    NULL if malloc failed
 ******************************************************/
PcbPtr createnullPcb()
{
    PcbPtr new_process_Ptr;
    if (!(new_process_Ptr = (PcbPtr)malloc(sizeof(Pcb))))
    {
        fprintf(stderr, "ERROR: Could not create new process control block\n");
        return NULL;
    }
    new_process_Ptr->pid = 0;
    new_process_Ptr->args[0] = "./process";
    new_process_Ptr->args[1] = NULL;
    new_process_Ptr->arrival_time = 0;
    new_process_Ptr->priority = 0;
    new_process_Ptr->remaining_cpu_time = 0;
    new_process_Ptr->mbytes = 0;
    new_process_Ptr->mem_block = NULL;
    new_process_Ptr->status = PCB_UNINITIALIZED;
    new_process_Ptr->next = NULL;

    return new_process_Ptr;
}

/*******************************************************
 * PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process)
 *    - queue process (or join queues) at end of queue
 *
 * returns head of queue
 ******************************************************/
PcbPtr enqPcb(PcbPtr q, PcbPtr p)
{
	PcbPtr tmp ;
	if(!q)
	{
		//puts("eq done");
		return p;
	}
	else
	{

		tmp=q;
		while(tmp)
		{
			if(tmp->next==NULL)
			{
				tmp->next=p;
				//puts("eq done");
				return q;

			}
			tmp=tmp->next;
			//puts("in loop");
		}
		//puts("eq done");
		return q;
	}
}

/*******************************************************
 * PcbPtr deqPcb (PcbPtr * headofQ);
 *    - dequeue process - take Pcb from head of queue.
 *
 * returns:
 *    PcbPtr if dequeued,
 *    NULL if queue was empty
 *    & sets new head of Q pointer in adrs at 1st arg
 *******************************************************/
PcbPtr deqPcb(PcbPtr * hPtr)
{
	//checkign if list is null
	if(!hPtr)
	{

		return NULL;
	}
	PcbPtr tmp=NULL;
	PcbPtr head=*hPtr;
	if(!head)
	{
		return NULL;
	}
	//checking to see if head next is null
	if(!head->next)
	{
		*hPtr=NULL;//dequeue the head
		return head;
	}
	else
	{
		tmp=head->next;//dequing from the queue
		*hPtr=tmp;
	}
	head->next=NULL;
	return head;

    /*** REPLACE THIS LINE WITH YOUR CODE ***/
}
PcbPtr deqHrrn(PcbPtr * hptr,int timer)
{
	PcbPtr temp=*hptr;
	PcbPtr temp1;
	PcbPtr prev;

	float waitingTime=0;//for calculating response ratio
	float ratio=0;//for calculating response ratio
	float max=0;//for calculating  highest response ratio

	waitingTime = timer - temp->arrival_time-temp->scheduled_service_time+ (temp->remaining_cpu_time);//assigning waiting time for the job
	ratio = 1 + (waitingTime/(temp->remaining_cpu_time));//assign the response ratio
	max=ratio;//set max to the ratio

	while(temp)
	{
		waitingTime = timer - temp->arrival_time-temp->scheduled_service_time+ (temp->remaining_cpu_time);//assigning waiting time for the job
		ratio = 1 + (waitingTime/(temp->remaining_cpu_time));//assign the response ratio

		 if(ratio>max||ratio==max)
		 {
			max=ratio;//set max to the ratio
			temp1=temp;//assigning the pcb ptr so we know which is highest ratio
		 }
		temp=temp->next;//iterating the queue
	}

	temp=*hptr;//assigning temp to head if we need to dequeue head
	//dequeue the head pcb
	if(temp==temp1)
	{
		*hptr=temp->next;
		temp->next=NULL;
		return temp;
	}
	while(temp)
	{

		if(temp==temp1)//finding the pcb to dequeue 
		{
			if(prev!=NULL)//checking if ther is a previous since we have a signly linked list we need to keep ttrack of previous
			{
				prev->next=temp->next;
				temp->next=NULL;//dequing the temp process
				return temp;
			}
		}
		prev=temp;//assigning prev
		temp=temp->next;//iterating the pcb list
	}

}
/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION IMPLEMENTATIONS MAY BE ADDED HERE ***/
/*** END OF SECTION MARKER ***/
/*** END OF CODE; DO NOT ADD MATERIAL BEYOND THIS POINT ***/
