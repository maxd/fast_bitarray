# FastBitarray

Fast implementation of BitArray for Ruby (base on C extension).

## Installation

Add this line to your application's Gemfile:

    gem 'fast_bitarray'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install fast_bitarray

## Usage

~~~
require 'fast_bitarray'

bitarray = FastBitarray.new(1)  # Set default capacity to one 32-bit block

bitarray.set_bit(42)            # Set bit
bitarray.set_bit(43)

bitarray.reset_bit(42)          # Reset bit

bitarray.count                  # Count of set bits ==> 1

...

bitarray1 = FastBitarray.new(1)
bitarray2 = FastBitarray.new(1)

bitarray1 + bitarray2           # Union
bitarray1 & bitarray2           # Intersection
bitarray1 - bitarray2           # Difference

~~~

## Benchmark

**FastBitarray in 4X times faster than Hash and Set on set operations.**
**FastBitarray in 64X times faster than Set on union, intersect and difference operations.**

~~~
### Set bit
Rehearsal ------------------------------------------------
FastBitarray   0.210000   0.000000   0.210000 (  0.207097)
Hash           0.660000   0.020000   0.680000 (  0.683163)
Set            1.030000   0.040000   1.070000 (  1.067454)
--------------------------------------- total: 1.960000sec

                   user     system      total        real
FastBitarray   0.210000   0.000000   0.210000 (  0.207795)
Hash           0.650000   0.020000   0.670000 (  0.669601)
Set            0.700000   0.020000   0.720000 (  0.718608)

### Union
Rehearsal ------------------------------------------------
FastBitarray   0.280000   0.020000   0.300000 (  0.301866)
Set           20.810000   0.490000  21.300000 ( 21.294582)
-------------------------------------- total: 21.600000sec

                   user     system      total        real
FastBitarray   0.260000   0.010000   0.270000 (  0.266758)
Set           20.190000   0.600000  20.790000 ( 20.786927)

### Intersect
Rehearsal ------------------------------------------------
FastBitarray   0.280000   0.010000   0.290000 (  0.295968)
Set           18.820000   0.210000  19.030000 ( 19.034555)
-------------------------------------- total: 19.320000sec

                   user     system      total        real
FastBitarray   0.260000   0.010000   0.270000 (  0.270213)
Set           18.390000   0.180000  18.570000 ( 18.555732)

### Difference
Rehearsal ------------------------------------------------
FastBitarray   0.180000   0.020000   0.200000 (  0.205645)
Set           19.570000   0.500000  20.070000 ( 20.060203)
-------------------------------------- total: 20.270000sec

                   user     system      total        real
FastBitarray   0.180000   0.010000   0.190000 (  0.192474)
Set           19.500000   0.470000  19.970000 ( 19.962924)
~~~

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
