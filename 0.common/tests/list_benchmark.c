
#include <stdio.h>
#include "list.h"
#include <sys/time.h>

static void
bm(char *label, void (*fn)())
{
  printf(" %18s", label);
  fflush(stdout);
  fn();
}

static int nnodes = 100000;
static float startTime;

static float gettime()
{
	struct timeval t;
	gettimeofday(&t,NULL); 
  float tmp = (float)t.tv_sec + (float)t.tv_usec/1000000.0;
  return tmp;
}
static void
start()
{
  startTime = gettime();;
}

static void
stop()
{
  float duration = gettime() - startTime;
  printf(": \x1b[32m%.4f\x1b[0ms\n", duration);
}

static void
bm_rpush()
{
  start();
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_rpush(list, list_node_new("foo"));
  }
  stop();
}

static void
bm_lpush()
{
  start();
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_lpush(list, list_node_new("foo"));
  }
  stop();
}

static void
bm_find()
{
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_lpush(list, list_node_new("foo"));
  }
  list_rpush(list, list_node_new("bar"));
  start();
  list_find(list, "bar");
  stop();
}

static void
bm_iterate()
{
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_lpush(list, list_node_new("foo"));
  }
  list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
  list_node_t *node;
  start();
  while ((node = list_iterator_next(it)))
    ;
  stop();
}

static void
bm_rpop()
{
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_lpush(list, list_node_new("foo"));
  }
  list_node_t *node;
  start();
  while ((node = list_rpop(list)))
    ;
  stop();
}

static void
bm_lpop()
{
  int n = nnodes;
  list_t *list = list_new();
  while (n--)
  {
    list_lpush(list, list_node_new("foo"));
  }
  list_node_t *node;
  start();
  while ((node = list_lpop(list)))
    ;
  stop();
}

static list_t *list;

static void
bm_at()
{
  start();
  list_at(list, 100000);
  stop();
}

static void
bm_at2()
{
  start();
  list_at(list, 1000000);
  stop();
}

static void
bm_at3()
{
  start();
  list_at(list, -100000);
  stop();
}

int main(void)
{
  int n = nnodes;
  list = list_new();
  while (n--)
    list_lpush(list, list_node_new("foo"));
  printf("\n %d nodes\n",nnodes);
  bm("lpush", bm_lpush);
  bm("rpush", bm_rpush);
  bm("lpop", bm_lpop);
  bm("rpop", bm_rpop);
  bm("find (last node)", bm_find);
  bm("iterate", bm_iterate);
  bm("at(100,000)", bm_at);
  bm("at(1,000,000)", bm_at2);
  bm("at(-100,000)", bm_at3);
  puts("");
  return 0;
}
