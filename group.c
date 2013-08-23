#include<stdio.h>
#include<ruby/ruby.h>


#define get_dat1(x) \
     struct symgrp *dat;\
     dat = ((struct symgrp *)(x))

#define ZERO INT2FIX(0)
#define ONE INT2FIX(1)
#define TWO INT2FIX(2)

VALUE cSymgrp;
 
struct symgrp {
  int num;
};

static void
symgrp_mark(struct symgrp *ptr)
{
  rb_gc_mark(ptr->num);
}

static VALUE
symgrp_alloc(VALUE klass)
{
  struct symgrp *ptr = ALLOC(struct symgrp);
  return Data_Wrap_Struct(klass, symgrp_mark, -1, ptr);
}

inline static void
symgrp_posint_check(VALUE num)
{
  //  printf("=%d\n", NUM2INT(num));
  switch (TYPE(num)) {
      case T_FIXNUM:
      case T_BIGNUM:
	if (NUM2INT(num) < 1) 
	  rb_raise(rb_eRuntimeError, "not permutation index");
	break;
      case T_FLOAT:
      case T_RATIONAL:
      default:
	if (!k_numeric_p(num) || !f_real_p(num))
	    rb_raise(rb_eTypeError, "not integer");
  }
}

static VALUE 
symgrp_initialize(VALUE self)
{
  struct symgrp *ptr;
  Data_Get_Struct(self, struct symgrp, ptr);
  ptr->num = 0;
  
  return self;
}

static VALUE
symgrp_new(VALUE self)
{
  VALUE res = symgrp_initialize(symgrp_alloc(self));
  return res;
}


static VALUE
symgrp_numref(VALUE self)
{
  struct symgrp *ptr;
  Data_Get_Struct(self, struct symgrp, ptr);
  return INT2FIX(ptr->num);
}

static VALUE
symgrp_numset(VALUE self, VALUE num)
{
  struct symgrp *ptr;
  Data_Get_Struct(self, struct symgrp, ptr);
  ptr->num = FIX2INT(num);
  return FIX2INT(num);
}

static VALUE
symgrp_inversion(VALUE self, VALUE a)
{
  VALUE ary;
  long len;
  int i,j;
  
  len = RARRAY_LEN(a);
  ary = rb_ary_new2(len);
  for(i = 1; i <= len; i++)
    for(j = 1; j <= len; j++){
      //      printf("i=%d argv[j-1]=%d\n", i,  FIX2INT(argv[j-1]));
      if(i == FIX2INT(rb_ary_entry(a,j-1))){	
	rb_ary_push(ary, INT2FIX(j));
	break;
      }  
    }
  return ary;
}


static VALUE
symgrp_inversion_number(VALUE self, VALUE a)
{
  int i,j, flag=0, len;

  len = RARRAY_LEN(a);
  for (j = 0; j < len; j++) {
    for(i = 0; i < j; i++) {
      symgrp_posint_check(rb_ary_entry(a,i));
      //      printf("*%d %d \n", FIX2INT(argv[i]), i);
      if (FIX2INT(rb_ary_entry(a,i)) > FIX2INT(rb_ary_entry(a,j)))
	flag += 1;
    }
  }
  
  return INT2FIX(flag);
}


static VALUE
symgrp_signature(VALUE self, VALUE a)
{
  VALUE flag = symgrp_inversion_number(self, a);
  return INT2FIX((FIX2INT(flag) % 2 == 0) ? 1 : -1);
}

static VALUE
symgrp_permute_p(VALUE self, VALUE a)
{
  int i;
  long len;
  VALUE ary;
  
  len = RARRAY_LEN(a);
  ary = rb_ary_new2(len);
  //  printf("%d\n", len);

  for(i = 0; i < len; i++){
    //    printf("argv[%d]=%d\n", i, FIX2INT(argv[i]));
    if (RTEST(rb_ary_entry(ary, FIX2INT(rb_ary_entry(a,i)))))
      return Qfalse;
    else
      rb_ary_store(ary, FIX2INT(rb_ary_entry(a,i)), ONE);
  }

  return Qtrue;
}

static VALUE
symgrp_cyclic_p(VALUE self, VALUE a)
{
  int i, idx;
  long len;
  VALUE v, vbeg, vend;

  len = RARRAY_LEN(a);
  symgrp_permute_p(self, a);

  idx = 0;
  vbeg = rb_ary_entry(a, 0);
  for (i = 0; i < len; i++) {
    v = rb_ary_entry(a, idx);
    if (NUM2INT(v)-1 == idx) return Qfalse;
    idx = NUM2INT(v)-1;
  }
  vend = rb_ary_entry(a, idx);
  //  printf("argv[%d]=%d\n i=%d\n", idx, NUM2INT(argv[idx]), i);   
  if (RTEST(rb_equal(vbeg, vend)) && i == len)
    return Qtrue;
  return Qfalse;
}

void Init_symgrp(void)
{
  cSymgrp = rb_define_class("Symgrp", rb_cArray);

  rb_define_alloc_func(cSymgrp, symgrp_alloc);

  rb_define_method(cSymgrp, "new" , symgrp_new, 0);
  rb_define_method(cSymgrp, "num", symgrp_numref, 0);
  rb_define_method(cSymgrp, "set", symgrp_numset, 1);
  rb_define_method(cSymgrp, "invnum", symgrp_inversion_number, 1);
  rb_define_method(cSymgrp, "sig", symgrp_signature, 1);
  rb_define_method(cSymgrp, "permute?", symgrp_permute_p, 1);
  rb_define_method(cSymgrp, "cyclic?", symgrp_cyclic_p, 1);
  rb_define_method(cSymgrp, "inv", symgrp_inversion, 1);
}

