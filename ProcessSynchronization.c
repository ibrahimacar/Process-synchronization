#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//threadWorks
void *threadWorkA();
void *threadWorkB();
void *threadWorkC();
//Mutexes
pthread_mutex_t N1;
pthread_mutex_t N2;
pthread_mutex_t DEST;
//Checks end of the threadworks if both equals 0 breaks the loop
int end,endd=1;

//This function returns whether number is prime or not
bool findPrime(int n){
     int i, flag = 0;

     for(i=2; i<=n/2; ++i)
     {
         // condition for nonprime number
         if(n%i==0)
         {
             flag=1;
             break;
         }
     }

     if (flag==0)
         return true;
     else
         return false;

 }

 //Deletes negative numbers of a file
 void deleteNegative(char* filename){

 char* outFileName = "tmp.txt";
 FILE *file = fopen ( filename, "r" );
 FILE *outFile=fopen(outFileName,"w+");

 if ( file != NULL )
 {
    char line [ 10 ]; /* or other suitable maximum line size*/
    while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line*/
    {

      if( atoi(line)>0 )
   {
       fprintf(outFile, "%s", line);
       printf(line);
   }

}

 fclose(file);
 fclose(outFile);

 if( !rename(outFileName,filename ) )
 {
   printf("Rename Error");
 }

    }
    else
    {
       perror ( filename ); /* why didn't the file open?*/
    }
}
//Deletes prime numbers of a file
void deletePrime(char* filename){

  char* outFileName = "tmp.txt";
  FILE *file = fopen ( filename, "r" );
  FILE *outFile=fopen(outFileName,"w+");

  if ( file != NULL )
  {
     char line [ 10 ]; /* or other suitable maximum line size*/
     while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line*/
     {

       if(!findPrime(atoi(line)))
    {
        fprintf(outFile, "%s", line);
        printf(line);
    }

  }
  fclose(file);
  fclose(outFile);
  if( !rename(outFileName,filename ) )
  {
    printf("Rename Error");
  }

     }
     else
     {
        perror ( filename ); /* why didn't the file open?*/
     }
}

//cuts and pastes to destination
int cutAndPaste(char* filename){
  char* destFileName = "dest.txt";
  char* temp="tmp.txt";
  int controller=1;
  FILE *file = fopen ( filename, "r" );
  FILE* outFile = fopen(destFileName, "a");
  FILE *tmp=fopen(temp, "w+");

  if ( file != NULL )
  {
    srand(time(NULL));
    int r = (rand()%10)+1;
    char line [ 10 ];
    for (int i = 0; i < r; i++) {

 if(fgets ( line, sizeof line, file )!=NULL){
  fprintf(outFile, "%s", line);
}
else
  controller=0;
    break;


}
     while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
     {
        fprintf(tmp, "%s", line);
        printf(line);

  }

  fclose(file);
  fclose(tmp);
  fclose(outFile);
  if( !rename(temp,filename))
  {
    printf("Rename Error");
  }

     }
     else
     {
        perror ( filename ); /* why didn't the file open?*/
     }


  return controller;

}
//used three mutex if thread has just n1 or n2 it releases it. if has both n1,destination
//or n2 destination uses cut and paste function atomically
void *threadWorkA()
{

while(1){
if(end==0 && endd==0){break;}

if(pthread_mutex_trylock(&N1)==0){
  printf("N1\n");
  if(pthread_mutex_trylock(&DEST)==0){
    printf("DEST\n");
    char *filename="numbers1.txt";
    endd=cutAndPaste(filename);
    pthread_mutex_unlock(&DEST);
    pthread_mutex_unlock(&N1);

  }
printf("N1__\n");

pthread_mutex_unlock(&N1);
}

else if(pthread_mutex_trylock(&N2)==0){

      if(pthread_mutex_trylock(&DEST)==0){
    printf("DEST2\n");
    char *filenamee="numbers2.txt";
    end=cutAndPaste(filenamee);

    pthread_mutex_unlock(&DEST);
    pthread_mutex_unlock(&N2);

  }
pthread_mutex_unlock(&N2);
printf("N2__" );
}


}
	pthread_exit(0);
}

//Deletes prime numbers. always tries to reach one of the file and if catchs lock
//deletes primes after that releases the file again
void *threadWorkB(){
while(1){
if(end==0 && endd ==0){break;}
if(pthread_mutex_trylock(&N1)==0){

char* filenameee= "numbers1.txt";

deletePrime(filenameee);

pthread_mutex_unlock(&N1);
}
else if(pthread_mutex_trylock(&N2)==0){
char* filenameeee= "numbers2.txt";
deletePrime(filenameeee);
pthread_mutex_trylock(&N2);
}
else if(pthread_mutex_trylock(&DEST)==0){
char* fileNameeee= "dest.txt";
deletePrime(fileNameeee);
pthread_mutex_trylock(&DEST);
}
}
}
//deletes negative numbers tries to reach always after do its job it releases the file.
void *threadWorkC(){
  while(1){
  if(end==0 && endd ==0){break;}
  if(pthread_mutex_trylock(&N1)==0){
  char* filename1= "numbers1.txt";
  deleteNegative(filename1);

  }
  else if(pthread_mutex_trylock(&N2)==0){
  char* filename2= "numbers2.txt";
  deleteNegative(filename2);

  }
  else if(pthread_mutex_trylock(&DEST)==0){
  char* fileName3= "dest.txt";
  deleteNegative(fileName3);

  }
  }
  }

int main(int argc, char* argv[])
{

	int threadCountA = 4;
  int threadCountB=1;
  int threadCountC=1;
	pthread_t tid[threadCountA+threadCountB+threadCountC];
 	int  i=0;

	for(i=0; i<threadCountA; i++)
	{
		pthread_create(&tid[i],NULL,threadWorkA,NULL);
	}
  pthread_create(&tid[threadCountA],NULL,threadWorkB,NULL);
  pthread_create(&tid[threadCountA+1],NULL,threadWorkC,NULL);
	for(i=0; i<threadCountA; i++)
	{
		pthread_join(tid[i+2],NULL);
	}


	return 1;
}
