#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<malloc.h>

int nwork, nmachine;
FILE* queneresult;

struct work  //工作的结构
{
	int workid;
	int workgeneration; 
	double benefit; //工作的收益
	double workload; //工作的时长
	double duedate; //工作的时限
	work *lastwork; //排好后下一个工作
	work *nextwork; //拍好后上一个工作
};

struct machine //生产线的结构
{
	work* firstwork;//为了方便定位工作
	work* lastwork;//用工作来定位生产线 每个生产线现有的工作为一个从工作A到工作B的链表
	double capacity;//生产线的capacity，暂时先不用
	double makespan;//生产线的结束时间
	double profit;//生产线的收益，我们的目标
	int avaliable;//生产线是否参与排序
	machine *lastmachine;//方便查找machine用
	machine *nextmachine;
};

struct sortresult
{
	work* work_quene_head;
	machine* machine_quene_head;
	work* work_pool_head;
};

work* getworkdata(FILE* fp) //工作录入函数
{
	work* datahead = NULL;
	work* datatail = NULL;
	work* datanode = NULL;
	int i;
	nwork = 0;
    ////printf("开始录入工作"); 
	datahead = (work*) malloc(sizeof(work));  
	if(datahead == NULL)
	{
		//printf("不能分配地址1");
		return NULL;
	}
	datahead->nextwork = NULL;
	datahead->lastwork = NULL;
	datatail = datahead;
	datahead->benefit = -1;
	datahead->duedate = -1;
	datahead->workload = -1;
	datahead->workgeneration = 0;
    fscanf(fp, "%lf",&datahead->benefit );//利益 duedate 时长 
	fscanf(fp, "%lf",&datahead->duedate);
	fscanf(fp, "%lf",&datahead->workload);
	datahead->workid = 1;
	if ((datahead->benefit == -1) || (datahead->duedate == -1) || (datahead->workload == -1))
	{
		printf("数据有误，请检查数据2");
		return NULL;
	}
	for (i=2; !feof(fp) ; i++)
	{
		if ((datanode = (work*)(malloc(sizeof(work)))) == NULL)
		{
				//printf("不能分配地址，内存无法建立链表3");
				return NULL;
		}
		datanode->nextwork = NULL;
		datanode->lastwork = datatail;
		datanode->workid = i;
		datanode->benefit = -1;
		datanode->duedate = -1;
		datanode->workload = -1;
		datanode->workgeneration = 0;
		fscanf(fp, "%lf",&datanode->benefit);
		fscanf(fp, "%lf",&datanode->duedate);
		fscanf(fp, "%lf",&datanode->workload);
		datatail->nextwork = datanode;
		datatail = datanode;
	//	printf("已录入%lf%lf%lf",datanode->benefit,datanode->duedate,datanode->workload);
		if ((datanode->benefit == -1) || (datanode->duedate == -1) || (datanode->workload == -1))
		{
			printf("数据有误，请检查数据5");
			return NULL;
		}
	}
	//printf("工作录入完成4");
	nwork = i-1;
//	printf("共有%d个工作", nwork);
	return datahead;
}

machine* getmachinedata(FILE* fp) //机器录入函数
{
	machine* datahead = NULL;
	machine* datatail = NULL;
	machine* datanode = NULL;
	int i=0;
	//printf("~1");
	if ((datahead = (machine*)(malloc(sizeof(machine)))) == NULL)
	{
		//printf("不能分配地址6");
		return NULL;
	}
	//printf("1111");
	datahead->nextmachine = NULL;
	datahead->lastmachine = NULL;
	datatail = datahead;
	//printf("xxxx");
	datahead->capacity = -1;
	datahead->makespan = -1;
	fscanf(fp, "%lf",&datahead->capacity);
	fscanf(fp, "%lf",&datahead->makespan);
	i++;
	//printf("第%d台机器的capacity是%lf",i,datahead->capacity);
	//printf("第%d台机器的makespan是%lf",i,datahead->makespan); 
	//printf("w");
	datahead->profit = 0;
	datahead->avaliable = 1;
	datahead->firstwork = NULL;
	datahead->lastwork = NULL;
	//printf("2222");
	if ((datahead->capacity == -1) || (datahead->makespan == -1))
	{
		//printf("数据有误，请检查数据7");
		return NULL;
	}
	for (i; !feof(fp); i++)
	{
		if ((datanode = (machine*)(malloc(sizeof(machine)))) == NULL)
		{
			//printf("不能分配地址，内存无法建立链表8");
			return NULL;
		}
		datanode->nextmachine = NULL;
		datanode->lastmachine = datatail;
		datanode->capacity = -1;
		datanode->makespan = -1;
		fscanf(fp, "%lf",&datanode->capacity);
		fscanf(fp, "%lf",&datanode->makespan);
		datanode->profit = 0;//默认0
		datanode->avaliable = 1;//有机会再用
		datanode->firstwork = NULL;
		datanode->lastwork = NULL;
		datatail->nextmachine = datanode;
		datatail = datanode;
		//printf("第%d台机器的capacity是%lf",i+1,datanode->capacity);
		//printf("第%d台机器的makespan是%lf",i+1,datanode->makespan); 
		if ((datahead->capacity == -1) || (datahead->makespan == -1))
		{
			//printf("数据有误，请检查数据10");
			return NULL;
		}
	}
	nmachine = i;
	//printf("机器录入完成。");
	return datahead;
}



