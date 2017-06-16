def require_path(pathes)
  require File.join(File.expand_path(File.dirname(__FILE__)), File.join(pathes))
end

require_path ['monomial.rb']

class Polynomial
  attr_reader :monomials, :max_powers
  
  def initialize(monomials=[])
    @monomials = []
    @max_powers = Hash.new(0)
    monomials.each do |m| @monomials << Monomial.new(m.powers, m.coef) end
    sync_max_powers!
  end

  def empty?
    @monomials.empty?
  end

  def sync_max_powers!
    @max_powers.clear
    @monomials.each do |m|
      m.powers.each do |key, value|
        @max_powers[key] = value if value > @max_powers[key]
      end
    end
    @symbols = @max_powers.keys.sort
  end

  def compress(monomials)
    return [] if monomials.empty?
    sync_max_powers!

    h = Hash.new{|h,k| h[k] = []}

    monomials.each do |m| h[m.get_hash_str] << m end
    result_monomials = []

    h.each do |key, value|
      raise RuntimeError unless value.all? {|v| v.equal_power? value.first}
      coef = value.map{|m| m.coef}.inject(:+)
      result_monomials << Monomial.new(value.first.powers, coef)
    end
    result_monomials.delete_if {|m| m.zero?}
  end

  def ==(other)
    my_hash = Hash.new{|h,k| h[k] = []}
    @monomials.each do |m| my_hash[m.get_hash_str] << m end

    other_hash = Hash.new{|h,k| h[k] = []}
    other.monomials.each do |m| other_hash[m.get_hash_str] << m end

    return false unless my_hash.size == other_hash.size
    my_hash.each do |key, value|
      return false unless other_hash[key] == value
    end

    true
  end

  def compress!
    @monomials = compress(@monomials)
    sync_max_powers!
    self
  end

  def *(other)
    if other.is_a?(Numeric)
      Polynomial.new(@monomials.map{ |m| m * other})
    elsif other.is_a?(Monomial)
      Polynomial.new(@monomials.map{ |m| m * other})
    elsif other.is_a?(Polynomial)
      Polynomial.new(mul(other)).compress!
    else
      raise NotImplementedError
    end
  end

  def +(other)
    if other.is_a?(Numeric)
      Polynomial.new(@monomials + [Monomial.new({}, other.to_f)]).compress!
    elsif other.is_a?(Monomial)
      Polynomial.new(@monomials + [other]).compress!
    elsif other.is_a?(Polynomial)
      Polynomial.new(@monomials + other.monomials).compress!
    else
      raise NotImplementedError
    end
  end

  def to_s
    return @monomials.first.to_s if @monomials.size == 1
    @monomials.first.to_s + @monomials[1..-1].map{|m| m.coef <= 0 ? m.to_s : "+" + m.to_s}.join
  end

  private

  def mul(other)
    monomials = []
    @monomials.each do |m| monomials.concat(other.monomials.map{|om| om * m}) end
    compress(monomials)
  end  
end
