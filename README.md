# FastBitarray [![Build Status](https://travis-ci.org/maxd/fast_bitarray.png)](https://travis-ci.org/maxd/fast_bitarray)

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
FastBitarray   0.150000   0.000000   0.150000 (  0.146805)
Hash           0.670000   0.030000   0.700000 (  0.691553)
Set            0.990000   0.040000   1.030000 (  1.031567)
--------------------------------------- total: 1.880000sec

                   user     system      total        real
FastBitarray   0.150000   0.000000   0.150000 (  0.150222)
Hash           0.650000   0.020000   0.670000 (  0.675739)
Set            0.720000   0.020000   0.740000 (  0.748211)

### Union
Rehearsal ------------------------------------------------
FastBitarray   0.290000   0.020000   0.310000 (  0.304275)
Set           21.450000   0.540000  21.990000 ( 22.023062)
-------------------------------------- total: 22.300000sec

                   user     system      total        real
FastBitarray   0.260000   0.010000   0.270000 (  0.272294)
Set           22.080000   0.400000  22.480000 ( 22.490656)

### Intersect
Rehearsal ------------------------------------------------
FastBitarray   0.290000   0.020000   0.310000 (  0.312937)
Set           20.530000   0.220000  20.750000 ( 20.759667)
-------------------------------------- total: 21.060000sec

                   user     system      total        real
FastBitarray   0.260000   0.020000   0.280000 (  0.276119)
Set           18.660000   0.180000  18.840000 ( 18.847334)

### Difference
Rehearsal ------------------------------------------------
FastBitarray   0.190000   0.030000   0.220000 (  0.217850)
Set           20.630000   0.560000  21.190000 ( 21.203560)
-------------------------------------- total: 21.410000sec

                   user     system      total        real
FastBitarray   0.190000   0.020000   0.210000 (  0.204440)
Set           19.890000   0.520000  20.410000 ( 20.418113)
~~~

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
