/*** NO CHANGES TO THIS FILE ARE ALLOWED EXCEPT WHERE INDICATED ***/

/*** START OF SECTION MARKER ***/
/*** PLEASE UPDATE THIS SECTION. NO CODE INCLUDING INCLUDE STATEMENTS MAY BE ADDED. ***/
/*
    HOSTD - Hypothetical Operating System Testbed Dispatcher

    usage:

        hostd <TESTFILE>
        where <TESTFILE> is the name of a job list
*/

/************************************************************************************************************************

    ** Revision history **

    Current version: 1.1 bis
    Date: 7 April 2016

    1.1: Added instructions to allow additional functions in hostd.c and pcb.c
    1.0: Original version

    Contributors:
    1. COMP3520 teaching staff
       Centre for Distributed and High Performance Computing
       School of Information Technologies
       The University of Sydney
       NSW 2006
       Australia

    2. Dr Ian G Graham

    Copyright of this code and associated material is vested in the original contributors.

    This code is NOT in the Public Domain. Unauthorised posting of this code or derivatives thereof is not permitted.

    ** DO NOT REMOVE THIS NOTICE. Just update the revision history whenever you make changes to the code. **

 ***********************************************************************************************************************/
/*** END OF SECTION MARKER ***/

/* Include files */
#include "hostd.h"
#include "pcb.c"
#include "mab.c"
/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION PROTOTYPES MAY BE ADDED HERE ***/
PcbPtr deqHrrn(PcbPtr * ,int );
/*** END OF SECTION MARKER ***/