work* sort_work(work* data, int mode, int cp)//工作的冒泡排序
{
	work* p = NULL;
	work* q = NULL;
	work* temp = (work*)malloc(sizeof(work));
	if((data == NULL)||(data->nextwork==NULL))
	return data;
	//printf("sortwork");
	if (mode == 0)//初始排序
	{
		for (p = data; p->nextwork != NULL; p = p->nextwork)
		{
			for (q = p->nextwork; q != NULL; q = q->nextwork)
			{
				if (((p->duedate) > ( q->duedate))&&(cp==0))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
				if (((p->duedate) < ( q->duedate))&&(cp==1))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
				if (((p->benefit) > ( q->benefit))&&(cp==2))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
				if (((p->benefit) < ( q->benefit))&&(cp==3))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
				if (((p->benefit)/(p->duedate-p->workload+1) > (q->benefit)/(q->duedate-q->workload+1))&&(cp==4))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
				if (((p->benefit)/(p->duedate-p->workload+1) < (q->benefit)/(q->duedate-q->workload+1))&&(cp==5))
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
			}
		}
	}
    else if (mode == 1)//从大到小
	{
		//printf("sortwork1.0");
		for (p = data; p->nextwork != NULL; p = p->nextwork)
		{
			//printf("sortwork1");
			for(q = p->nextwork; q!= NULL; q = q->nextwork)
			{
				//printf("sorkwork1.01");
			if (p->benefit < q->benefit)
			{
				temp->benefit = p->benefit;
				temp->duedate = p->duedate;
				temp->workid = p->workid;
				temp->workload = p->workload;
				temp->workgeneration = p->workgeneration;
				p->benefit = q->benefit;
				p->duedate = q->duedate;
				p->workid = q->workid;
				p->workload = q->workload;
				p->workgeneration = q->workgeneration;
				q->benefit = temp->benefit;
				q->duedate = temp->duedate;
				q->workid = temp->workid;
				q->workload = temp->workload;
				q->workgeneration = temp->workgeneration;
			}
			//printf("sortwork1.1");
		}
		}
	}
	else if (mode == 2)// 从小到大
	{
		for (p = data; p->nextwork != NULL; p = p->nextwork)
		{
			for (q = p->nextwork; q != NULL; q = q->nextwork)
			{
				if (p->benefit > q->benefit)
				{
					temp->benefit = p->benefit;
					temp->duedate = p->duedate;
					temp->workid = p->workid;
					temp->workload = p->workload;
					temp->workgeneration = p->workgeneration;
					p->benefit = q->benefit;
					p->duedate = q->duedate;
					p->workid = q->workid;
					p->workload = q->workload;
					p->workgeneration = q->workgeneration;
					q->benefit = temp->benefit;
					q->duedate = temp->duedate;
					q->workid = temp->workid;
					q->workload = temp->workload;
					q->workgeneration = temp->workgeneration;
				}
			}
		}
	}
	//printf("worksortted");
	free(temp);
	return data;
}

