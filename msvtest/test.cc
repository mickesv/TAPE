#define GOD 1
#define GOOD 0

// I am using this to testing my metrics suite...

int aGlobalVariable=10;
int aGlobalVariable2=10;
class myClass;

/*struct tttt {
  int r; 
};

struct ttt {
  int v;
  int c;
  tttt e;
  tttt* d;
};

struct tt {
  ttt w;
  ttt* s;
};

struct t {
  tt q;
  tt* a;
};


void xxbar(void)
{
  int i;
  int k;
  k=i;
}

void xbar(void)
{
  struct ttt q;
  q.v=10;
  xxbar();
}
*/
/*
int foo(void)
{
  int q[3]={1,2,3};
  int k;
  //  bar();
  xbar();
  xbar();
  //  woo(q[k]);
  return 10;
}

int woo(int shoo, char q, double w, int* p, int e)
{
  int k;
  if(1) {
    if(1) {
      int aGlobalVariable;
      aGlobalVariable=1;
    }
  }
  aGlobalVariable+=1;
  k=(shoo+2);
  foo();
  bar();

  return 1;
}

*/
class myClass {
 public:
  myClass() { // This ought to create a stack overflow! :-)
    c=new myClass();
  }
/*  int qwoo(int w) {
    qq=1;
    return 1;
  };
  int qqwoo(int shoo, char q, double w, int* p, int e) {
    qq=2;
    return 1;
  }
  
  void t() {
  };

  myClass* f() {
    return new myClass();
  }

  myClass* ff(int x) {
    return new myClass();
  }
*/

  int qq;
  myClass* c;
};

/*
class mySubClass : public myClass {
 public:
  int qwww() {
    qq=3;
    aGlobalVariable=2;
    return 1;
  };
};
*/


void bar(void)
{
  //  xbar();
  myClass theC;
  myClass *fooPtr;
  theC.c=0;
  
  if (theC.c == 0) {};
  for (int i = 0; fooPtr != theC.c; i++) {  // Why is not the conditional part even evaluated?
    theC.c=0;
  }

  theC.c=0;
  theC.c=fooPtr;
  return;
}


int main(void)
{
  myClass theC;
  theC.qwoo(1);
  theC.c->t();
  theC.c->f()->t();
  theC.c->ff(foo())->t();
  theC.c->ff(theC.c->qwoo(1))->t(); // Clang makes it to 8 (+/-1) calls and member references on this line.
  theC.c->c->t();
  theC.c->c->c->qq=foo();


  /*
  foo();
  theC.c->c->c->c->qwoo(1);
  foo();
  theC.f()->f()->f()->qwoo(1);
  
  foo();
  t theT;
  foo();
  theT.q.w.e.r=1;
  foo();
  theT.a->s->d->r=2;
  foo();
  t* theTT=new t();
  foo();
  theTT->a->s->d->r=3;
  */

  /*
  int i;
  int k;

  foo();
  i=1;
  k=i;
  */
  
  /*  foo();
  aGlobalVariable=20;
  foo();
  aGlobalVariable=aGlobalVariable2;
  foo();
  // These are the cases of loopDescent that I am looking for.
  // Let me count the ways.

  // For loops
  // Standard for loop. LoopDepth++ for foo call.
  for(i=0;i<10;i++)
    {
      foo();
    }

  // Use of foo in initialisation. LoopDepth unchanged.
  for (i=foo();i<=10;i++)
    {
      foo();
    }

  // Use of foo in iteration. LoopDepth++ for foo()
  for (i=0;i<=foo();i++)
    {
      foo();
    }

  // Use of foo in iteration. LoopDepth++ for foo()
  for (i=0;i<=10;i+=foo())
    {
      foo();
    }

  // Foo galore
  for (foo(); foo(); foo()) foo();

  // For statements not followed by compoundstatements. LoopDepth++
  for (i=0;i<=10;i++) foo();
  for (i=0;i<=10;i++) i=foo();


  // Nested loop
  for(;;) {
    for (;;) {
      foo();
    }}
  foo();

  // While
  while(GOD != GOOD) foo();

  while(GOD != GOOD) {
    foo();
  }

  while(foo()) {
    foo();
  }

  do {
    foo();
  } while(GOD!=GOOD);

  do {
  } while(foo());

  woo(i);


  // printf("Hello World");
  //  if(1) { printf("h2");} else { printf("H3"); }
  //  for(int i=0;i<10;i++) {
  //    printf("H4");
  //  }
  //  while(1==0) k=10;
  */
}
