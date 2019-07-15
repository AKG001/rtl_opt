/* Test for _Dependent_ptr. _Dependent_ptr test in which reload kills dependency. Refer figure 11 in document p0190r4 (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0190r4.pdf). */
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

_Atomic struct rcutest *gp;
_Atomic struct rcutest * _Dependent_ptr gsgp;

#define rcu_assign_pointer(p,v)		\
  atomic_store_explicit(&(p), (v), memory_order_release);

#define rcu_dereference(p) 		\
  atomic_load_explicit(&(p), memory_order_consume);

void thread0 ()
{
  struct rcutest *p;

  p = (struct rcutest *)malloc (sizeof (*p));
  assert (p);
  p->a = 42;
  rcu_assign_pointer (gp,p);		/* { dg-warning "\\\[-Wincompatible-pointer-types]" } */
}

void thread1 ()
{
  struct rcutest * _Dependent_ptr p = rcu_dereference (gp);		/* { dg-warning "\\\[-Wincompatible-pointer-types]" } */
  atomic_store_explicit(&gsgp, p, memory_order_relaxed);		/* { dg-warning "\\\[-Wincompatible-pointer-types]" } */
}

void thread2 ()
{
  struct rcutest * _Dependent_ptr p;

  p = atomic_load_explicit(&gsgp, memory_order_relaxed);		/* { dg-warning "\\\[-Wincompatible-pointer-types]" } */
  if (p)
    assert(p->a == 42);
}
