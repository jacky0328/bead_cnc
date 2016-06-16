#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#define plus 6

#define  H  5
#define  W  6
//#define  MAX_STEP 3
//#define DEBUG


/*
  紅火 -> 1
  藍水 -> 2
  綠葉 -> 3
  金星 -> 4
  紫月 -> 5
  粉紅新-> 6
*/

int MAX_STEP = 20;

int   weighting_table [7] = {0,1,1,1,1,1,1}; //  index 0 is not used

unsigned int wr_ptr,rd_ptr;
//int i,j,x,y;


int  point_num;
char  point_array[H*W*2];

void main_func(char* in_s);

//  function
int    get_val(int val);
void   print_data(int data[H][W]);
void   neighbor(int data[H][W]);
void   queue_init();
void   pop_queue(int* y, int*x);
void   push_queue(int y, int x);
int    queue_empty();
int    queue_array[H*W][2];
int    bfs(int data[H][W] ,int * combo_cnt);
int    search_point(char visit[H][W],int data[H][W]);
void   new_xy_val(int y,int x,int i,int *yy,int *xx);
char   check_dir(int y, int x, int i);
void   drop(int data[H][W]);

int   estimate_score(int data[H][W]);

int   sort_array[8][2];
void copy_array(int data[H][W], int data_tmp[H][W]);
void swap_data(int data_tmp[H][W],int i,int y,int x);
void sort_score(int sort_array[8][2], int num);
int abs(int a);
void clear_visit(char visit[H][W]);
int find_max_score(int score_array[H*W]);
void print_path(int * path);



static PyObject *calc_path(PyObject *self, PyObject *args) {
   //int i;
   //double d;
   char *s;

   if (!PyArg_ParseTuple(args, "si",&s,&MAX_STEP)) {
      return NULL;
   }

   /* Do something interesting here. */
   main_func(s);
   return Py_BuildValue("s",point_array);

}

static PyMethodDef bead_algorithm_funcs[] = {
  
   { "calc_path", calc_path, METH_VARARGS, NULL },
   { NULL, NULL, 0, NULL }
};

void initbead_algorithm(void)
{
    Py_InitModule3("bead_algorithm", bead_algorithm_funcs,
                   "Extension module example!");
}






void main_func(char* in_s)
{
	//or


/*
  紅火 -> 1
  藍水 -> 2
  綠葉 -> 3
  金星 -> 4
  紫月 -> 5
  粉紅新-> 6
*/
/*
int data[H][W]={
                  {1,1,1,5,5,5},
                  {1,1,1,4,1,1},
                  {1,1,1,4,4,4},
                  {2,2,1,4,1,4},
                  {1,2,1,4,1,4}};
*/

int  data[H][W];
int data_tmp[H][W];
int data_tmp2[H][W];
int final_index;
int first_step;
int last_dir;
int step,num;
	int i,x,y,xx,yy,xxx,yyy,x2,y2;
char	 visit[H][W];
int final_score;
#ifdef DEBUG
	printf("Original data\n");
	print_data(data);
#endif

int* path[H*W];
int  score_array[H*W];
int max_index;

int   index=0;
/////////////////////////////////////////////////////////////////////////////////////////////////

    for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
           data[y][x] =  in_s[index]- '0';
           path[y*W+x] = (int*)malloc(W*H*sizeof(int));
           index++;
        }

printf("MAX_STEP : %d\n",MAX_STEP);
print_data(data);


   //y=1;
   //x=1;
    for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
           step=0;
           copy_array(data, data_tmp);
           first_step=1;
           yy=y;
           xx=x;
           clear_visit(visit);
           visit[yy][xx]=1;
#ifdef DEBUG
//               printf("========================================\n");
//               printf("search_point:(x:%d,y:%d)\n",x,y);

#endif
           while(step < MAX_STEP)
           {
#ifdef DEBUG
//               printf("point:(x:%d,y:%d)\n",xx,yy);
#endif

               num=0;
               for(i=0;i<8;i++)
               {
                  new_xy_val(yy,xx,i,&y2,&x2);

                  if(check_dir(yy,xx,i) &
                                           (first_step |
                                           ((!first_step) & (abs(last_dir-i)!=4))
                                       )
                    )
                  {
                     copy_array(data_tmp, data_tmp2);
                     swap_data(data_tmp2,i,yy,xx);
#ifdef DEBUG
                     printf("After swap: (x: %d, y:%d, index:%d)\n",xx,yy,i);
                     print_data(data_tmp2);
#endif
                     sort_array[num][0] = estimate_score(data_tmp2);
                     sort_array[num][1] = i;
#ifdef DEBUG
                     printf("score:%d , index:%d\n",sort_array[num][0],i);
#endif // DEBUG
                     num++;
                  }
               }
               // sort score
               sort_score(sort_array,num);
               final_index = sort_array[num-1][1];
               final_score = sort_array[num-1][0];

               last_dir=final_index;
               swap_data(data_tmp,final_index,yy,xx);
#ifdef DEBUG
 //              printf("========================================\n");
  //             printf("Step %d:\n",step);
               printf("After swap: (x: %d, y:%d, choose_index:%d)\n",xx,yy,final_index);
               print_data(data_tmp);
#endif
               new_xy_val(yy,xx,final_index,&yyy,&xxx);

               path[y*W+x][step]  = yy*W+xx;
               score_array[y*W+x] = final_score;
               yy = yyy;
               xx = xxx;
               visit[yy][xx]=0;
               first_step=0;
                 step++;
           }
