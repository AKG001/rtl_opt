/* Test for _Dependent_ptr. _Dependent_ptr test: Integer-pointer subtraction dependency. Refer figure 20 in document p0190r4 (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0190r4.pdf). */
/* { dg-do compile } */
/* { dg-options "-std=c11 -pedantic" } */

#include "../gcc/gcc/ginclude/stdatomic.h"      /* Change the path where stdatomic.h is present inside the trunk */

typedef __SIZE_TYPE__ size_t;
extern void abort (void);
extern void exit (int);
extern void *malloc (size_t);
extern int assert ();

struct rcutest
{
  int a;
  int b;
  int c;
};

_Atomic int *gip;

#define rcu_assign_pointer(p,v)         \
  atomic_store_explicit(&(p), (v), memory_order_release);

#define rcu_dereference(p)              \
  atomic_load_explicit(&(p), memory_order_consume);

void thread0 (void)
{
  int *p;

  p = (int *)malloc(4*sizeof(*p));
  assert(p);
  p[0] = -1;
  p[42] = 42;
  rcu_assign_pointer (gip, &p[1]);
}

void thread1 (void)
{
  int * _Dependent_ptr p;

  p = rcu_dereference(gip);
  if (p)
    assert(*(p+p[0]) == 42);
}