double machine_profit(machine* id,double op)//原始profit，方便以后修改用
{
	work* i;
	double machineprofit = op;
	if(id->firstwork == NULL)
	{
		return op;
	}
	for (i = id->firstwork;i!= NULL; i = i->nextwork)
	{
		machineprofit = machineprofit + i->benefit;
	}
	return machineprofit;
}

double machine_workload(machine* id,double op)
{
	work* i;
	double machineworkload = op;
	if(id->firstwork == NULL)
	{
		return op;
	}
	for (i = id->firstwork;i!= NULL; i = i->nextwork)
	{
		machineworkload = machineworkload + i->workload;
	}
	return machineworkload;
}

void outputw(FILE* fp,sortresult data)
{
	for (work* workquenep = data.work_quene_head;workquenep != NULL;workquenep= workquenep->nextwork)
	{
		//printf("%d,%f,%f,%f\n", workquenep->workid, workquenep->benefit, workquenep->workload, workquenep->duedate);
		fprintf(fp, "%d,%f,%f,%f\n", workquenep->workid, workquenep->benefit, workquenep->workload, workquenep->duedate);
	}
}

void outputwr(FILE* fp, sortresult data)
{
	int i = 1;

	for(machine* machinequenep = data.machine_quene_head; machinequenep != NULL; machinequenep = machinequenep->nextmachine)
	{
		fprintf(fp, "%d  ", i);
		//printf("%d  ", i);
		for (work* workquenep = machinequenep->firstwork;workquenep != NULL;workquenep = workquenep->nextwork)
		{
			fprintf(fp, "%d,", workquenep->workid);
			//printf("%d,", workquenep->workid);
		}
		fprintf(fp,"beneift%lf",machine_profit(machinequenep,0));
		fprintf(fp,"makespan%lf",machine_workload(machinequenep,0));
		fprintf(fp, "\n");
		i = i + 1;
	}

}

void outputm(FILE* fp, sortresult data)
{
	int i = 1;
	for (machine* machinequenep = data.machine_quene_head; machinequenep != NULL; machinequenep = machinequenep->nextmachine)
	{
		fprintf(fp, "%d,%f,%f,%f\n",i, machinequenep->capacity, machinequenep->makespan, machinequenep->profit);
		//printf("%d,%f,%f,%f\n",i, machinequenep->capacity, machinequenep->makespan, machinequenep->profit);
		i = i + 1;
	}
}

void outputrcp(FILE* fp,sortresult data)
{
	machine* machinequenep = data.machine_quene_head;
	double rcpdata=machinequenep->profit;
	for (machinequenep; machinequenep != NULL; machinequenep = machinequenep->nextmachine)
	{
		if(rcpdata>machinequenep->profit)
		rcpdata=machinequenep->profit;
	}
	fprintf(fp, " %f ",rcpdata);
}

void freedata(sortresult data)
{
	machine* fm;
	machine* fmq = data.machine_quene_head;
	work* fw = NULL;
	work* fmw = NULL;
	work* fwp = data.work_pool_head;
	work* fwq = data.work_quene_head;
	while(fwp!=NULL)
	{
		fw = fwp;
		fwp = fwp->nextwork;
//		printf("free work pool");		
		free(fw);
	}
	while(fwq!=NULL)
	{
		fw = fwq;
		fwq = fwq->nextwork;
//		printf("free work quene");
		free(fw);
	}
	while(fmq!=NULL)
	{
		fm = fmq;
		fmw = fmq->firstwork;
		while(fmw!=NULL)
		{
			fw = fmw;
			fmw = fmw->nextwork;
//			printf("free machine work");
			free(fw);
		}
		fmq = fmq->nextmachine;
//		printf("free machine");
		free(fm);
	}
}