#ifdef DEBUG
               printf("========================================\n");
               printf("start_point (x:%d,y:%d) score:%d\n",x,y,final_score);
#endif // DEBUG

        }

       max_index = find_max_score(score_array);

        printf("max_index : %d, max_score : %d, (x:%d,y:%d)\n",max_index,score_array[max_index],max_index%W,max_index/W);

        print_path(path[max_index]);


        point_num = MAX_STEP;

       for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
           free (path[y*W+x]);

        }
}

void print_path(int * path)
{
    int i,x,y;
    for(i=0;i<MAX_STEP;i++)
    {
       x = path[i] %W;
       y = path[i] / W;
       printf("P(%2d,%2d) ",x,y);

       point_array[i*2+0] = x + '0';
       point_array[i*2+1] = y + '0';
       if((i%6)==5)
          printf("\n");
    }
          printf("\n");
}


void clear_visit(char visit[H][W])
{
    int x,y;
    for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        visit[y][x]=0;

}

int abs(int a)
{
    if(a>=0)
        return a;
    else
       return -a;
}

int find_max_score(int score_array[H*W])
{
    int i;

    int max_val=0,index;
    for(i=0;i<H*W;i++)
    {
        if(max_val < score_array[i])
        {
          max_val = score_array[i];
          index = i;
        }
    }

    return index;

}

void sort_score(int sort_array[8][2], int num)
{
    int i;
    int j;
    int tmp0,tmp1;
    for(i=0;i<num;i++)
        for(j=i+1;j<num;j++)
    {
          if(sort_array[i][0] >= sort_array[j][0])
          {
              //swap
              tmp0 = sort_array[i][0];
              tmp1 = sort_array[i][1];
              sort_array[i][0] = sort_array[j][0];
              sort_array[i][1] = sort_array[j][1];
              sort_array[j][0] = tmp0;
              sort_array[j][1] = tmp1;
          }
    }
}


void swap_data(int data_tmp[H][W],int i,int y,int x)
{
    int yy,xx,tmp;

    new_xy_val(y,x,i,&yy,&xx);
    tmp = data_tmp[y][x];
    data_tmp[y][x]=data_tmp[yy][xx];
    data_tmp[yy][xx] = tmp;
}

void copy_array(int data[H][W], int data_tmp[H][W])
{
    int y,x;
      for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
           data_tmp[y][x] = data[y][x];
        }
}

int   estimate_score(int data[H][W])
{
    int bead_cnt;
    int bead_total_cnt=0;
    int combo_cnt=0;
    int combo_total_cnt=0;
	int total_score;
      for(;;)
          {
            neighbor(data);
#ifdef DEBUG
 //           printf("After neightbor\n");
  //          print_data(data);
#endif
            bead_cnt=bfs(data, &combo_cnt);
            if(bead_cnt==0)
                break;
            else{
                bead_total_cnt+=bead_cnt;
                combo_total_cnt +=combo_cnt;
            }
            drop(data);
#ifdef DEBUG
    //        printf("After drop\n");
    //        print_data(data);
#endif
           }

             total_score= bead_total_cnt* combo_total_cnt;
#ifdef DEBUG
            printf("total_score:%d, total_comb:%d, total_bead\n",total_score,combo_total_cnt,bead_total_cnt);
#endif


           return total_score;
}


void   drop(int data[H][W])
{
   int tmp[H][W];
   int x,y;

   int dest_ptr=0;

   for(y=0;y<H;y++)
     for(x=0;x<W;x++)
     {
       tmp[y][x]=0;
     }

     for(x=0;x<W;x++)
     {
         dest_ptr=0;
         for(y=0;y<H;y++)
         {
            if(data[y][x] <= plus)
                tmp[dest_ptr++][x] = data[y][x];

         }
     }

     //copy to  data

     for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
           data[y][x]= tmp[y][x];
        }
}

