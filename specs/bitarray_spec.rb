require 'minitest/spec'
require 'minitest/autorun'

require 'fast_bitarray'

describe FastBitarray do

  before do
    @bitarray = FastBitarray.new(1)
  end

  it 'can be fill zeros after initialization' do
    31.times do |bit_index|
      @bitarray.has_bit?(bit_index).must_equal false
    end
  end

  it 'can return capacity' do
    @bitarray.capacity.must_equal 1
  end

  it 'can automatically increase capacity' do
    @bitarray.set_bit(32)
    @bitarray.capacity.must_equal 2
  end

  it 'can set bit' do
    @bitarray.set_bit(0)

    31.times do |bit_index|
      @bitarray.has_bit?(bit_index).must_equal (bit_index == 0)
    end
  end

  it 'can set several bits' do
    @bitarray.set_bit(0)
    @bitarray.set_bit(31)

    31.times do |bit_index|
      @bitarray.has_bit?(bit_index).must_equal (bit_index == 0 || bit_index == 31)
    end
  end

  it 'can reset bit' do
    @bitarray.set_bit(0)
    @bitarray.reset_bit(0)

    31.times do |bit_index|
      @bitarray.has_bit?(bit_index).must_equal false
    end
  end

  it 'can reset bit without increase capacity' do
    @bitarray.reset_bit(1024)
    @bitarray.capacity.must_equal 1
  end

  it 'can check bit without increase capacity' do
    @bitarray.has_bit?(1024)
    @bitarray.capacity.must_equal 1
  end

  it 'can count set bits' do
    @bitarray.count.must_equal 0

    @bitarray.set_bit(1024)
    @bitarray.count.must_equal 1
  end

  it 'can union FastBitarray-s' do
    @bitarray.set_bit(1)

    bitarray = FastBitarray.new(1)
    bitarray.set_bit(1024)

    result = @bitarray | bitarray
    result.has_bit?(1).must_equal true
    result.has_bit?(1024).must_equal true
    result.count.must_equal 2
  end

  it 'can intersect FastBitarray-s' do
    @bitarray.set_bit(1)
    @bitarray.set_bit(2)

    bitarray = FastBitarray.new(1)
    bitarray.set_bit(1024)
    bitarray.set_bit(2)

    result = @bitarray & bitarray
    result.has_bit?(1).must_equal false
    result.has_bit?(1024).must_equal false
    result.has_bit?(2).must_equal true
    result.count.must_equal 1
  end

  it 'can subtract FastBitarray-s' do
    @bitarray.set_bit(1)
    @bitarray.set_bit(2)

    bitarray = FastBitarray.new(1)
    bitarray.set_bit(1024)
    bitarray.set_bit(2)

    result = @bitarray - bitarray
    result.has_bit?(1).must_equal true
    result.has_bit?(1024).must_equal false
    result.has_bit?(2).must_equal false
    result.count.must_equal 1
  end

  it 'can return array of setted bits' do
    @bitarray.set_bit(1)
    @bitarray.set_bit(1024)

    @bitarray.to_a.must_equal [1, 1024]
  end

end