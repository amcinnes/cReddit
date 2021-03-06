#include "reddit.h"

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
      /* out of memory! */ 
      printf("not enough memory (realloc returned NULL)\n");
      exit(EXIT_FAILURE);
    }
 
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
 
    return realsize;
}
 
void redditGetSubreddit(char * sub, char * sorting, struct post * postList)
{
  CURL *curl_handle;
  struct MemoryStruct chunk;
 	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;   
	curl_handle = curl_easy_init();
 
 	//GET request 
  char url[256];
  strcpy(url,"http://reddit.com");
  strcat(url,sub);
  strcat(url,"/");
  strcat(url,sorting);
  strcat(url,".json");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "cReddit/0.0.1 opensource mainline by /u/blacksmid"); //Our user-agent!
 
  //Lets request
  curl_easy_perform(curl_handle);

  //and cleanup
  curl_easy_cleanup(curl_handle);

 	const char *js;
  int r;
  jsmn_parser p;
  jsmntok_t t[2500];
  js = chunk.memory;
  jsmn_init(&p);
  r = jsmn_parse(&p, js, t, 25000);
  int i =0;
  char buffer[2048];

  int atPost = 0;
  for(i = 0; i < 25000; ++i)
  {
	if(t[i].start == -1) continue;
  	if(t[i].type == 1 ) {i+=t[i].size;  continue;}
  	if(t[i].start >= t[i].end || t[i].end-t[i].start > 2040) continue;
  	memcpy(buffer,&chunk.memory[t[i].start],t[i].end-t[i].start);
  	buffer[t[i].end-t[i].start] = 0;
	
	if(strcmp("id",buffer) == 0)
	{
		i++;
		char *tmp = malloc(t[i].end-t[i].start+1);
		memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
		postList[atPost].id = malloc(t[i].end-t[i].start+1);
		tmp[t[i].end-t[i].start] = 0;
		strcpy(postList[atPost].id,tmp);
		free(tmp);
	}		
        if(strcmp("title",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].title = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].title,tmp);
		free(tmp);
        }
        if(strcmp("score",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].votes = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].votes,tmp);
		free(tmp);
        }
        if(strcmp("author",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].author = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].author,tmp);
		atPost++;
		free(tmp);
		if(atPost == 25)
			break;
        }
        if(strcmp("subreddit",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                tmp[t[i].end-t[i].start] = 0;
                postList[atPost].subreddit = malloc(t[i].end-t[i].start+1);
                strcpy(postList[atPost].subreddit,tmp);
		free(tmp);
        }

  }
  if(chunk.memory)
    free(chunk.memory);
 
}
void redditGetThread(char * postid, struct comments * postList)
{
  	CURL *curl_handle;
  	struct MemoryStruct chunk;
	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  	chunk.size = 0;   
	curl_handle = curl_easy_init();
 
 	//GET request 
  	char url[256];
  	strcpy(url,"http://reddit.com/comments/");
  	strcat(url,postid);
  	strcat(url,"/GET.json");
	printw("Getting:%s\n",url);
	getch();
  	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "cReddit/0.0.1 opensource mainline by /u/blacksmid"); //Our user-agent!
 
  	//Lets request
  	curl_easy_perform(curl_handle);
/*
  	//and cleanup
  	curl_easy_cleanup(curl_handle);
/*
 	const char *js;
  	int r;
  	jsmn_parser p;
  	jsmntok_t t[2500];
  	js = chunk.memory;
  	jsmn_init(&p);
  	r = jsmn_parse(&p, js, t, 250);
  	int i =0;
  	char buffer[2048];

  	int atPost = 0;
  	for(i = 0; i < 250; ++i)
  	{
/*	printw("%i",i);
	if(t[i].start == -1) continue;
  	if(t[i].type == 1 ) {i+=t[i].size;  continue;}
  	if(t[i].start >= t[i].end || t[i].end-t[i].start > 2040) continue;
  	memcpy(buffer,&chunk.memory[t[i].start],t[i].end-t[i].start);
  	buffer[t[i].end-t[i].start] = 0;
	//printw("Buffer:%s\n",buffer);
	refresh();
	/*
	if(strcmp("id",buffer) == 0)
	{
		i++;
		char *tmp = malloc(t[i].end-t[i].start+1);
		memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
		postList[atPost].id = malloc(t[i].end-t[i].start+1);
		tmp[t[i].end-t[i].start] = 0;
		strcpy(postList[atPost].id,tmp);
		free(tmp);
	}		
        if(strcmp("title",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].title = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].title,tmp);
		free(tmp);
        }
        if(strcmp("score",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].votes = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].votes,tmp);
		free(tmp);
        }
        if(strcmp("author",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                postList[atPost].author = malloc(t[i].end-t[i].start+1);
                tmp[t[i].end-t[i].start] = 0;
                strcpy(postList[atPost].author,tmp);
		atPost++;
		free(tmp);
		if(atPost == 25)
			break;
        }
        if(strcmp("subreddit",buffer) == 0)
        {
                i++;
                char *tmp = malloc(t[i].end-t[i].start+1);
                memcpy(tmp,&chunk.memory[t[i].start],t[i].end-t[i].start);
                tmp[t[i].end-t[i].start] = 0;
                postList[atPost].subreddit = malloc(t[i].end-t[i].start+1);
                strcpy(postList[atPost].subreddit,tmp);
		free(tmp);
        }

  }*/
  if(chunk.memory)
    free(chunk.memory);
 
}

