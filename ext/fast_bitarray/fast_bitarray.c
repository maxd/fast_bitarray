#include <ruby.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

const unsigned int UNSIGNED_INT_BITS = 8 * sizeof(unsigned int);

ID CAPACITY_VARIABLE_NAME, DATA_VARIABLE_NAME, MIN_POSITION_VARIABLE_NAME, MAX_POSITION_VARIABLE_NAME;

static VALUE fast_bitarray_init(VALUE self, VALUE rb_capacity) {
  Check_Type(rb_capacity, T_FIXNUM);

  unsigned int capacity = FIX2INT(rb_capacity);
  unsigned int *data = MEMZERO(ALLOC_N(unsigned int, capacity), unsigned int, capacity);

  rb_ivar_set(self, CAPACITY_VARIABLE_NAME, rb_capacity);
  rb_ivar_set(self, DATA_VARIABLE_NAME, rb_str_new((const char *)data, capacity * sizeof(unsigned int)));
  rb_ivar_set(self, MIN_POSITION_VARIABLE_NAME, INT2FIX(INT_MAX));
  rb_ivar_set(self, MAX_POSITION_VARIABLE_NAME, INT2FIX(0));

  return self;
}

static VALUE fast_bitarray_capacity(VALUE self) {
  return rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
}

static void fast_bitarray_increase_capacity(VALUE self, unsigned int required_capacity) {
  VALUE rb_capacity = rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
  unsigned int capacity = FIX2INT(rb_capacity);

  VALUE rb_data = rb_ivar_get(self, DATA_VARIABLE_NAME);
  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_data);

  unsigned int new_capacity = capacity;
  while (new_capacity <= required_capacity) {
    new_capacity = 2 * new_capacity;
  }

  unsigned int *new_data = MEMZERO(ALLOC_N(unsigned int, new_capacity), unsigned int, new_capacity);
  MEMCPY(data, new_data, unsigned int, capacity);

  rb_ivar_set(self, CAPACITY_VARIABLE_NAME, INT2FIX(new_capacity));
  rb_ivar_set(self, DATA_VARIABLE_NAME, rb_str_new((const char *)new_data, new_capacity * sizeof(unsigned int)));
}

static void fast_bitarray_update_range(VALUE self, unsigned int position) {
  VALUE rb_min_position = rb_ivar_get(self, MIN_POSITION_VARIABLE_NAME);
  VALUE rb_max_position = rb_ivar_get(self, MAX_POSITION_VARIABLE_NAME);

  unsigned int min_position = FIX2INT(rb_min_position);
  unsigned int max_position = FIX2INT(rb_max_position);

  if (position < min_position) {
    min_position = position;
  }

  if (position > max_position) {
    max_position = position;
  }

  rb_ivar_set(self, MIN_POSITION_VARIABLE_NAME, INT2FIX(min_position));
  rb_ivar_set(self, MAX_POSITION_VARIABLE_NAME, INT2FIX(max_position));
}

static VALUE fast_bitarray_set_bit(VALUE self, VALUE rb_bit_index) {
  if (!NIL_P(rb_bit_index) && FIXNUM_P(rb_bit_index)) {
    unsigned int bit_index = FIX2INT(rb_bit_index);

    unsigned int quotient = bit_index / UNSIGNED_INT_BITS;
    unsigned int modulus = bit_index % UNSIGNED_INT_BITS;

    VALUE rb_capacity = rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
    if (quotient >= FIX2INT(rb_capacity)) {
      fast_bitarray_increase_capacity(self, quotient);
    }

    fast_bitarray_update_range(self, quotient);

    VALUE rb_data = rb_ivar_get(self, DATA_VARIABLE_NAME);
    unsigned int *data = (unsigned int *)RSTRING_PTR(rb_data);
    data[quotient] = data[quotient] | (1 << modulus);

    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE fast_bitarray_reset_bit(VALUE self, VALUE rb_bit_index) {
  if (!NIL_P(rb_bit_index) && FIXNUM_P(rb_bit_index)) {
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
  } else {
    return Qfalse;
  }
}

static VALUE fast_bitarray_has_bit(VALUE self, VALUE rb_bit_index) {
  Check_Type(rb_bit_index, T_FIXNUM);

  unsigned int bit_index = FIX2INT(rb_bit_index);

  unsigned int quotient = bit_index / UNSIGNED_INT_BITS;
  unsigned int modulus = bit_index % UNSIGNED_INT_BITS;

  VALUE rb_capacity = rb_ivar_get(self, CAPACITY_VARIABLE_NAME);
  if (quotient < FIX2INT(rb_capacity)) {
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
  VALUE rb_data = rb_ivar_get(self, DATA_VARIABLE_NAME);
  unsigned int *data = (unsigned int *)RSTRING_PTR(rb_data);

  unsigned int result = 0;

  VALUE rb_min_position = rb_ivar_get(self, MIN_POSITION_VARIABLE_NAME);
  VALUE rb_max_position = rb_ivar_get(self, MAX_POSITION_VARIABLE_NAME);

  unsigned int min_position = FIX2INT(rb_min_position);
  unsigned int max_position = FIX2INT(rb_max_position);

  unsigned int from = MAX(min_position, 0);
  unsigned int to = MIN(max_position, (RSTRING_LEN(rb_data) / sizeof(unsigned int)));

  for (unsigned int i = from; i <= to; i++) {
    unsigned int v = data[i];

    unsigned int c = ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
    c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
    c += ((v >> 24) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;

    result += c;
  }

  return INT2FIX(result);
}

// Init BitArray class

void Init_fast_bitarray(void) {
  DATA_VARIABLE_NAME = rb_intern("data");
  CAPACITY_VARIABLE_NAME = rb_intern("capacity");
  MIN_POSITION_VARIABLE_NAME = rb_intern("min_position");
  MAX_POSITION_VARIABLE_NAME = rb_intern("max_position");

  VALUE cFastBitarray = rb_define_class("FastBitarray", rb_cObject);

  rb_define_method(cFastBitarray, "initialize", fast_bitarray_init, 1);

  rb_define_method(cFastBitarray, "capacity", fast_bitarray_capacity, 0);

  rb_define_method(cFastBitarray, "set_bit", fast_bitarray_set_bit, 1);
  rb_define_method(cFastBitarray, "reset_bit", fast_bitarray_reset_bit, 1);
  rb_define_method(cFastBitarray, "has_bit?", fast_bitarray_has_bit, 1);

  rb_define_method(cFastBitarray, "count", fast_bitarray_count, 0);
}