machine* resort_machine(machine* machinepool,machine* resortmachine)
{
	
	int juge0 = 0;
	machine* sorrting_machine = resortmachine;
	machine* sorrting_l = resortmachine->lastmachine;
	machine* sorrting_n = resortmachine->nextmachine;
	if (machinepool == resortmachine)
	{
		machinepool = machinepool->nextmachine;
		machinepool->lastmachine = NULL;
		resortmachine->nextmachine = NULL;
		resortmachine->lastmachine = NULL;
	}
	else if(sorrting_n != NULL)
	{
		sorrting_l->nextmachine = sorrting_n;
		sorrting_n->lastmachine = sorrting_l;
		resortmachine->nextmachine = NULL;
		resortmachine->lastmachine = NULL;
	}
	else
	{
		sorrting_l->nextmachine = NULL;
		resortmachine->nextmachine = NULL;
		resortmachine->lastmachine = NULL;
	}
	for(sorrting_machine = machinepool; (sorrting_machine->profit < resortmachine->profit)&&(sorrting_machine!= NULL); sorrting_machine = sorrting_machine->nextmachine)
	{
		if (sorrting_machine->nextmachine ==NULL)
		{
			juge0 = 1;
			sorrting_machine->nextmachine = resortmachine;
			resortmachine->lastmachine = sorrting_machine;
		}
		
	}
	if(juge0 ==0)
	{
	sorrting_l = sorrting_machine->lastmachine;
	sorrting_n = sorrting_machine;
	if(sorrting_l!=NULL)
	sorrting_l->nextmachine = resortmachine;
	resortmachine->lastmachine = sorrting_l;
	sorrting_machine->lastmachine = resortmachine;
	resortmachine->nextmachine = sorrting_machine;
    }
	for(sorrting_machine;sorrting_machine->lastmachine!=NULL;sorrting_machine=sorrting_machine->lastmachine)
	;
	return sorrting_machine;
}

int fillwork (work* data, int workpooloption)
{
	work* datatail;
	work* currentwork = data;
	work* datanode = NULL;
	//printf("cha0");
	//printf("%d%d",nmachine,nwork);
	for(currentwork;currentwork->nextwork!=NULL;currentwork=currentwork->nextwork)
	;
	datatail = currentwork;
	currentwork = NULL;
	if(workpooloption!=0)
	{
		for (nwork; (nwork % (workpooloption*nmachine) )!= 0; nwork++)
		{
		if ((datanode = (work*)(malloc(sizeof(work)))) == NULL)
		{
			//printf("不能分配地址，内存无法建立链表11");
			return NULL;
		}
		//printf("cha1");
		datanode->nextwork = NULL;
		datanode->lastwork = datatail;
			//printf("cha2");
		datanode->workid = nwork+1;
		datanode->benefit = 0;
		datanode->duedate = 100000;
		datanode->workload = 0;
		datanode->workgeneration = 0;
		//printf("%d",datanode->workid);
			//printf("cha3");
		datatail->nextwork = datanode;
		datatail = datanode;
			//printf("cha4");
		}
		
	    for (int i=0;i<(nmachine * workpooloption) ; i++)
		{
		if ((datanode = (work*)(malloc(sizeof(work)))) == NULL)
		{
			//printf("不能分配地址，内存无法建立链表11");
			return NULL;
		}
		//printf("cha1");
		datanode->nextwork = NULL;
		datanode->lastwork = datatail;
			//printf("cha2");
		datanode->workid = nwork+1;
		datanode->benefit = 0;
		datanode->duedate = 100000;
		datanode->workload = 0;
		datanode->workgeneration = 0;
		//printf("%d",datanode->workid);
			//printf("cha3");
		datatail->nextwork = datanode;
		datatail = datanode;
		nwork++;
			//printf("cha4");
		}
	}
	else
	{
		if ((datanode = (work*)(malloc(sizeof(work)))) == NULL)
		{
			//printf("不能分配地址，内存无法建立链表11");
			return NULL;
		}
		//printf("cha1");
		datanode->nextwork = NULL;
		datanode->lastwork = datatail;
			//printf("cha2");
		datanode->workid = nwork+1;
		datanode->benefit = 0;
		datanode->duedate = 100000;
		datanode->workload = 0;
		datanode->workgeneration = 0;
		//printf("%d",datanode->workid);
			//printf("cha3");
		datatail->nextwork = datanode;
		datatail = datanode;
		nwork++;
			//printf("cha4");
	}

	
	
	
	//printf("xyz");
	return 0;
}


sortresult sortinitialization(work* workhead,machine* machinehead)
{
	sortresult sortresult_current;
	sortresult_current.machine_quene_head = machinehead;
	sortresult_current.work_quene_head = workhead;
	sortresult_current.work_pool_head = NULL;
	return sortresult_current;
}

