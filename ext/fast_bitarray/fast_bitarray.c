#include <ruby.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

const unsigned int UNSIGNED_INT_BITS = 8 * sizeof(unsigned int);

ID CAPACITY_VARIABLE_NAME, DATA_VARIABLE_NAME;
VALUE cFastBitarray;

static VALUE fast_bitarray_init(VALUE self, VALUE rb_capacity) {
  Check_Type(rb_capacity, T_FIXNUM);

  unsigned int capacity = FIX2INT(rb_capacity);
  unsigned int *data = MEMZERO(ALLOC_N(unsigned int, capacity), unsigned int, capacity);

  rb_ivar_set(self, CAPACITY_VARIABLE_NAME, rb_capacity);
  rb_ivar_set(self, DATA_VARIABLE_NAME, rb_str_new((const char *)data, capacity * sizeof(unsigned int)));

  return self;
}

static VALUE fast_bitarray_capacity(VALUE self) {
  return rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
}

static void fast_bitarray_increase_capacity(VALUE self, unsigned int required_capacity) {
  unsigned int capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));

  unsigned int new_capacity = capacity;
  while (new_capacity < required_capacity) {
    new_capacity = 2 * new_capacity;
  }

  unsigned int *new_data = MEMZERO(ALLOC_N(unsigned int, new_capacity), unsigned int, new_capacity);
  MEMCPY(new_data, data, unsigned int, capacity);

  rb_ivar_set(self, CAPACITY_VARIABLE_NAME, INT2FIX(new_capacity));
  rb_ivar_set(self, DATA_VARIABLE_NAME, rb_str_new((const char *)new_data, new_capacity * sizeof(unsigned int)));
}

static VALUE fast_bitarray_set_bit(VALUE self, VALUE rb_bit_index) {
  if (NIL_P(rb_bit_index) || !FIXNUM_P(rb_bit_index)) {
    rb_raise(rb_eArgError, "Bit index must be a number");
  }

  unsigned int bit_index = FIX2INT(rb_bit_index);

  unsigned int quotient = bit_index / UNSIGNED_INT_BITS;
  unsigned int modulus = bit_index % UNSIGNED_INT_BITS;

  if (quotient + 1 > FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME))) {
    fast_bitarray_increase_capacity(self, quotient + 1);
  }

  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));
  data[quotient] = data[quotient] | (1 << modulus);

  return Qtrue;
}

static VALUE fast_bitarray_reset_bit(VALUE self, VALUE rb_bit_index) {
  if (NIL_P(rb_bit_index) || !FIXNUM_P(rb_bit_index)) {
    rb_raise(rb_eArgError, "Bit index must be a number");
  }

  unsigned int bit_index = FIX2INT(rb_bit_index);

  unsigned int quotient = bit_index / UNSIGNED_INT_BITS;
  unsigned int modulus = bit_index % UNSIGNED_INT_BITS;

  VALUE rb_capacity = rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
  if (quotient < FIX2INT(rb_capacity)) {
    VALUE rb_data = rb_ivar_get(self, DATA_VARIABLE_NAME);
    unsigned int *data = (unsigned int *)RSTRING_PTR(rb_data);
    data[quotient] = data[quotient] & ~(1 << modulus);
  }

  return Qtrue;
}

static VALUE fast_bitarray_has_bit(VALUE self, VALUE rb_bit_index) {
  if (NIL_P(rb_bit_index) || !FIXNUM_P(rb_bit_index)) {
    rb_raise(rb_eArgError, "Bit index must be a number");
  }

  unsigned int bit_index = FIX2INT(rb_bit_index);

  unsigned int quotient = bit_index / UNSIGNED_INT_BITS;
  unsigned int modulus = bit_index % UNSIGNED_INT_BITS;

  if (quotient < FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME))) {
    VALUE rb_data = rb_ivar_get(self, DATA_VARIABLE_NAME);
    unsigned int *data = (unsigned int *)RSTRING_PTR(rb_data);

    if ((data[quotient] & (1 << modulus)) != 0) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  } else {
    return Qfalse;
  }
}

static VALUE fast_bitarray_count(VALUE self) {
  unsigned int capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));

  unsigned int result = 0;

  unsigned int i;
  for (i = 0; i < capacity; i++) {
    unsigned int v = data[i];

    unsigned int c = ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
    c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
    c += ((v >> 24) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;

    result += c;
  }

  return INT2FIX(result);
}

static VALUE fast_bitarray_to_a(VALUE self) {
  unsigned int capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));

  VALUE result = rb_ary_new();

  unsigned int i;
  for (i = 0; i < capacity; i++) {
    unsigned int v = data[i];

    unsigned int mask = 1;
    unsigned int b;
    for(b = 0; b < UNSIGNED_INT_BITS; b++) {
      if (v & mask) {
        rb_ary_push(result, INT2FIX(UNSIGNED_INT_BITS * i + b));
      }
      mask = mask << 1;
    }
  }

  return result;
}