int main (int argc, char *argv[])
{
    /*** START OF SECTION MARKER ***/
    /*** You may add variable declarations in this section. ***/

    FILE * input_list_stream = NULL;
    PcbPtr input_queue = NULL;
	PcbPtr real_time_queue =NULL;
	PcbPtr user_job_queue =NULL;
	PcbPtr feedback_queue = NULL;
	PcbPtr feedback_queue1 = NULL;
	PcbPtr hrrn_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
	PcbPtr process1 = NULL;
	PcbPtr process2 =NULL;
	MabPtr mab_doubly_linked_list = createnullMab(64,MEMORY_SIZE);
	MabPtr realTime=createnullMab(0,64);
	int realTimeFlag=0;
	int memoryCounter =0;
    int timer = 0;
	
    
/*** END OF SECTION MARKER ***/

/*** START OF SECTION MARKER ***/
/*** COMMENTS MAY BE ADDED, MODIFIED OR DELETED IN THIS SECTION ***/

//  1. Populate the FCFS queue

    if (argc == 0)
    {
        fprintf(stderr, "FATAL: Bad arguments array\n");
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(input_list_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int val_check;
        process = createnullPcb();
        if ((val_check = fscanf(input_list_stream, "%d, %d, %d, %d",
          &(process->arrival_time), &(process->priority),
          &(process->scheduled_service_time), &(process->mbytes))) == 4)
        {
            process->remaining_cpu_time = process->scheduled_service_time;
            process->status = PCB_INITIALIZED;
            input_queue = enqPcb(input_queue, process);
        }
        else if (val_check >= 0)
        {
            free(process);
            fprintf(stderr, "ERROR: Invalid input file \"%s\"\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        else if (ferror(input_list_stream))
        {
            free(process);
            fprintf(stderr, "ERROR: Could not read input file \"%s\"\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        else {
            free(process);
            break;
        }
    }
/*** END OF SECTION MARKER ***/

/*** START OF SECTION MARKER ***/
/*** CODE AND COMMENTS MAY BE ADDED, DELETED OR MODIFIED IN THIS SECTION ***/

/*** COMMENTS THAT BEGIN WITH // ARE FOR EXERCISE 4 ONLY ***/
/*** YOU NEED TO WRITE YOUR OWN COMMENTS ***/

//  2. Whenever there is a running process or the FCFS queue is not empty:

     while (current_process!=NULL||input_queue!=NULL||hrrn_queue!=NULL||feedback_queue!=NULL||feedback_queue1!=NULL) // REPLACE THIS LINE WITH YOUR CODE (ONE LINE ONLY)
    {
			while(input_queue != NULL && input_queue->arrival_time<=timer)
			{
				process1= deqPcb(&input_queue);//dequign so we can add to either realtime or the feedback queue
				if(process1->priority==0 && process1->mbytes<=64)
				{
					process1->mem_block=realTime;
					realTimeFlag=1;//flagging the memory blcok
					real_time_queue=enqPcb(real_time_queue,process1);//enquing on the realtime queue
					
				}
				else
				{
					if(process1->priority==0)
					{
						fprintf(stderr, "More than Memory available to real time");
						process1=createnullPcb();
					}
					else
					{
						
						user_job_queue=enqPcb(user_job_queue,process1);	//adding to the user queue
					}
				}
				
				
			}
			while((user_job_queue!=NULL) && ((user_job_queue->mbytes+memoryCounter)<=960))
			{
				int mbytes=user_job_queue->mbytes;
				user_job_queue->mem_block=memAlloc(&mab_doubly_linked_list,mbytes);//allocating memory
				
				// allocating memory blocks to the respective priority levels
				if(user_job_queue->mem_block!=NULL)
				{
					
					process2= deqPcb(&user_job_queue);//dequeing from user job queue 
					if(process2->priority==1)
					{
						feedback_queue=enqPcb(feedback_queue,process2);//adding priority 1 feed back
						
					}
					else if(process2->priority==2)
					{
						feedback_queue1=enqPcb(feedback_queue1,process2);//adding priority queue 2 feedback
						
					}
					else
					{
						hrrn_queue=enqPcb(hrrn_queue,process2);//adding priority 3feedback
						
					}	

				}
				else
				{
					break;
				}
				
			}
			if(current_process!=NULL)
			{
				
				current_process->remaining_cpu_time--;
				if(current_process->remaining_cpu_time==0)
				{
					//if we need to free a realtime process do this
					if(realTimeFlag==1)
					{
						//puts("entered");
						realTimeFlag=0;//setting the flag back to available
						terminatePcb(current_process);
						free(current_process);
						current_process = NULL;
					}
					//free  the current memory block
					else
					{
						memoryCounter=memoryCounter-current_process->mbytes;
						memFree(&mab_doubly_linked_list,current_process->mem_block);
						terminatePcb(current_process);
						free(current_process);
						current_process = NULL;
					}
				}
				else if(current_process->priority==1)
				{
					current_process->priority++;//increasing priority every second
					if(feedback_queue!=NULL ||feedback_queue1!=NULL)
					{
						process=suspendPcb(current_process);//suspending a process
						current_process=NULL;//assignign the current process to null
						feedback_queue1=enqPcb(feedback_queue1,process);//adding at the end of the highest priority feedback
					}
				}
				else if(current_process->priority==2)
				{
					current_process->priority++;//increasing priority every second
					if(feedback_queue!=NULL ||feedback_queue1!=NULL||hrrn_queue!=NULL)
					{
						process=suspendPcb(current_process);
						current_process=NULL;
							hrrn_queue=enqPcb(hrrn_queue,process);//adding to hrrn_queue

							//puts("suspended from feedbackqueue1");

					}
				}
				else if(feedback_queue!=NULL ||feedback_queue1!=NULL)
				{
					if(feedback_queue!=NULL)
					{
						process=suspendPcb(current_process);//suspending a process
						current_process=NULL;
							hrrn_queue=enqPcb(hrrn_queue,process);//adding at the end of hrrn_queue

					}
					if(feedback_queue1!=NULL)
					{
						process=suspendPcb(current_process);
						current_process=NULL;
							hrrn_queue=enqPcb(hrrn_queue,process);

							//puts("suspended from feedbackqueue1");

					}
				}
			}
			if(current_process==NULL&&(feedback_queue!=NULL||feedback_queue1!=NULL||hrrn_queue!=NULL||real_time_queue!=NULL))
			{
				//alwyas check to see if a real time process is available ebfore anything else
				if(real_time_queue!=NULL)
				{
					current_process=deqPcb(&real_time_queue);
					current_process->mem_block=realTime;
					realTimeFlag=1;//allocating memory block
				}
				else
				{
					if(feedback_queue!=NULL ||feedback_queue1!=NULL)//checkign which queue to dequeu from
					{
						if(feedback_queue!=NULL)
						{
						
							current_process=deqPcb(&feedback_queue);
							

						}
						else
						{
							if(feedback_queue1!=NULL)
							{	
							 current_process=deqPcb(&feedback_queue1);
							
							}
						}
					}
					else
					{
				
						if (hrrn_queue!=NULL) // checking if hrrn queue is not null
						{
							current_process=deqHrrn(&hrrn_queue,timer);
						}
					}
				}
				
						startPcb(current_process);// starting the process


			}
			
			
			sleep(1);
		timer++;
    }
//  3. Terminate the HOST dispatcher
    exit(EXIT_SUCCESS);
}

/*** END OF SECTION MARKER ***/

/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION IMPLEMENTATIONS MAY BE ADDED HERE ***/
/*** END OF SECTION MARKER ***/
/*** END OF CODE MARKER; DO NOT ADD MATERIAL BEYOND THIS POINT ***/