int   bfs(int data[H][W], int * combo_cnt)
{

       int bead_cnt,y,x;
       int bead_cnt_total_cnt=0;

       char visit[H][W];

       // init visit array
       for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
            visit[y][x]=0;
        }


        *combo_cnt=0;
       //
        for(y=0;y<H;y++)
        for(x=0;x<W;x++)
        {
            if((~visit[y][x]) & (data[y][x]>plus))
            {
                queue_init();
                push_queue(y,x);
                visit[y][x] =1;
                bead_cnt=search_point(visit,data);
                bead_cnt_total_cnt += bead_cnt;
                (*combo_cnt)++;
            }
        }

 //       printf("score:%d\n",score);
        return bead_cnt_total_cnt;
}

int   search_point(char visit[H][W], int data[H][W])
{
       int y,x,i;
       int yy,xx;
       int combo_cnt=1;

       /*
           7  0  1
           6  *  2
           5  4  3
       */

        while(!queue_empty())
        {
           pop_queue(&y,&x);
           //printf("pop:  x:%d , y : %d\n",x,y);
           int val = data[y][x];

           if((x==2) && (y==4))
              x=2;

           for(i=0;i<8;i++)  // 0 ~ 7
           {
              new_xy_val(y,x,i,&yy,&xx);
              if(check_dir(y,x,i) & (~visit[yy][xx]) & (data[yy][xx] == val))
              {
                 push_queue(yy,xx);
                 visit[yy][xx] = 1;
                 combo_cnt++;
                // printf("push x:%d , y : %d, i : %d , visit:%d\n",xx,yy,i,visit[yy][xx]);
              }
           }
        }
   //     printf("combo_cnt: %d\n",combo_cnt);
        return combo_cnt;
}

int   queue_empty()
{
        return (wr_ptr == rd_ptr);
}

void new_xy_val(int y,int x,int i,int *yy,int *xx)
{
         switch(i)
         {
         case 0 :
                   *yy = y+1;
                   *xx = x;
                   break;
         case 1 :
                   *yy = y+1;
                   *xx = x+1;
                   break;
         case 2 :
                   *yy = y;
                   *xx = x+1;
                   break;
         case 3 :
                   *yy = y-1;
                   *xx = x+1;
                   break;
         case 4 :
                   *yy = y-1;
                   *xx = x;
                   break;
         case 5 :
                   *yy = y-1;
                   *xx = x-1;
                   break;
         case 6 :
                   *yy = y;
                   *xx = x-1;
                   break;
         default :  // case 7
                   *yy = y+1;
                   *xx = x-1;
                   break;
         }
}


char    check_dir(int y, int x, int i)
{
        if((x==0) & ((i==7) | (i==6) | (i==5)))
           return 0;
        else if((x==(W-1)) & ((i==1) | (i==2) | (i==3)))
           return 0;
        else if((y==0) & ((i==5) | (i==4) | (i==3)))
            return 0;
        else if((y==(H-1)) & ((i==7) | (i==0) | (i==1)))
            return 0;
        else
            return 1;
}

void   queue_init()
{
        wr_ptr =0;
        rd_ptr =0;
}

void   push_queue(int y, int x)
{
        queue_array[wr_ptr][0] = y;
        queue_array[wr_ptr][1] = x;
        wr_ptr++;
}

void    pop_queue(int* y, int*x)
{
       *y =  queue_array[rd_ptr][0];
       *x =  queue_array[rd_ptr][1];
       rd_ptr++;

}

void neighbor(int data[H][W])
{
    int x,y;

    for(y=0;y<H;y++)
	{
		  for(x=0;x<W-2;x++)
		  {
		  	   if( (get_val(data[y][x]  ) == get_val(data[y][x+1])) &&
                   (get_val(data[y][x+1]) == get_val(data[y][x+2])) &&
                   (get_val(data[y][x])   != 0) )
               {

                    data[y][x] = get_val(data[y][x]) + plus;
		            data[y][x+1] = get_val(data[y][x+1]) + plus;
		            data[y][x+2] = get_val(data[y][x+2]) + plus;
               }
		  }

	}


	for(x=0;x<W;x++)
	{
		  for(y=0;y<H-2;y++)
		  {
		  	   if( (get_val(data[y][x]  ) == get_val(data[y+1][x])) &&
                   (get_val(data[y+1][x]) == get_val(data[y+2][x])) &&
                   (get_val(data[y][x]  ) != 0) )
               {

                    data[y][x]   = get_val(data[y][x]) + plus;
		            data[y+1][x] = get_val(data[y+1][x]) + plus;
		            data[y+2][x] = get_val(data[y+2][x]) + plus;
               }
		  }

	}
}


void print_data(int data[H][W])
{
    int x,y;
        for(y=0;y<H;y++)
        {
           for(x=0;x<W;x++)
           {
               printf("%02d ",data[y][x]);
           }
            printf("\n");
        }
        printf("\n");
}

int   get_val(int val){
	if(val > plus)
	   return val - plus;
	else
	   return val;
}