work* machinesortstep(work* cwork,machine* cmachine, int maxgeneration)
{
	int cid = cwork->workid;
	double creleasetime = 0;
	double ct = 0;
	double pprofit;
	work* mm;
	work* op;
	work* opn;
	work* opl;
	work* backtopool = NULL;
	work* lbacktopool = NULL;
	work* ccworkpool = cwork;
	work* templ;
	work* tempn;
	work* machinememory[nwork];
	int mmn = 0;
	printf("%d in",cid);
	fprintf(queneresult,"%d in %p",cid,cmachine);
//把工作放到一个较好的位置
    //printf("machinesortstepstart");
    //printf("%d--",cwork->workid);
    //printf("%d",cmachine);
    
    
    if(cwork->workgeneration > maxgeneration)
	{
		pprofit = cmachine->profit;
		for(mm=cmachine->firstwork;mm!=NULL;mm = mm->nextwork)
		{
			machinememory[mmn] = mm;
			mmn++;
		}
	}
    
    printf("machine  states1\r\n");
    printf("%p",cmachine->firstwork); 
	/*for(work* cprintwork=cmachine->firstwork;cprintwork!=NULL;cprintwork=cprintwork->nextwork)
		printf("%d, ",cprintwork->workid);
	printf("\r\n");
	
    printf("machine  states save\r\n");
	for(int o=0;o<mmn;o++)
		printf("%d, ",machinememory[o]->workid);
	printf("\r\n");
	*/			
	if (cmachine->firstwork == NULL)
	{
		cmachine->firstwork = cwork;
		cwork->lastwork = NULL;
		cwork->nextwork = NULL;
	}
	else
	{
		 printf("machinesortstep1");
		for (op = cmachine->firstwork;op!= NULL;op = op->nextwork)
		{
			opl = op->lastwork;
			//printf("machinesortstep2");
			creleasetime = creleasetime + op->workload;
			if ((creleasetime > cmachine->capacity - cwork->workload) || (creleasetime > cwork->duedate - cwork->workload))
			{
					 //printf("machinesortstep3");
				if (op == cmachine->firstwork)
				{
					cmachine->firstwork = cwork;
					cwork->lastwork = NULL;
					cwork->nextwork = op;
					op->lastwork = cwork;
				}
				else
				{
				cwork->nextwork = op;
				cwork->lastwork = op->lastwork;
				op->lastwork->nextwork = cwork;
				op->lastwork = cwork;
				}
				break;
			}
			else if(op->benefit<cwork->benefit)
			{
				if (op == cmachine->firstwork)
				{
					cmachine->firstwork = cwork;
					cwork->lastwork = NULL;
					cwork->nextwork = op;
					op->lastwork = cwork;
				}
				else
				{
				cwork->nextwork = op;
				cwork->lastwork = op->lastwork;
				op->lastwork->nextwork = cwork;
				op->lastwork = cwork;
				}
				//printf("machinesortstep3.0");
				break;
				 
			}
			
			else if(op->nextwork==NULL)
		{
			op->nextwork = cwork;
			cwork->lastwork = op;
			cwork->nextwork = NULL;
			break;
		}
			
			//printf("machinesortstep4");
			//creleasetime = creleasetime + op->workload;
			
		}
		
	}
	//printf("%p",cmachine);
	/*
	printf("\r\n");
	printf("machine in states\r\n");
	for(work* cprintwork=cmachine->firstwork;cprintwork!=NULL;cprintwork=cprintwork->nextwork)
		printf("%d, ",cprintwork->workid);
	printf("\r\n");
	*/
	op= cmachine->firstwork ; 
	while (op != NULL)//检查工作
	{
		printf("jiancha1");
		opn = op->nextwork;
		opl = op->lastwork;
		ct = ct + op->workload;
		if ((ct > op->duedate ) || (ct > cmachine->capacity))
		{
			printf("ko%d",op->workid);
			if((opl!=NULL)&&(opn!=NULL))
			{
			printf("jiancha1.1");
			opl->nextwork = opn;
			opn->lastwork = opl;		
		    }
		    else if(opl==NULL)
		    {
		    	if(opn!=NULL)
		    	{
				opn->lastwork = NULL;
		    	cmachine->firstwork = opn;
				}
		    	else
		    	{
		    		cmachine->firstwork = NULL;
				}
			}
			else if(opn==NULL)
			{
				opl->nextwork =NULL;
			}
			lbacktopool = backtopool;
		    backtopool = op;
			backtopool->nextwork = lbacktopool;
			backtopool->lastwork = NULL;
			printf("jiancha1.2");
			if (lbacktopool != NULL)
			{
				lbacktopool->lastwork = backtopool;
			}
			ct = ct - op->workload;
            op = opn;

		}
		else
		op = op->nextwork ;
		printf("jiancha2");
	}
	/*
		printf("\r\n");
		printf("machine out states\r\n");
	for(work* cprintwork=cmachine->firstwork;cprintwork!=NULL;cprintwork=cprintwork->nextwork)
	printf("%d, ",cprintwork->workid);
	
		printf("\r\n");
		*/
	//printf("jiancha3.0");
	cmachine->profit = machine_profit(cmachine,0);
	//printf("profit after this iteration %lf\r\n",cmachine->profit);
	//printf("profit before this iteration %lf\r\n",pprofit);
	if(cwork->workgeneration > maxgeneration)///////////////////////////////////////////////////////////////////GENERATIONN 
	{
		if(cmachine->profit<=pprofit)
		{
			printf("??");
			printf("%d",cwork->workid);

		    cmachine->firstwork = machinememory[0];
			mm = cmachine->firstwork;
			mm->lastwork = NULL;	
			printf("%d",mm->workid);
			for(int i=0;i<mmn-1;i++)
			{
				mm->nextwork = machinememory[i+1];
				mm->nextwork->lastwork = mm;
				mm = mm->nextwork;
			}
			mm->nextwork = NULL;
	   
	   printf("%d",cwork->workid);
	    	cwork->workgeneration = -cwork->workgeneration;
	    	cwork->lastwork = NULL;
     		cwork->nextwork = NULL;
     		backtopool = NULL;
     		cmachine->profit = machine_profit(cmachine,0);
     		
     		return cwork;
		}

	}
	printf("jiancha3");
	if(backtopool!=NULL)
	{
	for(work* toutput=backtopool;toutput!=NULL;toutput=toutput->nextwork)
	{
	//	printf(";%d out++++++++++++",toutput->workid);
	//	printf(";%d generation ",toutput->workgeneration);
		fprintf(queneresult,"%d out",toutput->workid);
    }
	}
	return backtopool;
}

