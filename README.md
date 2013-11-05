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
~~~

## Benchmark

~~~
Rehearsal ------------------------------------------------
FastBitarray   1.060000   0.000000   1.060000 (  1.057206)
Hash           4.250000   0.120000   4.370000 (  4.373504)
Set            6.180000   0.240000   6.420000 (  6.419945)
-------------------------------------- total: 11.850000sec

                   user     system      total        real
FastBitarray   1.050000   0.000000   1.050000 (  1.052929)
Hash           4.030000   0.120000   4.150000 (  4.155798)
Set            4.320000   0.120000   4.440000 (  4.445093)
~~~

**FastBitarray in 4X times faster than Hash and Set.**

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