static VALUE fast_bitarray_union(VALUE self, VALUE rb_fast_bitarray) {
  Check_Type(rb_fast_bitarray, T_OBJECT);
  if (!RTEST(rb_obj_is_instance_of(rb_fast_bitarray, cFastBitarray))) {
    rb_raise(rb_eArgError, "Required instance of FastBitarray class");
  }

  unsigned int own_capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int arg_capacity = FIX2INT(rb_ivar_get(rb_fast_bitarray, CAPACITY_VARIABLE_NAME));

  VALUE result_args[1] = { INT2FIX(MAX(own_capacity, arg_capacity)) };
  VALUE result = rb_class_new_instance(1, result_args, cFastBitarray);

  unsigned int *own_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));
  unsigned int *arg_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(rb_fast_bitarray, DATA_VARIABLE_NAME));
  unsigned int *result_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(result, DATA_VARIABLE_NAME));

  unsigned int i;
  for (i = 0; i < MIN(own_capacity, arg_capacity); i++) {
    result_data[i] = own_data[i] | arg_data[i];
  }

  if (own_capacity > arg_capacity) {
    for (i = arg_capacity; i < own_capacity; i++) {
      result_data[i] = own_data[i];
    }
  } else if (arg_capacity > own_capacity) {
    for (i = own_capacity; i < arg_capacity; i++) {
      result_data[i] = arg_data[i];
    }
  }

  return result;
}

static VALUE fast_bitarray_intersection(VALUE self, VALUE rb_fast_bitarray) {
  Check_Type(rb_fast_bitarray, T_OBJECT);
  if (!RTEST(rb_obj_is_instance_of(rb_fast_bitarray, cFastBitarray))) {
    rb_raise(rb_eArgError, "Required instance of FastBitarray class");
  }

  unsigned int own_capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int arg_capacity = FIX2INT(rb_ivar_get(rb_fast_bitarray, CAPACITY_VARIABLE_NAME));

  VALUE result_args[1] = { INT2FIX(MAX(own_capacity, arg_capacity)) };
  VALUE result = rb_class_new_instance(1, result_args, cFastBitarray);

  unsigned int *own_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));
  unsigned int *arg_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(rb_fast_bitarray, DATA_VARIABLE_NAME));
  unsigned int *result_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(result, DATA_VARIABLE_NAME));

  unsigned int i;
  for (i = 0; i < MIN(own_capacity, arg_capacity); i++) {
    result_data[i] = own_data[i] & arg_data[i];
  }

  return result;
}

static VALUE fast_bitarray_difference(VALUE self, VALUE rb_fast_bitarray) {
  Check_Type(rb_fast_bitarray, T_OBJECT);
  if (!RTEST(rb_obj_is_instance_of(rb_fast_bitarray, cFastBitarray))) {
    rb_raise(rb_eArgError, "Required instance of FastBitarray class");
  }

  unsigned int own_capacity = FIX2INT(rb_ivar_get(self, CAPACITY_VARIABLE_NAME));
  unsigned int arg_capacity = FIX2INT(rb_ivar_get(rb_fast_bitarray, CAPACITY_VARIABLE_NAME));

  VALUE result_args[1] = { INT2FIX(MAX(own_capacity, arg_capacity)) };
  VALUE result = rb_class_new_instance(1, result_args, cFastBitarray);

  unsigned int *own_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(self, DATA_VARIABLE_NAME));
  unsigned int *arg_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(rb_fast_bitarray, DATA_VARIABLE_NAME));
  unsigned int *result_data = (unsigned int *)RSTRING_PTR(rb_ivar_get(result, DATA_VARIABLE_NAME));

  unsigned int i;
  for (i = 0; i < MIN(own_capacity, arg_capacity); i++) {
    result_data[i] = own_data[i] & ~arg_data[i];
  }

  if (own_capacity > arg_capacity) {
    unsigned int i;
    for (i = arg_capacity; i < own_capacity; i++) {
      result_data[i] = own_data[i];
    }
  }

  return result;
}

// Init BitArray class

void Init_fast_bitarray(void) {
  DATA_VARIABLE_NAME = rb_intern("data");
  CAPACITY_VARIABLE_NAME = rb_intern("capacity");

  cFastBitarray = rb_define_class("FastBitarray", rb_cObject);

  rb_define_method(cFastBitarray, "initialize", fast_bitarray_init, 1);

  rb_define_method(cFastBitarray, "capacity", fast_bitarray_capacity, 0);

  rb_define_method(cFastBitarray, "set_bit", fast_bitarray_set_bit, 1);
  rb_define_method(cFastBitarray, "reset_bit", fast_bitarray_reset_bit, 1);
  rb_define_method(cFastBitarray, "has_bit?", fast_bitarray_has_bit, 1);

  rb_define_method(cFastBitarray, "count", fast_bitarray_count, 0);
  rb_define_method(cFastBitarray, "to_a", fast_bitarray_to_a, 0);

  rb_define_method(cFastBitarray, "|", fast_bitarray_union, 1);
  rb_define_method(cFastBitarray, "+", fast_bitarray_union, 1);
  rb_define_method(cFastBitarray, "&", fast_bitarray_intersection, 1);
  rb_define_method(cFastBitarray, "-", fast_bitarray_difference, 1);
}