sortresult mainsortstep(sortresult data, int workgenerationlimit, int workpoollimit)
{
	/*读取数据*/
	work* workhead = data.work_quene_head;
	work* workpoolcurrent = data.work_pool_head;
	work* currentwork = workhead;
	work* currentgenerationwork;
	work* sortworkhead = workhead;
	work* workpooltail = NULL;
	work* workpooltaill = NULL;
	work* workpooltailn = NULL;
	work* wpwc;
	int generationchoice = 1;
	machine* currentmachine= data.machine_quene_head;
	int check0=0;
	/*切开要排的工作，加入本轮pool中*/
	//printf("kai");

	if(workpoolcurrent!=NULL)
	{
		
	//	printf("workpool "); 
	/*    for (workpoolcurrent;workpoolcurrent->nextwork != NULL;workpoolcurrent = workpoolcurrent->nextwork)
	    {
	    	printf("%d",workpoolcurrent->workid);
		    //printf("kai1");
	    }
	    */
	    workpoolcurrent->nextwork = workhead;
	    workhead->lastwork = workpoolcurrent;
	    workpoolcurrent = data.work_pool_head;	
	    //printf("kai2");
	    for (int i = 1;i < (nmachine*workpoollimit); i++)
	    {
		    currentwork = currentwork->nextwork;
	    }
	    workhead = currentwork->nextwork;
	    data.work_quene_head = workhead;
	    if(workhead != NULL)
		    workhead->lastwork = NULL;
	    currentwork->nextwork = NULL;
	    currentwork = data.work_pool_head;
    }
    else
    {
    	//printf("kai1N");
    //	printf("workpoolNULL!");
    	workpoolcurrent=data.work_quene_head;
    	for (int i = 1;i < (nmachine*workpoollimit); i++)
	    {
		currentwork = currentwork->nextwork;
	    }
	    workhead = currentwork->nextwork;
	    currentwork->nextwork = NULL;
	    if(workhead != NULL)
		    workhead->lastwork = NULL;
		data.work_quene_head = workhead;
		currentwork = workpoolcurrent;
    }
    /*排序开始*/
    //printf("kai3");
	currentwork = sort_work(workpoolcurrent, 1,0);
	printf("\r\ncurrent work pool is \r\n");
	for(work* printcurrentwork=currentwork;printcurrentwork!=NULL;printcurrentwork=printcurrentwork->nextwork)
    printf("%d, ",printcurrentwork->workid);
	if(currentwork==NULL)
	printf("no currentwork work pool\r\n");
	printf("kai3.5");
	
	while(currentwork!=NULL)
	{
		currentgenerationwork = currentwork;
		while(currentgenerationwork->workgeneration > generationchoice)
		{
			
			currentgenerationwork = currentgenerationwork->nextwork;
			if(currentgenerationwork == NULL)
			{
				generationchoice++;
				currentgenerationwork = currentwork;
			}
			else
			{
				currentwork = currentgenerationwork;
			}
		
		}
		printf("\r\ncurrentwork %d generation %d",currentwork->workid,currentwork->workgeneration);
		
		if (currentwork->workgeneration <0 )
		{
			printf("kai4");
			currentwork->workgeneration = -currentwork->workgeneration;
			currentwork=currentwork->nextwork;
			continue;
		}
		printf("kai4.1");	
		
		printf("kai4.2");
		currentwork->workgeneration ++;
		if(workpoolcurrent == currentwork)
		{
			workpoolcurrent = workpoolcurrent->nextwork;
			if(workpoolcurrent!=NULL)
			workpoolcurrent->lastwork = NULL;
			printf("第一个工作被排进去了");
		}
		else
		{
			printf("kai4.201");
			currentwork->lastwork->nextwork = currentwork->nextwork ;
			if(currentwork->nextwork!=NULL)
			currentwork->nextwork->lastwork = currentwork->lastwork ;
		}
		//printf("kai4.21");
		/*for(currentmachine;currentmachine!=NULL;currentmachine->nextmachine )
		{
			if((machine_workload(currentmachine,currentmachine->makespan)+))
		}*/
		workpooltailn = machinesortstep(currentwork, currentmachine,workgenerationlimit); //..........................................
		
		if(workpooltailn!=NULL)
		{
		    if(workpoolcurrent==NULL)
		    {
		    	workpoolcurrent = workpooltailn;
	    	}
		    else
		    {
		    	for(workpooltail=workpoolcurrent;workpooltail->nextwork!=NULL;workpooltail=workpooltail->nextwork)
			    ;
		     	workpooltail->nextwork = workpooltailn;
		    	workpooltailn->lastwork = workpooltail;
		    }
		}
		
		 
		
		
		data.machine_quene_head = resort_machine(data.machine_quene_head,currentmachine);
		printf("kai4.27");
		currentmachine = data.machine_quene_head;
		//printf("kai4.28");	
		currentwork = sort_work(workpoolcurrent, 1,0);
	//	printf("kai4.3");
		if (currentwork == NULL)
		{
			//printf("kai4.4");
			break;
		}
	}
	printf("kai5.0");
	data.work_pool_head = sort_work(workpoolcurrent, 1,0);
/*	if(data.work_pool_head==NULL)
		printf("there is no workpool after this iteration \r\n");
	else 
		printf("workpoolheadis %d\r\n",data.work_pool_head->workid);
	if(data.work_quene_head==NULL)
		printf("all work has been scheduled after this iteration\r\n");
	else
		printf("workqueneheadis %d\r\n",data.work_quene_head->workid);
		*/
	////printf("kai5");
	for (currentwork = data.work_pool_head;currentwork != NULL; currentwork = currentwork->nextwork)
		currentwork->workgeneration = 0;
	////printf("kai8");
	for (currentmachine = data.machine_quene_head; currentmachine != NULL; currentmachine = currentmachine->nextmachine)
	{
	//	printf("\r\n ......machine");
		for (currentwork = currentmachine->firstwork ;currentwork != NULL; currentwork = currentwork->nextwork)
		{
			currentwork->workgeneration = 0 ;
	//		printf(",%d",currentwork->workid);
		}

	}
	////printf("kai9");
	return data;
}
/*
int main(int argc, char* argv[])
{
	nmachine = 2;
	sortresult ordata;
	FILE* wd,*md,*m0,*w0,*resultt,*resultcompare;
	char resultfile[500];
	work* workdata0;
	machine* machinedata0;
	////printf("kaishi0");
	wd=fopen("workdata.txt","r+");
	md=fopen("machinedata.txt","r+");
	////printf("kaishi"); 
	workdata0 = (work*)malloc(sizeof(work));
	////printf("k");
	machinedata0 = getmachinedata(md);
	////printf("1");
	workdata0 = getworkdata(wd);
	//printf("2");
	fillwork(workdata0);
	sort_work(workdata0,0);
	//printf("3");
	ordata = sortinitialization(workdata0,machinedata0);
	//printf("4");
	fclose(wd);
	fclose(md);
	m0=fopen("m0.txt","w+");
	w0=fopen("w0.txt","w+");
	//printf("5");
	outputm(m0, ordata);
	//printf("5.1");
	outputw(w0, ordata);
	//printf("5.3");
	while (ordata.work_quene_head != NULL)
	{
		//printf("5.5");
		ordata = mainsortstep(ordata);
	}
	//printf("6");
	sprintf(resultfile,"%s",argv[1]);
	resultt=fopen(resultfile,"w+");
	outputwr(resultt, ordata);
	fclose(resultt);
	resultcompare = fopen("resultall.rcp","a+");
	outputrcp(resultcompare,ordata);
	fclose(resultcompare);
return 0;
}
*/ 
int main(int argc, char* argv[])
{
	nmachine = 2;
	sortresult ordata;
	FILE* wd,*md,*m0,*w0,*resultt,*resultcompare;
	char resultfile[500];
	char workdata1[200];
	char machinedata1[200];
	char quenefile[500];
	work* workdata0;
	machine* machinedata0;
	////printf("kaishi0");
//	printf("%s %s %d",argv[0],argv[1],argv[2]);
//	sprintf(workdata1,"%s\\savedataall\\workdata%d.txt",argv[1],atoi(argv[2]));
//	sprintf(machinedata1,"%s\\savedataall\\machinedata%d.txt",argv[1],atoi(argv[2]));
    wd=fopen("workdata1.txt","r+");
    md=fopen("machinedata1.txt","r+");

//	wd=fopen(workdata1,"r+");
//	md=fopen(machinedata1,"r+");


	////printf("kaishi"); 
	workdata0 = (work*)malloc(sizeof(work));
	////printf("k");
	machinedata0 = getmachinedata(md);
	////printf("1");
	workdata0 = getworkdata(wd);
	//printf("2");
//	fillwork(workdata0,atoi(argv[4]));
//	sort_work(workdata0,0,atoi(argv[5]));

    fillwork(workdata0,1);
	sort_work(workdata0,0,2);

	//printf("3");
	ordata = sortinitialization(workdata0,machinedata0);
	//printf("4");
	fclose(wd);
	fclose(md);
	m0=fopen("m0.txt","w+");
	w0=fopen("w0.txt","w+");
	//printf("5");
	outputm(m0, ordata);
	//printf("5.1");
	outputw(w0, ordata);
	//printf("5.3");
//	sprintf(quenefile,"%s\\result\\result%d.quene",argv[1],atoi(argv[2]));
    sprintf(quenefile,"quene.txt") ;
	queneresult = fopen(quenefile,"w+");
	while (ordata.work_quene_head != NULL)
	{
		printf("5.5");
		//printf("\r\nnew++\r\n");
	//	ordata = mainsortstep(ordata,atoi(argv[3]),atoi(argv[4]));//workgeneration limit/workpool limit
		ordata = mainsortstep(ordata,2,1);//workgeneration limit/workpool limit
	}
	//printf("6");
//	sprintf(resultfile,"%s\\result\\result%d.lbo",argv[1],atoi(argv[2]));
	sprintf(resultfile,"result2.txt");
	resultt=fopen(resultfile,"w+");
	outputwr(resultt, ordata);
	fclose(resultt);
	fclose(queneresult);
	resultcompare = fopen("resultall.rcp","a+");
	
	outputrcp(resultcompare,ordata);
	fclose(resultcompare);
//	printf("start free");
	
	freedata(ordata);
	
return 0;
}

